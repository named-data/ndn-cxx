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

#include "encoding/encoding-buffer.hpp"
#include "encoding/block.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

class BufferEstimatorFixture
{
public:
  EncodingBuffer buffer;
  EncodingEstimator estimator;
};

BOOST_AUTO_TEST_SUITE(Encoding)
BOOST_AUTO_TEST_SUITE(TestEncodingBuffer)

BOOST_AUTO_TEST_CASE(ConstructFromBlock)
{
  auto buf = make_shared<Buffer>(10);
  Block block(0xab, buf);
  block.encode();

  {
    EncodingBuffer buffer(block);
    BOOST_CHECK_EQUAL(buffer.size(), 12);
    BOOST_CHECK_EQUAL(buffer.capacity(), 12);
  }

  (*buf)[1] = 0xe0;
  (*buf)[2] = 2;
  block = Block(buf, buf->begin() + 1, buf->begin() + 5);
  BOOST_CHECK_EQUAL(block.type(), 0xe0);

  {
    EncodingBuffer buffer(block);
    BOOST_CHECK_EQUAL(buffer.size(), 4);
    BOOST_CHECK_EQUAL(buffer.capacity(), 10);
  }
}

BOOST_FIXTURE_TEST_SUITE(PrependVarNumber, BufferEstimatorFixture)

BOOST_AUTO_TEST_CASE(OneByte1)
{
  size_t s1 = buffer.prependVarNumber(252);
  size_t s2 = estimator.prependVarNumber(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
}

BOOST_AUTO_TEST_CASE(ThreeBytes1)
{
  size_t s1 = buffer.prependVarNumber(253);
  size_t s2 = estimator.prependVarNumber(253);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
}

BOOST_AUTO_TEST_CASE(ThreeBytes2)
{
  size_t s1 = buffer.prependVarNumber(255);
  size_t s2 = estimator.prependVarNumber(255);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
}

BOOST_AUTO_TEST_CASE(ThreeBytes3)
{
  size_t s1 = buffer.prependVarNumber(65535);
  size_t s2 = estimator.prependVarNumber(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
}

BOOST_AUTO_TEST_CASE(FiveBytes1)
{
  size_t s1 = buffer.prependVarNumber(65536);
  size_t s2 = estimator.prependVarNumber(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
}

BOOST_AUTO_TEST_CASE(FiveBytes2)
{
  size_t s1 = buffer.prependVarNumber(4294967295LL);
  size_t s2 = estimator.prependVarNumber(4294967295LL);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
}

BOOST_AUTO_TEST_CASE(NineBytes)
{
  size_t s1 = buffer.prependVarNumber(4294967296LL);
  size_t s2 = estimator.prependVarNumber(4294967296LL);
  BOOST_CHECK_EQUAL(buffer.size(), 9);
  BOOST_CHECK_EQUAL(s1, 9);
  BOOST_CHECK_EQUAL(s2, 9);
}

BOOST_AUTO_TEST_SUITE_END() // PrependVarNumber

BOOST_FIXTURE_TEST_SUITE(PrependNonNegativeNumber, BufferEstimatorFixture)

BOOST_AUTO_TEST_CASE(NonNegativeNumberOneByte1)
{
  size_t s1 = buffer.prependNonNegativeInteger(252);
  size_t s2 = estimator.prependNonNegativeInteger(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberOneByte2)
{
  size_t s1 = buffer.prependNonNegativeInteger(255);
  size_t s2 = estimator.prependNonNegativeInteger(255);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberTwoBytes1)
{
  size_t s1 = buffer.prependNonNegativeInteger(256);
  size_t s2 = estimator.prependNonNegativeInteger(256);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberTwoBytes2)
{
  size_t s1 = buffer.prependNonNegativeInteger(65535);
  size_t s2 = estimator.prependNonNegativeInteger(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberFourBytes1)
{
  size_t s1 = buffer.prependNonNegativeInteger(65536);
  size_t s2 = estimator.prependNonNegativeInteger(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberFourBytes2)
{
  size_t s1 = buffer.prependNonNegativeInteger(4294967295LL);
  size_t s2 = estimator.prependNonNegativeInteger(4294967295LL);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
}

BOOST_AUTO_TEST_CASE(NonNegativeNumberEightBytes)
{
  size_t s1 = buffer.prependNonNegativeInteger(4294967296LL);
  size_t s2 = estimator.prependNonNegativeInteger(4294967296LL);
  BOOST_CHECK_EQUAL(buffer.size(), 8);
  BOOST_CHECK_EQUAL(s1, 8);
  BOOST_CHECK_EQUAL(s2, 8);
}

BOOST_AUTO_TEST_SUITE_END() // PrependNonNegativeNumber

BOOST_AUTO_TEST_SUITE_END() // TestEncodingBuffer
BOOST_AUTO_TEST_SUITE_END() // Encoding

} // namespace tests
} // namespace ndn
