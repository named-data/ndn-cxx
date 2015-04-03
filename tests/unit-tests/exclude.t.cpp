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

#include "exclude.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestExclude)

BOOST_AUTO_TEST_CASE(Basic)
{
  Exclude e;
  e.excludeOne(name::Component("b"));
  BOOST_CHECK_EQUAL(e.size(), 1);
  BOOST_CHECK_EQUAL(e.toUri(), "b");

  e.excludeOne(name::Component("d"));
  BOOST_CHECK_EQUAL(e.size(), 2);
  BOOST_CHECK_EQUAL(e.toUri(), "b,d");

  e.excludeOne(name::Component("a"));
  BOOST_CHECK_EQUAL(e.size(), 3);
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,d");

  e.excludeOne(name::Component("aa"));
  BOOST_CHECK_EQUAL(e.size(), 4);
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,d,aa");

  e.excludeOne(name::Component("cc"));
  BOOST_CHECK_EQUAL(e.size(), 5);
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,d,aa,cc");

  e.excludeOne(name::Component("c"));
  BOOST_CHECK_EQUAL(e.size(), 6);
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,c,d,aa,cc");
}

BOOST_AUTO_TEST_CASE(EqualityComparable)
{
  Exclude e1;
  Exclude e2;
  BOOST_CHECK_EQUAL(e1, e2);

  e1.excludeOne(name::Component("T"));
  BOOST_CHECK_NE(e1, e2);

  e2.excludeOne(name::Component("D"));
  BOOST_CHECK_NE(e1, e2);

  e2.clear();
  e2.excludeOne(name::Component("T"));
  BOOST_CHECK_EQUAL(e1, e2);

  e2.clear();
  const uint8_t EXCLUDE[] = { 0x10, 0x15, 0x13, 0x00, 0x08, 0x01, 0x41, 0x08, 0x01, 0x42,
                              0x08, 0x01, 0x43, 0x13, 0x00, 0x08, 0x01, 0x44, 0x08, 0x01,
                              0x45, 0x13, 0x00 };
  e2.wireDecode(Block(EXCLUDE, sizeof(EXCLUDE)));

  e1.clear();
  e1.excludeBefore(name::Component("A"));
  e1.excludeOne(name::Component("B"));
  e1.excludeRange(name::Component("C"), name::Component("D"));
  e1.excludeAfter(name::Component("E"));
  BOOST_CHECK_EQUAL(e1, e2);
}

BOOST_AUTO_TEST_CASE(Before)
{
  // based on http://redmine.named-data.net/issues/1158
  ndn::Exclude e;
  BOOST_REQUIRE_NO_THROW(e.excludeBefore(name::Component("PuQxMaf91")));

  BOOST_CHECK_EQUAL(e.toUri(), "*,PuQxMaf91");
}

BOOST_AUTO_TEST_CASE(Ranges)
{
// example: ANY /b /d ANY /f

  Exclude e;
  e.excludeOne(name::Component("b0"));
  BOOST_CHECK_EQUAL(e.size(), 1);
  BOOST_CHECK_EQUAL(e.toUri(), "b0");

  e.excludeRange(name::Component(), name::Component("b1"));
  BOOST_CHECK_EQUAL(e.size(), 2);
  BOOST_CHECK_EQUAL(e.toUri(), "*,b1");

  e.excludeRange(name::Component(), name::Component("c0"));
  BOOST_CHECK_EQUAL(e.size(), 2);
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0");

  e.excludeRange(name::Component("a0"), name::Component("c0"));
  BOOST_CHECK_EQUAL(e.size(), 2);
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0");

  e.excludeRange(name::Component("d0"), name::Component("e0"));
  BOOST_CHECK_EQUAL(e.size(), 4);
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0,d0,*,e0");

  e.excludeRange(name::Component("c1"), name::Component("d1"));
  BOOST_CHECK_EQUAL(e.size(), 4);
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0,c1,*,e0");

  e.excludeRange(name::Component("a1"), name::Component("d1"));
  BOOST_CHECK_EQUAL(e.size(), 2);
  BOOST_CHECK_EQUAL(e.toUri(), "*,e0");

  e.excludeBefore(name::Component("e2"));
  BOOST_CHECK_EQUAL(e.size(), 2);
  BOOST_CHECK_EQUAL(e.toUri(), "*,e2");

  e.excludeAfter(name::Component("f0"));
  BOOST_CHECK_EQUAL(e.size(), 3);
  BOOST_CHECK_EQUAL(e.toUri(), "*,e2,f0,*");

  e.excludeAfter(name::Component("e5"));
  BOOST_CHECK_EQUAL(e.size(), 3);
  BOOST_CHECK_EQUAL(e.toUri(), "*,e2,e5,*");

  e.excludeAfter(name::Component("b2"));
  BOOST_CHECK_EQUAL(e.size(), 1);
  BOOST_CHECK_EQUAL(e.toUri(), "*");

  BOOST_REQUIRE_THROW(e.excludeRange(name::Component("d0"), name::Component("a0")),
                      Exclude::Error);
}

BOOST_AUTO_TEST_CASE(Malformed)
{
  Exclude e1;
  BOOST_CHECK_THROW(e1.wireEncode(), Exclude::Error);

  Exclude e2;

  // top-level TLV-TYPE is not tlv::Exclude
  const uint8_t NON_EXCLUDE[] = { 0x01, 0x02, 0x13, 0x00 };
  BOOST_CHECK_THROW(e2.wireDecode(Block(NON_EXCLUDE, sizeof(NON_EXCLUDE))),
                    tlv::Error);

  // Exclude element is empty
  const uint8_t EMPTY_EXCLUDE[] = { 0x10, 0x00 };
  BOOST_CHECK_THROW(e2.wireDecode(Block(EMPTY_EXCLUDE, sizeof(EMPTY_EXCLUDE))),
                    Exclude::Error);

  // Exclude element contains unknown element
  const uint8_t UNKNOWN_COMP1[] = { 0x10, 0x02, 0xAA, 0x00 };
  BOOST_CHECK_THROW(e2.wireDecode(Block(UNKNOWN_COMP1, sizeof(UNKNOWN_COMP1))),
                    Exclude::Error);

  // Exclude element contains unknown element
  const uint8_t UNKNOWN_COMP2[] = { 0x10, 0x05, 0x08, 0x01, 0x54, 0xAA, 0x00 };
  BOOST_CHECK_THROW(e2.wireDecode(Block(UNKNOWN_COMP2, sizeof(UNKNOWN_COMP2))),
                    Exclude::Error);

  // // <Exclude><Any/></Exclude>
  // const uint8_t ONLY_ANY[] = { 0x10, 0x02, 0x13, 0x00 };
  // BOOST_CHECK_THROW(e2.wireDecode(Block(ONLY_ANY, sizeof(ONLY_ANY))),
  //                   Exclude::Error);

  // <Exclude><Any/><Any/></Exclude>
  const uint8_t ANY_ANY[] = { 0x10, 0x04, 0x13, 0x00, 0x13, 0x00 };
  BOOST_CHECK_THROW(e2.wireDecode(Block(ANY_ANY, sizeof(ANY_ANY))),
                                  Exclude::Error);

  // // <Exclude><Any/><NameComponent>T</NameComponent><Any/></Exclude>
  // const uint8_t ANY_COMPONENT_ANY[] = { 0x10, 0x07, 0x13, 0x00, 0x08, 0x01, 0x54, 0x13, 0x00 };
  // BOOST_CHECK_THROW(e2.wireDecode(Block(ANY_COMPONENT_ANY, sizeof(ANY_COMPONENT_ANY))),
  //                   Exclude::Error);

  uint8_t WIRE[] = {
    0x10, 0x20, // Exclude
          0x01, 0x1E, // ImplicitSha256DigestComponent with incorrect length
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd
  };

  BOOST_CHECK_THROW(Exclude().wireDecode(Block(WIRE, sizeof(WIRE))), Exclude::Error);
}

BOOST_AUTO_TEST_CASE(ImplicitSha256Digest)
{
  uint8_t WIRE[] = {
    0x10, 0x22, // Exclude
          0x01, 0x20, // ImplicitSha256DigestComponent
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd
  };

  Block block(WIRE, sizeof(WIRE));

  Exclude exclude;
  BOOST_CHECK_NO_THROW(exclude.wireDecode(block));
}

BOOST_AUTO_TEST_CASE(ExcludeEmptyComponent) // Bug #2660
{
  Exclude e1, e2;

  e1.excludeOne(name::Component());
  e2.excludeOne(name::Component(""));

  BOOST_CHECK_EQUAL(e1, e2);
  BOOST_CHECK_EQUAL(e1.toUri(), e2.toUri());
  BOOST_CHECK(e1.wireEncode() == e2.wireEncode());

  BOOST_CHECK_EQUAL("...", e1.toUri());

  uint8_t WIRE[] {0x10, 0x02, 0x08, 0x00};
  BOOST_CHECK_EQUAL_COLLECTIONS(e1.wireEncode().begin(), e1.wireEncode().end(),
                                WIRE, WIRE + sizeof(WIRE));

  Exclude e3(Block(WIRE, sizeof(WIRE)));
  BOOST_CHECK_EQUAL(e1, e3);
  BOOST_CHECK_EQUAL(e1.toUri(), e3.toUri());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
