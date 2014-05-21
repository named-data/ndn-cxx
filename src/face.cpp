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
 *
 * Based on code originally written by Jeff Thompson <jefft0@remap.ucla.edu>
 */

#include "face.hpp"
#include "detail/face-impl.hpp"

#include "interest.hpp"
#include "data.hpp"
#include "security/identity-certificate.hpp"

#include "util/time.hpp"
#include "util/random.hpp"

namespace ndn {

Face::Face()
  : m_nfdController(new nfd::Controller(*this))
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(make_shared<Impl>(ref(*this)))
{
  const std::string socketName = UnixTransport::getDefaultSocketName(m_impl->m_config);
  construct(make_shared<UnixTransport>(socketName),
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<boost::asio::io_service>& ioService)
  : m_nfdController(new nfd::Controller(*this))
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(make_shared<Impl>(ref(*this)))
{
  const std::string socketName = UnixTransport::getDefaultSocketName(m_impl->m_config);
  construct(make_shared<UnixTransport>(socketName),
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
  : m_nfdController(make_shared<nfd::Controller>(ref(*this)))
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(make_shared<Impl>(ref(*this)))
{
  const std::string socketName = UnixTransport::getDefaultSocketName(m_impl->m_config);
  construct(make_shared<UnixTransport>(socketName),
            shared_ptr<boost::asio::io_service>(&ioService, NullIoDeleter()));
}

Face::Face(const std::string& host, const std::string& port/* = "6363"*/)
  : m_nfdController(make_shared<nfd::Controller>(ref(*this)))
  , m_impl(make_shared<Impl>(ref(*this)))
{
  construct(make_shared<TcpTransport>(host, port),
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<Transport>& transport)
  : m_nfdController(make_shared<nfd::Controller>(ref(*this)))
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(make_shared<Impl>(ref(*this)))
{
  construct(transport,
            make_shared<boost::asio::io_service>());
}

Face::Face(const shared_ptr<Transport>& transport,
           boost::asio::io_service& ioService)
  : m_nfdController(make_shared<nfd::Controller>(ref(*this)))
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(make_shared<Impl>(ref(*this)))
{
  construct(transport,
            shared_ptr<boost::asio::io_service>(&ioService, NullIoDeleter()));
}

void
Face::construct(const shared_ptr<Transport>& transport,
                const shared_ptr<boost::asio::io_service>& ioService)
{
  m_impl->m_pitTimeoutCheckTimerActive = false;
  m_transport = transport;
  m_ioService = ioService;

  m_impl->m_pitTimeoutCheckTimer      = make_shared<monotonic_deadline_timer>(ref(*m_ioService));
  m_impl->m_processEventsTimeoutTimer = make_shared<monotonic_deadline_timer>(ref(*m_ioService));

  std::string protocol = "nrd-0.1";

  try
    {
      protocol = m_impl->m_config.getParsedConfiguration().get<std::string>("protocol");
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
      // do nothing
    }
  else if (isSupportedNfdProtocol(protocol))
    {
      m_isDirectNfdFibManagementRequested = true;
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

  shared_ptr<Interest> interestToExpress = make_shared<Interest>(interest);

  // If the same ioService thread, dispatch directly calls the method
  m_ioService->dispatch(bind(&Impl::asyncExpressInterest, m_impl,
                             interestToExpress, onData, onTimeout));

  return reinterpret_cast<const PendingInterestId*>(interestToExpress.get());
}

const PendingInterestId*
Face::expressInterest(const Name& name,
                      const Interest& tmpl,
                      const OnData& onData, const OnTimeout& onTimeout/* = OnTimeout()*/)
{
  return expressInterest(Interest(tmpl)
                           .setName(name)
                           .setNonce(0),
                         onData, onTimeout);
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
  m_ioService->post(bind(&Impl::asyncRemovePendingInterest, m_impl, pendingInterestId));
}



const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const IdentityCertificate& certificate)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                onSuccess, onFailure,
                                certificate);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const IdentityCertificate& certificate)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                RegisterPrefixSuccessCallback(), onFailure,
                                certificate);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const Name& identity)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                onSuccess, onFailure,
                                identity);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const Name& identity)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                RegisterPrefixSuccessCallback(), onFailure,
                                identity);
}


const InterestFilterId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  getIoService().post(bind(&Impl::asyncSetInterestFilter, m_impl, filter));

  return reinterpret_cast<const InterestFilterId*>(filter.get());
}

const RegisteredPrefixId*
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const IdentityCertificate& certificate)
{
  return m_impl->registerPrefix(prefix, shared_ptr<InterestFilterRecord>(),
                                onSuccess, onFailure,
                                certificate);
}

const RegisteredPrefixId*
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const Name& identity)
{
  return m_impl->registerPrefix(prefix, shared_ptr<InterestFilterRecord>(),
                                onSuccess, onFailure,
                                identity);
}


void
Face::unsetInterestFilter(const RegisteredPrefixId* registeredPrefixId)
{
  m_ioService->post(bind(&Impl::asyncUnregisterPrefix, m_impl, registeredPrefixId,
                         UnregisterPrefixSuccessCallback(), UnregisterPrefixFailureCallback()));
}

void
Face::unsetInterestFilter(const InterestFilterId* interestFilterId)
{
  m_ioService->post(bind(&Impl::asyncUnsetInterestFilter, m_impl, interestFilterId));
}

void
Face::unregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                       const UnregisterPrefixSuccessCallback& onSuccess,
                       const UnregisterPrefixFailureCallback& onFailure)
{
  m_ioService->post(bind(&Impl::asyncUnregisterPrefix, m_impl, registeredPrefixId,
                         onSuccess, onFailure));
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
          m_impl->m_processEventsTimeoutTimer->expires_from_now(time::milliseconds(timeout));
          m_impl->m_processEventsTimeoutTimer->async_wait(&fireProcessEventsTimeout);
        }

      if (keepThread) {
        // work will ensure that m_ioService is running until work object exists
        m_impl->m_ioServiceWork = make_shared<boost::asio::io_service::work>(ref(*m_ioService));
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
      m_impl->m_pendingInterestTable.clear();
      m_impl->m_registeredPrefixTable.clear();
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
  m_impl->m_pendingInterestTable.clear();
  m_impl->m_registeredPrefixTable.clear();

  if (m_transport->isConnected())
    m_transport->close();

  m_impl->m_pitTimeoutCheckTimer->cancel();
  m_impl->m_processEventsTimeoutTimer->cancel();
  m_impl->m_pitTimeoutCheckTimerActive = false;
}

void
Face::fireProcessEventsTimeout(const boost::system::error_code& error)
{
  if (!error) // can fire for some other reason, e.g., cancelled
    throw Face::ProcessEventsTimeout();
}


void
Face::onReceiveElement(const Block& blockFromDaemon)
{
  const Block& block = nfd::LocalControlHeader::getPayload(blockFromDaemon);

  if (block.type() == Tlv::Interest)
    {
      shared_ptr<Interest> interest = make_shared<Interest>();
      interest->wireDecode(block);
      if (&block != &blockFromDaemon)
        interest->getLocalControlHeader().wireDecode(blockFromDaemon);

      m_impl->processInterestFilters(*interest);
    }
  else if (block.type() == Tlv::Data)
    {
      shared_ptr<Data> data = make_shared<Data>();
      data->wireDecode(block);
      if (&block != &blockFromDaemon)
        data->getLocalControlHeader().wireDecode(blockFromDaemon);

      m_impl->satisfyPendingInterests(*data);

      if (m_impl->m_pendingInterestTable.empty()) {
        m_impl->m_pitTimeoutCheckTimer->cancel(); // this will cause checkPitExpire invocation
      }
    }
  // ignore any other type
}



} // namespace ndn
