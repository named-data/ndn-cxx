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

#include "encoding/tlv.hpp"
#include "security/key-chain.hpp"
#include "util/time.hpp"
#include "util/random.hpp"
#include "util/face-uri.hpp"

namespace ndn {

Face::Face()
  : m_internalIoService(new boost::asio::io_service())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(new KeyChain())
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(new Impl(*this))
{
  construct(m_internalKeyChain);
}

Face::Face(boost::asio::io_service& ioService)
  : m_ioService(ioService)
  , m_internalKeyChain(new KeyChain())
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(new Impl(*this))
{
  construct(m_internalKeyChain);
}

Face::Face(const std::string& host, const std::string& port/* = "6363"*/)
  : m_internalIoService(new boost::asio::io_service())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(new KeyChain())
  , m_impl(new Impl(*this))
{
  construct(make_shared<TcpTransport>(host, port),
            m_internalKeyChain);
}

Face::Face(const shared_ptr<Transport>& transport)
  : m_internalIoService(new boost::asio::io_service())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(new KeyChain())
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(new Impl(*this))
{
  construct(transport,
            m_internalKeyChain);
}

Face::Face(const shared_ptr<Transport>& transport,
           boost::asio::io_service& ioService)
  : m_ioService(ioService)
  , m_internalKeyChain(new KeyChain())
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(new Impl(*this))
{
  construct(transport,
            m_internalKeyChain);
}

Face::Face(shared_ptr<Transport> transport,
           boost::asio::io_service& ioService,
           KeyChain& keyChain)
  : m_ioService(ioService)
  , m_internalKeyChain(nullptr)
  , m_isDirectNfdFibManagementRequested(false)
  , m_impl(new Impl(*this))
{
  construct(transport,
            &keyChain);
}

void
Face::construct(KeyChain* keyChain)
{
  // transport=unix:///var/run/nfd.sock
  // transport=tcp://localhost:6363

  const ConfigFile::Parsed& parsed = m_impl->m_config.getParsedConfiguration();

  const auto transportType = parsed.get_optional<std::string>("transport");
  if (!transportType)
    {
      // transport not specified, use default Unix transport.
      construct(UnixTransport::create(m_impl->m_config), keyChain);
      return;
    }

  unique_ptr<util::FaceUri> uri;
  try
    {
      uri.reset(new util::FaceUri(*transportType));
    }
  catch (const util::FaceUri::Error& error)
    {
      throw ConfigFile::Error(error.what());
    }

  shared_ptr<Transport> transport;
  const std::string protocol = uri->getScheme();

  if (protocol == "unix")
    {
      construct(UnixTransport::create(m_impl->m_config), keyChain);

    }
  else if (protocol == "tcp" || protocol == "tcp4" || protocol == "tcp6")
    {
      construct(TcpTransport::create(m_impl->m_config), keyChain);
    }
  else
    {
      throw ConfigFile::Error("Unsupported transport protocol \"" + protocol + "\"");
    }
}

void
Face::construct(shared_ptr<Transport> transport,
                KeyChain* keyChain)
{
  m_nfdController = new nfd::Controller(*this, *keyChain);

  m_impl->m_pitTimeoutCheckTimerActive = false;
  m_transport = transport;

  m_impl->m_pitTimeoutCheckTimer      = make_shared<monotonic_deadline_timer>(ref(m_ioService));
  m_impl->m_processEventsTimeoutTimer = make_shared<monotonic_deadline_timer>(ref(m_ioService));
  m_impl->ensureConnected(false);

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

Face::~Face()
{
  if (m_internalKeyChain != nullptr) {
    delete m_internalKeyChain;
  }

  delete m_nfdController;
  delete m_impl;
}

const PendingInterestId*
Face::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout)
{
  shared_ptr<Interest> interestToExpress = make_shared<Interest>(interest);

  // Use `interestToExpress` to avoid wire format creation for the original Interest
  if (interestToExpress->wireEncode().size() > MAX_NDN_PACKET_SIZE)
    throw Error("Interest size exceeds maximum limit");

  // If the same ioService thread, dispatch directly calls the method
  m_ioService.dispatch(bind(&Impl::asyncExpressInterest, m_impl,
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
  // Use original `data`, since wire format should already exist for the original Data
  if (data.wireEncode().size() > MAX_NDN_PACKET_SIZE)
    throw Error("Data size exceeds maximum limit");

  shared_ptr<const Data> dataPtr;
  try {
    dataPtr = data.shared_from_this();
  }
  catch (const bad_weak_ptr& e) {
    std::cerr << "Face::put WARNING: the supplied Data should be created using make_shared<Data>()"
              << std::endl;
    dataPtr = make_shared<Data>(data);
  }

  // If the same ioService thread, dispatch directly calls the method
  m_ioService.dispatch(bind(&Impl::asyncPutData, m_impl, dataPtr));
}

void
Face::removePendingInterest(const PendingInterestId* pendingInterestId)
{
  m_ioService.post(bind(&Impl::asyncRemovePendingInterest, m_impl, pendingInterestId));
}

size_t
Face::getNPendingInterests() const
{
  return m_impl->m_pendingInterestTable.size();
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const IdentityCertificate& certificate,
                        uint64_t flags)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  nfd::CommandOptions options;
  if (certificate.getName().empty()) {
    options.setSigningDefault();
  }
  else {
    options.setSigningCertificate(certificate);
  }

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                onSuccess, onFailure,
                                flags, options);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const IdentityCertificate& certificate,
                        uint64_t flags)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  nfd::CommandOptions options;
  if (certificate.getName().empty()) {
    options.setSigningDefault();
  }
  else {
    options.setSigningCertificate(certificate);
  }

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                RegisterPrefixSuccessCallback(), onFailure,
                                flags, options);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const Name& identity,
                        uint64_t flags)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  nfd::CommandOptions options;
  options.setSigningIdentity(identity);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                onSuccess, onFailure,
                                flags, options);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const Name& identity,
                        uint64_t flags)
{
  shared_ptr<InterestFilterRecord> filter =
    make_shared<InterestFilterRecord>(interestFilter, onInterest);

  nfd::CommandOptions options;
  options.setSigningIdentity(identity);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                RegisterPrefixSuccessCallback(), onFailure,
                                flags, options);
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
                     const IdentityCertificate& certificate,
                     uint64_t flags)
{
  nfd::CommandOptions options;
  if (certificate.getName().empty()) {
    options.setSigningDefault();
  }
  else {
    options.setSigningCertificate(certificate);
  }

  return m_impl->registerPrefix(prefix, shared_ptr<InterestFilterRecord>(),
                                onSuccess, onFailure,
                                flags, options);
}

const RegisteredPrefixId*
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const Name& identity,
                     uint64_t flags)
{
  nfd::CommandOptions options;
  options.setSigningIdentity(identity);

  return m_impl->registerPrefix(prefix, shared_ptr<InterestFilterRecord>(),
                                onSuccess, onFailure,
                                flags, options);
}

void
Face::unsetInterestFilter(const RegisteredPrefixId* registeredPrefixId)
{
  m_ioService.post(bind(&Impl::asyncUnregisterPrefix, m_impl, registeredPrefixId,
                        UnregisterPrefixSuccessCallback(), UnregisterPrefixFailureCallback()));
}

void
Face::unsetInterestFilter(const InterestFilterId* interestFilterId)
{
  m_ioService.post(bind(&Impl::asyncUnsetInterestFilter, m_impl, interestFilterId));
}

void
Face::unregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                       const UnregisterPrefixSuccessCallback& onSuccess,
                       const UnregisterPrefixFailureCallback& onFailure)
{
  m_ioService.post(bind(&Impl::asyncUnregisterPrefix, m_impl, registeredPrefixId,
                        onSuccess, onFailure));
}

void
Face::processEvents(const time::milliseconds& timeout/* = time::milliseconds::zero()*/,
                    bool keepThread/* = false*/)
{
  if (m_ioService.stopped()) {
    m_ioService.reset(); // ensure that run()/poll() will do some work
  }

  try {
    if (timeout < time::milliseconds::zero())
      {
        // do not block if timeout is negative, but process pending events
        m_ioService.poll();
        return;
      }

    if (timeout > time::milliseconds::zero())
      {
        m_impl->m_processEventsTimeoutTimer->expires_from_now(time::milliseconds(timeout));
        m_impl->m_processEventsTimeoutTimer->async_wait(&fireProcessEventsTimeout);
      }

    if (keepThread) {
      // work will ensure that m_ioService is running until work object exists
      m_impl->m_ioServiceWork = make_shared<boost::asio::io_service::work>(ref(m_ioService));
    }

    m_ioService.run();
  }
  catch (Face::ProcessEventsTimeout&) {
    // break
    m_impl->m_ioServiceWork.reset();
  }
  catch (...) {
    m_impl->m_ioServiceWork.reset();
    m_impl->m_pendingInterestTable.clear();
    m_impl->m_registeredPrefixTable.clear();
    throw;
  }
}

void
Face::shutdown()
{
  m_ioService.post(bind(&Face::asyncShutdown, this));
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

  m_impl->m_ioServiceWork.reset();
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

  if (block.type() == tlv::Interest)
    {
      shared_ptr<Interest> interest = make_shared<Interest>(block);
      if (&block != &blockFromDaemon)
        interest->getLocalControlHeader().wireDecode(blockFromDaemon);

      m_impl->processInterestFilters(*interest);
    }
  else if (block.type() == tlv::Data)
    {
      shared_ptr<Data> data = make_shared<Data>(block);
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
