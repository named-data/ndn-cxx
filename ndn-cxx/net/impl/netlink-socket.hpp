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
 *
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#ifndef NDN_NET_NETLINK_SOCKET_HPP
#define NDN_NET_NETLINK_SOCKET_HPP

#include "ndn-cxx/net/network-monitor.hpp"
#include "ndn-cxx/util/signal/signal.hpp"

#include <boost/asio/generic/raw_protocol.hpp>
#include <map>
#include <vector>

#ifndef NDN_CXX_HAVE_NETLINK
#error "This file should not be included ..."
#endif

namespace ndn {
namespace net {

class NetlinkMessage;

class NetlinkSocket : noncopyable
{
public:
  using Error = NetworkMonitor::Error;
  using MessageCallback = std::function<void(const NetlinkMessage&)>;

  void
  joinGroup(int group);

  void
  registerNotificationCallback(MessageCallback cb);

protected:
  explicit
  NetlinkSocket(boost::asio::io_service& io);

  ~NetlinkSocket();

  void
  open(int protocol);

  void
  registerRequestCallback(uint32_t seq, MessageCallback cb);

  virtual std::string
  nlmsgTypeToString(uint16_t type) const;

private:
  void
  asyncWait();

  void
  receiveAndValidate();

protected:
  shared_ptr<boost::asio::generic::raw_protocol::socket> m_sock; ///< netlink socket descriptor
  uint32_t m_pid; ///< port ID of this socket
  uint32_t m_seqNum; ///< sequence number of the last netlink request sent to the kernel

private:
  std::vector<uint8_t> m_buffer; ///< buffer for netlink messages from the kernel
  std::map<uint32_t, MessageCallback> m_pendingRequests; ///< request sequence number => callback
};

class RtnlSocket final : public NetlinkSocket
{
public:
  explicit
  RtnlSocket(boost::asio::io_service& io);

  void
  open();

  void
  sendDumpRequest(uint16_t nlmsgType,
                  const void* payload, size_t payloadLen,
                  MessageCallback cb);

protected:
  std::string
  nlmsgTypeToString(uint16_t type) const final;
};

class GenlSocket;

class GenlFamilyResolver : noncopyable
{
public:
  GenlFamilyResolver(std::string familyName, GenlSocket& socket);

  util::Signal<GenlFamilyResolver, uint16_t> onResolved;
  util::Signal<GenlFamilyResolver> onError;

private:
  void
  asyncResolve();

  void
  handleResolve(const NetlinkMessage& nlmsg);

private:
  GenlSocket& m_sock;
  std::string m_family;
};

class GenlSocket final : public NetlinkSocket
{
public:
  explicit
  GenlSocket(boost::asio::io_service& io);

  void
  open();

  void
  sendRequest(const std::string& familyName, uint8_t command,
              const void* payload, size_t payloadLen,
              MessageCallback messageCb, std::function<void()> errorCb);

  void
  sendRequest(uint16_t familyId, uint8_t command,
              const void* payload, size_t payloadLen,
              MessageCallback messageCb);

protected:
  std::string
  nlmsgTypeToString(uint16_t type) const final;

private:
  std::map<std::string, uint16_t> m_cachedFamilyIds; ///< family name => family id
  std::map<std::string, GenlFamilyResolver> m_familyResolvers; ///< family name => resolver instance
};

} // namespace net
} // namespace ndn

#endif // NDN_NET_NETLINK_SOCKET_HPP
