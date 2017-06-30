/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "network-interface.hpp"
#include "detail/linux-if-constants.hpp"
#include "../util/logger.hpp"
#include "../util/string-helper.hpp"

#include <net/if.h>

NDN_LOG_INIT(ndn.NetworkMonitor);

namespace ndn {
namespace net {

NetworkInterface::NetworkInterface()
  : m_index(0)
  , m_type(InterfaceType::UNKNOWN)
  , m_flags(0)
  , m_state(InterfaceState::UNKNOWN)
  , m_mtu(0)
{
}

bool
NetworkInterface::addNetworkAddress(const NetworkAddress& address)
{
  if (!address.getIp().is_unspecified()) {
    // need to erase the existing address before inserting
    // because the address flags may have changed
    bool isNew = m_netAddresses.erase(address) == 0;
    m_netAddresses.insert(address);
    if (isNew) {
      NDN_LOG_DEBUG("added address " << address << " to " << m_name);
      onAddressAdded(address);
      return true;
    }
  }
  return false;
}

bool
NetworkInterface::removeNetworkAddress(const NetworkAddress& address)
{
  if (m_netAddresses.erase(address) > 0) {
    NDN_LOG_DEBUG("removed address " << address << " from " << m_name);
    onAddressRemoved(address);
    return true;
  }
  return false;
}

void
NetworkInterface::setIndex(int index)
{
  m_index = index;
}

void
NetworkInterface::setName(const std::string& name)
{
  BOOST_ASSERT(!name.empty());
  m_name = name;
}

void
NetworkInterface::setType(InterfaceType type)
{
  m_type = type;
}

void
NetworkInterface::setFlags(uint32_t flags)
{
  m_flags = flags;
}

void
NetworkInterface::setState(InterfaceState state)
{
  if (m_state != state) {
    std::swap(m_state, state);
    onStateChanged(state, m_state);
  }
}

void
NetworkInterface::setMtu(uint32_t mtu)
{
  if (m_mtu != mtu) {
    std::swap(m_mtu, mtu);
    onMtuChanged(mtu, m_mtu);
  }
}

void
NetworkInterface::setEthernetAddress(const ethernet::Address& address)
{
  m_etherAddress = address;
}

void
NetworkInterface::setEthernetBroadcastAddress(const ethernet::Address& address)
{
  m_etherBrdAddress = address;
}

std::ostream&
operator<<(std::ostream& os, InterfaceType type)
{
  switch (type) {
    case InterfaceType::UNKNOWN:
      return os << "unknown";
    case InterfaceType::LOOPBACK:
      return os << "loopback";
    case InterfaceType::ETHERNET:
      return os << "ether";
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, InterfaceState state)
{
  switch (state) {
    case InterfaceState::UNKNOWN:
      return os << "unknown";
    case InterfaceState::DOWN:
      return os << "down";
    case InterfaceState::NO_CARRIER:
      return os << "no-carrier";
    case InterfaceState::DORMANT:
      return os << "dormant";
    case InterfaceState::RUNNING:
      return os << "running";
  }
  return os;
}

static void
printFlag(std::ostream& os, uint32_t& flags, uint32_t flagVal, const char* flagStr)
{
  if (flags & flagVal) {
    flags &= ~flagVal;
    os << flagStr << (flags ? "," : "");
  }
}

std::ostream&
operator<<(std::ostream& os, const NetworkInterface& netif)
{
  os << netif.getIndex() << ": " << netif.getName() << ": ";

  auto flags = netif.getFlags();
  os << "<";
#define PRINT_IFF(flag) printFlag(os, flags, IFF_##flag, #flag)
  PRINT_IFF(UP);
  PRINT_IFF(BROADCAST);
  PRINT_IFF(DEBUG);
  PRINT_IFF(LOOPBACK);
  PRINT_IFF(POINTOPOINT);
#if defined(IFF_NOTRAILERS)
  PRINT_IFF(NOTRAILERS);
#endif
  PRINT_IFF(RUNNING);
  PRINT_IFF(NOARP);
  PRINT_IFF(PROMISC);
  PRINT_IFF(ALLMULTI);
  PRINT_IFF(MULTICAST);
#if defined(__linux__)
  PRINT_IFF(MASTER);
  PRINT_IFF(SLAVE);
  PRINT_IFF(PORTSEL);
  PRINT_IFF(AUTOMEDIA);
  PRINT_IFF(DYNAMIC);
#elif defined(__APPLE__) || defined(__FreeBSD__)
  PRINT_IFF(OACTIVE);
  PRINT_IFF(SIMPLEX);
  PRINT_IFF(LINK0);
  PRINT_IFF(LINK1);
  PRINT_IFF(LINK2);
#endif
#if defined(__FreeBSD__)
  PRINT_IFF(CANTCONFIG);
  PRINT_IFF(PPROMISC);
  PRINT_IFF(MONITOR);
  PRINT_IFF(STATICARP);
  PRINT_IFF(DYING);
  PRINT_IFF(RENAMING);
#endif
#undef PRINT_IFF
#if defined(__linux__)
#define PRINT_IF_FLAG(flag) printFlag(os, flags, linux_if::FLAG_##flag, #flag)
  PRINT_IF_FLAG(LOWER_UP);
  PRINT_IF_FLAG(DORMANT);
  PRINT_IF_FLAG(ECHO);
#undef PRINT_IF_FLAG
#endif
  if (flags) {
    // print unknown flags in hex
    os << AsHex{flags};
  }
  os << ">";

  os << " state " << netif.getState() << " mtu " << netif.getMtu() << "\n"
     << "    link/" << netif.getType() << " " << netif.getEthernetAddress()
     << " brd " << netif.getEthernetBroadcastAddress() << "\n";

  for (const auto& addr : netif.getNetworkAddresses()) {
    os << "    " << (addr.getFamily() == AddressFamily::V4 ? "inet " : "inet6 ") << addr;
    if (netif.canBroadcast() && !addr.getBroadcast().is_unspecified())
      os << " brd " << addr.getBroadcast();
    os << " scope " << addr.getScope() << "\n";
  }

  return os;
}

} // namespace net
} // namespace ndn
