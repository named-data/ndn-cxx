/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"

#include "tests/boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestBufferSource)

BOOST_AUTO_TEST_CASE(Basic)
{
  const uint8_t in[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };
  std::ostringstream os1;
  bufferSource(in) >> streamSink(os1);
  std::string out1 = os1.str();
  BOOST_CHECK_EQUAL_COLLECTIONS(out1.begin(), out1.end(), in, in + sizeof(in));

  const std::string in2 =
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
  BOOST_CHECK_EQUAL_COLLECTIONS(out2.begin(), out2.end(), in2.begin(), in2.end());

  std::vector<uint8_t> in3(in, in + sizeof(in));
  std::ostringstream os3;
  bufferSource(in3) >> streamSink(os3);
  std::string out3 = os3.str();
  BOOST_CHECK_EQUAL_COLLECTIONS(out3.begin(), out3.end(), in3.begin(), in3.end());

  InputBuffers in4{{in}, {reinterpret_cast<const uint8_t*>(in2.data()), in2.size()}};
  std::ostringstream os4;
  bufferSource(in4) >> streamSink(os4);
  std::string out4 = os4.str();
  BOOST_CHECK_EQUAL(out4.size(), sizeof(in) + in2.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(out4.begin(), out4.begin() + sizeof(in),
                                in4[0].begin(), in4[0].end());
  BOOST_CHECK_EQUAL_COLLECTIONS(out4.begin() + sizeof(in), out4.end(),
                                in4[1].begin(), in4[1].end());
}

BOOST_AUTO_TEST_SUITE_END() // TestBufferSource
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
