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

#include "face.hpp"
#include "detail/face-impl.hpp"

#include "encoding/tlv.hpp"
#include "net/face-uri.hpp"
#include "security/signing-helpers.hpp"
#include "util/random.hpp"
#include "util/time.hpp"

// NDN_LOG_INIT(ndn.Face) is declared in face-impl.hpp

// A callback scheduled through io.post and io.dispatch may be invoked after the face is destructed.
// To prevent this situation, use these macros to capture Face::m_impl as weak_ptr and skip callback
// execution if the face has been destructed.
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

Face::OversizedPacketError::OversizedPacketError(char pktType, const Name& name, size_t wireSize)
  : Error((pktType == 'I' ? "Interest " : pktType == 'D' ? "Data " : "Nack ") +
          name.toUri() + " encodes into " + to_string(wireSize) + " octets, "
          "exceeding the implementation limit of " + to_string(MAX_NDN_PACKET_SIZE) + " octets")
  , pktType(pktType)
  , name(name)
  , wireSize(wireSize)
{
}

Face::Face(shared_ptr<Transport> transport)
  : m_internalIoService(make_unique<boost::asio::io_service>())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(make_unique<KeyChain>())
  , m_impl(make_shared<Impl>(*this))
{
  construct(std::move(transport), *m_internalKeyChain);
}

Face::Face(boost::asio::io_service& ioService)
  : m_ioService(ioService)
  , m_internalKeyChain(make_unique<KeyChain>())
  , m_impl(make_shared<Impl>(*this))
{
  construct(nullptr, *m_internalKeyChain);
}

Face::Face(const std::string& host, const std::string& port)
  : m_internalIoService(make_unique<boost::asio::io_service>())
  , m_ioService(*m_internalIoService)
  , m_internalKeyChain(make_unique<KeyChain>())
  , m_impl(make_shared<Impl>(*this))
{
  construct(make_shared<TcpTransport>(host, port), *m_internalKeyChain);
}

Face::Face(shared_ptr<Transport> transport, KeyChain& keyChain)
  : m_internalIoService(make_unique<boost::asio::io_service>())
  , m_ioService(*m_internalIoService)
  , m_impl(make_shared<Impl>(*this))
{
  construct(std::move(transport), keyChain);
}

Face::Face(shared_ptr<Transport> transport, boost::asio::io_service& ioService)
  : m_ioService(ioService)
  , m_internalKeyChain(make_unique<KeyChain>())
  , m_impl(make_shared<Impl>(*this))
{
  construct(std::move(transport), *m_internalKeyChain);
}

Face::Face(shared_ptr<Transport> transport, boost::asio::io_service& ioService, KeyChain& keyChain)
  : m_ioService(ioService)
  , m_impl(make_shared<Impl>(*this))
{
  construct(std::move(transport), keyChain);
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
    FaceUri uri(transportUri);
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
  catch (const FaceUri::Error& error) {
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
  m_transport = std::move(transport);

  m_nfdController = make_unique<nfd::Controller>(*this, keyChain);

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
  shared_ptr<Interest> interest2 = make_shared<Interest>(interest);
  interest2->getNonce();

  IO_CAPTURE_WEAK_IMPL(dispatch) {
    impl->asyncExpressInterest(interest2, afterSatisfied, afterNacked, afterTimeout);
  } IO_CAPTURE_WEAK_IMPL_END

  return reinterpret_cast<const PendingInterestId*>(interest2.get());
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
Face::put(Data data)
{
  IO_CAPTURE_WEAK_IMPL(dispatch) {
    impl->asyncPutData(data);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::put(lp::Nack nack)
{
  IO_CAPTURE_WEAK_IMPL(dispatch) {
    impl->asyncPutNack(nack);
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
Face::doProcessEvents(time::milliseconds timeout, bool keepThread)
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
template<typename NetPkt>
static void
extractLpLocalFields(NetPkt& netPacket, const lp::Packet& lpPacket)
{
  addTagFromField<lp::IncomingFaceIdTag, lp::IncomingFaceIdField>(netPacket, lpPacket);
  addTagFromField<lp::CongestionMarkTag, lp::CongestionMarkField>(netPacket, lpPacket);
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
        NDN_LOG_DEBUG(">N " << nack->getInterest() << '~' << nack->getHeader().getReason());
        m_impl->nackPendingInterests(*nack);
      }
      else {
        extractLpLocalFields(*interest, lpPacket);
        NDN_LOG_DEBUG(">I " << *interest);
        m_impl->processIncomingInterest(std::move(interest));
      }
      break;
    }
    case tlv::Data: {
      auto data = make_shared<Data>(netPacket);
      extractLpLocalFields(*data, lpPacket);
      NDN_LOG_DEBUG(">D " << data->getName());
      m_impl->satisfyPendingInterests(*data);
      break;
    }
  }
}

} // namespace ndn
