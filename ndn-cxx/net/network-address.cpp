/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/net/network-address.hpp"

#ifdef __linux__
#include <linux/if_addr.h>
#endif

namespace ndn {
namespace net {

std::ostream&
operator<<(std::ostream& os, AddressScope scope)
{
  switch (scope) {
    case AddressScope::NOWHERE:
      return os << "nowhere";
    case AddressScope::HOST:
      return os << "host";
    case AddressScope::LINK:
      return os << "link";
    case AddressScope::GLOBAL:
      return os << "global";
  }
  return os;
}

NetworkAddress::NetworkAddress(AddressFamily family,
                               const boost::asio::ip::address& ip,
                               const boost::asio::ip::address& broadcast,
                               uint8_t prefixLength,
                               AddressScope scope,
                               uint32_t flags)
  : m_family(family)
  , m_ip(ip)
  , m_broadcast(broadcast)
  , m_prefixLength(prefixLength)
  , m_scope(scope)
  , m_flags(flags)
{
}

bool
NetworkAddress::isDeprecated() const
{
#ifdef __linux__
  return m_flags & IFA_F_DEPRECATED;
#else
  // should probably check for IN6_IFF_DEPRECATED on macOS and FreeBSD, but the
  // NetworkMonitor backend for those platforms doesn't provide any address flags
  return false;
#endif
}

std::ostream&
operator<<(std::ostream& os, const NetworkAddress& addr)
{
  return os << addr.getIp() << '/' << static_cast<unsigned int>(addr.getPrefixLength());
}

} // namespace net
} // namespace ndn
