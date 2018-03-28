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

#include "block-literal.hpp"
#include "boost-test.hpp"
#include <boost/mpl/vector.hpp>

namespace ndn {
namespace name {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(TestNameComponent)

BOOST_AUTO_TEST_SUITE(Decode)

BOOST_AUTO_TEST_CASE(Generic)
{
  Component comp("0807 6E646E2D637878"_block);
  BOOST_CHECK_EQUAL(comp.type(), tlv::GenericNameComponent);
  BOOST_CHECK_EQUAL(comp.toUri(), "ndn-cxx");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("ndn-cxx"), comp);

  comp.wireDecode("0800"_block);
  BOOST_CHECK_EQUAL(comp.toUri(), "...");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("..."), comp);
  BOOST_CHECK_EQUAL(Component::fromEscapedString(".%2E."), comp);

  comp.wireDecode("0801 2E"_block);
  BOOST_CHECK_EQUAL(comp.toUri(), "....");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("...."), comp);
  BOOST_CHECK_EQUAL(Component::fromEscapedString("%2E..%2E"), comp);

  comp.wireDecode("0803 2E412E"_block);
  BOOST_CHECK_EQUAL(comp.toUri(), ".A.");
  BOOST_CHECK_EQUAL(Component::fromEscapedString(".A."), comp);

  comp.wireDecode("0807 666F6F25626172"_block);
  BOOST_CHECK_EQUAL(comp.toUri(), "foo%25bar");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("foo%25bar"), comp);

  comp.wireDecode("0804 2D2E5F7E"_block);
  BOOST_CHECK_EQUAL(comp.toUri(), "-._~");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("-._~"), comp);

  comp = Component(":/?#[]@");
  BOOST_CHECK_EQUAL(comp.toUri(), "%3A%2F%3F%23%5B%5D%40");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("%3A%2F%3F%23%5B%5D%40"), comp);

  BOOST_CHECK_THROW(Component::fromEscapedString(""), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("."), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString(".."), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("8=A"), Component::Error);
}

BOOST_AUTO_TEST_CASE(Digest)
{
  std::string uriPrefix = "sha256digest=";
  std::string hexLower = "28bad4b5275bd392dbb670c75cf0b66f13f7942b21e80f55c0e86b374753a548";
  std::string hexUpper = "28BAD4B5275BD392DBB670C75CF0B66F13F7942B21E80F55C0E86B374753A548";

  Component comp("0120 28BAD4B5275BD392DBB670C75CF0B66F13F7942B21E80F55C0E86B374753A548"_block);
  BOOST_CHECK_EQUAL(comp.toUri(), uriPrefix + hexLower);
  BOOST_CHECK_EQUAL(Component::fromEscapedString(uriPrefix + hexLower), comp);
  BOOST_CHECK_EQUAL(Component::fromEscapedString(uriPrefix + hexUpper), comp);

  BOOST_CHECK_THROW(comp.wireDecode("0108 A791806951F25C4D"_block), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString(uriPrefix), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString(uriPrefix + "=a791806951f25c4d"),
                    Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("1=" + hexLower), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("SHA256DIGEST=" + hexLower), Component::Error);
}

BOOST_AUTO_TEST_CASE(OtherType)
{
  Component comp("0907 6E646E2D637878"_block);
  BOOST_CHECK_EQUAL(comp.type(), 0x09);
  BOOST_CHECK_EQUAL(comp.toUri(), "9=ndn-cxx");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("9=ndn-cxx"), comp);

  comp.wireDecode("FDFFFF00"_block);
  BOOST_CHECK_EQUAL(comp.type(), 0xFFFF);
  BOOST_CHECK_EQUAL(comp.toUri(), "65535=...");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("65535=..."), comp);

  comp.wireDecode("FD576501 2E"_block);
  BOOST_CHECK_EQUAL(comp.type(), 0x5765);
  BOOST_CHECK_EQUAL(comp.toUri(), "22373=....");
  BOOST_CHECK_EQUAL(Component::fromEscapedString("22373=...."), comp);

  BOOST_CHECK_THROW(Component::fromEscapedString("3="), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("3=."), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("3=.."), Component::Error);
}

BOOST_AUTO_TEST_CASE(InvalidType)
{
  Component comp;
  BOOST_CHECK_THROW(comp.wireDecode("0001 80"_block), Component::Error);
  BOOST_CHECK_THROW(comp.wireDecode("FE0001000001 80"_block), Component::Error);

  BOOST_CHECK_THROW(Component::fromEscapedString("0=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("65536=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("-1=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("+=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("0x1=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("Z=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("09=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("0x2=A"), Component::Error);
  BOOST_CHECK_THROW(Component::fromEscapedString("+9=A"), Component::Error);
}

BOOST_AUTO_TEST_SUITE_END() // Decode

BOOST_AUTO_TEST_CASE(Compare)
{
  std::vector<Component> comps = {
    Component("0120 0000000000000000000000000000000000000000000000000000000000000000"_block),
    Component("0120 0000000000000000000000000000000000000000000000000000000000000001"_block),
    Component("0120 FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"_block),
    Component(0x02),
    Component("0201 44"_block),
    Component("0201 46"_block),
    Component("0202 4141"_block),
    Component(),
    Component("D"),
    Component("F"),
    Component("AA"),
    Component(0x53B2),
    Component("FD53B201 44"_block),
    Component("FD53B201 46"_block),
    Component("FD53B202 4141"_block),
  };

  for (size_t i = 0; i < comps.size(); ++i) {
    for (size_t j = 0; j < comps.size(); ++j) {
      Component lhs = comps[i];
      Component rhs = comps[j];
      BOOST_CHECK_EQUAL(lhs == rhs, i == j);
      BOOST_CHECK_EQUAL(lhs != rhs, i != j);
      BOOST_CHECK_EQUAL(lhs <  rhs, i <  j);
      BOOST_CHECK_EQUAL(lhs <= rhs, i <= j);
      BOOST_CHECK_EQUAL(lhs >  rhs, i >  j);
      BOOST_CHECK_EQUAL(lhs >= rhs, i >= j);
    }
  }
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
  Component c(bytes.begin(), bytes.end());
  BOOST_CHECK_EQUAL(c.type(), tlv::GenericNameComponent);
  BOOST_CHECK_EQUAL(c.value_size(), 0);
  BOOST_CHECK_EQUAL(c.size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(OneOctet, T, ContainerTypes)
{
  T bytes{1};
  Component c(0x09, bytes.begin(), bytes.end());
  BOOST_CHECK_EQUAL(c.type(), 0x09);
  BOOST_CHECK_EQUAL(c.value_size(), 1);
  BOOST_CHECK_EQUAL(c.size(), 3);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(FourOctets, T, ContainerTypes)
{
  T bytes{1, 2, 3, 4};
  Component c(0xFCEC, bytes.begin(), bytes.end());
  BOOST_CHECK_EQUAL(c.type(), 0xFCEC);
  BOOST_CHECK_EQUAL(c.value_size(), 4);
  BOOST_CHECK_EQUAL(c.size(), 8);
}

BOOST_AUTO_TEST_SUITE_END() // CreateFromIterators

BOOST_AUTO_TEST_SUITE(NamingConvention)

template<typename ArgType>
struct ConventionTest
{
  function<Component(ArgType)> makeComponent;
  function<ArgType(const Component&)> getValue;
  function<Name&(Name&, ArgType)> append;
  Name expected;
  ArgType value;
  function<bool(const Component&)> isComponent;
};

class NumberWithMarker
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {bind(&Component::fromNumberWithMarker, 0xAA, _1),
            bind(&Component::toNumberWithMarker, _1, 0xAA),
            bind(&Name::appendNumberWithMarker, _1, 0xAA, _2),
            Name("/%AA%03%E8"),
            1000,
            bind(&Component::isNumberWithMarker, _1, 0xAA)};
  }
};

class Segment
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&Component::fromSegment,
            bind(&Component::toSegment, _1),
            bind(&Name::appendSegment, _1, _2),
            Name("/%00%27%10"),
            10000,
            bind(&Component::isSegment, _1)};
  }
};

class SegmentOffset
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&Component::fromSegmentOffset,
            bind(&Component::toSegmentOffset, _1),
            bind(&Name::appendSegmentOffset, _1, _2),
            Name("/%FB%00%01%86%A0"),
            100000,
            bind(&Component::isSegmentOffset, _1)};
  }
};

class Version
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&Component::fromVersion,
            bind(&Component::toVersion, _1),
            [] (Name& name, uint64_t version) -> Name& { return name.appendVersion(version); },
            Name("/%FD%00%0FB%40"),
            1000000,
            bind(&Component::isVersion, _1)};
  }
};

class Timestamp
{
public:
  ConventionTest<time::system_clock::TimePoint>
  operator()() const
  {
    return {&Component::fromTimestamp,
            bind(&Component::toTimestamp, _1),
            [] (Name& name, time::system_clock::TimePoint t) -> Name& { return name.appendTimestamp(t); },
            Name("/%FC%00%04%7BE%E3%1B%00%00"),
            time::getUnixEpoch() + 14600_days, // 40 years
            bind(&Component::isTimestamp, _1)};
  }
};

class SequenceNumber
{
public:
  ConventionTest<uint64_t>
  operator()() const
  {
    return {&Component::fromSequenceNumber,
            bind(&Component::toSequenceNumber, _1),
            bind(&Name::appendSequenceNumber, _1, _2),
            Name("/%FE%00%98%96%80"),
            10000000,
            bind(&Component::isSequenceNumber, _1)};
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
  Component invalidComponent1;
  Component invalidComponent2("1234567890");

  auto test = T()();

  const Name& expected = test.expected;
  BOOST_TEST_MESSAGE("Check " << expected[0].toUri());

  BOOST_CHECK_EQUAL(expected[0].isGeneric(), true);

  Component actualComponent = test.makeComponent(test.value);
  BOOST_CHECK_EQUAL(actualComponent, expected[0]);

  Name actualName;
  test.append(actualName, test.value);
  BOOST_CHECK_EQUAL(actualName, expected);

  BOOST_CHECK_EQUAL(test.isComponent(expected[0]), true);
  BOOST_CHECK_EQUAL(test.getValue(expected[0]), test.value);

  BOOST_CHECK_EQUAL(test.isComponent(invalidComponent1), false);
  BOOST_CHECK_EQUAL(test.isComponent(invalidComponent2), false);

  BOOST_CHECK_THROW(test.getValue(invalidComponent1), Component::Error);
  BOOST_CHECK_THROW(test.getValue(invalidComponent2), Component::Error);
}

BOOST_AUTO_TEST_SUITE_END() // NamingConvention

BOOST_AUTO_TEST_SUITE_END() // TestNameComponent

} // namespace tests
} // namespace name
} // namespace ndn
