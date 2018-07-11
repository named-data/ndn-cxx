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

#include "network-monitor-impl-netlink.hpp"
#include "linux-if-constants.hpp"
#include "netlink-util.hpp"
#include "../network-address.hpp"
#include "../network-interface.hpp"
#include "../../util/logger.hpp"
#include "../../util/time.hpp"

#include <cerrno>
#include <cstdlib>

#include <linux/if_addr.h>
#include <linux/if_link.h>
#include <net/if_arp.h>
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

struct RtnlRequest
{
  nlmsghdr nlh;
  ifinfomsg ifi;
  rtattr rta alignas(NLMSG_ALIGNTO); // rtattr has to be aligned
  uint32_t rtext;                    // space for IFLA_EXT_MASK
};

NetworkMonitorImplNetlink::NetworkMonitorImplNetlink(boost::asio::io_service& io)
  : m_socket(make_shared<boost::asio::posix::stream_descriptor>(io))
  , m_pid(0)
  , m_sequenceNo(static_cast<uint32_t>(time::system_clock::now().time_since_epoch().count()))
  , m_isEnumeratingLinks(false)
  , m_isEnumeratingAddresses(false)
{
  NDN_LOG_TRACE("creating NETLINK_ROUTE socket");
  initSocket(NETLINK_ROUTE);
  for (auto group : {RTNLGRP_LINK,
                     RTNLGRP_IPV4_IFADDR, RTNLGRP_IPV4_ROUTE,
                     RTNLGRP_IPV6_IFADDR, RTNLGRP_IPV6_ROUTE}) {
    joinGroup(group);
  }

  asyncRead();

  NDN_LOG_TRACE("enumerating links");
  sendDumpRequest(RTM_GETLINK);
  m_isEnumeratingLinks = true;
}

NetworkMonitorImplNetlink::~NetworkMonitorImplNetlink()
{
  boost::system::error_code error;
  m_socket->close(error);
}

shared_ptr<const NetworkInterface>
NetworkMonitorImplNetlink::getNetworkInterface(const std::string& ifname) const
{
  for (const auto& e : m_interfaces) {
    if (e.second->getName() == ifname)
      return e.second;
  }
  return nullptr;
}

std::vector<shared_ptr<const NetworkInterface>>
NetworkMonitorImplNetlink::listNetworkInterfaces() const
{
  std::vector<shared_ptr<const NetworkInterface>> v;
  v.reserve(m_interfaces.size());

  for (const auto& e : m_interfaces) {
    v.push_back(e.second);
  }
  return v;
}

bool
NetworkMonitorImplNetlink::isEnumerating() const
{
  return m_isEnumeratingLinks || m_isEnumeratingAddresses;
}

void
NetworkMonitorImplNetlink::initSocket(int family)
{
  int fd = ::socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, family);
  if (fd < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot create netlink socket ("s + std::strerror(errno) + ")"));
  }
  m_socket->assign(fd);

  // increase socket receive buffer to 1MB to avoid losing messages
  const int bufsize = 1 * 1024 * 1024;
  if (::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) < 0) {
    // not a fatal error
    NDN_LOG_DEBUG("setting SO_RCVBUF failed: " << std::strerror(errno));
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
  const int one = 1;
  if (::setsockopt(fd, SOL_NETLINK, NETLINK_EXT_ACK, &one, sizeof(one)) < 0) {
    // not a fatal error
    NDN_LOG_DEBUG("setting NETLINK_EXT_ACK failed: " << std::strerror(errno));
  }
#endif // NDN_CXX_HAVE_NETLINK_EXT_ACK
}

void
NetworkMonitorImplNetlink::joinGroup(int group)
{
  if (::setsockopt(m_socket->native_handle(), SOL_NETLINK, NETLINK_ADD_MEMBERSHIP,
                   &group, sizeof(group)) < 0) {
    BOOST_THROW_EXCEPTION(Error("Cannot join netlink group " + to_string(group) +
                                " (" + std::strerror(errno) + ")"));
  }
}

void
NetworkMonitorImplNetlink::sendDumpRequest(uint16_t nlmsgType)
{
  auto request = make_shared<RtnlRequest>();
  request->nlh.nlmsg_len = sizeof(RtnlRequest);
  request->nlh.nlmsg_type = nlmsgType;
  request->nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  request->nlh.nlmsg_seq = ++m_sequenceNo;
  request->nlh.nlmsg_pid = m_pid;
  request->ifi.ifi_family = AF_UNSPEC;
  request->rta.rta_type = IFLA_EXT_MASK;
  request->rta.rta_len = RTA_LENGTH(sizeof(request->rtext));
  request->rtext = RTEXT_FILTER_SKIP_STATS;

  boost::asio::async_write(*m_socket, boost::asio::buffer(request.get(), sizeof(RtnlRequest)),
    // capture 'request' to prevent its premature deallocation
    [request] (const boost::system::error_code& error, size_t) {
      if (!error) {
        auto type = request->nlh.nlmsg_type;
        NDN_LOG_TRACE("sent dump request type=" << type << nlmsgTypeToString(type)
                      << " seq=" << request->nlh.nlmsg_seq);
      }
      else if (error != boost::asio::error::operation_aborted) {
        NDN_LOG_ERROR("write failed: " << error.message());
        BOOST_THROW_EXCEPTION(Error("Failed to send netlink request (" + error.message() + ")"));
      }
    });
}

void
NetworkMonitorImplNetlink::asyncRead()
{
  m_socket->async_read_some(boost::asio::buffer(m_buffer),
    // capture a copy of 'm_socket' to prevent its deallocation while the handler is still pending
    [this, socket = m_socket] (auto&&... args) {
      this->handleRead(std::forward<decltype(args)>(args)..., socket);
    });
}

void
NetworkMonitorImplNetlink::handleRead(const boost::system::error_code& error, size_t nBytesRead,
                                      const shared_ptr<boost::asio::posix::stream_descriptor>& socket)
{
  if (!socket->is_open() ||
      error == boost::asio::error::operation_aborted) {
    // socket was closed, ignore the error
    NDN_LOG_TRACE("socket closed or operation aborted");
    return;
  }
  if (error) {
    NDN_LOG_ERROR("read failed: " << error.message());
    BOOST_THROW_EXCEPTION(Error("Netlink socket read failed (" + error.message() + ")"));
  }

  NDN_LOG_TRACE("read " << nBytesRead << " bytes from netlink socket");

  NetlinkMessage nlmsg(m_buffer.data(), nBytesRead);
  for (; nlmsg.isValid(); nlmsg = nlmsg.getNext()) {
    NDN_LOG_TRACE("parsing " << (nlmsg->nlmsg_flags & NLM_F_MULTI ? "multi-part " : "") <<
                  "message type=" << nlmsg->nlmsg_type << nlmsgTypeToString(nlmsg->nlmsg_type) <<
                  " len=" << nlmsg->nlmsg_len <<
                  " seq=" << nlmsg->nlmsg_seq <<
                  " pid=" << nlmsg->nlmsg_pid);

    if (isEnumerating() && (nlmsg->nlmsg_pid != m_pid || nlmsg->nlmsg_seq != m_sequenceNo)) {
      NDN_LOG_TRACE("seq/pid mismatch, ignoring");
      continue;
    }

    if (nlmsg->nlmsg_flags & NLM_F_DUMP_INTR) {
      NDN_LOG_ERROR("netlink dump is inconsistent");
      // TODO: discard the rest of the message and retry the dump
      break;
    }

    if (nlmsg->nlmsg_type == NLMSG_DONE) {
      break;
    }

    parseNetlinkMessage(nlmsg);
  }

  if (nlmsg->nlmsg_type == NLMSG_DONE) {
    if (m_isEnumeratingLinks) {
      // links enumeration complete, now request all the addresses
      m_isEnumeratingLinks = false;
      NDN_LOG_TRACE("enumerating addresses");
      sendDumpRequest(RTM_GETADDR);
      m_isEnumeratingAddresses = true;
    }
    else if (m_isEnumeratingAddresses) {
      // links and addresses enumeration complete
      m_isEnumeratingAddresses = false;
      // TODO: enumerate routes
      NDN_LOG_DEBUG("enumeration complete");
      this->emitSignal(onEnumerationCompleted);
    }
  }

  asyncRead();
}

void
NetworkMonitorImplNetlink::parseNetlinkMessage(const NetlinkMessage& nlmsg)
{
  switch (nlmsg->nlmsg_type) {
  case RTM_NEWLINK:
  case RTM_DELLINK:
    parseLinkMessage(nlmsg);
    if (!isEnumerating())
      this->emitSignal(onNetworkStateChanged); // backward compat
    break;

  case RTM_NEWADDR:
  case RTM_DELADDR:
    parseAddressMessage(nlmsg);
    if (!isEnumerating())
      this->emitSignal(onNetworkStateChanged); // backward compat
    break;

  case RTM_NEWROUTE:
  case RTM_DELROUTE:
    parseRouteMessage(nlmsg);
    if (!isEnumerating())
      this->emitSignal(onNetworkStateChanged); // backward compat
    break;

  case NLMSG_ERROR:
    parseErrorMessage(nlmsg);
    break;
  }
}

static InterfaceType
ifiTypeToInterfaceType(uint16_t type)
{
  switch (type) {
    case ARPHRD_ETHER:
      return InterfaceType::ETHERNET;
    case ARPHRD_LOOPBACK:
      return InterfaceType::LOOPBACK;
    default:
      return InterfaceType::UNKNOWN;
  }
}

static AddressFamily
ifaFamilyToAddressFamily(uint8_t family)
{
  switch (family) {
    case AF_INET:
      return AddressFamily::V4;
    case AF_INET6:
      return AddressFamily::V6;
    default:
      return AddressFamily::UNSPECIFIED;
  }
}

static AddressScope
ifaScopeToAddressScope(uint8_t scope)
{
  switch (scope) {
    case RT_SCOPE_NOWHERE:
      return AddressScope::NOWHERE;
    case RT_SCOPE_HOST:
      return AddressScope::HOST;
    case RT_SCOPE_LINK:
      return AddressScope::LINK;
    default:
      return AddressScope::GLOBAL;
  }
}

void
NetworkMonitorImplNetlink::parseLinkMessage(const NetlinkMessage& nlmsg)
{
  const ifinfomsg* ifi = nlmsg.getPayload<ifinfomsg>();
  if (ifi == nullptr) {
    NDN_LOG_WARN("malformed ifinfomsg");
    return;
  }

  if (ifiTypeToInterfaceType(ifi->ifi_type) == InterfaceType::UNKNOWN) {
    NDN_LOG_DEBUG("unhandled interface type " << ifi->ifi_type);
    return;
  }

  shared_ptr<NetworkInterface> interface;
  auto it = m_interfaces.find(ifi->ifi_index);
  if (it != m_interfaces.end()) {
    interface = it->second;
    BOOST_ASSERT(interface != nullptr);
    BOOST_ASSERT(interface->getIndex() == ifi->ifi_index);
  }

  if (nlmsg->nlmsg_type == RTM_DELLINK) {
    if (interface != nullptr) {
      NDN_LOG_DEBUG("removing interface " << interface->getName());
      m_interfaces.erase(it);
      this->emitSignal(onInterfaceRemoved, interface);
    }
    return;
  }

  if (interface == nullptr) {
    interface = makeNetworkInterface();
    interface->setIndex(ifi->ifi_index);
  }
  interface->setType(ifiTypeToInterfaceType(ifi->ifi_type));
  interface->setFlags(ifi->ifi_flags);

  auto attrs = nlmsg.getAttributes<rtattr>(ifi);
  NDN_LOG_TRACE("message contains " << attrs.size() << " attributes");

  auto address = attrs.getAttributeByType<ethernet::Address>(IFLA_ADDRESS);
  if (address)
    interface->setEthernetAddress(*address);

  auto broadcast = attrs.getAttributeByType<ethernet::Address>(IFLA_BROADCAST);
  if (broadcast)
    interface->setEthernetBroadcastAddress(*broadcast);

  auto name = attrs.getAttributeByType<std::string>(IFLA_IFNAME);
  if (name)
    interface->setName(*name);

  auto mtu = attrs.getAttributeByType<uint32_t>(IFLA_MTU);
  if (mtu)
    interface->setMtu(*mtu);

  auto state = attrs.getAttributeByType<uint8_t>(IFLA_OPERSTATE);
  updateInterfaceState(*interface, state ? *state : linux_if::OPER_STATE_UNKNOWN);

  if (it == m_interfaces.end()) {
    NDN_LOG_DEBUG("adding interface " << interface->getName());
    m_interfaces[interface->getIndex()] = interface;
    this->emitSignal(onInterfaceAdded, interface);
  }
}

void
NetworkMonitorImplNetlink::parseAddressMessage(const NetlinkMessage& nlmsg)
{
  const ifaddrmsg* ifa = nlmsg.getPayload<ifaddrmsg>();
  if (ifa == nullptr) {
    NDN_LOG_WARN("malformed ifaddrmsg");
    return;
  }

  auto it = m_interfaces.find(ifa->ifa_index);
  if (it == m_interfaces.end()) {
    // unknown interface, ignore message
    NDN_LOG_TRACE("unknown interface index " << ifa->ifa_index);
    return;
  }
  auto interface = it->second;
  BOOST_ASSERT(interface != nullptr);

  auto attrs = nlmsg.getAttributes<rtattr>(ifa);
  NDN_LOG_TRACE("message contains " << attrs.size() << " attributes");

  namespace ip = boost::asio::ip;
  ip::address ipAddr, broadcastAddr;
  if (ifa->ifa_family == AF_INET) {
    auto v4 = attrs.getAttributeByType<ip::address_v4>(IFA_LOCAL);
    if (v4)
      ipAddr = *v4;

    v4 = attrs.getAttributeByType<ip::address_v4>(IFA_BROADCAST);
    if (v4)
      broadcastAddr = *v4;
  }
  else if (ifa->ifa_family == AF_INET6) {
    auto v6 = attrs.getAttributeByType<ip::address_v6>(IFA_ADDRESS);
    if (v6) {
      if (v6->is_link_local())
        v6->scope_id(ifa->ifa_index);

      ipAddr = *v6;
    }
  }

  uint32_t flags = ifa->ifa_flags; // overwritten by IFA_FLAGS if supported and present
#ifdef NDN_CXX_HAVE_IFA_FLAGS
  auto extFlags = attrs.getAttributeByType<uint32_t>(IFA_FLAGS);
  if (extFlags)
    flags = *extFlags;
#endif // NDN_CXX_HAVE_IFA_FLAGS

  NetworkAddress address(ifaFamilyToAddressFamily(ifa->ifa_family),
                         ipAddr,
                         broadcastAddr,
                         ifa->ifa_prefixlen,
                         ifaScopeToAddressScope(ifa->ifa_scope),
                         flags);
  BOOST_ASSERT(address.getFamily() != AddressFamily::UNSPECIFIED);

  if (nlmsg->nlmsg_type == RTM_NEWADDR)
    interface->addNetworkAddress(address);
  else if (nlmsg->nlmsg_type == RTM_DELADDR)
    interface->removeNetworkAddress(address);
}

void
NetworkMonitorImplNetlink::parseRouteMessage(const NetlinkMessage& nlmsg)
{
  // TODO
}

void
NetworkMonitorImplNetlink::parseErrorMessage(const NetlinkMessage& nlmsg)
{
  const nlmsgerr* err = nlmsg.getPayload<nlmsgerr>();
  if (err == nullptr) {
    NDN_LOG_WARN("malformed nlmsgerr");
    return;
  }

  if (err->error == 0) {
    // an error code of zero indicates an ACK message, not an error
    NDN_LOG_TRACE("ACK");
    return;
  }

  NDN_LOG_ERROR("NLMSG_ERROR: " << std::strerror(std::abs(err->error)));

#ifdef NDN_CXX_HAVE_NETLINK_EXT_ACK
  if (!(nlmsg->nlmsg_flags & NLM_F_ACK_TLVS))
    return;

  size_t errLen = NLMSG_LENGTH(sizeof(nlmsgerr));
  if (!(nlmsg->nlmsg_flags & NLM_F_CAPPED))
    errLen += err->msg.nlmsg_len - NLMSG_HDRLEN; // don't count the inner nlmsghdr twice

  if (nlmsg->nlmsg_len <= errLen)
    return;

  auto nla = reinterpret_cast<const nlattr*>(reinterpret_cast<const uint8_t*>(&*nlmsg) + errLen);
  auto attrs = NetlinkMessageAttributes<nlattr>(nla, nlmsg->nlmsg_len - errLen);
  auto msg = attrs.getAttributeByType<std::string>(NLMSGERR_ATTR_MSG);
  if (msg)
    NDN_LOG_ERROR("kernel message: " << *msg);
#endif // NDN_CXX_HAVE_NETLINK_EXT_ACK
}

void
NetworkMonitorImplNetlink::updateInterfaceState(NetworkInterface& interface, uint8_t operState)
{
  if (operState == linux_if::OPER_STATE_UP) {
    interface.setState(InterfaceState::RUNNING);
  }
  else if (operState == linux_if::OPER_STATE_DORMANT) {
    interface.setState(InterfaceState::DORMANT);
  }
  else {
    // fallback to flags
    auto flags = interface.getFlags();
    if ((flags & linux_if::FLAG_LOWER_UP) && !(flags & linux_if::FLAG_DORMANT))
      interface.setState(InterfaceState::RUNNING);
    else if (flags & IFF_UP)
      interface.setState(InterfaceState::NO_CARRIER);
    else
      interface.setState(InterfaceState::DOWN);
  }
}

} // namespace net
} // namespace ndn
