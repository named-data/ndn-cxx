/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/key-locator.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

#include "tests/boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestKeyLocator)

BOOST_AUTO_TEST_CASE(TypeNone)
{
  KeyLocator a;
  BOOST_CHECK_EQUAL(a.empty(), true);
  BOOST_CHECK_EQUAL(a.getType(), tlv::Invalid);
  BOOST_CHECK_THROW(a.getName(), KeyLocator::Error);
  BOOST_CHECK_THROW(a.getKeyDigest(), KeyLocator::Error);

  Block wire = a.wireEncode();
  // These octets are obtained from the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (auto it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x1c, 0x00
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  KeyLocator b(wire);
  BOOST_CHECK_EQUAL(a, b);
  BOOST_CHECK_EQUAL(a.empty(), true);
  BOOST_CHECK_EQUAL(b.getType(), tlv::Invalid);
  BOOST_CHECK_THROW(b.getName(), KeyLocator::Error);
  BOOST_CHECK_THROW(b.getKeyDigest(), KeyLocator::Error);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "None");
}

BOOST_AUTO_TEST_CASE(TypeName)
{
  KeyLocator a;
  a.setName("/N");
  BOOST_CHECK_EQUAL(a.empty(), false);
  BOOST_CHECK_EQUAL(a.getType(), tlv::Name);
  BOOST_CHECK_EQUAL(a.getName(), Name("/N"));
  BOOST_CHECK_THROW(a.getKeyDigest(), KeyLocator::Error);

  Block wire = a.wireEncode();
  // These octets are obtained from the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (auto it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x1c, 0x05, 0x07, 0x03, 0x08, 0x01, 0x4e
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  KeyLocator b(wire);
  BOOST_CHECK_EQUAL(a, b);
  BOOST_CHECK_EQUAL(b.getType(), tlv::Name);
  BOOST_CHECK_EQUAL(b.getName(), Name("/N"));
  BOOST_CHECK_THROW(b.getKeyDigest(), KeyLocator::Error);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "Name=/N");

  KeyLocator c("/N");
  BOOST_CHECK_EQUAL(a, c);
}

BOOST_AUTO_TEST_CASE(TypeKeyDigest)
{
  std::string digestOctets = "\x12\x34\x56\x78\x9a\xbc\xde\xf1\x23\x45";
  ConstBufferPtr digestBuffer = make_shared<Buffer>(digestOctets.data(), digestOctets.size());
  Block expectedDigestBlock = makeBinaryBlock(tlv::KeyDigest, digestOctets.data(), digestOctets.size());

  KeyLocator a;
  a.setKeyDigest(digestBuffer);
  BOOST_CHECK_EQUAL(a.empty(), false);
  BOOST_CHECK_EQUAL(a.getType(), tlv::KeyDigest);
  BOOST_CHECK_EQUAL(a.getKeyDigest(), expectedDigestBlock);
  BOOST_CHECK_THROW(a.getName(), KeyLocator::Error);

  Block wire = a.wireEncode();
  // These octets are obtained from the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (auto it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x1c, 0x0c, 0x1d, 0x0a, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf1, 0x23, 0x45
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  KeyLocator b(wire);
  BOOST_CHECK_EQUAL(a, b);
  BOOST_CHECK_EQUAL(b.getType(), tlv::KeyDigest);
  BOOST_CHECK_EQUAL(b.getKeyDigest(), expectedDigestBlock);
  BOOST_CHECK_THROW(b.getName(), KeyLocator::Error);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "KeyDigest=123456789A...");

  b.setKeyDigest("1D03BCDEF1"_block);
  BOOST_CHECK_EQUAL(b.getType(), tlv::KeyDigest);
  BOOST_CHECK_EQUAL(b.getKeyDigest(), "1D03BCDEF1"_block);
  BOOST_CHECK_THROW(b.getName(), KeyLocator::Error);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "KeyDigest=BCDEF1");
}

BOOST_AUTO_TEST_CASE(TypeUnknown)
{
  const auto wire = "1C037F01CC"_block;
  KeyLocator a(wire);
  BOOST_CHECK_EQUAL(a.empty(), false);
  BOOST_CHECK_EQUAL(a.getType(), 127);

  KeyLocator b(wire);
  BOOST_CHECK_EQUAL(a, b);
  BOOST_CHECK_EQUAL(b.getType(), 127);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(b), "Unknown(127)");

  b.setName("/N");
  BOOST_CHECK_NE(a, b);
}

BOOST_AUTO_TEST_CASE(Clear)
{
  KeyLocator a("/foo");
  BOOST_CHECK_EQUAL(a.empty(), false);

  a.clear();
  BOOST_CHECK_EQUAL(a.empty(), true);
  BOOST_CHECK_EQUAL(a.getType(), tlv::Invalid);
  BOOST_CHECK_EQUAL(a, KeyLocator{});
}

BOOST_AUTO_TEST_CASE(Equality)
{
  KeyLocator a;
  KeyLocator b;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/B");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  const char digestOctets[] = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD";
  auto digestBuffer = make_shared<Buffer>(digestOctets, 8);

  a.setKeyDigest(digestBuffer);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setKeyDigest(digestBuffer);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyLocator

} // namespace tests
} // namespace ndn
