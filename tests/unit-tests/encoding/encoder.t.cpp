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

#include "encoding/encoder.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace encoding {
namespace tests {

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestEncoder)

BOOST_AUTO_TEST_CASE(Basic)
{
  Encoder e;
  BOOST_CHECK_GT(e.capacity(), 100);

  Encoder e1(100);
  BOOST_CHECK_EQUAL(e1.capacity(), 100);

  Encoder e2(100, 100);
  BOOST_CHECK_EQUAL(e2.capacity(), 100);

  BOOST_CHECK_EQUAL(e.prependByte(1), 1);
  BOOST_CHECK_EQUAL(e.appendByte(1), 1);

  uint8_t buf1[] = {'t', 'e', 's', 't', '1'};
  BOOST_CHECK_EQUAL(e1.prependByteArray(buf1, sizeof(buf1)), 5);
  BOOST_CHECK_EQUAL(e1.appendByteArray(buf1, sizeof(buf1)), 5);

  std::vector<uint8_t> buf2 = {'t', 'e', 's', 't', '2'};
  BOOST_CHECK_EQUAL(e1.prependRange(buf2.begin(), buf2.end()), 5);
  BOOST_CHECK_EQUAL(e1.appendRange(buf2.begin(), buf2.end()), 5);

  std::list<uint8_t> buf3 = {'t', 'e', 's', 't', '2'};
  BOOST_CHECK_EQUAL(e2.prependRange(buf3.begin(), buf3.end()), 5);
  BOOST_CHECK_EQUAL(e2.appendRange(buf3.begin(), buf3.end()), 5);

  uint8_t expected1[] = {1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(e.buf(), e.buf() + e.size(),
                                expected1, expected1 + sizeof(expected1));

  const Encoder& constE = e;
  BOOST_CHECK_EQUAL_COLLECTIONS(constE.buf(), constE.buf() + constE.size(),
                                expected1, expected1 + sizeof(expected1));

  uint8_t expected2[] = {'t', 'e', 's', 't', '2',
                           't', 'e', 's', 't', '1', 't', 'e', 's', 't', '1',
                         't', 'e', 's', 't', '2'};
  BOOST_CHECK_EQUAL_COLLECTIONS(e1.begin(), e1.end(),
                                expected2, expected2 + sizeof(expected2));
  const Encoder& constE1 = e1;
  BOOST_CHECK_EQUAL_COLLECTIONS(constE1.begin(), constE1.end(),
                                expected2, expected2 + sizeof(expected2));

  BOOST_CHECK_THROW(e1.block(), tlv::Error);
  BOOST_CHECK_NO_THROW(e1.block(false));

  e1.prependVarNumber(20);
  e1.prependVarNumber(100);

  BOOST_CHECK_NO_THROW(e1.block());
}

BOOST_AUTO_TEST_CASE(Tlv)
{
  Encoder e;

  BOOST_CHECK_EQUAL(e.prependVarNumber(1), 1);
  BOOST_CHECK_EQUAL(e.appendVarNumber(1), 1);

  BOOST_CHECK_EQUAL(e.prependVarNumber(252), 1);
  BOOST_CHECK_EQUAL(e.appendVarNumber(252), 1);

  BOOST_CHECK_EQUAL(e.prependVarNumber(253), 3);
  BOOST_CHECK_EQUAL(e.appendVarNumber(253), 3);

  BOOST_CHECK_EQUAL(e.prependVarNumber(65536), 5);
  BOOST_CHECK_EQUAL(e.appendVarNumber(65536), 5);

  BOOST_CHECK_EQUAL(e.prependVarNumber(4294967296LL), 9);
  BOOST_CHECK_EQUAL(e.appendVarNumber(4294967296LL), 9);

  //

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(1), 1);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(1), 1);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(252), 1);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(252), 1);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(253), 1);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(253), 1);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(255), 1);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(255), 1);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(256), 2);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(256), 2);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(65535), 2);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(65535), 2);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(65536), 4);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(65536), 4);

  BOOST_CHECK_EQUAL(e.prependNonNegativeInteger(4294967296LL), 8);
  BOOST_CHECK_EQUAL(e.appendNonNegativeInteger(4294967296LL), 8);

  //

  uint8_t buf[] = {0x01, 0x03, 0x00, 0x00, 0x00};
  Block block1(buf, sizeof(buf));

  BOOST_CHECK_EQUAL(e.prependByteArrayBlock(100, buf, sizeof(buf)), 7);
  BOOST_CHECK_EQUAL(e.appendByteArrayBlock(100, buf, sizeof(buf)), 7);

  BOOST_CHECK_EQUAL(e.prependBlock(block1), 5);
  BOOST_CHECK_EQUAL(e.appendBlock(block1), 5);

  Block block2(100, block1);

  BOOST_CHECK_EQUAL(e.prependBlock(block2), 7);
  BOOST_CHECK_EQUAL(e.appendBlock(block2), 7);
}

BOOST_AUTO_TEST_CASE(Reserve)
{
  Encoder e(100, 0);
  BOOST_CHECK_EQUAL(e.capacity(), 100);

  e.reserve(100, true);
  BOOST_CHECK_EQUAL(e.capacity(), 100);

  e.reserve(200, true);
  BOOST_CHECK_EQUAL(e.capacity(), 200);

  e.reserve(100, false);
  BOOST_CHECK_EQUAL(e.capacity(), 200);

  e.reserveFront(1000);
  BOOST_CHECK_GT(e.capacity(), 1000);

  e.reserveBack(1000);
  BOOST_CHECK_GT(e.capacity(), 2000);
}

BOOST_AUTO_TEST_SUITE_END() // TestEncoder
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace encoding
} // namespace ndn
