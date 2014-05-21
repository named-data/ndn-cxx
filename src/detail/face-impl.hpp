/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "../util/scheduler.hpp"
#include "../util/config-file.hpp"

#include "transport/transport.hpp"
#include "transport/unix-transport.hpp"
#include "transport/tcp-transport.hpp"

#include "management/nfd-controller.hpp"

namespace ndn {

class Face::Impl : noncopyable
{
public:
  typedef std::list<shared_ptr<PendingInterest> > PendingInterestTable;
  typedef std::list<shared_ptr<InterestFilterRecord> > InterestFilterTable;
  typedef std::list<shared_ptr<RegisteredPrefix> > RegisteredPrefixTable;

  explicit
  Impl(Face& face)
    : m_face(face)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  void
  satisfyPendingInterests(Data& data)
  {
    for (PendingInterestTable::iterator i = m_pendingInterestTable.begin();
         i != m_pendingInterestTable.end();
         )
      {
        if ((*i)->getInterest()->matchesData(data))
          {
            // Copy pointers to the objects and remove the PIT entry before calling the callback.
            OnData onData = (*i)->getOnData();
            shared_ptr<const Interest> interest = (*i)->getInterest();

            PendingInterestTable::iterator next = i;
            ++next;
            m_pendingInterestTable.erase(i);
            i = next;

            if (static_cast<bool>(onData)) {
              onData(*interest, data);
            }
          }
        else
          ++i;
      }
  }

  void
  processInterestFilters(Interest& interest)
  {
    for (InterestFilterTable::iterator i = m_interestFilterTable.begin();
         i != m_interestFilterTable.end();
         ++i)
      {
        if ((*i)->doesMatch(interest.getName()))
          {
            (**i)(interest);
          }
      }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  void
  asyncExpressInterest(const shared_ptr<const Interest>& interest,
                       const OnData& onData, const OnTimeout& onTimeout)
  {
    if (!m_face.m_transport->isExpectingData())
      m_face.m_transport->resume();

    m_pendingInterestTable.push_back(make_shared<PendingInterest>(interest, onData, onTimeout));

    if (!interest->getLocalControlHeader().empty(false, true))
      {
        // encode only NextHopFaceId towards the forwarder
        m_face.m_transport->send(interest->getLocalControlHeader()
                                   .wireEncode(*interest, false, true),
                                 interest->wireEncode());
      }
    else
      {
        m_face.m_transport->send(interest->wireEncode());
      }

    if (!m_pitTimeoutCheckTimerActive) {
      m_pitTimeoutCheckTimerActive = true;
      m_pitTimeoutCheckTimer->expires_from_now(time::milliseconds(100));
      m_pitTimeoutCheckTimer->async_wait(bind(&Impl::checkPitExpire, this));
    }
  }

  void
  asyncRemovePendingInterest(const PendingInterestId* pendingInterestId)
  {
    m_pendingInterestTable.remove_if(MatchPendingInterestId(pendingInterestId));
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

  template<class SignatureGenerator>
  const RegisteredPrefixId*
  registerPrefix(const Name& prefix,
                 const shared_ptr<InterestFilterRecord>& filter,
                 const RegisterPrefixSuccessCallback& onSuccess,
                 const RegisterPrefixFailureCallback& onFailure,
                 const SignatureGenerator& signatureGenerator)
  {
    using namespace nfd;

    typedef void (nfd::Controller::*Registrator)
      (const nfd::ControlParameters&,
       const nfd::Controller::CommandSucceedCallback&,
       const nfd::Controller::CommandFailCallback&,
       const SignatureGenerator&,
       const time::milliseconds&);

    Registrator registrator, unregistrator;
    if (!m_face.m_isDirectNfdFibManagementRequested) {
      registrator = static_cast<Registrator>(&Controller::start<RibRegisterCommand>);
      unregistrator = static_cast<Registrator>(&Controller::start<RibUnregisterCommand>);
    }
    else {
      registrator = static_cast<Registrator>(&Controller::start<FibAddNextHopCommand>);
      unregistrator = static_cast<Registrator>(&Controller::start<FibRemoveNextHopCommand>);
    }

    ControlParameters parameters;
    parameters.setName(prefix);

    RegisteredPrefix::Unregistrator bindedUnregistrator =
      bind(unregistrator, m_face.m_nfdController, parameters, _1, _2,
           signatureGenerator,
           m_face.m_nfdController->getDefaultCommandTimeout());

    shared_ptr<RegisteredPrefix> prefixToRegister =
      ndn::make_shared<RegisteredPrefix>(prefix, filter, bindedUnregistrator);

    ((*m_face.m_nfdController).*registrator)(parameters,
                                             bind(&Impl::afterPrefixRegistered, this,
                                                  prefixToRegister, onSuccess),
                                             bind(onFailure, prefixToRegister->getPrefix(), _2),
                                             signatureGenerator,
                                             m_face.m_nfdController->getDefaultCommandTimeout());

    return reinterpret_cast<const RegisteredPrefixId*>(prefixToRegister.get());
  }

  void
  afterPrefixRegistered(const shared_ptr<RegisteredPrefix>& registeredPrefix,
                        const RegisterPrefixSuccessCallback& onSuccess)
  {
    m_registeredPrefixTable.push_back(registeredPrefix);

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
    RegisteredPrefixTable::iterator i = std::find_if(m_registeredPrefixTable.begin(),
                                                     m_registeredPrefixTable.end(),
                                                     MatchRegisteredPrefixId(registeredPrefixId));
    if (i != m_registeredPrefixTable.end())
      {
        const shared_ptr<InterestFilterRecord>& filter = (*i)->getFilter();
        if (static_cast<bool>(filter))
          {
            // it was a combined operation
            m_interestFilterTable.remove(filter);
          }

        (*i)->unregister(bind(&Impl::finalizeUnregisterPrefix, this, i, onSuccess),
                         bind(onFailure, _2));
      }
    else
      onFailure("Unrecognized PrefixId");

    // there cannot be two registered prefixes with the same id
  }

  void
  finalizeUnregisterPrefix(RegisteredPrefixTable::iterator item,
                           const UnregisterPrefixSuccessCallback& onSuccess)
  {
    m_registeredPrefixTable.erase(item);

    if (!m_pitTimeoutCheckTimerActive && m_registeredPrefixTable.empty())
      {
        m_face.m_transport->pause();
        if (!m_ioServiceWork) {
          m_processEventsTimeoutTimer->cancel();
        }
      }

    if (static_cast<bool>(onSuccess)) {
      onSuccess();
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////

  void
  checkPitExpire()
  {
    // Check for PIT entry timeouts.
    time::steady_clock::TimePoint now = time::steady_clock::now();

    PendingInterestTable::iterator i = m_pendingInterestTable.begin();
    while (i != m_pendingInterestTable.end())
      {
        if ((*i)->isTimedOut(now))
          {
            // Save the PendingInterest and remove it from the PIT.  Then call the callback.
            shared_ptr<PendingInterest> pendingInterest = *i;

            i = m_pendingInterestTable.erase(i);

            pendingInterest->callTimeout();
          }
        else
          ++i;
      }

    if (!m_pendingInterestTable.empty()) {
      m_pitTimeoutCheckTimerActive = true;

      m_pitTimeoutCheckTimer->expires_from_now(time::milliseconds(100));
      m_pitTimeoutCheckTimer->async_wait(bind(&Impl::checkPitExpire, this));
    }
    else {
      m_pitTimeoutCheckTimerActive = false;

      if (m_registeredPrefixTable.empty()) {
        m_face.m_transport->pause();
        if (!m_ioServiceWork) {
          m_processEventsTimeoutTimer->cancel();
        }
      }
    }
  }

private:
  Face& m_face;

  PendingInterestTable m_pendingInterestTable;
  InterestFilterTable m_interestFilterTable;
  RegisteredPrefixTable m_registeredPrefixTable;

  ConfigFile m_config;

  shared_ptr<boost::asio::io_service::work> m_ioServiceWork; // if thread needs to be preserved
  shared_ptr<monotonic_deadline_timer> m_pitTimeoutCheckTimer;
  bool m_pitTimeoutCheckTimerActive;
  shared_ptr<monotonic_deadline_timer> m_processEventsTimeoutTimer;

  friend class Face;
};

} // namespace ndn

#endif // NDN_DETAIL_FACE_IMPL_HPP
