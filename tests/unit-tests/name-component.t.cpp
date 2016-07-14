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

#include "name-component.hpp"

#include "boost-test.hpp"
#include <boost/mpl/vector.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestNameComponent)

static const uint8_t NAME_COMPONENT_WIRE[] = {
        0x08, 0x03, // NameComponent
          0x6e, 0x64, 0x6e};

static const uint8_t NAME_COMPONENT2_WIRE[] = {
        0x08, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x49 };

static const uint8_t DIGEST_COMPONENT_WIRE[] = {
        0x01, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x48 };

static const uint8_t DIGEST_COMPONENT2_WIRE[] = {
        0x01, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x49 };

static const uint8_t INVALID_COMPONENT_WIRE[] = {
        0x07, 0x03, // unknown component type
          0x6e, 0x64, 0x6e};

BOOST_AUTO_TEST_SUITE(Decode)

BOOST_AUTO_TEST_CASE(Generic)
{
  Block block(NAME_COMPONENT_WIRE, sizeof(NAME_COMPONENT_WIRE));
  name::Component comp;
  BOOST_REQUIRE_NO_THROW(comp.wireDecode(block));
  BOOST_CHECK_EQUAL(comp.toUri(), "ndn");
}

BOOST_AUTO_TEST_CASE(Digest)
{
  Block block(DIGEST_COMPONENT_WIRE, sizeof(DIGEST_COMPONENT_WIRE));
  name::Component comp;
  BOOST_REQUIRE_NO_THROW(comp.wireDecode(block));
  BOOST_REQUIRE_EQUAL(comp.toUri(), "sha256digest=28bad4b5275bd392dbb670c75cf0b66f"
                                                 "13f7942b21e80f55c0e86b374753a548");
}

BOOST_AUTO_TEST_CASE(Invalid)
{
  Block block(INVALID_COMPONENT_WIRE, sizeof(INVALID_COMPONENT_WIRE));
  name::Component comp;
  BOOST_REQUIRE_THROW(comp.wireDecode(block), name::Component::Error);
}

BOOST_AUTO_TEST_SUITE_END() // Decode

BOOST_AUTO_TEST_SUITE(Compare)

BOOST_AUTO_TEST_CASE(Generic)
{
  name::Component compD("D");
  name::Component compD2("D");
  name::Component compF("F");
  name::Component compAA("AA");

  BOOST_CHECK_EQUAL(compD == compD2, true);
  BOOST_CHECK_EQUAL(compD != compD2, false);
  BOOST_CHECK_EQUAL(compD < compD2, false);
  BOOST_CHECK_EQUAL(compD <= compD2, true);
  BOOST_CHECK_EQUAL(compD > compD2, false);
  BOOST_CHECK_EQUAL(compD >= compD2, true);

  BOOST_CHECK_EQUAL(compD == compF, false);
  BOOST_CHECK_EQUAL(compD != compF, true);
  BOOST_CHECK_EQUAL(compD < compF, true);
  BOOST_CHECK_EQUAL(compD <= compF, true);
  BOOST_CHECK_EQUAL(compD > compF, false);
  BOOST_CHECK_EQUAL(compD >= compF, false);

  BOOST_CHECK_EQUAL(compF == compAA, false);
  BOOST_CHECK_EQUAL(compF != compAA, true);
  BOOST_CHECK_EQUAL(compF < compAA, true);
  BOOST_CHECK_EQUAL(compF <= compAA, true);
  BOOST_CHECK_EQUAL(compF > compAA, false);
  BOOST_CHECK_EQUAL(compF >= compAA, false);
}

BOOST_AUTO_TEST_CASE(ZeroLength)
{
  name::Component comp0("");
  BOOST_REQUIRE_EQUAL(comp0.value_size(), 0);

  BOOST_CHECK_EQUAL(comp0, comp0);
  BOOST_CHECK_EQUAL(comp0, name::Component(""));
  BOOST_CHECK_LT(comp0, name::Component("A"));
  BOOST_CHECK_LE(comp0, name::Component("A"));
  BOOST_CHECK_NE(comp0, name::Component("A"));
  BOOST_CHECK_GT(name::Component("A"), comp0);
  BOOST_CHECK_GE(name::Component("A"), comp0);
}

BOOST_AUTO_TEST_CASE(Digest)
{
  name::Component digest1(Block(DIGEST_COMPONENT_WIRE, sizeof(DIGEST_COMPONENT_WIRE)));
  name::Component digest1b(Block(DIGEST_COMPONENT_WIRE, sizeof(DIGEST_COMPONENT_WIRE)));
  name::Component digest2(Block(DIGEST_COMPONENT2_WIRE, sizeof(DIGEST_COMPONENT2_WIRE)));
  name::Component generic0;
  name::Component generic2(Block(NAME_COMPONENT2_WIRE, sizeof(NAME_COMPONENT2_WIRE)));

  BOOST_CHECK_EQUAL(digest1 == digest1b, true);
  BOOST_CHECK_EQUAL(digest1 != digest1b, false);
  BOOST_CHECK_EQUAL(digest1 < digest1b, false);
  BOOST_CHECK_EQUAL(digest1 <= digest1b, true);
  BOOST_CHECK_EQUAL(digest1 > digest1b, false);
  BOOST_CHECK_EQUAL(digest1 >= digest1b, true);

  BOOST_CHECK_EQUAL(digest1 == digest2, false);
  BOOST_CHECK_EQUAL(digest1 != digest2, true);
  BOOST_CHECK_EQUAL(digest1 < digest2, true);
  BOOST_CHECK_EQUAL(digest1 <= digest2, true);
  BOOST_CHECK_EQUAL(digest1 > digest2, false);
  BOOST_CHECK_EQUAL(digest1 >= digest2, false);

  BOOST_CHECK_EQUAL(digest1 == generic0, false);
  BOOST_CHECK_EQUAL(digest1 != generic0, true);
  BOOST_CHECK_EQUAL(digest1 < generic0, true);
  BOOST_CHECK_EQUAL(digest1 <= generic0, true);
  BOOST_CHECK_EQUAL(digest1 > generic0, false);
  BOOST_CHECK_EQUAL(digest1 >= generic0, false);

  BOOST_CHECK_EQUAL(digest2 == generic2, false);
  BOOST_CHECK_EQUAL(digest2 != generic2, true);
  BOOST_CHECK_EQUAL(digest2 < generic2, true);
  BOOST_CHECK_EQUAL(digest2 <= generic2, true);
  BOOST_CHECK_EQUAL(digest2 > generic2, false);
  BOOST_CHECK_EQUAL(digest2 >= generic2, false);
}

BOOST_AUTO_TEST_SUITE_END() // Compare

BOOST_AUTO_TEST_SUITE(CreateFromIterators) // Bug 2490

typedef boost::mpl::vector<
  std::vector<uint8_t>,
  std::list<uint8_t>,
  std::vector<int8_t>,
  std::list<int8_t>
> ContainerTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(ZeroOctet, T, ContainerTypes)
{
  T bytes;
  name::Component c(bytes.begin(), bytes.end());
  BOOST_CHECK_EQUAL(c.value_size(), 0);
  BOOST_CHECK_EQUAL(c.size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(OneOctet, T, ContainerTypes)
{
  T bytes{1};
  name::Component c(bytes.begin(), bytes.end());
  BOOST_CHECK_EQUAL(c.value_size(), 1);
  BOOST_CHECK_EQUAL(c.size(), 3);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(FourOctets, T, ContainerTypes)
{
  T bytes{1, 2, 3, 4};
  name::Component c(bytes.begin(), bytes.end());
  BOOST_CHECK_EQUAL(c.value_size(), 4);
  BOOST_CHECK_EQUAL(c.size(), 6);
}

BOOST_AUTO_TEST_SUITE_END() // CreateFromIterators

BOOST_AUTO_TEST_SUITE_END() // TestNameComponent

} // namespace tests
} // namespace ndn
