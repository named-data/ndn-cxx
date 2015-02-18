/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "../management/nfd-controller.hpp"
#include "../management/nfd-command-options.hpp"

namespace ndn {

class Face::Impl : noncopyable
{
public:
  typedef ContainerWithOnEmptySignal<shared_ptr<PendingInterest>> PendingInterestTable;
  typedef std::list<shared_ptr<InterestFilterRecord> > InterestFilterTable;
  typedef ContainerWithOnEmptySignal<shared_ptr<RegisteredPrefix>> RegisteredPrefixTable;

  explicit
  Impl(Face& face)
    : m_face(face)
    , m_scheduler(m_face.getIoService())
    , m_processEventsTimeoutEvent(m_scheduler)
  {
    auto postOnEmptyPitOrNoRegisteredPrefixes = [this] {
      this->m_face.getIoService().post(bind(&Impl::onEmptyPitOrNoRegisteredPrefixes, this));
      // without this extra "post", transport can get paused (-async_read) and then resumed
      // (+async_read) from within onInterest/onData callback.  After onInterest/onData
      // finishes, there is another +async_read with the same memory block.  A few of such
      // async_read duplications can cause various effects and result in segfault.
    };

    m_pendingInterestTable.onEmpty.connect(postOnEmptyPitOrNoRegisteredPrefixes);
    m_registeredPrefixTable.onEmpty.connect(postOnEmptyPitOrNoRegisteredPrefixes);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  void
  satisfyPendingInterests(Data& data)
  {
    for (auto entry = m_pendingInterestTable.begin(); entry != m_pendingInterestTable.end(); ) {
      if ((*entry)->getInterest().matchesData(data)) {
        shared_ptr<PendingInterest> matchedEntry = *entry;

        entry = m_pendingInterestTable.erase(entry);

        matchedEntry->invokeDataCallback(data);
      }
      else
        ++entry;
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

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  void
  ensureConnected(bool wantResume)
  {
    if (!m_face.m_transport->isConnected())
      m_face.m_transport->connect(m_face.m_ioService,
                                  bind(&Face::onReceiveElement, &m_face, _1));

    if (wantResume && !m_face.m_transport->isExpectingData())
      m_face.m_transport->resume();
  }

  void
  asyncExpressInterest(const shared_ptr<const Interest>& interest,
                       const OnData& onData, const OnTimeout& onTimeout)
  {
    this->ensureConnected(true);

    auto entry =
      m_pendingInterestTable.insert(make_shared<PendingInterest>(interest,
                                                                 onData, onTimeout,
                                                                 ref(m_scheduler))).first;
    (*entry)->setDeleter([this, entry] { m_pendingInterestTable.erase(entry); });

    if (!interest->getLocalControlHeader().empty(nfd::LocalControlHeader::ENCODE_NEXT_HOP)) {
      // encode only NextHopFaceId towards the forwarder
      m_face.m_transport->send(interest->getLocalControlHeader()
                               .wireEncode(*interest, nfd::LocalControlHeader::ENCODE_NEXT_HOP),
                               interest->wireEncode());
    }
    else {
      m_face.m_transport->send(interest->wireEncode());
    }
  }

  void
  asyncRemovePendingInterest(const PendingInterestId* pendingInterestId)
  {
    m_pendingInterestTable.remove_if(MatchPendingInterestId(pendingInterestId));
  }

  void
  asyncPutData(const shared_ptr<const Data>& data)
  {
    this->ensureConnected(true);

    if (!data->getLocalControlHeader().empty(nfd::LocalControlHeader::ENCODE_CACHING_POLICY)) {
      m_face.m_transport->send(
        data->getLocalControlHeader().wireEncode(*data,
                                                 nfd::LocalControlHeader::ENCODE_CACHING_POLICY),
        data->wireEncode());
    }
    else {
      m_face.m_transport->send(data->wireEncode());
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  void
  asyncSetInterestFilter(const shared_ptr<InterestFilterRecord>& interestFilterRecord)
  {
    m_interestFilterTable.push_back(interestFilterRecord);
  }

  void
  asyncUnsetInterestFilter(const InterestFilterId* interestFilterId)
  {
    InterestFilterTable::iterator i = std::find_if(m_interestFilterTable.begin(),
                                                   m_interestFilterTable.end(),
                                                   MatchInterestFilterId(interestFilterId));
    if (i != m_interestFilterTable.end())
      {
        m_interestFilterTable.erase(i);
      }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 const shared_ptr<InterestFilterRecord>& filter,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 uint64_t flags,
                 const nfd::CommandOptions& options)
  {
    using namespace nfd;

    ControlParameters params;
    params.setName(prefix);
    params.setFlags(flags);

    auto prefixToRegister = make_shared<RegisteredPrefix>(prefix, filter, options);

    m_face.m_nfdController->start<RibRegisterCommand>(params,
                                                      bind(&Impl::afterPrefixRegistered, this,
                                                           prefixToRegister, onSuccess),
                                                      bind(onFailure, prefixToRegister->getPrefix(), _2),
                                                      options);

    return reinterpret_cast<const RegisteredPrefixId*>(prefixToRegister.get());
  }

  void
  afterPrefixRegistered(const shared_ptr<RegisteredPrefix>& registeredPrefix,
                        const RegisterPrefixSuccessCallback& onSuccess)
  {
    m_registeredPrefixTable.insert(registeredPrefix);

    if (static_cast<bool>(registeredPrefix->getFilter())) {
      // it was a combined operation
      m_interestFilterTable.push_back(registeredPrefix->getFilter());
    }

    if (static_cast<bool>(onSuccess)) {
      onSuccess(registeredPrefix->getPrefix());
    }
  }

  void
  asyncUnregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                        const UnregisterPrefixSuccessCallback& onSuccess,
                        const UnregisterPrefixFailureCallback& onFailure)
  {
    using namespace nfd;
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

      ControlParameters params;
      params.setName(record.getPrefix());
      m_face.m_nfdController->start<RibUnregisterCommand>(params,
                                                          bind(&Impl::finalizeUnregisterPrefix, this, i, onSuccess),
                                                          bind(onFailure, _2),
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

    if (static_cast<bool>(onSuccess)) {
      onSuccess();
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
