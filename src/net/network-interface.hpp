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

#ifndef NDN_NET_NETWORK_INTERFACE_HPP
#define NDN_NET_NETWORK_INTERFACE_HPP

#include "ethernet.hpp"
#include "network-address.hpp"
#include "../util/signal.hpp"
#include <set>

namespace ndn {
namespace net {

/** @brief Indicates the hardware type of a network interface
 */
enum class InterfaceType {
  UNKNOWN,
  LOOPBACK,
  ETHERNET,
  // we do not support anything else for now
};

std::ostream&
operator<<(std::ostream& os, InterfaceType type);

/** @brief Indicates the state of a network interface
 */
enum class InterfaceState {
  UNKNOWN,    ///< interface is in an unknown state
  DOWN,       ///< interface is administratively down
  NO_CARRIER, ///< interface is administratively up but has no carrier
  DORMANT,    ///< interface has a carrier but it cannot send or receive normal user traffic yet
  RUNNING,    ///< interface can be used to send and receive packets
};

std::ostream&
operator<<(std::ostream& os, InterfaceState state);

/**
 * @brief Represents one network interface attached to the host.
 *
 * Each network interface has a unique index, a name, and a set of flags indicating its
 * capabilities and current state. It may contain one hardware (Ethernet) address, and
 * zero or more network-layer (IP) addresses. Specific signals are emitted when the
 * interface data change.
 */
class NetworkInterface
{
public: // signals, marked 'mutable' so they can be connected on 'const NetworkInterface'
  /** @brief Fires when interface state changes
   */
  mutable util::Signal<NetworkInterface, InterfaceState /*old*/, InterfaceState /*new*/> onStateChanged;

  /** @brief Fires when interface mtu changes
   */
  mutable util::Signal<NetworkInterface, uint32_t /*old*/, uint32_t /*new*/> onMtuChanged;

  /** @brief Fires when a network-layer address is added to the interface
   */
  mutable util::Signal<NetworkInterface, NetworkAddress> onAddressAdded;

  /** @brief Fires when a network-layer address is removed from the interface
   */
  mutable util::Signal<NetworkInterface, NetworkAddress> onAddressRemoved;

public: // getters
  /** @brief Returns an opaque ID that uniquely identifies the interface on the system
   */
  int
  getIndex() const
  {
    return m_index;
  }

  /** @brief Returns the name of the interface, unique on the system
   */
  std::string
  getName() const
  {
    return m_name;
  }

  /** @brief Returns the hardware type of the interface
   */
  InterfaceType
  getType() const
  {
    return m_type;
  }

  /** @brief Returns a bitset of platform-specific flags enabled on the interface
   */
  uint32_t
  getFlags() const
  {
    return m_flags;
  }

  /** @brief Returns the current state of the interface
   */
  InterfaceState
  getState() const
  {
    return m_state;
  }

  /** @brief Returns the MTU (maximum transmission unit) of the interface
   */
  uint32_t
  getMtu() const
  {
    return m_mtu;
  }

  /** @brief Returns the link-layer (Ethernet) address of the interface
   */
  ethernet::Address
  getEthernetAddress() const
  {
    return m_etherAddress;
  }

  /** @brief Returns the link-layer (Ethernet) broadcast address of the interface
   */
  ethernet::Address
  getEthernetBroadcastAddress() const
  {
    return m_etherBrdAddress;
  }

  /** @brief Returns a list of all network-layer addresses present on the interface
   */
  const std::set<NetworkAddress>&
  getNetworkAddresses() const
  {
    return m_netAddresses;
  }

  /** @brief Returns true if the interface is a loopback interface
   */
  bool
  isLoopback() const
  {
    return (m_flags & IFF_LOOPBACK) != 0;
  }

  /** @brief Returns true if the interface is a point-to-point interface
   */
  bool
  isPointToPoint() const
  {
    return (m_flags & IFF_POINTOPOINT) != 0;
  }

  /** @brief Returns true if the interface supports broadcast communication
   */
  bool
  canBroadcast() const
  {
    return (m_flags & IFF_BROADCAST) != 0;
  }

  /** @brief Returns true if the interface supports multicast communication
   */
  bool
  canMulticast() const
  {
    return (m_flags & IFF_MULTICAST) != 0;
  }

  /** @brief Returns true if the interface is administratively up
   */
  bool
  isUp() const
  {
    return (m_flags & IFF_UP) != 0;
  }

public: // modifiers: they update information on this instance, but do not change netif in the OS
  bool
  addNetworkAddress(const NetworkAddress& address);

  bool
  removeNetworkAddress(const NetworkAddress& address);

  void
  setIndex(int index);

  void
  setName(const std::string& name);

  void
  setType(InterfaceType type);

  void
  setFlags(uint32_t flags);

  void
  setState(InterfaceState state);

  void
  setMtu(uint32_t mtu);

  void
  setEthernetAddress(const ethernet::Address& address);

  void
  setEthernetBroadcastAddress(const ethernet::Address& address);

private: // constructor
  NetworkInterface(); // accessible through NetworkMonitorImpl::makeNetworkInterface

private:
  int m_index;
  std::string m_name;
  InterfaceType m_type;
  uint32_t m_flags; // IFF_* in <net/if.h>
  InterfaceState m_state;
  uint32_t m_mtu;
  ethernet::Address m_etherAddress;
  ethernet::Address m_etherBrdAddress;
  std::set<NetworkAddress> m_netAddresses;

  friend class NetworkMonitorImpl;
};

std::ostream&
operator<<(std::ostream& os, const NetworkInterface& interface);

} // namespace net
} // namespace ndn

#endif // NDN_NET_NETWORK_INTERFACE_HPP
