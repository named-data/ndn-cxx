/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_UTIL_DUMMY_CLIENT_FACE_HPP
#define NDN_CXX_UTIL_DUMMY_CLIENT_FACE_HPP

#include "ndn-cxx/face.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include "ndn-cxx/util/signal/signal.hpp"

namespace ndn {

/**
 * \brief A client-side face for unit testing.
 */
class DummyClientFace : public Face
{
public:
  /**
   * \brief %Options for DummyClientFace.
   */
  class Options
  {
  public:
    Options(bool enablePacketLogging, bool enableRegistrationReply,
            const std::function<void(time::milliseconds)>& processEventsOverride)
      : enablePacketLogging(enablePacketLogging)
      , enableRegistrationReply(enableRegistrationReply)
      , processEventsOverride(processEventsOverride)
    {
    }

    Options(bool enablePacketLogging, bool enableRegistrationReply)
      : Options(enablePacketLogging, enableRegistrationReply, nullptr)
    {
    }

    Options()
      : Options(true, false)
    {
    }

  public:
    /** \brief If true, packets sent out of DummyClientFace will be appended to a container.
     */
    bool enablePacketLogging;

    /** \brief If true, prefix registration command will be automatically
     *         replied with a successful response.
     */
    bool enableRegistrationReply;

    /** \brief FaceId used in prefix registration replies.
     */
    uint64_t registrationReplyFaceId = 1;

    /** \brief If not empty, `face.processEvents()` will be overridden by this function.
     */
    std::function<void(time::milliseconds)> processEventsOverride;
  };

  class AlreadyLinkedError : public Error
  {
  public:
    AlreadyLinkedError();
  };

  /** \brief Create a dummy face with internal IO service.
   */
  explicit
  DummyClientFace(const Options& options = Options());

  /** \brief Create a dummy face with internal IO service and the specified KeyChain.
   */
  explicit
  DummyClientFace(KeyChain& keyChain, const Options& options = Options());

  /** \brief Create a dummy face with the provided IO service.
   */
  explicit
  DummyClientFace(boost::asio::io_service& ioService, const Options& options = Options());

  /** \brief Create a dummy face with the provided IO service and the specified KeyChain.
   */
  DummyClientFace(boost::asio::io_service& ioService, KeyChain& keyChain,
                  const Options& options = Options());

  ~DummyClientFace() override;

  /** \brief Cause the Face to receive an Interest packet.
   */
  void
  receive(const Interest& interest);

  /** \brief Cause the Face to receive a Data packet.
   */
  void
  receive(const Data& data);

  /** \brief Cause the Face to receive a Nack packet.
   */
  void
  receive(const lp::Nack& nack);

  /** \brief Link another DummyClientFace through a broadcast media.
   */
  void
  linkTo(DummyClientFace& other);

  /** \brief Unlink the broadcast media if previously linked.
   */
  void
  unlink();

private:
  class Transport;

  void
  construct(const Options& options);

  void
  enableBroadcastLink();

  void
  enablePacketLogging();

  void
  enableRegistrationReply(uint64_t faceId);

  void
  doProcessEvents(time::milliseconds timeout, bool keepThread) override;

public:
  /** \brief Interests sent out of this DummyClientFace.
   *
   *  Sent Interests are appended to this container if options.enablePacketLogger is true.
   *  User of this class is responsible for cleaning up the container, if necessary.
   *  After .expressInterest, .processEvents must be called before the Interest would show up here.
   */
  std::vector<Interest> sentInterests;

  /** \brief Data sent out of this DummyClientFace.
   *
   *  Sent Data are appended to this container if options.enablePacketLogger is true.
   *  User of this class is responsible for cleaning up the container, if necessary.
   *  After .put, .processEvents must be called before the Data would show up here.
   */
  std::vector<Data> sentData;

  /** \brief Nacks sent out of this DummyClientFace.
   *
   *  Sent Nacks are appended to this container if options.enablePacketLogger is true.
   *  User of this class is responsible for cleaning up the container, if necessary.
   *  After .put, .processEvents must be called before the NACK would show up here.
   */
  std::vector<lp::Nack> sentNacks;

  /** \brief Emits whenever an Interest is sent.
   *
   *  After .expressInterest, .processEvents must be called before this signal would be emitted.
   */
  signal::Signal<DummyClientFace, Interest> onSendInterest;

  /** \brief Emits whenever a Data packet is sent.
   *
   *  After .put, .processEvents must be called before this signal would be emitted.
   */
  signal::Signal<DummyClientFace, Data> onSendData;

  /** \brief Emits whenever a Nack is sent.
   *
   *  After .put, .processEvents must be called before this signal would be emitted.
   */
  signal::Signal<DummyClientFace, lp::Nack> onSendNack;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  struct BroadcastLink;
  shared_ptr<BroadcastLink> m_bcastLink;
  std::unique_ptr<KeyChain> m_internalKeyChain;
  KeyChain& m_keyChain;
  std::function<void(time::milliseconds)> m_processEventsOverride;
};

namespace util {
/// \deprecated Use ndn::DummyClientFace
using DummyClientFace = ::ndn::DummyClientFace;
} // namespace util

} // namespace ndn

#endif // NDN_CXX_UTIL_DUMMY_CLIENT_FACE_HPP
