/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
#include "util/sha256.hpp"

#include "boost-test.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestExclude)

BOOST_AUTO_TEST_SUITE(GenericComponent) // exclude GenericNameComponent

BOOST_AUTO_TEST_CASE(One)
{
  Exclude e;
  std::vector<Exclude::Range> enumerated;

  e.excludeOne(name::Component("b"));
  BOOST_CHECK_EQUAL(e.toUri(), "b");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[0].from, name::Component("b"));

  e.excludeOne(name::Component("d"));
  BOOST_CHECK_EQUAL(e.toUri(), "b,d");
  BOOST_CHECK_EQUAL(e.size(), 2);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 2);
  BOOST_CHECK_EQUAL(enumerated[0].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[0].from, name::Component("b"));
  BOOST_CHECK_EQUAL(enumerated[1].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[1].from, name::Component("d"));

  e.excludeOne(name::Component("a"));
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,d");
  BOOST_CHECK_EQUAL(e.size(), 3);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 3);
  BOOST_CHECK_EQUAL(enumerated[0].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[0].from, name::Component("a"));
  BOOST_CHECK_EQUAL(enumerated[1].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[1].from, name::Component("b"));
  BOOST_CHECK_EQUAL(enumerated[2].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[2].from, name::Component("d"));

  e.excludeOne(name::Component("aa"));
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,d,aa");
  BOOST_CHECK_EQUAL(e.size(), 4);

  e.excludeOne(name::Component("cc"));
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,d,aa,cc");
  BOOST_CHECK_EQUAL(e.size(), 5);

  e.excludeOne(name::Component("c"));
  BOOST_CHECK_EQUAL(e.toUri(), "a,b,c,d,aa,cc");
  BOOST_CHECK_EQUAL(e.size(), 6);
}

BOOST_AUTO_TEST_CASE(Before)
{
  // based on https://redmine.named-data.net/issues/1158
  ndn::Exclude e;
  BOOST_REQUIRE_NO_THROW(e.excludeBefore(name::Component("PuQxMaf91")));

  BOOST_CHECK_EQUAL(e.toUri(), "*,PuQxMaf91");
}

BOOST_AUTO_TEST_CASE(Ranges)
{
  // example: ANY /b /d ANY /f

  Exclude e;
  std::vector<Exclude::Range> enumerated;

  e.excludeOne(name::Component("b0"));
  BOOST_CHECK_EQUAL(e.toUri(), "b0");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].isSingular(), true);
  BOOST_CHECK_EQUAL(enumerated[0].from, name::Component("b0"));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(enumerated[0]), "{b0}");
  BOOST_CHECK_EQUAL(enumerated[0], (Exclude::Range{false, name::Component("b0"), false, name::Component("b0")}));
  BOOST_CHECK_NE(enumerated[0], (Exclude::Range{false, name::Component("b0"), false, name::Component("b1")}));

  e.excludeBefore(name::Component("b1"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,b1");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("b1"));

  e.excludeBefore(name::Component("c0"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("c0"));

  e.excludeRange(name::Component("a0"), name::Component("c0"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("c0"));

  e.excludeRange(name::Component("d0"), name::Component("e0"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0,d0,*,e0");
  BOOST_CHECK_EQUAL(e.size(), 2);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 2);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("c0"));
  BOOST_CHECK_EQUAL(enumerated[1].fromInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[1].from, name::Component("d0"));
  BOOST_CHECK_EQUAL(enumerated[1].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[1].to, name::Component("e0"));

  e.excludeRange(name::Component("c1"), name::Component("d1"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,c0,c1,*,e0");
  BOOST_CHECK_EQUAL(e.size(), 2);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 2);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("c0"));
  BOOST_CHECK_EQUAL(enumerated[1].fromInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[1].from, name::Component("c1"));
  BOOST_CHECK_EQUAL(enumerated[1].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[1].to, name::Component("e0"));

  e.excludeRange(name::Component("a1"), name::Component("d1"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,e0");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("e0"));

  e.excludeBefore(name::Component("e2"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,e2");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("e2"));

  e.excludeAfter(name::Component("f0"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,e2,f0,*");
  BOOST_CHECK_EQUAL(e.size(), 2);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 2);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("e2"));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(enumerated[0]), "(-∞,e2]");
  BOOST_CHECK_EQUAL(enumerated[0], (Exclude::Range{true, name::Component("ignore"), false, name::Component("e2")}));
  BOOST_CHECK_EQUAL(enumerated[1].fromInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[1].from, name::Component("f0"));
  BOOST_CHECK_EQUAL(enumerated[1].toInfinity, true);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(enumerated[1]), "[f0,+∞)");
  BOOST_CHECK_EQUAL(enumerated[1], (Exclude::Range{false, name::Component("f0"), true, name::Component("ignore")}));

  e.excludeAfter(name::Component("e5"));
  BOOST_CHECK_EQUAL(e.toUri(), "*,e2,e5,*");
  BOOST_CHECK_EQUAL(e.size(), 2);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 2);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component("e2"));
  BOOST_CHECK_EQUAL(enumerated[1].fromInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[1].from, name::Component("e5"));
  BOOST_CHECK_EQUAL(enumerated[1].toInfinity, true);

  e.excludeAfter(name::Component("b2"));
  BOOST_CHECK_EQUAL(e.toUri(), "*");
  BOOST_CHECK_EQUAL(e.size(), 1);
  enumerated.clear();
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, true);

  BOOST_REQUIRE_THROW(e.excludeRange(name::Component("d0"), name::Component("a0")),
                      Exclude::Error);
}

BOOST_AUTO_TEST_SUITE_END() // GenericComponent

BOOST_AUTO_TEST_SUITE(ImplicitDigest) // exclude ImplicitSha256DigestComponent

/** \brief make a name::Component with an octet repeated util::Sha256::DIGEST_SIZE times
 *  \param octet the octet to fill the component
 *  \param isDigest whether to make an ImplicitSha256DigestComponent or a GenericNameComponent
 *  \param lastOctet if non-negative, set the last octet to a different value
 */
static name::Component
makeComponent(uint8_t octet, bool isDigest, int lastOctet = -1)
{
  uint8_t wire[util::Sha256::DIGEST_SIZE];
  std::memset(wire, octet, sizeof(wire));
  if (lastOctet >= 0) {
    wire[util::Sha256::DIGEST_SIZE - 1] = static_cast<uint8_t>(lastOctet);
  }

  if (isDigest) {
    return name::Component::fromImplicitSha256Digest(wire, sizeof(wire));
  }
  else {
    return name::Component(wire, sizeof(wire));
  }
}

BOOST_AUTO_TEST_CASE(One)
{
  name::Component digestC = makeComponent(0xCC, true);;
  name::Component genericC = makeComponent(0xCC, false);
  name::Component digestD = makeComponent(0xDD, true);

  Exclude e;
  e.excludeOne(digestC);
  BOOST_CHECK_EQUAL(e.isExcluded(digestC), true);
  BOOST_CHECK_EQUAL(e.isExcluded(genericC), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digestD), false);

  e.clear();
  e.excludeOne(genericC);
  BOOST_CHECK_EQUAL(e.isExcluded(digestC), false);
  BOOST_CHECK_EQUAL(e.isExcluded(genericC), true);
}

BOOST_AUTO_TEST_CASE(BeforeDigest)
{
  name::Component digestBA = makeComponent(0xBB, true, 0xBA);
  name::Component digestBB = makeComponent(0xBB, true);
  name::Component digestBC = makeComponent(0xBB, true, 0xBC);

  Exclude e;
  e.excludeBefore(digestBB);
  BOOST_CHECK_EQUAL(e.isExcluded(digestBA), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digestBB), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digestBC), false);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), false);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), false);

  BOOST_CHECK_EQUAL(e.size(), 1);
  std::vector<Exclude::Range> enumerated;
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, true);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, digestBB);
}

BOOST_AUTO_TEST_CASE(BeforeGeneric)
{
  name::Component digest0 = makeComponent(0x00, true);
  name::Component digest9 = makeComponent(0x99, true);
  name::Component digestF = makeComponent(0xFF, true);

  Exclude e;
  e.excludeBefore(name::Component(""));
  BOOST_CHECK_EQUAL(e.isExcluded(digest0), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digest9), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digestF), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), false);
}

BOOST_AUTO_TEST_CASE(AfterDigest)
{
  name::Component digestBA = makeComponent(0xBB, true, 0xBA);
  name::Component digestBB = makeComponent(0xBB, true);
  name::Component digestBC = makeComponent(0xBB, true, 0xBC);

  Exclude e;
  e.excludeAfter(digestBB);
  BOOST_CHECK_EQUAL(e.isExcluded(digestBA), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digestBB), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digestBC), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), true);

  BOOST_CHECK_EQUAL(e.size(), 1);
  std::vector<Exclude::Range> enumerated;
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].from, digestBB);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, true);
}

BOOST_AUTO_TEST_CASE(AfterDigestFF)
{
  name::Component digest00 = makeComponent(0x00, true);
  name::Component digest99 = makeComponent(0x99, true);
  name::Component digestFE = makeComponent(0xFF, true, 0xFE);
  name::Component digestFF = makeComponent(0xFF, true);

  Exclude e;
  e.excludeAfter(digestFF);
  BOOST_CHECK_EQUAL(e.isExcluded(digest00), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digest99), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digestFE), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digestFF), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), true);
}

BOOST_AUTO_TEST_CASE(AfterGeneric)
{
  name::Component digest0 = makeComponent(0x00, true);
  name::Component digest9 = makeComponent(0x99, true);
  name::Component digestF = makeComponent(0xFF, true);

  Exclude e;
  e.excludeAfter(name::Component(""));
  BOOST_CHECK_EQUAL(e.isExcluded(digest0), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digest9), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digestF), false);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), true);
}

BOOST_AUTO_TEST_CASE(RangeDigest)
{
  name::Component digest0 = makeComponent(0x00, true);
  name::Component digest7 = makeComponent(0x77, true);
  name::Component digest8 = makeComponent(0x88, true);
  name::Component digest9 = makeComponent(0x99, true);
  name::Component digestF = makeComponent(0xFF, true);

  Exclude e;
  e.excludeRange(digest7, digest9);
  BOOST_CHECK_EQUAL(e.isExcluded(digest0), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digest7), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digest8), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digest9), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digestF), false);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), false);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), false);
}

BOOST_AUTO_TEST_CASE(RangeDigestReverse)
{
  name::Component digest7 = makeComponent(0x77, true);
  name::Component digest9 = makeComponent(0x99, true);

  Exclude e;
  BOOST_CHECK_THROW(e.excludeRange(digest9, digest7), Exclude::Error);
}

BOOST_AUTO_TEST_CASE(RangeDigestGeneric)
{
  name::Component digest0 = makeComponent(0x00, true);
  name::Component digest7 = makeComponent(0x77, true);
  name::Component digest9 = makeComponent(0x99, true);
  name::Component digestF = makeComponent(0xFF, true);

  Exclude e;
  e.excludeRange(digest9, name::Component(""));
  BOOST_CHECK_EQUAL(e.isExcluded(digest0), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digest7), false);
  BOOST_CHECK_EQUAL(e.isExcluded(digest9), true);
  BOOST_CHECK_EQUAL(e.isExcluded(digestF), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("")), true);
  BOOST_CHECK_EQUAL(e.isExcluded(name::Component("generic")), false);

  BOOST_CHECK_EQUAL(e.size(), 1);
  std::vector<Exclude::Range> enumerated;
  std::copy(e.begin(), e.end(), std::back_inserter(enumerated));
  BOOST_REQUIRE_EQUAL(enumerated.size(), 1);
  BOOST_CHECK_EQUAL(enumerated[0].fromInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].from, digest9);
  BOOST_CHECK_EQUAL(enumerated[0].toInfinity, false);
  BOOST_CHECK_EQUAL(enumerated[0].to, name::Component(""));
}

BOOST_AUTO_TEST_CASE(RangeGenericDigest)
{
  name::Component digestF = makeComponent(0xFF, true);

  Exclude e;
  BOOST_CHECK_THROW(e.excludeRange(name::Component(""), digestF), Exclude::Error);
}

BOOST_AUTO_TEST_SUITE_END() // ImplicitDigest

BOOST_AUTO_TEST_SUITE(WireCompare) // wireEncode, wireDecode, operator==, operator!=

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

  // // <Exclude><Any/><GenericNameComponent>T</GenericNameComponent><Any/></Exclude>
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
  BOOST_CHECK(exclude.wireEncode() == block);
}

BOOST_AUTO_TEST_CASE(EmptyComponent) // Bug #2660
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

BOOST_AUTO_TEST_SUITE_END() // WireCompare

BOOST_AUTO_TEST_SUITE_END() // TestExclude

} // namespace tests
} // namespace ndn
