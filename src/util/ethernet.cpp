/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014,  Regents of the University of California,
 *                      Arizona Board of Regents,
 *                      Colorado State University,
 *                      University Pierre & Marie Curie, Sorbonne University,
 *                      Washington University in St. Louis,
 *                      Beijing Institute of Technology,
 *                      The University of Memphis
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

#include "ethernet.hpp"

#include <stdio.h>
#include <ostream>

namespace ndn {
namespace util {
namespace ethernet {

Address
getBroadcastAddress()
{
  static Address bcast(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  return bcast;
}

Address
getDefaultMulticastAddress()
{
  static Address mcast(0x01, 0x00, 0x5E, 0x00, 0x17, 0xAA);
  return mcast;
}

Address::Address()
{
  assign(0);
}

Address::Address(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t a5, uint8_t a6)
{
  elems[0] = a1;
  elems[1] = a2;
  elems[2] = a3;
  elems[3] = a4;
  elems[4] = a5;
  elems[5] = a6;
}

Address::Address(const uint8_t octets[])
{
  std::copy(octets, octets + size(), begin());
}

Address::Address(const Address& address)
{
  std::copy(address.begin(), address.end(), begin());
}

bool
Address::isBroadcast() const
{
  return elems[0] == 0xFF && elems[1] == 0xFF && elems[2] == 0xFF &&
         elems[3] == 0xFF && elems[4] == 0xFF && elems[5] == 0xFF;
}

bool
Address::isMulticast() const
{
  return (elems[0] & 1) != 0;
}

bool
Address::isNull() const
{
  return elems[0] == 0x0 && elems[1] == 0x0 && elems[2] == 0x0 &&
         elems[3] == 0x0 && elems[4] == 0x0 && elems[5] == 0x0;
}

std::string
Address::toString(char sep) const
{
  char s[18]; // 12 digits + 5 separators + null terminator
  ::snprintf(s, sizeof(s), "%02x%c%02x%c%02x%c%02x%c%02x%c%02x",
             elems[0], sep, elems[1], sep, elems[2], sep,
             elems[3], sep, elems[4], sep, elems[5]);
  return std::string(s);
}

Address
Address::fromString(const std::string& str)
{
  unsigned short temp[ADDR_LEN];
  char sep[5][2]; // 5 * (1 separator char + 1 null terminator)
  int n = 0; // num of chars read from the input string

  // ISO C++98 does not support the 'hh' type modifier
  /// \todo use SCNx8 (cinttypes) when we enable C++11
  int ret = ::sscanf(str.c_str(), "%2hx%1[:-]%2hx%1[:-]%2hx%1[:-]%2hx%1[:-]%2hx%1[:-]%2hx%n",
                     &temp[0], &sep[0][0], &temp[1], &sep[1][0], &temp[2], &sep[2][0],
                     &temp[3], &sep[3][0], &temp[4], &sep[4][0], &temp[5], &n);

  if (ret < 11 || static_cast<size_t>(n) != str.length())
    return Address();

  Address a;
  for (size_t i = 0; i < ADDR_LEN; ++i)
    {
      // check that all separators are actually the same char (: or -)
      if (i < 5 && sep[i][0] != sep[0][0])
        return Address();

      // check that each value fits into a uint8_t
      if (temp[i] > 0xFF)
        return Address();

      a[i] = static_cast<uint8_t>(temp[i]);
    }

  return a;
}

std::ostream&
operator<<(std::ostream& o, const Address& a)
{
  return o << a.toString();
}

} // namespace ethernet
} // namespace util
} // namespace ndn
