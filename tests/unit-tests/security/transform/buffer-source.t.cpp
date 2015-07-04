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

#include "security/transform/buffer-source.hpp"
#include "security/transform/stream-sink.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestBufferSource)

BOOST_AUTO_TEST_CASE(Basic)
{
  uint8_t in[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };

  std::ostringstream os1;
  bufferSource(in, sizeof(in)) >> streamSink(os1);
  std::string out1 = os1.str();
  BOOST_CHECK_EQUAL_COLLECTIONS(in, in + sizeof(in), out1.begin(), out1.end());

  std::string in2 =
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567"
    "0123456701234567012345670123456701234567012345670123456701234567";

  std::ostringstream os2;
  bufferSource(in2) >> streamSink(os2);
  std::string out2 = os2.str();
  BOOST_CHECK_EQUAL_COLLECTIONS(in2.begin(), in2.end(), out2.begin(), out2.end());

  Buffer in3(in, sizeof(in));
  std::ostringstream os3;
  bufferSource(in3) >> streamSink(os3);
  std::string out3 = os3.str();
  BOOST_CHECK_EQUAL_COLLECTIONS(in3.begin(), in3.end(), out3.begin(), out3.end());
}

BOOST_AUTO_TEST_SUITE_END() // TestBufferSource
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
