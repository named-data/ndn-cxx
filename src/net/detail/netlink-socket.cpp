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

#include "netlink-socket.hpp"
#include "netlink-message.hpp"
#include "../../util/logger.hpp"
#include "../../util/time.hpp"

#include <cerrno>
#include <cstring>
#include <sys/socket.h>

#include <boost/asio/write.hpp>

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif
#ifndef RTEXT_FILTER_SKIP_STATS
#define RTEXT_FILTER_SKIP_STATS (1 << 3)
#endif

NDN_LOG_INIT(ndn.NetworkMonitor);

namespace ndn {
namespace net {

NetlinkSocket::NetlinkSocket(boost::asio::io_service& io)
  : m_sock(make_shared<boost::asio::posix::stream_descriptor>(io))
  , m_pid(0)
  , m_seqNum(static_cast<uint32_t>(time::system_clock::now().time_since_epoch().count()))
  , m_buffer(16 * 1024) // 16 KiB
{
}

NetlinkSocket::~NetlinkSocket()
{
  boost::system::error_code ec;
  m_sock->close(ec);
}

void
NetlinkSocket::open(int protocol)
{
  int fd = ::socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, protocol);
  if (fd < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot create netlink socket ("s + std::strerror(errno) + ")"));
  }
  m_sock->assign(fd);

  // increase socket receive buffer to 1MB to avoid losing messages
  const int bufsize = 1 * 1024 * 1024;
  if (::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) < 0) {
    // not a fatal error
    NDN_LOG_DEBUG("setting SO_RCVBUF failed: " << std::strerror(errno));
  }

  // enable control messages for received packets to get the destination group
  const int one = 1;
  if (::setsockopt(fd, SOL_NETLINK, NETLINK_PKTINFO, &one, sizeof(one)) < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot enable NETLINK_PKTINFO ("s + std::strerror(errno) + ")"));
  }

  sockaddr_nl addr{};
  addr.nl_family = AF_NETLINK;
  if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot bind netlink socket ("s + std::strerror(errno) + ")"));
  }

  // find out what pid has been assigned to us
  socklen_t len = sizeof(addr);
  if (::getsockname(fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot obtain netlink socket address ("s + std::strerror(errno) + ")"));
  }
  if (len != sizeof(addr)) {
    BOOST_THROW_EXCEPTION(Error("Wrong address length (" + to_string(len) + ")"));
  }
  if (addr.nl_family != AF_NETLINK) {
    BOOST_THROW_EXCEPTION(Error("Wrong address family (" + to_string(addr.nl_family) + ")"));
  }
  m_pid = addr.nl_pid;
  NDN_LOG_TRACE("our pid is " << m_pid);

#ifdef NDN_CXX_HAVE_NETLINK_EXT_ACK
  // enable extended ACK reporting
  if (::setsockopt(fd, SOL_NETLINK, NETLINK_EXT_ACK, &one, sizeof(one)) < 0) {
    // not a fatal error
    NDN_LOG_DEBUG("setting NETLINK_EXT_ACK failed: " << std::strerror(errno));
  }
#endif // NDN_CXX_HAVE_NETLINK_EXT_ACK
}

void
NetlinkSocket::joinGroup(int group)
{
  if (::setsockopt(m_sock->native_handle(), SOL_NETLINK, NETLINK_ADD_MEMBERSHIP,
                   &group, sizeof(group)) < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot join netlink group " + to_string(group) +
                                " (" + std::strerror(errno) + ")"));
  }
}

void
NetlinkSocket::registerNotificationCallback(MessageCallback cb)
{
  registerRequestCallback(0, std::move(cb));
}

void
NetlinkSocket::registerRequestCallback(uint32_t seq, MessageCallback cb)
{
  if (cb == nullptr) {
    m_pendingRequests.erase(seq);
  }
  else {
    bool wasEmpty = m_pendingRequests.empty();
    m_pendingRequests.emplace(seq, std::move(cb));
    if (wasEmpty)
      asyncWait();
  }
}

static const char*
nlmsgTypeToString(uint16_t type)
{
#define NLMSG_STRINGIFY(x) case NLMSG_##x: return "<" #x ">"
#define RTM_STRINGIFY(x) case RTM_##x: return "<" #x ">"
  switch (type) {
    NLMSG_STRINGIFY(NOOP);
    NLMSG_STRINGIFY(ERROR);
    NLMSG_STRINGIFY(DONE);
    NLMSG_STRINGIFY(OVERRUN);
    RTM_STRINGIFY(NEWLINK);
    RTM_STRINGIFY(DELLINK);
    RTM_STRINGIFY(GETLINK);
    RTM_STRINGIFY(NEWADDR);
    RTM_STRINGIFY(DELADDR);
    RTM_STRINGIFY(GETADDR);
    RTM_STRINGIFY(NEWROUTE);
    RTM_STRINGIFY(DELROUTE);
    RTM_STRINGIFY(GETROUTE);
    default:
      return "";
  }
#undef NLMSG_STRINGIFY
#undef RTM_STRINGIFY
}

void
NetlinkSocket::asyncWait()
{
  m_sock->async_read_some(boost::asio::null_buffers(),
    // capture a copy of 'm_sock' to prevent its deallocation while the handler is still pending
    [this, sock = m_sock] (const boost::system::error_code& ec, size_t) {
      if (!sock->is_open() || ec == boost::asio::error::operation_aborted) {
        // socket was closed, ignore the error
        NDN_LOG_DEBUG("netlink socket closed or operation aborted");
      }
      else if (ec) {
        NDN_LOG_ERROR("read failed: " << ec.message());
        BOOST_THROW_EXCEPTION(Error("Netlink socket read error (" + ec.message() + ")"));
      }
      else {
        receiveAndValidate();
        if (!m_pendingRequests.empty())
          asyncWait();
      }
  });
}

void
NetlinkSocket::receiveAndValidate()
{
  sockaddr_nl sender{};
  iovec iov{};
  iov.iov_base = m_buffer.data();
  iov.iov_len = m_buffer.size();
  uint8_t cmsgBuffer[CMSG_SPACE(sizeof(nl_pktinfo))];
  msghdr msg{};
  msg.msg_name = &sender;
  msg.msg_namelen = sizeof(sender);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = cmsgBuffer;
  msg.msg_controllen = sizeof(cmsgBuffer);

  ssize_t nBytesRead = ::recvmsg(m_sock->native_handle(), &msg, 0);
  if (nBytesRead < 0) {
    std::string errorString = std::strerror(errno);
    if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
      // not a fatal error
      NDN_LOG_DEBUG("recvmsg failed: " << errorString);
      return;
    }
    NDN_LOG_ERROR("recvmsg failed: " << errorString);
    BOOST_THROW_EXCEPTION(Error("Netlink socket receive error (" + errorString + ")"));
  }

  NDN_LOG_TRACE("read " << nBytesRead << " bytes from netlink socket");

  if (msg.msg_flags & MSG_TRUNC) {
    NDN_LOG_ERROR("truncated message");
    BOOST_THROW_EXCEPTION(Error("Received truncated netlink message"));
    // TODO: grow the buffer and start over
  }

  if (msg.msg_namelen >= sizeof(sender) && sender.nl_pid != 0) {
    NDN_LOG_TRACE("ignoring message from pid=" << sender.nl_pid);
    return;
  }

  uint32_t nlGroup = 0;
  for (cmsghdr* cmsg = CMSG_FIRSTHDR(&msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
    if (cmsg->cmsg_level == SOL_NETLINK &&
        cmsg->cmsg_type == NETLINK_PKTINFO &&
        cmsg->cmsg_len == CMSG_LEN(sizeof(nl_pktinfo))) {
      const nl_pktinfo* pktinfo = reinterpret_cast<nl_pktinfo*>(CMSG_DATA(cmsg));
      nlGroup = pktinfo->group;
    }
  }

  NetlinkMessage nlmsg(m_buffer.data(), static_cast<size_t>(nBytesRead));
  for (; nlmsg.isValid(); nlmsg = nlmsg.getNext()) {
    NDN_LOG_TRACE("parsing " << (nlmsg->nlmsg_flags & NLM_F_MULTI ? "multi-part " : "") <<
                  "message type=" << nlmsg->nlmsg_type << nlmsgTypeToString(nlmsg->nlmsg_type) <<
                  " len=" << nlmsg->nlmsg_len <<
                  " seq=" << nlmsg->nlmsg_seq <<
                  " pid=" << nlmsg->nlmsg_pid <<
                  " group=" << nlGroup);

    auto cbIt = m_pendingRequests.end();
    if (nlGroup != 0) {
      // it's a multicast notification
      cbIt = m_pendingRequests.find(0);
    }
    else if (nlmsg->nlmsg_pid == m_pid) {
      // it's for us
      cbIt = m_pendingRequests.find(nlmsg->nlmsg_seq);
    }
    else {
      NDN_LOG_TRACE("pid mismatch, ignoring");
      continue;
    }

    if (cbIt == m_pendingRequests.end()) {
      NDN_LOG_TRACE("no handler registered, ignoring");
      continue;
    }
    else if (nlmsg->nlmsg_flags & NLM_F_DUMP_INTR) {
      NDN_LOG_ERROR("dump is inconsistent");
      BOOST_THROW_EXCEPTION(Error("Inconsistency detected in netlink dump"));
      // TODO: discard the rest of the message and retry the dump
    }
    else {
      // invoke the callback
      BOOST_ASSERT(cbIt->second);
      cbIt->second(nlmsg);
    }

    // garbage collect the handler if we don't need it anymore:
    // do it only if this is a reply message (i.e. not a notification) and either
    //   (1) it's not a multi-part message, in which case this is the only fragment, or
    //   (2) it's the last fragment of a multi-part message
    if (nlGroup == 0 && (!(nlmsg->nlmsg_flags & NLM_F_MULTI) || nlmsg->nlmsg_type == NLMSG_DONE)) {
      NDN_LOG_TRACE("removing handler for seq=" << nlmsg->nlmsg_seq);
      BOOST_ASSERT(cbIt != m_pendingRequests.end());
      m_pendingRequests.erase(cbIt);
    }
  }
}

RtnlSocket::RtnlSocket(boost::asio::io_service& io)
  : NetlinkSocket(io)
{
}

void
RtnlSocket::open()
{
  NDN_LOG_TRACE("opening rtnetlink socket");
  NetlinkSocket::open(NETLINK_ROUTE);
}

void
RtnlSocket::sendDumpRequest(uint16_t nlmsgType, MessageCallback cb)
{
  struct RtnlRequest
  {
    nlmsghdr nlh;
    alignas(NLMSG_ALIGNTO) ifinfomsg ifi;
    alignas(NLMSG_ALIGNTO) rtattr rta;
    alignas(NLMSG_ALIGNTO) uint32_t rtext; // space for IFLA_EXT_MASK
  };

  auto request = make_shared<RtnlRequest>();
  request->nlh.nlmsg_type = nlmsgType;
  request->nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  request->nlh.nlmsg_seq = ++m_seqNum;
  request->nlh.nlmsg_pid = m_pid;
  request->ifi.ifi_family = AF_UNSPEC;
  request->rta.rta_type = IFLA_EXT_MASK;
  request->rta.rta_len = RTA_LENGTH(sizeof(request->rtext));
  request->rtext = RTEXT_FILTER_SKIP_STATS;
  request->nlh.nlmsg_len = NLMSG_SPACE(sizeof(ifinfomsg)) + request->rta.rta_len;

  registerRequestCallback(request->nlh.nlmsg_seq, std::move(cb));

  boost::asio::async_write(*m_sock, boost::asio::buffer(request.get(), request->nlh.nlmsg_len),
    // capture 'request' to prevent its premature deallocation
    [request] (const boost::system::error_code& ec, size_t) {
      if (!ec) {
        auto type = request->nlh.nlmsg_type;
        NDN_LOG_TRACE("sent dump request type=" << type << nlmsgTypeToString(type)
                      << " seq=" << request->nlh.nlmsg_seq);
      }
      else if (ec != boost::asio::error::operation_aborted) {
        NDN_LOG_ERROR("write failed: " << ec.message());
        BOOST_THROW_EXCEPTION(Error("Failed to send netlink request (" + ec.message() + ")"));
      }
  });
}

} // namespace net
} // namespace ndn
