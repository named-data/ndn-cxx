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

#ifndef NDN_NET_NETWORK_ADDRESS_HPP
#define NDN_NET_NETWORK_ADDRESS_HPP

#include "../common.hpp"
#include <boost/asio/ip/address.hpp>

namespace ndn {
namespace net {

enum class AddressFamily {
  UNSPECIFIED,
  V4,
  V6,
};

enum class AddressScope {
  NOWHERE,
  HOST,
  LINK,
  GLOBAL,
};

std::ostream&
operator<<(std::ostream& os, AddressScope scope);

/**
 * @brief Stores one IP address supported by a network interface.
 */
class NetworkAddress
{
public:
  NetworkAddress(AddressFamily family,
                 boost::asio::ip::address ip,
                 boost::asio::ip::address broadcast,
                 uint8_t prefixLength,
                 AddressScope scope,
                 uint32_t flags);

  /** @brief Returns the address family
   */
  AddressFamily
  getFamily() const
  {
    return m_family;
  }

  /** @brief Returns the IP address (v4 or v6)
   */
  boost::asio::ip::address
  getIp() const
  {
    return m_ip;
  }

  /** @brief Returns the IP broadcast address
   */
  boost::asio::ip::address
  getBroadcast() const
  {
    return m_broadcast;
  }

  /** @brief Returns the prefix length
   */
  uint8_t
  getPrefixLength() const
  {
    return m_prefixLength;
  }

  /** @brief Returns the address scope
   */
  AddressScope
  getScope() const
  {
    return m_scope;
  }

  /** @brief Returns a bitset of platform-specific flags enabled on the address
   */
  uint32_t
  getFlags() const
  {
    return m_flags;
  }

  friend bool
  operator<(const NetworkAddress& a, const NetworkAddress& b)
  {
    return a.m_ip < b.m_ip;
  }

private:
  AddressFamily m_family;
  boost::asio::ip::address m_ip;
  boost::asio::ip::address m_broadcast;
  uint8_t m_prefixLength;
  AddressScope m_scope;
  uint32_t m_flags; // IFA_F_* in if_addr.h
};

std::ostream&
operator<<(std::ostream& os, const NetworkAddress& address);

} // namespace net
} // namespace ndn

#endif // NDN_NET_NETWORK_ADDRESS_HPP
