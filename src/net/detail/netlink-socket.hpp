/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "../../common.hpp"
#include "../network-monitor.hpp"

#include <boost/asio/posix/stream_descriptor.hpp>
#include <map>
#include <vector>

#ifndef NDN_CXX_HAVE_RTNETLINK
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

private:
  void
  asyncWait();

  void
  receiveAndValidate();

protected:
  shared_ptr<boost::asio::posix::stream_descriptor> m_sock; ///< netlink socket descriptor
  uint32_t m_pid; ///< port ID of this socket
  uint32_t m_seqNum; ///< sequence number of the last netlink request sent to the kernel

private:
  std::vector<uint8_t> m_buffer; ///< buffer for netlink messages from the kernel
  std::map<uint32_t, MessageCallback> m_pendingRequests; ///< request sequence number => callback
};

class RtnlSocket : public NetlinkSocket
{
public:
  explicit
  RtnlSocket(boost::asio::io_service& io);

  void
  open();

  void
  sendDumpRequest(uint16_t nlmsgType, MessageCallback cb);
};

} // namespace net
} // namespace ndn

#endif // NDN_NET_NETLINK_SOCKET_HPP
