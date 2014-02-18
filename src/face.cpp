/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "face.hpp"

#include "security/signature-sha256-with-rsa.hpp"

#include "util/time.hpp"
#include "util/random.hpp"
#include <cstdlib>

#include "management/ndnd-controller.hpp"
#include "management/nfd-controller.hpp"

namespace ndn {

Face::Face()
{
  construct(shared_ptr<Transport>(new UnixTransport()),
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<boost::asio::io_service> &ioService)
{
  construct(shared_ptr<Transport>(new UnixTransport()),
            ioService);
}

Face::Face(const std::string &host, const std::string &port/* = "6363"*/)
{
  construct(shared_ptr<Transport>(new TcpTransport(host, port)),
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<Transport>& transport)
{
  construct(transport,
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<Transport>& transport,
           const shared_ptr<boost::asio::io_service> &ioService)
{
  construct(transport, ioService);
}

void
Face::construct(const shared_ptr<Transport>& transport,
                const shared_ptr<boost::asio::io_service> &ioService)
{
  m_pitTimeoutCheckTimerActive = false;
  m_transport = transport;
  m_ioService = ioService;

  m_pitTimeoutCheckTimer      = make_shared<boost::asio::deadline_timer>(boost::ref(*m_ioService));
  m_processEventsTimeoutTimer = make_shared<boost::asio::deadline_timer>(boost::ref(*m_ioService));

  if (std::getenv("NFD") != 0)
      m_fwController = make_shared<nfd::Controller>(boost::ref(*this));
  else
      m_fwController = make_shared<ndnd::Controller>(boost::ref(*this));
}


const PendingInterestId*
Face::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout)
{
  if (!m_transport->isConnected())
    m_transport->connect(*m_ioService,
                        bind(&Face::onReceiveElement, this, _1));

  shared_ptr<const Interest> interestToExpress(new Interest(interest));

  m_ioService->post(bind(&Face::asyncExpressInterest, this, interestToExpress, onData, onTimeout));

  return reinterpret_cast<const PendingInterestId*>(interestToExpress.get());
}

const PendingInterestId*
Face::expressInterest(const Name& name,
                      const Interest &tmpl,
                      const OnData& onData, const OnTimeout& onTimeout/* = OnTimeout()*/)
{
  return expressInterest(Interest(name,
                                  tmpl.getMinSuffixComponents(),
                                  tmpl.getMaxSuffixComponents(),
                                  tmpl.getExclude(),
                                  tmpl.getChildSelector(),
                                  tmpl.getMustBeFresh(),
                                  tmpl.getScope(),
                                  tmpl.getInterestLifetime()),
                         onData, onTimeout);
}

void
Face::asyncExpressInterest(const shared_ptr<const Interest> &interest,
                           const OnData& onData, const OnTimeout& onTimeout)
{
  m_pendingInterestTable.push_back(shared_ptr<PendingInterest>(new PendingInterest
                                                              (interest, onData, onTimeout)));

  m_transport->send(interest->wireEncode());

  if (!m_pitTimeoutCheckTimerActive) {
    m_pitTimeoutCheckTimerActive = true;
    m_pitTimeoutCheckTimer->expires_from_now(boost::posix_time::milliseconds(100));
    m_pitTimeoutCheckTimer->async_wait(bind(&Face::checkPitExpire, this));
  }
}

void
Face::put(const Data &data)
{
  if (!m_transport->isConnected())
    m_transport->connect(*m_ioService,
                        bind(&Face::onReceiveElement, this, _1));

  m_transport->send(data.wireEncode());
}


void
Face::removePendingInterest(const PendingInterestId *pendingInterestId)
{
  m_ioService->post(bind(&Face::asyncRemovePendingInterest, this, pendingInterestId));
}


void
Face::asyncRemovePendingInterest(const PendingInterestId *pendingInterestId)
{
  m_pendingInterestTable.remove_if(MatchPendingInterestId(pendingInterestId));
}

const RegisteredPrefixId*
Face::setInterestFilter(const Name& prefix,
                        const OnInterest& onInterest,
                        const OnSetInterestFilterFailed& onSetInterestFilterFailed)
{
  shared_ptr<RegisteredPrefix> prefixToRegister(new RegisteredPrefix(prefix, onInterest));

  m_fwController->selfRegisterPrefix(prefixToRegister->getPrefix(),
                                     bind(&RegisteredPrefixTable::push_back, &m_registeredPrefixTable, prefixToRegister),
                                     bind(onSetInterestFilterFailed, prefixToRegister->getPrefix(), _1));

  return reinterpret_cast<const RegisteredPrefixId*>(prefixToRegister.get());
}

void
Face::unsetInterestFilter(const RegisteredPrefixId *registeredPrefixId)
{
  m_ioService->post(bind(&Face::asyncUnsetInterestFilter, this, registeredPrefixId));
}

void
Face::asyncUnsetInterestFilter(const RegisteredPrefixId *registeredPrefixId)
{
  RegisteredPrefixTable::iterator i = std::find_if(m_registeredPrefixTable.begin(), m_registeredPrefixTable.end(),
                                                   MatchRegisteredPrefixId(registeredPrefixId));
  if (i != m_registeredPrefixTable.end())
    {
      m_fwController->selfDeregisterPrefix((*i)->getPrefix(),
                                           bind(&Face::finalizeUnsertInterestFilter, this, i),
                                           Controller::FailCallback());
    }

  // there cannot be two registered prefixes with the same id. if there are, then something is broken
}

void
Face::finalizeUnsertInterestFilter(RegisteredPrefixTable::iterator item)
{
  m_registeredPrefixTable.erase(item);

  if (!m_pitTimeoutCheckTimerActive && m_registeredPrefixTable.empty())
    {
      m_transport->close();
      if (!m_ioServiceWork) {
        m_processEventsTimeoutTimer->cancel();
      }
    }
}

void
Face::processEvents(Milliseconds timeout/* = 0 */, bool keepThread/* = false*/)
{
  try
    {
      if (timeout < 0)
        {
          // do not block if timeout is negative, but process pending events
          m_ioService->poll();
          return;
        }

      if (timeout > 0)
        {
          m_processEventsTimeoutTimer->expires_from_now(boost::posix_time::milliseconds(timeout));
          m_processEventsTimeoutTimer->async_wait(&fireProcessEventsTimeout);
        }

      if (keepThread) {
        // work will ensure that m_ioService is running until work object exists
        m_ioServiceWork = make_shared<boost::asio::io_service::work>(boost::ref(*m_ioService));
      }

      m_ioService->run();
      m_ioService->reset(); // so it is possible to run processEvents again (if necessary)
    }
  catch(Face::ProcessEventsTimeout &)
    {
      // break
      m_ioService->reset();
    }
  catch(const std::exception &)
    {
      m_ioService->reset();
      m_pendingInterestTable.clear();
      m_registeredPrefixTable.clear();
      throw;
    }
}

void
Face::shutdown()
{
  m_pendingInterestTable.clear();
  m_registeredPrefixTable.clear();

  m_transport->close();
  m_pitTimeoutCheckTimer->cancel();
  m_processEventsTimeoutTimer->cancel();
  m_pitTimeoutCheckTimerActive = false;
}

void
Face::fireProcessEventsTimeout(const boost::system::error_code& error)
{
  if (!error) // can fire for some other reason, e.g., cancelled
    throw Face::ProcessEventsTimeout();
}

void
Face::checkPitExpire()
{
  // Check for PIT entry timeouts.  Go backwards through the list so we can erase entries.
  MillisecondsSince1970 nowMilliseconds = getNowMilliseconds();

  PendingInterestTable::iterator i = m_pendingInterestTable.begin();
  while (i != m_pendingInterestTable.end())
    {
      if ((*i)->isTimedOut(nowMilliseconds))
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

    m_pitTimeoutCheckTimer->expires_from_now(boost::posix_time::milliseconds(100));
    m_pitTimeoutCheckTimer->async_wait(bind(&Face::checkPitExpire, this));
  }
  else {
    m_pitTimeoutCheckTimerActive = false;

    if (m_registeredPrefixTable.empty()) {
      m_transport->close();
      if (!m_ioServiceWork) {
        m_processEventsTimeoutTimer->cancel();
      }
    }
  }
}


void
Face::onReceiveElement(const Block &block)
{
  if (block.type() == Tlv::Interest)
    {
      shared_ptr<Interest> interest(new Interest());
      interest->wireDecode(block);

      RegisteredPrefixTable::iterator entry = getEntryForRegisteredPrefix(interest->getName());
      if (entry != m_registeredPrefixTable.end()) {
        (*entry)->getOnInterest()((*entry)->getPrefix(), *interest);
      }
    }
  else if (block.type() == Tlv::Data)
    {
      shared_ptr<Data> data(new Data());
      data->wireDecode(block);

      PendingInterestTable::iterator entry = getEntryIndexForExpressedInterest(data->getName());
      if (entry != m_pendingInterestTable.end()) {
        // Copy pointers to the needed objects and remove the PIT entry before the calling the callback.
        const OnData onData = (*entry)->getOnData();
        const shared_ptr<const Interest> interest = (*entry)->getInterest();
        m_pendingInterestTable.erase(entry);

        if (onData) {
          onData(*interest, *data);
        }

        if (m_pendingInterestTable.empty()) {
          m_pitTimeoutCheckTimer->cancel(); // this will cause checkPitExpire invocation
        }
      }
    }
}

Face::PendingInterestTable::iterator
Face::getEntryIndexForExpressedInterest(const Name& name)
{
  for (PendingInterestTable::iterator i = m_pendingInterestTable.begin ();
       i != m_pendingInterestTable.end(); ++i)
    {
      if ((*i)->getInterest()->matchesName(name))
        {
          return i;
        }
    }

  return m_pendingInterestTable.end();
}

Face::RegisteredPrefixTable::iterator
Face::getEntryForRegisteredPrefix(const Name& name)
{
  RegisteredPrefixTable::iterator longestPrefix = m_registeredPrefixTable.end();

  for (RegisteredPrefixTable::iterator i = m_registeredPrefixTable.begin();
       i != m_registeredPrefixTable.end();
       ++i)
    {
      if ((*i)->getPrefix().isPrefixOf(name))
        {

          if (longestPrefix == m_registeredPrefixTable.end() ||
              (*i)->getPrefix().size() > (*longestPrefix)->getPrefix().size())
            {
              longestPrefix = i;
            }
        }
    }
  return longestPrefix;
}

} // namespace ndn
