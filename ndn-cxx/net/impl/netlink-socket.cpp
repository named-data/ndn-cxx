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

#include "ndn-cxx/net/impl/netlink-socket.hpp"
#include "ndn-cxx/net/impl/netlink-message.hpp"
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/util/time.hpp"

#include <cerrno>
#include <linux/genetlink.h>
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

std::string
NetlinkSocket::nlmsgTypeToString(uint16_t type) const
{
#define NLMSG_STRINGIFY(x) case NLMSG_##x: return to_string(type) + "<" #x ">"
  switch (type) {
    NLMSG_STRINGIFY(NOOP);
    NLMSG_STRINGIFY(ERROR);
    NLMSG_STRINGIFY(DONE);
    NLMSG_STRINGIFY(OVERRUN);
    default:
      return to_string(type);
  }
#undef NLMSG_STRINGIFY
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
                  "message type=" << nlmsgTypeToString(nlmsg->nlmsg_type) <<
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
    [this, request] (const boost::system::error_code& ec, size_t) {
      if (!ec) {
        NDN_LOG_TRACE("sent dump request type=" << nlmsgTypeToString(request->nlh.nlmsg_type)
                      << " seq=" << request->nlh.nlmsg_seq);
      }
      else if (ec != boost::asio::error::operation_aborted) {
        NDN_LOG_ERROR("write failed: " << ec.message());
        BOOST_THROW_EXCEPTION(Error("Failed to send netlink request (" + ec.message() + ")"));
      }
  });
}

std::string
RtnlSocket::nlmsgTypeToString(uint16_t type) const
{
#define RTM_STRINGIFY(x) case RTM_##x: return to_string(type) + "<" #x ">"
  switch (type) {
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
      return NetlinkSocket::nlmsgTypeToString(type);
  }
#undef RTM_STRINGIFY
}

GenlSocket::GenlSocket(boost::asio::io_service& io)
  : NetlinkSocket(io)
{
  m_cachedFamilyIds["nlctrl"] = GENL_ID_CTRL;
}

void
GenlSocket::open()
{
  NDN_LOG_TRACE("opening genetlink socket");
  NetlinkSocket::open(NETLINK_GENERIC);
}

void
GenlSocket::sendRequest(const std::string& familyName, uint8_t command,
                        const void* payload, size_t payloadLen,
                        MessageCallback messageCb, std::function<void()> errorCb)
{
  auto it = m_cachedFamilyIds.find(familyName);
  if (it != m_cachedFamilyIds.end()) {
    if (it->second >= GENL_MIN_ID) {
      sendRequest(it->second, command, payload, payloadLen, std::move(messageCb));
    }
    else if (errorCb) {
      errorCb();
    }
    return;
  }

  auto ret = m_familyResolvers.emplace(std::piecewise_construct,
                                       std::forward_as_tuple(familyName),
                                       std::forward_as_tuple(familyName, *this));
  auto& resolver = ret.first->second;
  if (ret.second) {
    // cache the result
    resolver.onResolved.connectSingleShot([=] (uint16_t familyId) {
      m_cachedFamilyIds[familyName] = familyId;
    });
    resolver.onError.connectSingleShot([=] {
      m_cachedFamilyIds[familyName] = 0;
    });
  }
  resolver.onResolved.connectSingleShot([=, cb = std::move(messageCb)] (uint16_t familyId) {
    sendRequest(familyId, command, payload, payloadLen, std::move(cb));
  });
  if (errorCb) {
    resolver.onError.connectSingleShot(std::move(errorCb));
  }
}

void
GenlSocket::sendRequest(uint16_t familyId, uint8_t command,
                        const void* payload, size_t payloadLen, MessageCallback cb)
{
  struct GenlRequestHeader
  {
    alignas(NLMSG_ALIGNTO) nlmsghdr nlh;
    alignas(NLMSG_ALIGNTO) genlmsghdr genlh;
  };
  static_assert(sizeof(GenlRequestHeader) == NLMSG_SPACE(GENL_HDRLEN), "");

  auto hdr = make_shared<GenlRequestHeader>();
  hdr->nlh.nlmsg_len = sizeof(GenlRequestHeader) + payloadLen;
  hdr->nlh.nlmsg_type = familyId;
  hdr->nlh.nlmsg_flags = NLM_F_REQUEST;
  hdr->nlh.nlmsg_seq = ++m_seqNum;
  hdr->nlh.nlmsg_pid = m_pid;
  hdr->genlh.cmd = command;
  hdr->genlh.version = 1;

  registerRequestCallback(hdr->nlh.nlmsg_seq, std::move(cb));

  std::array<boost::asio::const_buffer, 2> bufs = {
    boost::asio::buffer(hdr.get(), sizeof(GenlRequestHeader)),
    boost::asio::buffer(payload, payloadLen)
  };
  boost::asio::async_write(*m_sock, bufs,
    // capture 'hdr' to prevent its premature deallocation
    [this, hdr] (const boost::system::error_code& ec, size_t) {
      if (!ec) {
        NDN_LOG_TRACE("sent genl request type=" << nlmsgTypeToString(hdr->nlh.nlmsg_type) <<
                      " cmd=" << static_cast<unsigned>(hdr->genlh.cmd) <<
                      " seq=" << hdr->nlh.nlmsg_seq);
      }
      else if (ec != boost::asio::error::operation_aborted) {
        NDN_LOG_ERROR("write failed: " << ec.message());
        BOOST_THROW_EXCEPTION(Error("Failed to send netlink request (" + ec.message() + ")"));
      }
  });
}

GenlFamilyResolver::GenlFamilyResolver(std::string familyName, GenlSocket& socket)
  : m_sock(socket)
  , m_family(std::move(familyName))
{
  if (m_family.size() >= GENL_NAMSIZ) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("netlink family name '" + m_family + "' too long"));
  }

  NDN_LOG_TRACE("resolving netlink family " << m_family);
  asyncResolve();
}

void
GenlFamilyResolver::asyncResolve()
{
  struct FamilyNameAttribute
  {
    alignas(NLMSG_ALIGNTO) nlattr nla;
    alignas(NLMSG_ALIGNTO) char name[GENL_NAMSIZ];
  };

  auto attr = make_shared<FamilyNameAttribute>();
  attr->nla.nla_type = CTRL_ATTR_FAMILY_NAME;
  attr->nla.nla_len = NLA_HDRLEN + m_family.size() + 1;
  ::strncpy(attr->name, m_family.data(), GENL_NAMSIZ);

  m_sock.sendRequest(GENL_ID_CTRL, CTRL_CMD_GETFAMILY, attr.get(), attr->nla.nla_len,
                     // capture 'attr' to prevent its premature deallocation
                     [this, attr] (const auto& msg) { this->handleResolve(msg); });
}

void
GenlFamilyResolver::handleResolve(const NetlinkMessage& nlmsg)
{
  switch (nlmsg->nlmsg_type) {
    case NLMSG_ERROR: {
      const nlmsgerr* err = nlmsg.getPayload<nlmsgerr>();
      if (err == nullptr) {
        NDN_LOG_WARN("malformed nlmsgerr");
      }
      else if (err->error != 0) {
        NDN_LOG_DEBUG("failed to resolve netlink family " << m_family << ": "
                      << std::strerror(std::abs(err->error)));
      }
      onError();
      break;
    }

    case GENL_ID_CTRL: {
      const genlmsghdr* genlh = nlmsg.getPayload<genlmsghdr>();
      if (genlh == nullptr) {
        NDN_LOG_WARN("malformed genlmsghdr");
        return onError();
      }
      if (genlh->cmd != CTRL_CMD_NEWFAMILY) {
        NDN_LOG_WARN("unexpected genl cmd=" << static_cast<unsigned>(genlh->cmd));
        return onError();
      }

      auto attrs = nlmsg.getAttributes<nlattr>(genlh);
      auto familyName = attrs.getAttributeByType<std::string>(CTRL_ATTR_FAMILY_NAME);
      if (familyName && *familyName != m_family) {
        NDN_LOG_WARN("CTRL_ATTR_FAMILY_NAME mismatch: " << *familyName << " != " << m_family);
        return onError();
      }
      auto familyId = attrs.getAttributeByType<uint16_t>(CTRL_ATTR_FAMILY_ID);
      if (!familyId) {
        NDN_LOG_WARN("missing CTRL_ATTR_FAMILY_ID");
        return onError();
      }
      if (*familyId < GENL_MIN_ID) {
        NDN_LOG_WARN("invalid CTRL_ATTR_FAMILY_ID=" << *familyId);
        return onError();
      }

      NDN_LOG_TRACE("resolved netlink family name=" << m_family << " id=" << *familyId);
      onResolved(*familyId);
      break;
    }

    default: {
      NDN_LOG_WARN("unexpected message type");
      onError();
      break;
    }
  }
}

std::string
GenlSocket::nlmsgTypeToString(uint16_t type) const
{
  if (type >= GENL_MIN_ID) {
    for (const auto& p : m_cachedFamilyIds) {
      if (p.second == type) {
        return to_string(type) + "<" + p.first + ">";
      }
    }
  }

  return NetlinkSocket::nlmsgTypeToString(type);
}

} // namespace net
} // namespace ndn
