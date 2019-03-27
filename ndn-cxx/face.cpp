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

#include "ndn-cxx/face.hpp"
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/impl/face-impl.hpp"
#include "ndn-cxx/net/face-uri.hpp"
#include "ndn-cxx/security/signing-helpers.hpp"
#include "ndn-cxx/util/random.hpp"
#include "ndn-cxx/util/time.hpp"

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
      NDN_THROW(ConfigFile::Error("Unsupported transport protocol \"" + protocol + "\""));
    }
  }
  catch (const Transport::Error&) {
    NDN_THROW_NESTED(ConfigFile::Error("Failed to create transport"));
  }
  catch (const FaceUri::Error&) {
    NDN_THROW_NESTED(ConfigFile::Error("Failed to create transport"));
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

PendingInterestHandle
Face::expressInterest(const Interest& interest,
                      const DataCallback& afterSatisfied,
                      const NackCallback& afterNacked,
                      const TimeoutCallback& afterTimeout)
{
  auto id = m_impl->m_pendingInterestTable.allocateId();

  auto interest2 = make_shared<Interest>(interest);
  interest2->getNonce();

  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncExpressInterest(id, interest2, afterSatisfied, afterNacked, afterTimeout);
  } IO_CAPTURE_WEAK_IMPL_END

  return PendingInterestHandle(*this, reinterpret_cast<const PendingInterestId*>(id));
}

void
Face::cancelPendingInterest(const PendingInterestId* pendingInterestId)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncRemovePendingInterest(reinterpret_cast<RecordId>(pendingInterestId));
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
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncPutData(data);
  } IO_CAPTURE_WEAK_IMPL_END
}

void
Face::put(lp::Nack nack)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncPutNack(nack);
  } IO_CAPTURE_WEAK_IMPL_END
}

RegisteredPrefixHandle
Face::setInterestFilter(const InterestFilter& filter, const InterestCallback& onInterest,
                        const RegisterPrefixFailureCallback& onFailure,
                        const security::SigningInfo& signingInfo, uint64_t flags)
{
  return setInterestFilter(filter, onInterest, nullptr, onFailure, signingInfo, flags);
}

RegisteredPrefixHandle
Face::setInterestFilter(const InterestFilter& filter, const InterestCallback& onInterest,
                        const RegisterPrefixSuccessCallback& onSuccess,
                        const RegisterPrefixFailureCallback& onFailure,
                        const security::SigningInfo& signingInfo, uint64_t flags)
{
  nfd::CommandOptions options;
  options.setSigningInfo(signingInfo);

  auto id = m_impl->registerPrefix(filter.getPrefix(), onSuccess, onFailure, flags, options,
                                   filter, onInterest);
  return RegisteredPrefixHandle(*this, reinterpret_cast<const RegisteredPrefixId*>(id));
}

InterestFilterHandle
Face::setInterestFilter(const InterestFilter& filter, const InterestCallback& onInterest)
{
  auto id = m_impl->m_interestFilterTable.allocateId();

  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncSetInterestFilter(id, filter, onInterest);
  } IO_CAPTURE_WEAK_IMPL_END

  return InterestFilterHandle(*this, reinterpret_cast<const InterestFilterId*>(id));
}

void
Face::clearInterestFilter(const InterestFilterId* interestFilterId)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncUnsetInterestFilter(reinterpret_cast<RecordId>(interestFilterId));
  } IO_CAPTURE_WEAK_IMPL_END
}

RegisteredPrefixHandle
Face::registerPrefix(const Name& prefix,
                     const RegisterPrefixSuccessCallback& onSuccess,
                     const RegisterPrefixFailureCallback& onFailure,
                     const security::SigningInfo& signingInfo,
                     uint64_t flags)
{
  nfd::CommandOptions options;
  options.setSigningInfo(signingInfo);

  auto id = m_impl->registerPrefix(prefix, onSuccess, onFailure, flags, options, nullopt, nullptr);
  return RegisteredPrefixHandle(*this, reinterpret_cast<const RegisteredPrefixId*>(id));
}

void
Face::unregisterPrefixImpl(const RegisteredPrefixId* registeredPrefixId,
                           const UnregisterPrefixSuccessCallback& onSuccess,
                           const UnregisterPrefixFailureCallback& onFailure)
{
  IO_CAPTURE_WEAK_IMPL(post) {
    impl->asyncUnregisterPrefix(reinterpret_cast<RecordId>(registeredPrefixId),
                                onSuccess, onFailure);
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
      m_impl->m_processEventsTimeoutEvent = m_impl->m_scheduler.schedule(timeout,
        [&io = m_ioService, &work = m_impl->m_ioServiceWork] {
          io.stop();
          work.reset();
        });
    }

    if (keepThread) {
      // work will ensure that m_ioService is running until work object exists
      m_impl->m_ioServiceWork = make_unique<boost::asio::io_service::work>(m_ioService);
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
    impl->m_pendingInterestTable.clear();
    impl->m_registeredPrefixTable.clear();

    if (m_transport->isConnected())
      m_transport->close();

    impl->m_ioServiceWork.reset();
  } IO_CAPTURE_WEAK_IMPL_END
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

PendingInterestHandle::PendingInterestHandle(Face& face, const PendingInterestId* id)
  : CancelHandle([&face, id] { face.cancelPendingInterest(id); })
  , m_id(id)
{
}

RegisteredPrefixHandle::RegisteredPrefixHandle(Face& face, const RegisteredPrefixId* id)
  : CancelHandle([&face, id] { face.unregisterPrefixImpl(id, nullptr, nullptr); })
  , m_face(&face)
  , m_id(id)
{
  // The lambda passed to CancelHandle constructor cannot call this->unregister,
  // because base class destructor cannot access the member fields of this subclass.
}

void
RegisteredPrefixHandle::unregister(const UnregisterPrefixSuccessCallback& onSuccess,
                                   const UnregisterPrefixFailureCallback& onFailure)
{
  if (m_id == nullptr) {
    if (onFailure != nullptr) {
      onFailure("RegisteredPrefixHandle is empty");
    }
    return;
  }

  m_face->unregisterPrefixImpl(m_id, onSuccess, onFailure);
  m_face = nullptr;
  m_id = nullptr;
}

InterestFilterHandle::InterestFilterHandle(Face& face, const InterestFilterId* id)
  : CancelHandle([&face, id] { face.clearInterestFilter(id); })
  , m_id(id)
{
}

} // namespace ndn
