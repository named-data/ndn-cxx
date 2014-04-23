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
#include "util/config-file.hpp"
#include <cstdlib>

#include "management/ndnd-controller.hpp"
#include "management/nfd-controller.hpp"
#include "management/nrd-controller.hpp"

namespace ndn {

Face::Face()
{
  const std::string socketName = UnixTransport::getDefaultSocketName(m_config);
  construct(shared_ptr<Transport>(new UnixTransport(socketName)),
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<boost::asio::io_service>& ioService)
{
  const std::string socketName = UnixTransport::getDefaultSocketName(m_config);
  construct(shared_ptr<Transport>(new UnixTransport(socketName)),
            ioService);
}

class NullIoDeleter
{
public:
  void
  operator()(boost::asio::io_service*)
  {
  }
};

Face::Face(boost::asio::io_service& ioService)
{
  const std::string socketName = UnixTransport::getDefaultSocketName(m_config);
  construct(shared_ptr<Transport>(new UnixTransport(socketName)),
            shared_ptr<boost::asio::io_service>(&ioService, NullIoDeleter()));
}

Face::Face(const std::string& host, const std::string& port/* = "6363"*/)
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
           boost::asio::io_service& ioService)
{
  construct(transport,
            shared_ptr<boost::asio::io_service>(&ioService, NullIoDeleter()));
}

void
Face::setController(const shared_ptr<Controller>& controller)
{
  m_fwController = controller;
}

void
Face::construct(const shared_ptr<Transport>& transport,
                const shared_ptr<boost::asio::io_service>& ioService)
{
  m_pitTimeoutCheckTimerActive = false;
  m_transport = transport;
  m_ioService = ioService;

  m_pitTimeoutCheckTimer      = make_shared<monotonic_deadline_timer>(boost::ref(*m_ioService));
  m_processEventsTimeoutTimer = make_shared<monotonic_deadline_timer>(boost::ref(*m_ioService));

  std::string protocol = "nrd-0.1";

  try
    {
      protocol = m_config.getParsedConfiguration().get<std::string>("protocol");
    }
  catch (boost::property_tree::ptree_bad_path& error)
    {
      // protocol not specified
    }
  catch (boost::property_tree::ptree_bad_data& error)
    {
      throw ConfigFile::Error(error.what());
    }

  if (isSupportedNrdProtocol(protocol))
    {
      m_fwController = make_shared<nrd::Controller>(boost::ref(*this));
    }
  else if (isSupportedNfdProtocol(protocol))
    {
      m_fwController = make_shared<nfd::Controller>(boost::ref(*this));
    }
  else if (isSupportedNdndProtocol(protocol))
    {
      m_fwController = make_shared<ndnd::Controller>(boost::ref(*this));
    }
  else
    {
      throw Face::Error("Cannot create controller for unsupported protocol \"" + protocol + "\"");
    }
}

const PendingInterestId*
Face::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout)
{
  if (!m_transport->isConnected())
    m_transport->connect(*m_ioService,
                        bind(&Face::onReceiveElement, this, _1));

  shared_ptr<const Interest> interestToExpress(new Interest(interest));

  // If the same ioService thread, dispatch directly calls the method
  m_ioService->dispatch(bind(&Face::asyncExpressInterest, this,
                             interestToExpress, onData, onTimeout));

  return reinterpret_cast<const PendingInterestId*>(interestToExpress.get());
}

const PendingInterestId*
Face::expressInterest(const Name& name,
                      const Interest& tmpl,
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
Face::asyncExpressInterest(const shared_ptr<const Interest>& interest,
                           const OnData& onData, const OnTimeout& onTimeout)
{
  if (!m_transport->isExpectingData())
    m_transport->resume();

  m_pendingInterestTable.push_back(shared_ptr<PendingInterest>(new PendingInterest
                                                               (interest, onData, onTimeout)));

  if (!interest->getLocalControlHeader().empty(false, true))
    {
      // encode only NextHopFaceId towards the forwarder
      m_transport->send(interest->getLocalControlHeader().wireEncode(*interest, false, true),
                        interest->wireEncode());
    }
  else
    {
      m_transport->send(interest->wireEncode());
    }

  if (!m_pitTimeoutCheckTimerActive) {
    m_pitTimeoutCheckTimerActive = true;
    m_pitTimeoutCheckTimer->expires_from_now(time::milliseconds(100));
    m_pitTimeoutCheckTimer->async_wait(bind(&Face::checkPitExpire, this));
  }
}

void
Face::put(const Data& data)
{
  if (!m_transport->isConnected())
    m_transport->connect(*m_ioService,
                         bind(&Face::onReceiveElement, this, _1));

  if (!data.getLocalControlHeader().empty(false, true))
    {
      m_transport->send(data.getLocalControlHeader().wireEncode(data, false, true),
                        data.wireEncode());
    }
  else
    {
      m_transport->send(data.wireEncode());
    }
}

void
Face::removePendingInterest(const PendingInterestId* pendingInterestId)
{
  m_ioService->post(bind(&Face::asyncRemovePendingInterest, this, pendingInterestId));
}


void
Face::asyncRemovePendingInterest(const PendingInterestId* pendingInterestId)
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
                                     bind(&RegisteredPrefixTable::push_back,
                                          &m_registeredPrefixTable, prefixToRegister),
                                     bind(onSetInterestFilterFailed,
                                          prefixToRegister->getPrefix(), _1));

  return reinterpret_cast<const RegisteredPrefixId*>(prefixToRegister.get());
}

void
Face::unsetInterestFilter(const RegisteredPrefixId* registeredPrefixId)
{
  m_ioService->post(bind(&Face::asyncUnsetInterestFilter, this, registeredPrefixId));
}

void
Face::asyncUnsetInterestFilter(const RegisteredPrefixId* registeredPrefixId)
{
  RegisteredPrefixTable::iterator i = std::find_if(m_registeredPrefixTable.begin(),
                                                   m_registeredPrefixTable.end(),
                                                   MatchRegisteredPrefixId(registeredPrefixId));
  if (i != m_registeredPrefixTable.end())
    {
      m_fwController->selfDeregisterPrefix((*i)->getPrefix(),
                                           bind(&Face::finalizeUnsetInterestFilter, this, i),
                                           Controller::FailCallback());
    }

  // there cannot be two registered prefixes with the same id
}

void
Face::finalizeUnsetInterestFilter(RegisteredPrefixTable::iterator item)
{
  m_registeredPrefixTable.erase(item);

  if (!m_pitTimeoutCheckTimerActive && m_registeredPrefixTable.empty())
    {
      m_transport->pause();
      if (!m_ioServiceWork) {
        m_processEventsTimeoutTimer->cancel();
      }
    }
}

void
Face::processEvents(const time::milliseconds& timeout/* = time::milliseconds::zero()*/,
                    bool keepThread/* = false*/)
{
  try
    {
      if (timeout < time::milliseconds::zero())
        {
          // do not block if timeout is negative, but process pending events
          m_ioService->poll();
          return;
        }

      if (timeout > time::milliseconds::zero())
        {
          m_processEventsTimeoutTimer->expires_from_now(time::milliseconds(timeout));
          m_processEventsTimeoutTimer->async_wait(&fireProcessEventsTimeout);
        }

      if (keepThread) {
        // work will ensure that m_ioService is running until work object exists
        m_ioServiceWork = make_shared<boost::asio::io_service::work>(boost::ref(*m_ioService));
      }

      m_ioService->run();
      m_ioService->reset(); // so it is possible to run processEvents again (if necessary)
    }
  catch (Face::ProcessEventsTimeout&)
    {
      // break
      m_ioService->reset();
    }
  catch (std::exception&)
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
  m_ioService->post(bind(&Face::asyncShutdown, this));
}

void
Face::asyncShutdown()
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
    m_pitTimeoutCheckTimer->async_wait(bind(&Face::checkPitExpire, this));
  }
  else {
    m_pitTimeoutCheckTimerActive = false;

    if (m_registeredPrefixTable.empty()) {
      m_transport->pause();
      if (!m_ioServiceWork) {
        m_processEventsTimeoutTimer->cancel();
      }
    }
  }
}


void
Face::onReceiveElement(const Block& blockFromDaemon)
{
  const Block& block = nfd::LocalControlHeader::getPayload(blockFromDaemon);

  if (block.type() == Tlv::Interest)
    {
      shared_ptr<Interest> interest(new Interest());
      interest->wireDecode(block);
      if (&block != &blockFromDaemon)
        interest->getLocalControlHeader().wireDecode(blockFromDaemon);

      processInterestFilters(*interest);
    }
  else if (block.type() == Tlv::Data)
    {
      shared_ptr<Data> data(new Data());
      data->wireDecode(block);
      if (&block != &blockFromDaemon)
        data->getLocalControlHeader().wireDecode(blockFromDaemon);

      satisfyPendingInterests(*data);

      if (m_pendingInterestTable.empty()) {
        m_pitTimeoutCheckTimer->cancel(); // this will cause checkPitExpire invocation
      }
    }
  // ignore any other type
}

void
Face::satisfyPendingInterests(Data& data)
{
  for (PendingInterestTable::iterator i = m_pendingInterestTable.begin ();
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
Face::processInterestFilters(Interest& interest)
{
  for (RegisteredPrefixTable::iterator i = m_registeredPrefixTable.begin();
       i != m_registeredPrefixTable.end();
       ++i)
    {
      if ((*i)->getPrefix().isPrefixOf(interest.getName()))
        {
          (*i)->getOnInterest()((*i)->getPrefix(), interest);
        }
    }
}

} // namespace ndn
