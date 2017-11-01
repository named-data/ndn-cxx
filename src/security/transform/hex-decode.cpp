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
 */

#include "hex-decode.hpp"

namespace ndn {
namespace security {
namespace transform {

// hex decoding pad
static const int8_t C2H[] = {
// 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0-15
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 16-31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 32-47
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, // 48-63
  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 64-79
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 80-95
  -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 96-111
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 112-127
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 128-143
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 144-159
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 160-175
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 176-191
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 192-207
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 208-223
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 224-239
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 240-255
};
static_assert(std::extent<decltype(C2H)>::value == 256, "");


HexDecode::HexDecode()
  : m_hasOddByte(false)
  , m_oddByte(0)
{
}

size_t
HexDecode::convert(const uint8_t* hex, size_t hexLen)
{
  if (hexLen == 0)
    return 0;

  setOutputBuffer(toBytes(hex, hexLen));

  size_t totalDecodedLen = hexLen + (m_hasOddByte ? 1 : 0);
  if (totalDecodedLen % 2 == 1) {
    m_oddByte = hex[hexLen - 1];
    m_hasOddByte = true;
  }
  else
    m_hasOddByte = false;

  return hexLen;
}

void
HexDecode::finalize()
{
  if (m_hasOddByte)
    BOOST_THROW_EXCEPTION(Error(getIndex(), "Incomplete input"));
}

unique_ptr<Transform::OBuffer>
HexDecode::toBytes(const uint8_t* hex, size_t hexLen)
{
  size_t bufferSize = (hexLen + (m_hasOddByte ? 1 : 0)) >> 1;
  auto buffer = make_unique<OBuffer>(bufferSize);
  auto it = buffer->begin();

  if (m_hasOddByte) {
    if (C2H[hex[0]] < 0 || C2H[m_oddByte] < 0)
      BOOST_THROW_EXCEPTION(Error(getIndex(), "Wrong input byte"));

    *it = (C2H[m_oddByte] << 4) + C2H[hex[0]];
    ++it;
    hex += 1;
    hexLen -= 1;
  }

  while (hexLen >= 2) {
    if (C2H[hex[0]] < 0 || C2H[hex[1]] < 0)
      BOOST_THROW_EXCEPTION(Error(getIndex(), "Wrong input byte"));

    *it = (C2H[hex[0]] << 4) + C2H[hex[1]];
    ++it;
    hex += 2;
    hexLen -= 2;
  }

  return buffer;
}

unique_ptr<Transform>
hexDecode()
{
  return make_unique<HexDecode>();
}

} // namespace transform
} // namespace security
} // namespace ndn
