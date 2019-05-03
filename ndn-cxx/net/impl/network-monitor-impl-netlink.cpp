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

#include "ndn-cxx/net/impl/network-monitor-impl-netlink.hpp"
#include "ndn-cxx/net/impl/linux-if-constants.hpp"
#include "ndn-cxx/net/impl/netlink-message.hpp"
#include "ndn-cxx/net/network-address.hpp"
#include "ndn-cxx/net/network-interface.hpp"
#include "ndn-cxx/util/logger.hpp"

#include <linux/if_addr.h>
#include <linux/if_link.h>
#include <net/if_arp.h>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#ifndef RTEXT_FILTER_SKIP_STATS
#define RTEXT_FILTER_SKIP_STATS (1 << 3)
#endif

NDN_LOG_INIT(ndn.NetworkMonitor);

namespace ndn {
namespace net {

NetworkMonitorImplNetlink::NetworkMonitorImplNetlink(boost::asio::io_service& io)
  : m_rtnlSocket(io)
  , m_genlSocket(io)
{
  m_rtnlSocket.open();

  for (auto group : {RTNLGRP_LINK,
                     RTNLGRP_IPV4_IFADDR, RTNLGRP_IPV4_ROUTE,
                     RTNLGRP_IPV6_IFADDR, RTNLGRP_IPV6_ROUTE}) {
    m_rtnlSocket.joinGroup(group);
  }
  m_rtnlSocket.registerNotificationCallback([this] (const auto& msg) { this->parseRtnlMessage(msg); });

  enumerateLinks();
}

shared_ptr<const NetworkInterface>
NetworkMonitorImplNetlink::getNetworkInterface(const std::string& ifname) const
{
  for (const auto& interface : m_interfaces | boost::adaptors::map_values) {
    if (interface->getName() == ifname)
      return interface;
  }
  return nullptr;
}

std::vector<shared_ptr<const NetworkInterface>>
NetworkMonitorImplNetlink::listNetworkInterfaces() const
{
  std::vector<shared_ptr<const NetworkInterface>> v;
  v.reserve(m_interfaces.size());
  boost::push_back(v, m_interfaces | boost::adaptors::map_values);
  return v;
}

void
NetworkMonitorImplNetlink::enumerateLinks()
{
  NDN_LOG_TRACE("enumerating links");
  m_phase = ENUMERATING_LINKS;

  struct IfInfoMessage
  {
    alignas(NLMSG_ALIGNTO) ifinfomsg ifi;
    alignas(RTA_ALIGNTO) rtattr rta;
    alignas(RTA_ALIGNTO) uint32_t rtext; // space for IFLA_EXT_MASK
  };

  auto payload = make_shared<IfInfoMessage>();
  payload->ifi.ifi_family = AF_UNSPEC;
  payload->rta.rta_type = IFLA_EXT_MASK;
  payload->rta.rta_len = RTA_LENGTH(sizeof(payload->rtext));
  payload->rtext = RTEXT_FILTER_SKIP_STATS;

  m_rtnlSocket.sendDumpRequest(RTM_GETLINK, payload.get(), sizeof(IfInfoMessage),
                               // capture 'payload' to prevent its premature deallocation
                               [this, payload] (const auto& msg) { this->parseRtnlMessage(msg); });
}

void
NetworkMonitorImplNetlink::enumerateAddrs()
{
  NDN_LOG_TRACE("enumerating addresses");
  m_phase = ENUMERATING_ADDRS;

  struct IfAddrMessage
  {
    alignas(NLMSG_ALIGNTO) ifaddrmsg ifa;
  };

  auto payload = make_shared<IfAddrMessage>();
  payload->ifa.ifa_family = AF_UNSPEC;

  m_rtnlSocket.sendDumpRequest(RTM_GETADDR, payload.get(), sizeof(IfAddrMessage),
                               // capture 'payload' to prevent its premature deallocation
                               [this, payload] (const auto& msg) { this->parseRtnlMessage(msg); });
}

void
NetworkMonitorImplNetlink::enumerateRoutes()
{
  // TODO: enumerate routes
  //NDN_LOG_TRACE("enumerating routes");
  //m_phase = ENUMERATING_ROUTES;

  NDN_LOG_DEBUG("enumeration complete");
  m_phase = ENUMERATION_COMPLETE;
  this->emitSignal(onEnumerationCompleted);
}

void
NetworkMonitorImplNetlink::parseRtnlMessage(const NetlinkMessage& nlmsg)
{
  switch (nlmsg->nlmsg_type) {
  case RTM_NEWLINK:
  case RTM_DELLINK:
    parseLinkMessage(nlmsg);
    if (m_phase == ENUMERATION_COMPLETE)
      this->emitSignal(onNetworkStateChanged); // backward compat
    break;

  case RTM_NEWADDR:
  case RTM_DELADDR:
    parseAddressMessage(nlmsg);
    if (m_phase == ENUMERATION_COMPLETE)
      this->emitSignal(onNetworkStateChanged); // backward compat
    break;

  case RTM_NEWROUTE:
  case RTM_DELROUTE:
    parseRouteMessage(nlmsg);
    if (m_phase == ENUMERATION_COMPLETE)
      this->emitSignal(onNetworkStateChanged); // backward compat
    break;

  case NLMSG_DONE:
    parseDoneMessage(nlmsg);
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

static void
updateInterfaceState(NetworkInterface& interface, uint8_t operState)
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

#ifdef NDN_CXX_HAVE_NETLINK_EXT_ACK
static void
parseExtAckAttributes(const NetlinkMessageAttributes<nlattr>& attrs, bool isError)
{
  NDN_LOG_TRACE("  message contains " << attrs.size() << " attributes");

  auto msg = attrs.getAttributeByType<std::string>(NLMSGERR_ATTR_MSG);
  if (msg && !msg->empty()) {
    if (isError)
      NDN_LOG_ERROR("  extended err: " << *msg);
    else
      NDN_LOG_DEBUG("  extended msg: " << *msg);
  }
}
#endif // NDN_CXX_HAVE_NETLINK_EXT_ACK

void
NetworkMonitorImplNetlink::parseLinkMessage(const NetlinkMessage& nlmsg)
{
  const ifinfomsg* ifi = nlmsg.getPayload<ifinfomsg>();
  if (ifi == nullptr) {
    NDN_LOG_WARN("malformed ifinfomsg");
    return;
  }

  if (ifiTypeToInterfaceType(ifi->ifi_type) == InterfaceType::UNKNOWN) {
    NDN_LOG_DEBUG("  unhandled interface type " << ifi->ifi_type);
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
      NDN_LOG_DEBUG("  removing interface " << interface->getName());
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
  NDN_LOG_TRACE("  message contains " << attrs.size() << " attributes");

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
    NDN_LOG_DEBUG("  adding interface " << interface->getName());
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
    NDN_LOG_TRACE("  unknown interface index " << ifa->ifa_index);
    return;
  }
  auto interface = it->second;
  BOOST_ASSERT(interface != nullptr);

  auto attrs = nlmsg.getAttributes<rtattr>(ifa);
  NDN_LOG_TRACE("  message contains " << attrs.size() << " attributes");

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
NetworkMonitorImplNetlink::parseDoneMessage(const NetlinkMessage& nlmsg)
{
  const int* errcode = nlmsg.getPayload<int>();
  if (errcode == nullptr) {
    NDN_LOG_WARN("malformed NLMSG_DONE");
  }
  else {
    if (*errcode != 0) {
      NDN_LOG_ERROR("NLMSG_DONE err=" << *errcode << " " << std::strerror(std::abs(*errcode)));
    }
#ifdef NDN_CXX_HAVE_NETLINK_EXT_ACK
    if (nlmsg->nlmsg_flags & NLM_F_ACK_TLVS) {
      parseExtAckAttributes(nlmsg.getAttributes<nlattr>(errcode), *errcode != 0);
    }
#endif // NDN_CXX_HAVE_NETLINK_EXT_ACK
  }

  switch (m_phase) {
  case ENUMERATING_LINKS:
    // links enumeration complete, now request all the addresses
    enumerateAddrs();
    break;
  case ENUMERATING_ADDRS:
    // links and addresses enumeration complete, now request all the routes
    enumerateRoutes();
    break;
  default:
    break;
  }
}

void
NetworkMonitorImplNetlink::parseErrorMessage(const NetlinkMessage& nlmsg)
{
  const nlmsgerr* err = nlmsg.getPayload<nlmsgerr>();
  if (err == nullptr) {
    NDN_LOG_WARN("malformed NLMSG_ERROR");
    return;
  }

  if (err->error != 0)
    NDN_LOG_ERROR("NLMSG_ERROR for seq=" << err->msg.nlmsg_seq
                  << " err=" << err->error << " " << std::strerror(std::abs(err->error)));

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
  parseExtAckAttributes(attrs, err->error != 0);
#endif // NDN_CXX_HAVE_NETLINK_EXT_ACK
}

} // namespace net
} // namespace ndn
