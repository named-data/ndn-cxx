/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "encoding/block-helpers.hpp"

#include "boost-test.hpp"
#include "name.hpp"

namespace ndn {
namespace encoding {
namespace tests {

BOOST_AUTO_TEST_SUITE(EncodingBlockHelpers)

BOOST_AUTO_TEST_CASE(NonNegativeInteger)
{
  Block b = makeNonNegativeIntegerBlock(100, 1000);
  BOOST_CHECK_EQUAL(b.type(), 100);
  BOOST_CHECK_GT(b.value_size(), 0);
  BOOST_CHECK_EQUAL(readNonNegativeInteger(b), 1000);

  BOOST_CHECK_THROW(readNonNegativeInteger(Block()), tlv::Error);
}

BOOST_AUTO_TEST_CASE(Empty)
{
  Block b = makeEmptyBlock(200);
  BOOST_CHECK_EQUAL(b.type(), 200);
  BOOST_CHECK_EQUAL(b.value_size(), 0);
}

BOOST_AUTO_TEST_CASE(String)
{
  Block b = makeStringBlock(100, "Hello, world!");
  BOOST_CHECK_EQUAL(b.type(), 100);
  BOOST_CHECK_GT(b.value_size(), 0);
  BOOST_CHECK_EQUAL(readString(b), "Hello, world!");
}

BOOST_AUTO_TEST_CASE(Data)
{
  std::string buf1{1, 1, 1, 1};
  const uint8_t buf2[]{1, 1, 1, 1};
  std::list<uint8_t> buf3{1, 1, 1, 1};

  Block b1 = makeBinaryBlock(100, buf1.data(), buf1.size());
  Block b2 = makeBinaryBlock(100, buf2, sizeof(buf2));
  Block b3 = makeBinaryBlock(100, buf1.begin(), buf1.end()); // fast encoding (random access iterator)
  Block b4 = makeBinaryBlock(100, buf3.begin(), buf3.end()); // slow encoding (general iterator)

  BOOST_CHECK(b1 == b2);
  BOOST_CHECK(b1 == b3);
  BOOST_CHECK_EQUAL(b1.type(), 100);
  BOOST_CHECK_EQUAL(b1.value_size(), buf1.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(b1.value_begin(), b1.value_end(),
                                buf2, buf2 + sizeof(buf2));
}

BOOST_AUTO_TEST_CASE(Nested)
{
  Name name("ndn:/Hello/World!");
  Block b1 = makeNestedBlock(100, name);

  BOOST_CHECK_EQUAL(b1.type(), 100);
  b1.parse();
  BOOST_CHECK_EQUAL(b1.elements().size(), 1);
  BOOST_CHECK_EQUAL(b1.elements().begin()->type(), name.wireEncode().type());
  BOOST_CHECK(*b1.elements().begin() == name.wireEncode());
}

BOOST_AUTO_TEST_SUITE_END() // EncodingBlockHelpers

} // namespace tests
} // namespace encoding
} // namespace ndn
