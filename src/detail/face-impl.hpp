/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#ifndef NDN_DETAIL_FACE_IMPL_HPP
#define NDN_DETAIL_FACE_IMPL_HPP

#include "../face.hpp"
#include "container-with-on-empty-signal.hpp"
#include "lp-field-tag.hpp"
#include "pending-interest.hpp"
#include "registered-prefix.hpp"
#include "../lp/packet.hpp"
#include "../lp/tags.hpp"
#include "../mgmt/nfd/command-options.hpp"
#include "../mgmt/nfd/controller.hpp"
#include "../transport/tcp-transport.hpp"
#include "../transport/unix-transport.hpp"
#include "../util/config-file.hpp"
#include "../util/logger.hpp"
#include "../util/scheduler.hpp"
#include "../util/signal.hpp"

NDN_LOG_INIT(ndn.Face);
// INFO level: prefix registration, etc.
//
// DEBUG level: packet logging.
// Each log entry starts with a direction symbol ('<' denotes an outgoing packet, '>' denotes an
// incoming packet) and a packet type symbol ('I' denotes an Interest, 'D' denotes a Data, 'N'
// denotes a Nack). Interest is printed as its string representation, Data is printed as name only,
// Nack is printed as the Interest followed by the Nack reason and delimited by a '~' symbol. A
// log line about an incoming packet may be followed by zero or more lines about Interest matching
// InterestFilter, Data satisfying Interest, or Nack rejecting Interest, which are also written at
// DEBUG level.
//
// TRACE level: more detailed unstructured messages.

namespace ndn {

/** @brief implementation detail of Face
 */
class Face::Impl : noncopyable
{
public:
  using PendingInterestTable = ContainerWithOnEmptySignal<shared_ptr<PendingInterest>>;
  using InterestFilterTable = std::list<shared_ptr<InterestFilterRecord>>;
  using RegisteredPrefixTable = ContainerWithOnEmptySignal<shared_ptr<RegisteredPrefix>>;

  explicit
  Impl(Face& face)
    : m_face(face)
    , m_scheduler(m_face.getIoService())
    , m_processEventsTimeoutEvent(m_scheduler)
  {
    auto postOnEmptyPitOrNoRegisteredPrefixes = [this] {
      this->m_face.getIoService().post([this] { this->onEmptyPitOrNoRegisteredPrefixes(); });
      // without this extra "post", transport can get paused (-async_read) and then resumed
      // (+async_read) from within onInterest/onData callback.  After onInterest/onData
      // finishes, there is another +async_read with the same memory block.  A few of such
      // async_read duplications can cause various effects and result in segfault.
    };

    m_pendingInterestTable.onEmpty.connect(postOnEmptyPitOrNoRegisteredPrefixes);
    m_registeredPrefixTable.onEmpty.connect(postOnEmptyPitOrNoRegisteredPrefixes);
  }

public: // consumer
  void
  asyncExpressInterest(shared_ptr<const Interest> interest,
                       const DataCallback& afterSatisfied,
                       const NackCallback& afterNacked,
                       const TimeoutCallback& afterTimeout)
  {
    NDN_LOG_DEBUG("<I " << *interest);
    this->ensureConnected(true);

    const Interest& interest2 = *interest;
    auto i = m_pendingInterestTable.insert(make_shared<PendingInterest>(
      std::move(interest), afterSatisfied, afterNacked, afterTimeout, ref(m_scheduler))).first;
    // In dispatchInterest, an InterestCallback may respond with Data right away and delete
    // the PendingInterestTable entry. shared_ptr is retained to ensure PendingInterest instance
    // remains valid in this case.
    shared_ptr<PendingInterest> entry = *i;
    entry->setDeleter([this, i] { m_pendingInterestTable.erase(i); });

    lp::Packet lpPacket;
    addFieldFromTag<lp::NextHopFaceIdField, lp::NextHopFaceIdTag>(lpPacket, interest2);
    addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, interest2);

    entry->recordForwarding();
    m_face.m_transport->send(finishEncoding(std::move(lpPacket), interest2.wireEncode(),
                                            'I', interest2.getName()));
    dispatchInterest(*entry, interest2);
  }

  void
  asyncRemovePendingInterest(const PendingInterestId* pendingInterestId)
  {
    m_pendingInterestTable.remove_if(MatchPendingInterestId(pendingInterestId));
  }

  void
  asyncRemoveAllPendingInterests()
  {
    m_pendingInterestTable.clear();
  }

  /** @return whether the Data should be sent to the forwarder, if it does not come from the forwarder
   */
  bool
  satisfyPendingInterests(const Data& data)
  {
    bool hasAppMatch = false, hasForwarderMatch = false;
    for (auto i = m_pendingInterestTable.begin(); i != m_pendingInterestTable.end(); ) {
      shared_ptr<PendingInterest> entry = *i;
      if (!entry->getInterest()->matchesData(data)) {
        ++i;
        continue;
      }

      NDN_LOG_DEBUG("   satisfying " << *entry->getInterest() << " from " << entry->getOrigin());
      i = m_pendingInterestTable.erase(i);

      if (entry->getOrigin() == PendingInterestOrigin::APP) {
        hasAppMatch = true;
        entry->invokeDataCallback(data);
      }
      else {
        hasForwarderMatch = true;
      }
    }
    // if Data matches no pending Interest record, it is sent to the forwarder as unsolicited Data
    return hasForwarderMatch || !hasAppMatch;
  }

  /** @return a Nack to be sent to the forwarder, or nullopt if no Nack should be sent
   */
  optional<lp::Nack>
  nackPendingInterests(const lp::Nack& nack)
  {
    optional<lp::Nack> outNack;
    for (auto i = m_pendingInterestTable.begin(); i != m_pendingInterestTable.end(); ) {
      shared_ptr<PendingInterest> entry = *i;
      if (!nack.getInterest().matchesInterest(*entry->getInterest())) {
        ++i;
        continue;
      }

      NDN_LOG_DEBUG("   nacking " << *entry->getInterest() << " from " << entry->getOrigin());

      optional<lp::Nack> outNack1 = entry->recordNack(nack);
      if (!outNack1) {
        ++i;
        continue;
      }

      if (entry->getOrigin() == PendingInterestOrigin::APP) {
        entry->invokeNackCallback(*outNack1);
      }
      else {
        outNack = outNack1;
      }
      i = m_pendingInterestTable.erase(i);
    }
    // send "least severe" Nack from any PendingInterest record originated from forwarder, because
    // it is unimportant to consider Nack reason for the unlikely case when forwarder sends multiple
    // Interests to an app in a short while
    return outNack;
  }

public: // producer
  void
  asyncSetInterestFilter(shared_ptr<InterestFilterRecord> interestFilterRecord)
  {
    NDN_LOG_INFO("setting InterestFilter: " << interestFilterRecord->getFilter());
    m_interestFilterTable.push_back(std::move(interestFilterRecord));
  }

  void
  asyncUnsetInterestFilter(const InterestFilterId* interestFilterId)
  {
    InterestFilterTable::iterator i = std::find_if(m_interestFilterTable.begin(),
                                                   m_interestFilterTable.end(),
                                                   MatchInterestFilterId(interestFilterId));
    if (i != m_interestFilterTable.end()) {
      NDN_LOG_INFO("unsetting InterestFilter: " << (*i)->getFilter());
      m_interestFilterTable.erase(i);
    }
  }

  void
  processIncomingInterest(shared_ptr<const Interest> interest)
  {
    const Interest& interest2 = *interest;
    auto i = m_pendingInterestTable.insert(make_shared<PendingInterest>(
      std::move(interest), ref(m_scheduler))).first;
    // In dispatchInterest, an InterestCallback may respond with Data right away and delete
    // the PendingInterestTable entry. shared_ptr is retained to ensure PendingInterest instance
    // remains valid in this case.
    shared_ptr<PendingInterest> entry = *i;
    entry->setDeleter([this, i] { m_pendingInterestTable.erase(i); });

    this->dispatchInterest(*entry, interest2);
  }

  void
  dispatchInterest(PendingInterest& entry, const Interest& interest)
  {
    for (const auto& filter : m_interestFilterTable) {
      if (filter->doesMatch(entry)) {
        NDN_LOG_DEBUG("   matches " << filter->getFilter());
        entry.recordForwarding();
        filter->invokeInterestCallback(interest);
      }
    }
  }

  void
  asyncPutData(const Data& data)
  {
    NDN_LOG_DEBUG("<D " << data.getName());
    bool shouldSendToForwarder = satisfyPendingInterests(data);
    if (!shouldSendToForwarder) {
      return;
    }

    this->ensureConnected(true);

    lp::Packet lpPacket;
    addFieldFromTag<lp::CachePolicyField, lp::CachePolicyTag>(lpPacket, data);
    addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, data);

    m_face.m_transport->send(finishEncoding(std::move(lpPacket), data.wireEncode(),
                                            'D', data.getName()));
  }

  void
  asyncPutNack(const lp::Nack& nack)
  {
    NDN_LOG_DEBUG("<N " << nack.getInterest() << '~' << nack.getHeader().getReason());
    optional<lp::Nack> outNack = nackPendingInterests(nack);
    if (!outNack) {
      return;
    }

    this->ensureConnected(true);

    lp::Packet lpPacket;
    lpPacket.add<lp::NackField>(outNack->getHeader());
    addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, *outNack);

    const Interest& interest = outNack->getInterest();
    m_face.m_transport->send(finishEncoding(std::move(lpPacket), interest.wireEncode(),
                                            'N', interest.getName()));
  }

public: // prefix registration
  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 shared_ptr<InterestFilterRecord> filter,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 uint64_t flags,
                 const nfd::CommandOptions& options)
  {
    NDN_LOG_INFO("registering prefix: " << prefix);
    auto record = make_shared<RegisteredPrefix>(prefix, filter, options);

    nfd::ControlParameters params;
    params.setName(prefix);
    params.setFlags(flags);
    m_face.m_nfdController->start<nfd::RibRegisterCommand>(
      params,
      [=] (const nfd::ControlParameters&) { this->afterPrefixRegistered(record, onSuccess); },
      [=] (const nfd::ControlResponse& resp) {
        NDN_LOG_INFO("register prefix failed: " << record->getPrefix());
        onFailure(record->getPrefix(), resp.getText());
      },
      options);

    return reinterpret_cast<const RegisteredPrefixId*>(record.get());
  }

  void
  afterPrefixRegistered(shared_ptr<RegisteredPrefix> registeredPrefix,
                        const RegisterPrefixSuccessCallback& onSuccess)
  {
    NDN_LOG_INFO("registered prefix: " << registeredPrefix->getPrefix());
    m_registeredPrefixTable.insert(registeredPrefix);

    if (registeredPrefix->getFilter() != nullptr) {
      // it was a combined operation
      m_interestFilterTable.push_back(registeredPrefix->getFilter());
    }

    if (onSuccess != nullptr) {
      onSuccess(registeredPrefix->getPrefix());
    }
  }

  void
  asyncUnregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                        const UnregisterPrefixSuccessCallback& onSuccess,
                        const UnregisterPrefixFailureCallback& onFailure)
  {
    auto i = std::find_if(m_registeredPrefixTable.begin(),
                          m_registeredPrefixTable.end(),
                          MatchRegisteredPrefixId(registeredPrefixId));
    if (i != m_registeredPrefixTable.end()) {
      RegisteredPrefix& record = **i;
      const shared_ptr<InterestFilterRecord>& filter = record.getFilter();

      if (filter != nullptr) {
        // it was a combined operation
        m_interestFilterTable.remove(filter);
      }

      NDN_LOG_INFO("unregistering prefix: " << record.getPrefix());

      nfd::ControlParameters params;
      params.setName(record.getPrefix());
      m_face.m_nfdController->start<nfd::RibUnregisterCommand>(
        params,
        [=] (const nfd::ControlParameters&) { this->finalizeUnregisterPrefix(i, onSuccess); },
        [=] (const nfd::ControlResponse& resp) {
          NDN_LOG_INFO("unregister prefix failed: " << params.getName());
          onFailure(resp.getText());
        },
        record.getCommandOptions());
    }
    else {
      if (onFailure != nullptr) {
        onFailure("Unrecognized PrefixId");
      }
    }

    // there cannot be two registered prefixes with the same id
  }

  void
  finalizeUnregisterPrefix(RegisteredPrefixTable::iterator item,
                           const UnregisterPrefixSuccessCallback& onSuccess)
  {
    NDN_LOG_INFO("unregistered prefix: " << (*item)->getPrefix());
    m_registeredPrefixTable.erase(item);

    if (onSuccess != nullptr) {
      onSuccess();
    }
  }

public: // IO routine
  void
  ensureConnected(bool wantResume)
  {
    if (!m_face.m_transport->isConnected())
      m_face.m_transport->connect(m_face.m_ioService,
                                  [=] (const Block& wire) { m_face.onReceiveElement(wire); });

    if (wantResume && !m_face.m_transport->isReceiving()) {
      m_face.m_transport->resume();
    }
  }

  void
  onEmptyPitOrNoRegisteredPrefixes()
  {
    if (m_pendingInterestTable.empty() && m_registeredPrefixTable.empty()) {
      m_face.m_transport->pause();
      if (!m_ioServiceWork) {
        m_processEventsTimeoutEvent.cancel();
      }
    }
  }

private:
  /** @brief Finish packet encoding
   *  @param lpPacket NDNLP packet without FragmentField
   *  @param wire wire encoding of Interest or Data
   *  @param pktType packet type, 'I' for Interest, 'D' for Data, 'N' for Nack
   *  @param name packet name
   *  @return wire encoding of either NDNLP or bare network packet
   *  @throw Face::OversizedPacketError wire encoding exceeds limit
   */
  Block
  finishEncoding(lp::Packet&& lpPacket, Block wire, char pktType, const Name& name)
  {
    if (!lpPacket.empty()) {
      lpPacket.add<lp::FragmentField>(std::make_pair(wire.begin(), wire.end()));
      wire = lpPacket.wireEncode();
    }

    if (wire.size() > MAX_NDN_PACKET_SIZE) {
      BOOST_THROW_EXCEPTION(Face::OversizedPacketError(pktType, name, wire.size()));
    }

    return wire;
  }

private:
  Face& m_face;
  util::Scheduler m_scheduler;
  util::scheduler::ScopedEventId m_processEventsTimeoutEvent;

  PendingInterestTable m_pendingInterestTable;
  InterestFilterTable m_interestFilterTable;
  RegisteredPrefixTable m_registeredPrefixTable;

  unique_ptr<boost::asio::io_service::work> m_ioServiceWork; // if thread needs to be preserved

  friend class Face;
};

} // namespace ndn

#endif // NDN_DETAIL_FACE_IMPL_HPP
