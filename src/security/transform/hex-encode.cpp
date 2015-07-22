/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "hex-encode.hpp"

namespace ndn {
namespace security {
namespace transform {

static const char H2CL[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static const char H2CU[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

HexEncode::HexEncode(bool useUpperCase)
  : m_useUpperCase(useUpperCase)
{
}

size_t
HexEncode::convert(const uint8_t* data, size_t dataLen)
{
  setOutputBuffer(toHex(data, dataLen));
  return dataLen;
}

unique_ptr<Transform::OBuffer>
HexEncode::toHex(const uint8_t* data, size_t dataLen)
{
  const char* encodePad = (m_useUpperCase) ? H2CU : H2CL;

  auto encoded = make_unique<OBuffer>(dataLen * 2);
  uint8_t* buf = &encoded->front();
  for (size_t i = 0; i < dataLen; i++) {
    buf[0] = encodePad[((data[i] >> 4) & 0x0F)];
    buf++;
    buf[0] = encodePad[(data[i] & 0x0F)];
    buf++;
  }
  return encoded;
}



unique_ptr<Transform>
hexEncode(bool useUpperCase)
{
  return make_unique<HexEncode>(useUpperCase);
}

} // namespace transform
} // namespace security
} // namespace ndn
