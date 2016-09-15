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

#include "face.hpp"
#include "detail/face-impl.hpp"

#include "encoding/tlv.hpp"
#include "security/key-chain.hpp"
#include "security/signing-helpers.hpp"
#include "util/time.hpp"
#include "util/random.hpp"
#include "util/face-uri.hpp"

// A callback scheduled through io.post and io.dispatch may be invoked after the face
// is destructed. To prevent this situation, these macros captures Face::m_impl as weak_ptr,
// and skips callback execution if the face has been destructed.
#define IO_CAPTURE_WEAK_IMPL(OP) \
  { \
    weak_ptr<Impl> implWeak(m_impl); \
    m_ioService.OP([=] { \
      auto impl = implWeak.lock(); \
      if (impl != nullptr) {
#define IO_CAPTURE_WEAK_IMPL_END \
      } \
    }); \
  }

namespace ndn {

Face::Face(shared_ptr<Transport> transport)
  : m_internalIoService(new boost::asio::io_service())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(new KeyChain())
  , m_impl(make_shared<Impl>(*this))
{
  construct(transport, *m_internalKeyChain);
}

Face::Face(boost::asio::io_service& ioService)
  : m_ioService(ioService)
  , m_internalKeyChain(new KeyChain())
  , m_impl(make_shared<Impl>(*this))
{
  construct(nullptr, *m_internalKeyChain);
}

Face::Face(const std::string& host, const std::string& port)
  : m_internalIoService(new boost::asio::io_service())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(new KeyChain())
  , m_impl(make_shared<Impl>(*this))
{
  construct(make_shared<TcpTransport>(host, port), *m_internalKeyChain);
}

Face::Face(shared_ptr<Transport> transport, KeyChain& keyChain)
  : m_internalIoService(new boost::asio::io_service())
  , m_ioService(*m_internalIoService)
  , m_impl(make_shared<Impl>(*this))
{
  construct(transport, keyChain);
}

Face::Face(shared_ptr<Transport> transport, boost::asio::io_service& ioService)
  : m_ioService(ioService)
  , m_internalKeyChain(new KeyChain())
  , m_impl(make_shared<Impl>(*this))
{
  construct(transport, *m_internalKeyChain);
}

Face::Face(shared_ptr<Transport> transport, boost::asio::io_service& ioService, KeyChain& keyChain)
  : m_ioService(ioService)
  , m_impl(make_shared<Impl>(*this))
{
  construct(transport, keyChain);
}

shared_ptr<Transport>
Face::makeDefaultTransport()
{
  // transport=unix:///var/run/nfd.sock
  // transport=tcp://localhost:6363

  std::string transportUri;

  const char* transportEnviron = getenv("NDN_CLIENT_TRANSPORT");
  if (transportEnviron != nullptr) {
    transportUri = transportEnviron;
  }
  else {
    ConfigFile config;
    transportUri = config.getParsedConfiguration().get<std::string>("transport", "");
  }

  if (transportUri.empty()) {
    // transport not specified, use default Unix transport.
    return UnixTransport::create("");
  }

  std::string protocol;
  try {
    util::FaceUri uri(transportUri);
    protocol = uri.getScheme();

    if (protocol == "unix") {
      return UnixTransport::create(transportUri);
    }
    else if (protocol == "tcp" || protocol == "tcp4" || protocol == "tcp6") {
      return TcpTransport::create(transportUri);
    }
    else {
      BOOST_THROW_EXCEPTION(ConfigFile::Error("Unsupported transport protocol \"" + protocol + "\""));
    }
  }
  catch (const Transport::Error& error) {
    BOOST_THROW_EXCEPTION(ConfigFile::Error(error.what()));
  }
  catch (const util::FaceUri::Error& error) {
    BOOST_THROW_EXCEPTION(ConfigFile::Error(error.what()));
  }
}

void
Face::construct(shared_ptr<Transport> transport, KeyChain& keyChain)
{
  if (transport == nullptr) {
    transport = makeDefaultTransport();
  }
  BOOST_ASSERT(transport != nullptr);
  m_transport = transport;

  m_nfdController.reset(new nfd::Controller(*this, keyChain));

  IO_CAPTURE_WEAK_IMPL(post) {
    impl->ensureConnected(false);
  } IO_CAPTURE_WEAK_IMPL_END
}

Face::~Face() = default;

shared_ptr<Transport>
Face::getTransport()
{
  return m_transport;
}

const PendingInterestId*
Face::expressInterest(const Interest& interest,
                      const DataCallback& afterSatisfied,
                      const NackCallback& afterNacked,
                      const TimeoutCallback& afterTimeout)
{
  shared_ptr<Interest> interestToExpress = make_shared<Interest>(interest);

  // Use `interestToExpress` to avoid wire format creation for the original Interest
  if (interestToExpress->wireEncode().size() > MAX_NDN_PACKET_SIZE) {
    BOOST_THROW_EXCEPTION(Error("Interest size exceeds maximum limit"));
  }

  // If the same ioService thread, dispatch directly calls the method
  IO_CAPTURE_WEAK_IMPL(dispatch) {
    impl->asyncExpressInterest(interestToExpress, afterSatisfied, afterNacked, afterTimeout);
  } IO_CAPTURE_WEAK_IMPL_END

  return reinterpret_cast<const PendingInterestId*>(interestToExpress.get());
}

const PendingInterestId*
Face::expressInterest(const Interest& interest,
                      const OnData& onData,
                      const OnTimeout& onTimeout)
{
  return this->expressInterest(
    interest,
    [onData] (const Interest& interest, const Data& data) {
      if (onData != nullptr) {
        onData(interest, const_cast<Data&>(data));
      }
    },
    [onTimeout] (const Interest& interest, const lp::Nack& nack) {
      if (onTimeout != nullptr) {
        onTimeout(interest);
      }
    },
    onTimeout
  );
}

const PendingInterestId*
Face::expressInterest(const Name& name, const Interest& tmpl,
                      const OnData& onData, const OnTimeout& onTimeout)
{
  return expressInterest(Interest(tmpl).setName(name).setNonce(0),
                         onData, onTimeout);
}

void
Face::removePendingInterest(const PendingInterestId* pendingInterestId)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncRemovePendingInterest(pendingInterestId);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::removeAllPendingInterests()
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncRemoveAllPendingInterests();
  } IO_CAPTURE_WEAK_IMPL_END
}

size_t
Face::getNPendingInterests() const
{
  return m_impl->m_pendingInterestTable.size();
}

void
Face::put(const Data& data)
{
  Block wire = data.wireEncode();

  shared_ptr<lp::CachePolicyTag> cachePolicyTag = data.getTag<lp::CachePolicyTag>();
  if (cachePolicyTag != nullptr) {
    lp::Packet packet;
    packet.add<lp::CachePolicyField>(*cachePolicyTag);
    packet.add<lp::FragmentField>(std::make_pair(wire.begin(), wire.end()));
    wire = packet.wireEncode();
  }

  if (wire.size() > MAX_NDN_PACKET_SIZE)
    BOOST_THROW_EXCEPTION(Error("Data size exceeds maximum limit"));

  IO_CAPTURE_WEAK_IMPL(dispatch) {
    impl->asyncSend(wire);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::put(const lp::Nack& nack)
{
  lp::Packet packet;
  packet.add<lp::NackField>(nack.getHeader());
  const Block& interestWire = nack.getInterest().wireEncode();
  packet.add<lp::FragmentField>(std::make_pair(interestWire.begin(), interestWire.end()));

  Block wire = packet.wireEncode();

  if (wire.size() > MAX_NDN_PACKET_SIZE)
    BOOST_THROW_EXCEPTION(Error("Nack size exceeds maximum limit"));

  IO_CAPTURE_WEAK_IMPL(dispatch) {
    impl->asyncSend(wire);
  } IO_CAPTURE_WEAK_IMPL_END
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const InterestCallback& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const security::SigningInfo& signingInfo,
                        uint64_t flags)
{
  return setInterestFilter(interestFilter, onInterest, nullptr, onFailure, signingInfo, flags);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const InterestCallback& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const security::SigningInfo& signingInfo,
                        uint64_t flags)
{
  auto filter = make_shared<InterestFilterRecord>(interestFilter, onInterest);

  nfd::CommandOptions options;
  options.setSigningInfo(signingInfo);

  return m_impl->registerPrefix(interestFilter.getPrefix(), filter,
                                onSuccess, onFailure, flags, options);
}

const InterestFilterId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const InterestCallback& onInterest)
{
  auto filter = make_shared<InterestFilterRecord>(interestFilter, onInterest);

  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncSetInterestFilter(filter);
  } IO_CAPTURE_WEAK_IMPL_END

  return reinterpret_cast<const InterestFilterId*>(filter.get());
}

#ifdef NDN_FACE_KEEP_DEPRECATED_REGISTRATION_SIGNING

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const security::v1::IdentityCertificate& certificate,
                        uint64_t flags)
{
  security::SigningInfo signingInfo;
  if (!certificate.getName().empty()) {
    signingInfo = signingByCertificate(certificate.getName());
  }
  return setInterestFilter(interestFilter, onInterest, onSuccess, onFailure, signingInfo, flags);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const security::v1::IdentityCertificate& certificate,
                        uint64_t flags)
{
  security::SigningInfo signingInfo;
  if (!certificate.getName().empty()) {
    signingInfo = signingByCertificate(certificate.getName());
  }
  return setInterestFilter(interestFilter, onInterest, onFailure, signingInfo, flags);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const Name& identity,
                        uint64_t flags)
{
  security::SigningInfo signingInfo = signingByIdentity(identity);
  return setInterestFilter(interestFilter, onInterest,
                           onSuccess, onFailure,
                           signingInfo, flags);
}

const RegisteredPrefixId*
Face::setInterestFilter(const InterestFilter& interestFilter,
                        const OnInterest& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const Name& identity,
                        uint64_t flags)
{
  security::SigningInfo signingInfo = signingByIdentity(identity);
  return setInterestFilter(interestFilter, onInterest, onFailure, signingInfo, flags);
}

#endif // NDN_FACE_KEEP_DEPRECATED_REGISTRATION_SIGNING

const RegisteredPrefixId*
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const security::SigningInfo& signingInfo,
                     uint64_t flags)
{
  nfd::CommandOptions options;
  options.setSigningInfo(signingInfo);

  return m_impl->registerPrefix(prefix, nullptr, onSuccess, onFailure, flags, options);
}

#ifdef NDN_FACE_KEEP_DEPRECATED_REGISTRATION_SIGNING
const RegisteredPrefixId*
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const security::v1::IdentityCertificate& certificate,
                     uint64_t flags)
{
  security::SigningInfo signingInfo;
  if (!certificate.getName().empty()) {
    signingInfo = signingByCertificate(certificate.getName());
  }
  return registerPrefix(prefix, onSuccess, onFailure, signingInfo, flags);
}

const RegisteredPrefixId*
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const Name& identity,
                     uint64_t flags)
{
  security::SigningInfo signingInfo = signingByIdentity(identity);
  return registerPrefix(prefix, onSuccess, onFailure, signingInfo, flags);
}
#endif // NDN_FACE_KEEP_DEPRECATED_REGISTRATION_SIGNING

void
Face::unsetInterestFilter(const RegisteredPrefixId* registeredPrefixId)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncUnregisterPrefix(registeredPrefixId, nullptr, nullptr);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::unsetInterestFilter(const InterestFilterId* interestFilterId)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncUnsetInterestFilter(interestFilterId);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::unregisterPrefix(const RegisteredPrefixId* registeredPrefixId,
                       const UnregisterPrefixSuccessCallback& onSuccess,
                       const UnregisterPrefixFailureCallback& onFailure)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncUnregisterPrefix(registeredPrefixId, onSuccess, onFailure);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::doProcessEvents(const time::milliseconds& timeout, bool keepThread)
{
  if (m_ioService.stopped()) {
    m_ioService.reset(); // ensure that run()/poll() will do some work
  }

  try {
    if (timeout < time::milliseconds::zero()) {
      // do not block if timeout is negative, but process pending events
      m_ioService.poll();
      return;
    }

    if (timeout > time::milliseconds::zero()) {
      boost::asio::io_service& ioService = m_ioService;
      unique_ptr<boost::asio::io_service::work>& work = m_impl->m_ioServiceWork;
      m_impl->m_processEventsTimeoutEvent = m_impl->m_scheduler.scheduleEvent(timeout,
        [&ioService, &work] {
          ioService.stop();
          work.reset();
        });
    }

    if (keepThread) {
      // work will ensure that m_ioService is running until work object exists
      m_impl->m_ioServiceWork.reset(new boost::asio::io_service::work(m_ioService));
    }

    m_ioService.run();
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
  IO_CAPTURE_WEAK_IMPL(post) {
    this->asyncShutdown();
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::asyncShutdown()
{
  m_impl->m_pendingInterestTable.clear();
  m_impl->m_registeredPrefixTable.clear();

  if (m_transport->isConnected())
    m_transport->close();

  m_impl->m_ioServiceWork.reset();
}

/**
 * @brief extract local fields from NDNLPv2 packet and tag onto a network layer packet
 */
template<typename NETPKT>
static void
extractLpLocalFields(NETPKT& netPacket, const lp::Packet& lpPacket)
{
  if (lpPacket.has<lp::IncomingFaceIdField>()) {
    netPacket.setTag(make_shared<lp::IncomingFaceIdTag>(lpPacket.get<lp::IncomingFaceIdField>()));
  }
}

void
Face::onReceiveElement(const Block& blockFromDaemon)
{
  lp::Packet lpPacket(blockFromDaemon); // bare Interest/Data is a valid lp::Packet,
                                        // no need to distinguish

  Buffer::const_iterator begin, end;
  std::tie(begin, end) = lpPacket.get<lp::FragmentField>();
  Block netPacket(&*begin, std::distance(begin, end));
  switch (netPacket.type()) {
    case tlv::Interest: {
      auto interest = make_shared<Interest>(netPacket);
      if (lpPacket.has<lp::NackField>()) {
        auto nack = make_shared<lp::Nack>(std::move(*interest));
        nack->setHeader(lpPacket.get<lp::NackField>());
        extractLpLocalFields(*nack, lpPacket);
        m_impl->nackPendingInterests(*nack);
      }
      else {
        extractLpLocalFields(*interest, lpPacket);
        m_impl->processInterestFilters(*interest);
      }
      break;
    }
    case tlv::Data: {
      auto data = make_shared<Data>(netPacket);
      extractLpLocalFields(*data, lpPacket);
      m_impl->satisfyPendingInterests(*data);
      break;
    }
  }
}

} // namespace ndn
