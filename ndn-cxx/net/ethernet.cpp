/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2022 Regents of the University of California,
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

#include "ndn-cxx/net/ethernet.hpp"

#include <boost/functional/hash.hpp>

#include <cinttypes>
#include <cstdio>
#include <ostream>

namespace ndn {
namespace ethernet {

Address::Address()
{
  fill(0);
}

Address::Address(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6)
{
  data()[0] = a1;
  data()[1] = a2;
  data()[2] = a3;
  data()[3] = a4;
  data()[4] = a5;
  data()[5] = a6;
}

Address::Address(const uint8_t octets[ADDR_LEN])
{
  std::copy(octets, octets + size(), begin());
}

bool
Address::isBroadcast() const
{
  return *this == getBroadcastAddress();
}

bool
Address::isMulticast() const
{
  return (front() & 1) != 0;
}

bool
Address::isNull() const
{
  return *this == Address();
}

std::string
Address::toString(char sep) const
{
  const auto& a = *this;
  char s[18]; // 12 digits + 5 separators + null terminator

  std::snprintf(s, sizeof(s),
                "%02" PRIx8 "%c%02" PRIx8 "%c%02" PRIx8 "%c%02" PRIx8 "%c%02" PRIx8 "%c%02" PRIx8,
                a[0], sep, a[1], sep, a[2], sep, a[3], sep, a[4], sep, a[5]);
  return s;
}

Address
Address::fromString(const std::string& str)
{
  Address a;
  char sep[5][2]; // 5 * (1 separator char + 1 null terminator)
  int n = 0; // num of chars read from the input string

  int ret = std::sscanf(str.data(),
                        "%2" SCNx8 "%1[:-]%2" SCNx8 "%1[:-]%2" SCNx8 "%1[:-]"
                        "%2" SCNx8 "%1[:-]%2" SCNx8 "%1[:-]%2" SCNx8 "%n",
                        &a[0], &sep[0][0], &a[1], &sep[1][0], &a[2], &sep[2][0],
                        &a[3], &sep[3][0], &a[4], &sep[4][0], &a[5], &n);

  if (ret < 11 || static_cast<size_t>(n) != str.length())
    return {};

  // check that all separators are the same char (: or -)
  for (size_t i = 1; i < 5; ++i) {
    if (sep[i][0] != sep[0][0])
      return {};
  }

  return a;
}

Address
getBroadcastAddress()
{
  return { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
}

Address
getDefaultMulticastAddress()
{
  return { 0x01, 0x00, 0x5E, 0x00, 0x17, 0xAA };
}

std::ostream&
operator<<(std::ostream& o, const Address& a)
{
  return o << a.toString();
}

} // namespace ethernet
} // namespace ndn

std::size_t
std::hash<ndn::ethernet::Address>::operator()(const ndn::ethernet::Address& a) const noexcept
{
  return boost::hash_range(a.cbegin(), a.cend());
}
