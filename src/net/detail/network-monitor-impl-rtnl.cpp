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
 *
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#include "network-monitor-impl-rtnl.hpp"
#include "linux-if-constants.hpp"
#include "../network-address.hpp"
#include "../network-interface.hpp"
#include "../../util/logger.hpp"
#include "../../util/time.hpp"

#include <boost/asio/write.hpp>

#include <cerrno>
#include <cstdlib>
#include <net/if_arp.h>
#include <sys/socket.h>

NDN_LOG_INIT(ndn.NetworkMonitor);

namespace ndn {
namespace net {

NetworkMonitorImplRtnl::NetworkMonitorImplRtnl(boost::asio::io_service& io)
  : m_socket(make_shared<boost::asio::posix::stream_descriptor>(io))
  , m_pid(0)
  , m_sequenceNo(static_cast<uint32_t>(time::system_clock::now().time_since_epoch().count()))
  , m_isEnumeratingLinks(false)
  , m_isEnumeratingAddresses(false)
{
  initSocket();
  asyncRead();

  NDN_LOG_TRACE("enumerating links");
  sendDumpRequest(RTM_GETLINK);
  m_isEnumeratingLinks = true;
}

NetworkMonitorImplRtnl::~NetworkMonitorImplRtnl()
{
  boost::system::error_code error;
  m_socket->close(error);
}

shared_ptr<const NetworkInterface>
NetworkMonitorImplRtnl::getNetworkInterface(const std::string& ifname) const
{
  for (const auto& e : m_interfaces) {
    if (e.second->getName() == ifname)
      return e.second;
  }
  return nullptr;
}

std::vector<shared_ptr<const NetworkInterface>>
NetworkMonitorImplRtnl::listNetworkInterfaces() const
{
  std::vector<shared_ptr<const NetworkInterface>> v;
  v.reserve(m_interfaces.size());

  for (const auto& e : m_interfaces) {
    v.push_back(e.second);
  }
  return v;
}

bool
NetworkMonitorImplRtnl::isEnumerating() const
{
  return m_isEnumeratingLinks || m_isEnumeratingAddresses;
}

void
NetworkMonitorImplRtnl::initSocket()
{
  NDN_LOG_TRACE("creating netlink socket");

  int fd = ::socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);
  if (fd < 0) {
    BOOST_THROW_EXCEPTION(Error(std::string("Cannot create netlink socket (") +
                                std::strerror(errno) + ")"));
  }
  m_socket->assign(fd);

  sockaddr_nl addr{};
  addr.nl_family = AF_NETLINK;
  addr.nl_groups = RTMGRP_LINK | RTMGRP_NOTIFY |
                   RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE |
                   RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;
  if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    BOOST_THROW_EXCEPTION(Error(std::string("Cannot bind netlink socket (") +
                                std::strerror(errno) + ")"));
  }

  // find out what pid has been assigned to us
  socklen_t len = sizeof(addr);
  if (::getsockname(fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0) {
    BOOST_THROW_EXCEPTION(Error(std::string("Cannot obtain netlink socket address (") +
                                std::strerror(errno) + ")"));
  }
  if (len != sizeof(addr)) {
    BOOST_THROW_EXCEPTION(Error("Wrong address length (" + to_string(len) + ")"));
  }
  if (addr.nl_family != AF_NETLINK) {
    BOOST_THROW_EXCEPTION(Error("Wrong address family (" + to_string(addr.nl_family) + ")"));
  }
  m_pid = addr.nl_pid;
  NDN_LOG_TRACE("our pid is " << m_pid);
}

void
NetworkMonitorImplRtnl::sendDumpRequest(uint16_t nlmsgType)
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
  request->rtext = 1 << 3; // RTEXT_FILTER_SKIP_STATS

  boost::asio::async_write(*m_socket, boost::asio::buffer(request.get(), sizeof(RtnlRequest)),
    // capture 'request' to prevent its premature deallocation
    [request] (const boost::system::error_code& error, size_t) {
      if (error && error != boost::asio::error::operation_aborted) {
        NDN_LOG_ERROR("write failed: " << error.message());
        BOOST_THROW_EXCEPTION(Error("Failed to send netlink request (" + error.message() + ")"));
      }
    });
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
    RTM_STRINGIFY(NEWADDR);
    RTM_STRINGIFY(DELADDR);
    RTM_STRINGIFY(NEWROUTE);
    RTM_STRINGIFY(DELROUTE);
    default:
      return "";
  }
#undef NLMSG_STRINGIFY
#undef RTM_STRINGIFY
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
NetworkMonitorImplRtnl::asyncRead()
{
  m_socket->async_read_some(boost::asio::buffer(m_buffer),
                            bind(&NetworkMonitorImplRtnl::handleRead, this, _1, _2, m_socket));
}

void
NetworkMonitorImplRtnl::handleRead(const boost::system::error_code& error, size_t nBytesRead,
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

  const nlmsghdr* nlh = reinterpret_cast<const nlmsghdr*>(m_buffer.data());
  if (!isEnumerating() || (nlh->nlmsg_seq == m_sequenceNo && nlh->nlmsg_pid == m_pid)) {
    parseNetlinkMessage(nlh, nBytesRead);
  }
  else {
    NDN_LOG_TRACE("seq/pid mismatch, ignoring");
  }

  asyncRead();
}

void
NetworkMonitorImplRtnl::parseNetlinkMessage(const nlmsghdr* nlh, size_t len)
{
  while (NLMSG_OK(nlh, len)) {
    NDN_LOG_TRACE("parsing " << (nlh->nlmsg_flags & NLM_F_MULTI ? "multi-part " : "") <<
                  "message type=" << nlh->nlmsg_type << nlmsgTypeToString(nlh->nlmsg_type) <<
                  " len=" << nlh->nlmsg_len <<
                  " seq=" << nlh->nlmsg_seq <<
                  " pid=" << nlh->nlmsg_pid);

    if (nlh->nlmsg_flags & NLM_F_DUMP_INTR) {
      NDN_LOG_ERROR("netlink dump was interrupted");
      // TODO: technically we should retry the dump...
      break;
    }

    if (nlh->nlmsg_type == NLMSG_DONE)
      break;

    switch (nlh->nlmsg_type) {
      case RTM_NEWLINK:
      case RTM_DELLINK:
        parseLinkMessage(nlh, reinterpret_cast<const ifinfomsg*>(NLMSG_DATA(nlh)));
        if (!isEnumerating())
          this->emitSignal(onNetworkStateChanged); // backward compat
        break;

      case RTM_NEWADDR:
      case RTM_DELADDR:
        parseAddressMessage(nlh, reinterpret_cast<const ifaddrmsg*>(NLMSG_DATA(nlh)));
        if (!isEnumerating())
          this->emitSignal(onNetworkStateChanged); // backward compat
        break;

      case RTM_NEWROUTE:
      case RTM_DELROUTE:
        parseRouteMessage(nlh, reinterpret_cast<const rtmsg*>(NLMSG_DATA(nlh)));
        if (!isEnumerating())
          this->emitSignal(onNetworkStateChanged); // backward compat
        break;

      case NLMSG_ERROR: {
        const nlmsgerr* err = reinterpret_cast<const nlmsgerr*>(NLMSG_DATA(nlh));
        if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(nlmsgerr)))
          NDN_LOG_ERROR("truncated NLMSG_ERROR");
        else if (err->error == 0)
          // an error code of zero indicates an ACK message, not an error
          NDN_LOG_TRACE("ACK");
        else
          NDN_LOG_ERROR("NLMSG_ERROR: " << std::strerror(std::abs(err->error)));
        break;
      }
    }

    nlh = NLMSG_NEXT(nlh, len);
  }

  if (nlh->nlmsg_type == NLMSG_DONE && m_isEnumeratingLinks) {
    // links enumeration complete, now request all the addresses
    m_isEnumeratingLinks = false;
    NDN_LOG_TRACE("enumerating addresses");
    sendDumpRequest(RTM_GETADDR);
    m_isEnumeratingAddresses = true;
  }
  else if (nlh->nlmsg_type == NLMSG_DONE && m_isEnumeratingAddresses) {
    // links and addresses enumeration complete
    m_isEnumeratingAddresses = false;
    // TODO: enumerate routes
    NDN_LOG_DEBUG("enumeration complete");
    this->emitSignal(onEnumerationCompleted);
  }
}

void
NetworkMonitorImplRtnl::parseLinkMessage(const nlmsghdr* nlh, const ifinfomsg* ifi)
{
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

  if (nlh->nlmsg_type == RTM_DELLINK) {
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

  const rtattr* rta = reinterpret_cast<const rtattr*>(IFLA_RTA(ifi));
  size_t rtaTotalLen = IFLA_PAYLOAD(nlh);
  uint8_t operState = linux_if::OPER_STATE_UNKNOWN;

  while (RTA_OK(rta, rtaTotalLen)) {
    size_t attrLen = RTA_PAYLOAD(rta);

    switch (rta->rta_type) {
      case IFLA_ADDRESS:
        if (attrLen == ethernet::ADDR_LEN) {
          ethernet::Address addr(reinterpret_cast<const uint8_t*>(RTA_DATA(rta)));
          interface->setEthernetAddress(addr);
        }
        break;

      case IFLA_BROADCAST:
        if (attrLen == ethernet::ADDR_LEN) {
          ethernet::Address addr(reinterpret_cast<const uint8_t*>(RTA_DATA(rta)));
          interface->setEthernetBroadcastAddress(addr);
        }
        break;

      case IFLA_IFNAME: {
        auto attrData = reinterpret_cast<const char*>(RTA_DATA(rta));
        if (::strnlen(attrData, attrLen) <= attrLen)
          interface->setName(attrData);
        break;
      }

      case IFLA_MTU:
        if (attrLen == sizeof(uint32_t))
          interface->setMtu(*(reinterpret_cast<const uint32_t*>(RTA_DATA(rta))));
        break;

      case IFLA_OPERSTATE:
        if (attrLen == sizeof(uint8_t))
          operState = *(reinterpret_cast<const uint8_t*>RTA_DATA(rta));
        break;
    }

    rta = RTA_NEXT(rta, rtaTotalLen);
  }

  updateInterfaceState(*interface, operState);

  if (it == m_interfaces.end()) {
    NDN_LOG_DEBUG("adding interface " << interface->getName());
    m_interfaces[interface->getIndex()] = interface;
    this->emitSignal(onInterfaceAdded, interface);
  }
}

void
NetworkMonitorImplRtnl::parseAddressMessage(const nlmsghdr* nlh, const ifaddrmsg* ifa)
{
  auto it = m_interfaces.find(ifa->ifa_index);
  if (it == m_interfaces.end()) {
    // unknown interface, ignore message
    NDN_LOG_TRACE("unknown interface index " << ifa->ifa_index);
    return;
  }
  auto interface = it->second;
  BOOST_ASSERT(interface != nullptr);

  namespace ip = boost::asio::ip;
  ip::address ipAddr, broadcastAddr;
  uint32_t flags = ifa->ifa_flags; // will be overridden by IFA_FLAGS if the attribute is present

  const rtattr* rta = reinterpret_cast<const rtattr*>(IFA_RTA(ifa));
  size_t rtaTotalLen = IFA_PAYLOAD(nlh);

  while (RTA_OK(rta, rtaTotalLen)) {
    auto attrData = reinterpret_cast<const unsigned char*>(RTA_DATA(rta));
    size_t attrLen = RTA_PAYLOAD(rta);

    switch (rta->rta_type) {
      case IFA_LOCAL:
        if (ifa->ifa_family == AF_INET && attrLen == sizeof(ip::address_v4::bytes_type)) {
          ip::address_v4::bytes_type bytes;
          std::copy_n(attrData, bytes.size(), bytes.begin());
          ipAddr = ip::address_v4(bytes);
        }
        break;

      case IFA_ADDRESS:
        if (ifa->ifa_family == AF_INET6 && attrLen == sizeof(ip::address_v6::bytes_type)) {
          ip::address_v6::bytes_type bytes;
          std::copy_n(attrData, bytes.size(), bytes.begin());
          ip::address_v6 v6Addr(bytes);
          if (v6Addr.is_link_local())
            v6Addr.scope_id(ifa->ifa_index);
          ipAddr = v6Addr;
        }
        break;

      case IFA_BROADCAST:
        if (ifa->ifa_family == AF_INET && attrLen == sizeof(ip::address_v4::bytes_type)) {
          ip::address_v4::bytes_type bytes;
          std::copy_n(attrData, bytes.size(), bytes.begin());
          broadcastAddr = ip::address_v4(bytes);
        }
        break;

#ifdef NDN_CXX_HAVE_IFA_FLAGS
      case IFA_FLAGS:
        if (attrLen == sizeof(uint32_t))
          flags = *(reinterpret_cast<const uint32_t*>(attrData));
        break;
#endif // NDN_CXX_HAVE_IFA_FLAGS
    }

    rta = RTA_NEXT(rta, rtaTotalLen);
  }

  NetworkAddress address(ifaFamilyToAddressFamily(ifa->ifa_family),
                         ipAddr,
                         broadcastAddr,
                         ifa->ifa_prefixlen,
                         ifaScopeToAddressScope(ifa->ifa_scope),
                         flags);
  BOOST_ASSERT(address.getFamily() != AddressFamily::UNSPECIFIED);

  if (nlh->nlmsg_type == RTM_NEWADDR)
    interface->addNetworkAddress(address);
  else if (nlh->nlmsg_type == RTM_DELADDR)
    interface->removeNetworkAddress(address);
}

void
NetworkMonitorImplRtnl::parseRouteMessage(const nlmsghdr* nlh, const rtmsg* rtm)
{
  // TODO
}

void
NetworkMonitorImplRtnl::updateInterfaceState(NetworkInterface& interface, uint8_t operState)
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
