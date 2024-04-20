/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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
 * \brief Options for DummyClientFace.
 */
struct DummyClientFaceOptions
{
  DummyClientFaceOptions() = default;

  DummyClientFaceOptions(bool enablePacketLogging, bool enableRegistrationReply,
                         const std::function<void(time::milliseconds)>& processEventsOverride = nullptr)
    : enablePacketLogging(enablePacketLogging)
    , enableRegistrationReply(enableRegistrationReply)
    , processEventsOverride(processEventsOverride)
  {
  }

  /// If true, packets sent out of DummyClientFace will be appended to a container.
  bool enablePacketLogging = true;

  /// If true, prefix registration commands will be automatically replied to with a successful response.
  bool enableRegistrationReply = false;

  /// FaceId used in prefix registration replies.
  uint64_t registrationReplyFaceId = 1;

  /// If not empty, `face.processEvents()` will be overridden by this function.
  std::function<void(time::milliseconds)> processEventsOverride;
};

/**
 * \brief A client-side face for unit testing.
 */
class DummyClientFace : public Face
{
public:
  class AlreadyLinkedError : public Error
  {
  public:
    AlreadyLinkedError();
  };

  using Options = DummyClientFaceOptions;

  /**
   * \brief Create a dummy face with an internal I/O context.
   */
  explicit
  DummyClientFace(const Options& options = {});

  /**
   * \brief Create a dummy face with an internal I/O context and the specified KeyChain.
   */
  explicit
  DummyClientFace(KeyChain& keyChain, const Options& options = {});

  /**
   * \brief Create a dummy face with the provided I/O context.
   */
  explicit
  DummyClientFace(boost::asio::io_context& ioCtx, const Options& options = {});

  /**
   * \brief Create a dummy face with the specified I/O context and KeyChain.
   */
  DummyClientFace(boost::asio::io_context& ioCtx, KeyChain& keyChain, const Options& options = {});

  ~DummyClientFace() override;

  /**
   * \brief Cause the face to receive an Interest packet.
   */
  void
  receive(const Interest& interest);

  /**
   * \brief Cause the face to receive a Data packet.
   */
  void
  receive(const Data& data);

  /**
   * \brief Cause the face to receive a Nack packet.
   */
  void
  receive(const lp::Nack& nack);

  /**
   * \brief Link another DummyClientFace through a broadcast medium.
   */
  void
  linkTo(DummyClientFace& other);

  /**
   * \brief Unlink the broadcast medium if previously linked.
   */
  void
  unlink();

private:
  void
  construct(const Options& options);

  void
  enableBroadcastLink();

  void
  enablePacketLogging();

  void
  enableRegistrationReply(uint64_t faceId);

  void
  doProcessEvents(time::milliseconds timeout, bool keepRunning) override;

public:
  /**
   * \brief Interests sent out of this DummyClientFace.
   *
   * Sent Interests are appended to this container if Options::enablePacketLogging is true.
   * The user of this class is responsible for cleaning up the container, if necessary.
   *
   * \note After expressInterest(), processEvents() must be called before the %Interest shows up here.
   */
  std::vector<Interest> sentInterests;

  /**
   * \brief %Data sent out of this DummyClientFace.
   *
   * Sent %Data are appended to this container if Options::enablePacketLogging is true.
   * The user of this class is responsible for cleaning up the container, if necessary.
   *
   * \note After put(), processEvents() must be called before the %Data shows up here.
   */
  std::vector<Data> sentData;

  /**
   * \brief Nacks sent out of this DummyClientFace.
   *
   * Sent Nacks are appended to this container if Options::enablePacketLogging is true.
   * The user of this class is responsible for cleaning up the container, if necessary.
   *
   * \note After put(), processEvents() must be called before the %Nack shows up here.
   */
  std::vector<lp::Nack> sentNacks;

  /**
   * \brief Emitted whenever an %Interest is sent.
   * \note After expressInterest(), processEvents() must be called before this signal is emitted.
   */
  signal::Signal<DummyClientFace, Interest> onSendInterest;

  /**
   * \brief Emitted whenever a %Data packet is sent.
   * \note After put(), processEvents() must be called before this signal is emitted.
   */
  signal::Signal<DummyClientFace, Data> onSendData;

  /**
   * \brief Emitted whenever a %Nack is sent.
   * \note After put(), processEvents() must be called before this signal is emitted.
   */
  signal::Signal<DummyClientFace, lp::Nack> onSendNack;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  struct BroadcastLink;
  shared_ptr<BroadcastLink> m_bcastLink;
  std::unique_ptr<KeyChain> m_internalKeyChain;
  KeyChain& m_keyChain;
  std::function<void(time::milliseconds)> m_processEventsOverride;
};

} // namespace ndn

#endif // NDN_CXX_UTIL_DUMMY_CLIENT_FACE_HPP
