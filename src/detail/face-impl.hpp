/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "../common.hpp"
#include "../face.hpp"

#include "registered-prefix.hpp"
#include "pending-interest.hpp"
#include "container-with-on-empty-signal.hpp"

#include "../util/scheduler.hpp"
#include "../util/config-file.hpp"
#include "../util/signal.hpp"

#include "../transport/transport.hpp"
#include "../transport/unix-transport.hpp"
#include "../transport/tcp-transport.hpp"

#include "../mgmt/nfd/controller.hpp"
#include "../mgmt/nfd/command-options.hpp"

#include "../lp/packet.hpp"
#include "../lp/tags.hpp"

namespace ndn {

/**
 * @brief implementation detail of Face
 */
class Face::Impl : noncopyable
{
public:
  typedef ContainerWithOnEmptySignal<shared_ptr<PendingInterest>> PendingInterestTable;
  typedef std::list<shared_ptr<InterestFilterRecord>> InterestFilterTable;
  typedef ContainerWithOnEmptySignal<shared_ptr<RegisteredPrefix>> RegisteredPrefixTable;

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
    this->ensureConnected(true);

    auto entry = m_pendingInterestTable.insert(make_shared<PendingInterest>(
      interest, afterSatisfied, afterNacked, afterTimeout, ref(m_scheduler))).first;
    (*entry)->setDeleter([this, entry] { m_pendingInterestTable.erase(entry); });

    lp::Packet packet;

    shared_ptr<lp::NextHopFaceIdTag> nextHopFaceIdTag = interest->getTag<lp::NextHopFaceIdTag>();
    if (nextHopFaceIdTag != nullptr) {
      packet.add<lp::NextHopFaceIdField>(*nextHopFaceIdTag);
    }

    packet.add<lp::FragmentField>(std::make_pair(interest->wireEncode().begin(),
                                                 interest->wireEncode().end()));

    m_face.m_transport->send(packet.wireEncode());
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

  void
  satisfyPendingInterests(const Data& data)
  {
    for (auto entry = m_pendingInterestTable.begin(); entry != m_pendingInterestTable.end(); ) {
      if ((*entry)->getInterest()->matchesData(data)) {
        shared_ptr<PendingInterest> matchedEntry = *entry;
        entry = m_pendingInterestTable.erase(entry);
        matchedEntry->invokeDataCallback(data);
      }
      else {
        ++entry;
      }
    }
  }

  void
  nackPendingInterests(const lp::Nack& nack)
  {
    for (auto entry = m_pendingInterestTable.begin(); entry != m_pendingInterestTable.end(); ) {
      const Interest& pendingInterest = *(*entry)->getInterest();
      if (pendingInterest == nack.getInterest()) {
        shared_ptr<PendingInterest> matchedEntry = *entry;
        entry = m_pendingInterestTable.erase(entry);
        matchedEntry->invokeNackCallback(nack);
      }
      else {
        ++entry;
      }
    }
  }

public: // producer
  void
  asyncSetInterestFilter(shared_ptr<InterestFilterRecord> interestFilterRecord)
  {
    m_interestFilterTable.push_back(interestFilterRecord);
  }

  void
  asyncUnsetInterestFilter(const InterestFilterId* interestFilterId)
  {
    InterestFilterTable::iterator i = std::find_if(m_interestFilterTable.begin(),
                                                   m_interestFilterTable.end(),
                                                   MatchInterestFilterId(interestFilterId));
    if (i != m_interestFilterTable.end()) {
      m_interestFilterTable.erase(i);
    }
  }

  void
  processInterestFilters(Interest& interest)
  {
    for (const auto& filter : m_interestFilterTable) {
      if (filter->doesMatch(interest.getName())) {
        filter->invokeInterestCallback(interest);
      }
    }
  }

  void
  asyncSend(const Block& wire)
  {
    this->ensureConnected(true);
    m_face.m_transport->send(wire);
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
    nfd::ControlParameters params;
    params.setName(prefix);
    params.setFlags(flags);

    auto prefixToRegister = make_shared<RegisteredPrefix>(prefix, filter, options);

    m_face.m_nfdController->start<nfd::RibRegisterCommand>(
      params,
      [=] (const nfd::ControlParameters&) { this->afterPrefixRegistered(prefixToRegister, onSuccess); },
      [=] (const nfd::ControlResponse& resp) { onFailure(prefixToRegister->getPrefix(), resp.getText()); },
      options);

    return reinterpret_cast<const RegisteredPrefixId*>(prefixToRegister.get());
  }

  void
  afterPrefixRegistered(shared_ptr<RegisteredPrefix> registeredPrefix,
                        const RegisterPrefixSuccessCallback& onSuccess)
  {
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

      nfd::ControlParameters params;
      params.setName(record.getPrefix());
      m_face.m_nfdController->start<nfd::RibUnregisterCommand>(
        params,
        [=] (const nfd::ControlParameters&) { this->finalizeUnregisterPrefix(i, onSuccess); },
        [=] (const nfd::ControlResponse& resp) { onFailure(resp.getText()); },
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
