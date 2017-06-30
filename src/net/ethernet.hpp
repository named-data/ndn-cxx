/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2017 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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
 */

#ifndef NDN_NET_ETHERNET_HPP
#define NDN_NET_ETHERNET_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace ndn {
namespace ethernet {

const uint16_t ETHERTYPE_NDN = 0x8624;

const size_t ADDR_LEN     = 6;      ///< Octets in one Ethernet address
const size_t TYPE_LEN     = 2;      ///< Octets in Ethertype field
const size_t HDR_LEN      = 14;     ///< Total octets in Ethernet header (without 802.1Q tag)
const size_t TAG_LEN      = 4;      ///< Octets in 802.1Q tag (TPID + priority + VLAN)
const size_t MIN_DATA_LEN = 46;     ///< Min octets in Ethernet payload (assuming no 802.1Q tag)
const size_t MAX_DATA_LEN = 1500;   ///< Max octets in Ethernet payload
const size_t CRC_LEN      = 4;      ///< Octets in Ethernet frame check sequence


/** \brief represents an Ethernet hardware address
 */
class Address : public std::array<uint8_t, ADDR_LEN>
{
public:
  /// Constructs a null Ethernet address (00:00:00:00:00:00)
  Address();

  /// Constructs a new Ethernet address with the given octets
  Address(uint8_t a1, uint8_t a2, uint8_t a3,
          uint8_t a4, uint8_t a5, uint8_t a6);

  /// Constructs a new Ethernet address with the given octets
  explicit
  Address(const uint8_t octets[ADDR_LEN]);

  /// True if this is a broadcast address (ff:ff:ff:ff:ff:ff)
  bool
  isBroadcast() const;

  /// True if this is a multicast address
  bool
  isMulticast() const;

  /// True if this is a null address (00:00:00:00:00:00)
  bool
  isNull() const;

  /**
   * \brief Converts the address to a human-readable string
   *
   * \param sep A character used to visually separate the octets,
   *            usually ':' (the default value) or '-'
   */
  std::string
  toString(char sep = ':') const;

  /**
   * \brief Creates an Address from a string containing an Ethernet address
   *        in hexadecimal notation, with colons or hyphens as separators
   *
   * \param str The string to be parsed
   * \return Always an instance of Address, which will be null
   *         if the parsing fails
   */
  static Address
  fromString(const std::string& str);
};

/// Returns the Ethernet broadcast address (ff:ff:ff:ff:ff:ff)
Address
getBroadcastAddress();

/// Returns the default Ethernet multicast address for NDN
Address
getDefaultMulticastAddress();

std::ostream&
operator<<(std::ostream& o, const Address& a);

} // namespace ethernet
} // namespace ndn

namespace std {

// specialize std::hash<> for ethernet::Address
template<>
struct hash<ndn::ethernet::Address>
{
  size_t
  operator()(const ndn::ethernet::Address& a) const noexcept;
};

} // namespace std

#endif // NDN_NET_ETHERNET_HPP
