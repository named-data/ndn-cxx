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

#include "name-component.hpp"
#include "name.hpp"

#include "boost-test.hpp"
#include <boost/mpl/vector.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestNameComponent)

static const uint8_t NAME_COMPONENT_WIRE[] = {
        0x08, 0x03, // NameComponent
          0x6e, 0x64, 0x6e
};
static const uint8_t NAME_COMPONENT2_WIRE[] = {
        0x08, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x49
};
static const uint8_t DIGEST_COMPONENT_WIRE[] = {
        0x01, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x48
};
static const uint8_t DIGEST_COMPONENT2_WIRE[] = {
        0x01, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x49
};
static const uint8_t INVALID_COMPONENT_WIRE[] = {
        0x07, 0x03, // unknown component type
          0x6e, 0x64, 0x6e
};

BOOST_AUTO_TEST_SUITE(Decode)

BOOST_AUTO_TEST_CASE(Generic)
{
  Block block(NAME_COMPONENT_WIRE, sizeof(NAME_COMPONENT_WIRE));
  name::Component comp;
  BOOST_CHECK_NO_THROW(comp.wireDecode(block));
  BOOST_CHECK_EQUAL(comp.toUri(), "ndn");
}

BOOST_AUTO_TEST_CASE(Digest)
{
  Block block(DIGEST_COMPONENT_WIRE, sizeof(DIGEST_COMPONENT_WIRE));
  name::Component comp;
  BOOST_CHECK_NO_THROW(comp.wireDecode(block));
  BOOST_CHECK_EQUAL(comp.toUri(), "sha256digest=28bad4b5275bd392dbb670c75cf0b66f"
                                               "13f7942b21e80f55c0e86b374753a548");
}

BOOST_AUTO_TEST_CASE(Invalid)
{
  Block block(INVALID_COMPONENT_WIRE, sizeof(INVALID_COMPONENT_WIRE));
  name::Component comp;
  BOOST_CHECK_THROW(comp.wireDecode(block), name::Component::Error);
}

BOOST_AUTO_TEST_SUITE_END() // Decode

BOOST_AUTO_TEST_SUITE(Compare)

BOOST_AUTO_TEST_CASE(Generic)
{
  name::Component empty;
  name::Component compD("D");
  name::Component compD2("D");
  name::Component compF("F");
  name::Component compAA("AA");

  BOOST_CHECK_EQUAL(empty == empty, true);
  BOOST_CHECK_EQUAL(empty != empty, false);
  BOOST_CHECK_EQUAL(empty < empty, false);
  BOOST_CHECK_EQUAL(empty <= empty, true);
  BOOST_CHECK_EQUAL(empty == compD, false);
  BOOST_CHECK_EQUAL(empty < compD, true);

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

BOOST_AUTO_TEST_CASE(ZeroLength)
{
  name::Component comp0("");
  BOOST_CHECK_EQUAL(comp0.value_size(), 0);

  BOOST_CHECK_EQUAL(comp0, comp0);
  BOOST_CHECK_EQUAL(comp0, name::Component(""));
  BOOST_CHECK_LT(comp0, name::Component("A"));
  BOOST_CHECK_LE(comp0, name::Component("A"));
  BOOST_CHECK_NE(comp0, name::Component("A"));
  BOOST_CHECK_GT(name::Component("A"), comp0);
  BOOST_CHECK_GE(name::Component("A"), comp0);
}

BOOST_AUTO_TEST_SUITE_END() // Compare

BOOST_AUTO_TEST_CASE(ToUri)
{
  using name::Component;

  BOOST_CHECK_EQUAL(Component("").toUri(), "...");
  BOOST_CHECK_EQUAL(Component(".").toUri(), "....");
  BOOST_CHECK_EQUAL(Component("..").toUri(), ".....");
  BOOST_CHECK_EQUAL(Component(".dot-with-other-chars").toUri(), ".dot-with-other-chars");

  BOOST_CHECK_EQUAL(Component("foo42").toUri(), "foo42");
  BOOST_CHECK_EQUAL(Component("foo%bar").toUri(), "foo%25bar");
  BOOST_CHECK_EQUAL(Component("-._~").toUri(), "-._~");
  BOOST_CHECK_EQUAL(Component(":/?#[]@").toUri(), "%3A%2F%3F%23%5B%5D%40");

  // sha256digest component is tested in Decode/Digest
}

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

BOOST_AUTO_TEST_SUITE(NamingConvention)

template<typename ArgType>
struct ConventionTest
{
  function<name::Component(ArgType)> makeComponent;
  function<ArgType(const name::Component&)> getValue;
  function<Name&(Name&, ArgType)> append;
  Name expected;
  ArgType value;
  function<bool(const name::Component&)> isComponent;
};

class NumberWithMarker
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {bind(&name::Component::fromNumberWithMarker, 0xAA, _1),
            bind(&name::Component::toNumberWithMarker, _1, 0xAA),
            bind(&Name::appendNumberWithMarker, _1, 0xAA, _2),
            Name("/%AA%03%E8"),
            1000,
            bind(&name::Component::isNumberWithMarker, _1, 0xAA)};
  }
};

class Segment
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&name::Component::fromSegment,
            bind(&name::Component::toSegment, _1),
            bind(&Name::appendSegment, _1, _2),
            Name("/%00%27%10"),
            10000,
            bind(&name::Component::isSegment, _1)};
  }
};

class SegmentOffset
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&name::Component::fromSegmentOffset,
            bind(&name::Component::toSegmentOffset, _1),
            bind(&Name::appendSegmentOffset, _1, _2),
            Name("/%FB%00%01%86%A0"),
            100000,
            bind(&name::Component::isSegmentOffset, _1)};
  }
};

class Version
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&name::Component::fromVersion,
            bind(&name::Component::toVersion, _1),
            [] (Name& name, uint64_t version) -> Name& { return name.appendVersion(version); },
            Name("/%FD%00%0FB%40"),
            1000000,
            bind(&name::Component::isVersion, _1)};
  }
};

class Timestamp
{
public:
  ConventionTest<time::system_clock::TimePoint>
  operator()() const
  {
    return {&name::Component::fromTimestamp,
            bind(&name::Component::toTimestamp, _1),
            [] (Name& name, time::system_clock::TimePoint t) -> Name& { return name.appendTimestamp(t); },
            Name("/%FC%00%04%7BE%E3%1B%00%00"),
            time::getUnixEpoch() + 14600_days, // 40 years
            bind(&name::Component::isTimestamp, _1)};
  }
};

class SequenceNumber
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&name::Component::fromSequenceNumber,
            bind(&name::Component::toSequenceNumber, _1),
            bind(&Name::appendSequenceNumber, _1, _2),
            Name("/%FE%00%98%96%80"),
            10000000,
            bind(&name::Component::isSequenceNumber, _1)};
  }
};

using ConventionTests = boost::mpl::vector<
  NumberWithMarker,
  Segment,
  SegmentOffset,
  Version,
  Timestamp,
  SequenceNumber
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Convention, T, ConventionTests)
{
  name::Component invalidComponent1;
  name::Component invalidComponent2("1234567890");

  auto test = T()();

  const Name& expected = test.expected;
  BOOST_TEST_MESSAGE("Check " << expected[0].toUri());

  BOOST_CHECK_EQUAL(expected[0].isGeneric(), true);

  name::Component actualComponent = test.makeComponent(test.value);
  BOOST_CHECK_EQUAL(actualComponent, expected[0]);

  Name actualName;
  test.append(actualName, test.value);
  BOOST_CHECK_EQUAL(actualName, expected);

  BOOST_CHECK_EQUAL(test.isComponent(expected[0]), true);
  BOOST_CHECK_EQUAL(test.getValue(expected[0]), test.value);

  BOOST_CHECK_EQUAL(test.isComponent(invalidComponent1), false);
  BOOST_CHECK_EQUAL(test.isComponent(invalidComponent2), false);

  BOOST_CHECK_THROW(test.getValue(invalidComponent1), name::Component::Error);
  BOOST_CHECK_THROW(test.getValue(invalidComponent2), name::Component::Error);
}

BOOST_AUTO_TEST_SUITE_END() // NamingConvention

BOOST_AUTO_TEST_SUITE_END() // TestNameComponent

} // namespace tests
} // namespace ndn
