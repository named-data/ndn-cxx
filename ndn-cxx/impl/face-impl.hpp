/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_IMPL_FACE_IMPL_HPP
#define NDN_IMPL_FACE_IMPL_HPP

#include "ndn-cxx/face.hpp"
#include "ndn-cxx/impl/lp-field-tag.hpp"
#include "ndn-cxx/impl/pending-interest.hpp"
#include "ndn-cxx/impl/registered-prefix.hpp"
#include "ndn-cxx/lp/packet.hpp"
#include "ndn-cxx/lp/tags.hpp"
#include "ndn-cxx/mgmt/nfd/command-options.hpp"
#include "ndn-cxx/mgmt/nfd/controller.hpp"
#include "ndn-cxx/transport/tcp-transport.hpp"
#include "ndn-cxx/transport/unix-transport.hpp"
#include "ndn-cxx/util/config-file.hpp"
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/util/scheduler.hpp"
#include "ndn-cxx/util/signal.hpp"

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
  using PendingInterestTable = RecordContainer<PendingInterest>;
  using InterestFilterTable = RecordContainer<InterestFilterRecord>;
  using RegisteredPrefixTable = RecordContainer<RegisteredPrefix>;

  explicit
  Impl(Face& face)
    : m_face(face)
    , m_scheduler(m_face.getIoService())
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
  asyncExpressInterest(RecordId id, shared_ptr<const Interest> interest,
                       const DataCallback& afterSatisfied,
                       const NackCallback& afterNacked,
                       const TimeoutCallback& afterTimeout)
  {
    NDN_LOG_DEBUG("<I " << *interest);
    this->ensureConnected(true);

    const Interest& interest2 = *interest;
    auto& entry = m_pendingInterestTable.put(id, std::move(interest), afterSatisfied, afterNacked,
                                             afterTimeout, ref(m_scheduler));

    lp::Packet lpPacket;
    addFieldFromTag<lp::NextHopFaceIdField, lp::NextHopFaceIdTag>(lpPacket, interest2);
    addFieldFromTag<lp::CongestionMarkField, lp::CongestionMarkTag>(lpPacket, interest2);

    entry.recordForwarding();
    m_face.m_transport->send(finishEncoding(std::move(lpPacket), interest2.wireEncode(),
                                            'I', interest2.getName()));
    dispatchInterest(entry, interest2);
  }

  void
  asyncRemovePendingInterest(RecordId id)
  {
    m_pendingInterestTable.erase(id);
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
    m_pendingInterestTable.removeIf([&] (PendingInterest& entry) {
      if (!entry.getInterest()->matchesData(data)) {
        return false;
      }
      NDN_LOG_DEBUG("   satisfying " << *entry.getInterest() << " from " << entry.getOrigin());

      if (entry.getOrigin() == PendingInterestOrigin::APP) {
        hasAppMatch = true;
        entry.invokeDataCallback(data);
      }
      else {
        hasForwarderMatch = true;
      }

      return true;
    });
    // if Data matches no pending Interest record, it is sent to the forwarder as unsolicited Data
    return hasForwarderMatch || !hasAppMatch;
  }

  /** @return a Nack to be sent to the forwarder, or nullopt if no Nack should be sent
   */
  optional<lp::Nack>
  nackPendingInterests(const lp::Nack& nack)
  {
    optional<lp::Nack> outNack;
    m_pendingInterestTable.removeIf([&] (PendingInterest& entry) {
      if (!nack.getInterest().matchesInterest(*entry.getInterest())) {
        return false;
      }
      NDN_LOG_DEBUG("   nacking " << *entry.getInterest() << " from " << entry.getOrigin());

      optional<lp::Nack> outNack1 = entry.recordNack(nack);
      if (!outNack1) {
        return false;
      }

      if (entry.getOrigin() == PendingInterestOrigin::APP) {
        entry.invokeNackCallback(*outNack1);
      }
      else {
        outNack = outNack1;
      }
      return true;
    });
    // send "least severe" Nack from any PendingInterest record originated from forwarder, because
    // it is unimportant to consider Nack reason for the unlikely case when forwarder sends multiple
    // Interests to an app in a short while
    return outNack;
  }

public: // producer
  void
  asyncSetInterestFilter(RecordId id, const InterestFilter& filter,
                         const InterestCallback& onInterest)
  {
    NDN_LOG_INFO("setting InterestFilter: " << filter);
    m_interestFilterTable.put(id, filter, onInterest);
  }

  void
  asyncUnsetInterestFilter(RecordId id)
  {
    const InterestFilterRecord* record = m_interestFilterTable.get(id);
    if (record != nullptr) {
      NDN_LOG_INFO("unsetting InterestFilter: " << record->getFilter());
      m_interestFilterTable.erase(id);
    }
  }

  void
  processIncomingInterest(shared_ptr<const Interest> interest)
  {
    const Interest& interest2 = *interest;
    auto& entry = m_pendingInterestTable.insert(std::move(interest), ref(m_scheduler));
    dispatchInterest(entry, interest2);
  }

  void
  dispatchInterest(PendingInterest& entry, const Interest& interest)
  {
    m_interestFilterTable.forEach([&] (const InterestFilterRecord& filter) {
      if (!filter.doesMatch(entry)) {
        return;
      }
      NDN_LOG_DEBUG("   matches " << filter.getFilter());
      entry.recordForwarding();
      filter.invokeInterestCallback(interest);
    });
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
  RecordId
  registerPrefix(const Name& prefix,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 uint64_t flags, const nfd::CommandOptions& options,
                 const optional<InterestFilter>& filter, const InterestCallback& onInterest)
  {
    NDN_LOG_INFO("registering prefix: " << prefix);
    auto id = m_registeredPrefixTable.allocateId();

    m_face.m_nfdController->start<nfd::RibRegisterCommand>(
      nfd::ControlParameters().setName(prefix).setFlags(flags),
      [=] (const nfd::ControlParameters&) {
        NDN_LOG_INFO("registered prefix: " << prefix);

        RecordId filterId = 0;
        if (filter) {
          NDN_LOG_INFO("setting InterestFilter: " << *filter);
          InterestFilterRecord& filterRecord = m_interestFilterTable.insert(*filter, onInterest);
          filterId = filterRecord.getId();
        }

        m_registeredPrefixTable.put(id, prefix, options, filterId);

        if (onSuccess != nullptr) {
          onSuccess(prefix);
        }
      },
      [=] (const nfd::ControlResponse& resp) {
        NDN_LOG_INFO("register prefix failed: " << prefix);
        onFailure(prefix, resp.getText());
      },
      options);

    return id;
  }

  void
  asyncUnregisterPrefix(RecordId id,
                        const UnregisterPrefixSuccessCallback& onSuccess,
                        const UnregisterPrefixFailureCallback& onFailure)
  {
    const RegisteredPrefix* record = m_registeredPrefixTable.get(id);
    if (record == nullptr) {
      if (onFailure != nullptr) {
        onFailure("Unrecognized RegisteredPrefixHandle");
      }
      return;
    }

    if (record->getFilterId() != 0) {
      asyncUnsetInterestFilter(record->getFilterId());
    }

    NDN_LOG_INFO("unregistering prefix: " << record->getPrefix());
    m_face.m_nfdController->start<nfd::RibUnregisterCommand>(
      nfd::ControlParameters().setName(record->getPrefix()),
      [=] (const nfd::ControlParameters&) {
        NDN_LOG_INFO("unregistered prefix: " << record->getPrefix());
        m_registeredPrefixTable.erase(id);

        if (onSuccess != nullptr) {
          onSuccess();
        }
      },
      [=] (const nfd::ControlResponse& resp) {
        NDN_LOG_INFO("unregister prefix failed: " << record->getPrefix());
        onFailure(resp.getText());
      },
      record->getCommandOptions());
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
      NDN_THROW(Face::OversizedPacketError(pktType, name, wire.size()));
    }

    return wire;
  }

private:
  Face& m_face;
  Scheduler m_scheduler;
  scheduler::ScopedEventId m_processEventsTimeoutEvent;

  PendingInterestTable m_pendingInterestTable;
  InterestFilterTable m_interestFilterTable;
  RegisteredPrefixTable m_registeredPrefixTable;

  unique_ptr<boost::asio::io_service::work> m_ioServiceWork; // if thread needs to be preserved

  friend class Face;
};

} // namespace ndn

#endif // NDN_IMPL_FACE_IMPL_HPP
