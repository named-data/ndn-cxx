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

#include "name.hpp"

#include "boost-test.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/mpl/vector.hpp>
#include <unordered_map>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(TestName)

static const uint8_t TestName[] = {
        0x7,  0x14, // Name
          0x8,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x8,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x8,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78
};

const uint8_t Name1[] = {0x7,  0x7, // Name
                           0x8,  0x5, // NameComponent
                             0x6c,  0x6f,  0x63,  0x61,  0x6c};

const uint8_t Name2[] = {0x7,  0xc, // Name
                           0x8,  0x5, // NameComponent
                             0x6c,  0x6f,  0x63,  0x61,  0x6c,
                           0x8,  0x3, // NameComponent
                             0x6e,  0x64,  0x6e};

static const uint8_t TestNameComponent[] = {
        0x8, 0x3, // NameComponent
          0x6e, 0x64, 0x6e};

static const uint8_t TestDigestComponent[] = {
        0x1, 0x20, // ImplicitSha256DigestComponent
          0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
          0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
          0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
          0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x48 };

const uint8_t Component1[] = {0x7, 0x3, // Error in Type
                                0x6e, 0x64, 0x6e};


BOOST_AUTO_TEST_CASE(Basic)
{
  Name name("/hello/world");

  BOOST_CHECK_NO_THROW(name.at(0));
  BOOST_CHECK_NO_THROW(name.at(1));
  BOOST_CHECK_NO_THROW(name.at(-1));
  BOOST_CHECK_NO_THROW(name.at(-2));

  BOOST_CHECK_THROW(name.at(2), Name::Error);
  BOOST_CHECK_THROW(name.at(-3), Name::Error);
}

BOOST_AUTO_TEST_CASE(Encode)
{
  Name name("/local/ndn/prefix");

  const Block &wire = name.wireEncode();

  // for (Buffer::const_iterator i = wire.begin();
  //      i != wire.end();
  //      ++i)
  //   {
  //     std::ios::fmtflags saveFlags = std::cout.flags(std::ios::hex);

  //     if (i != wire.begin())
  //       std::cout << ", ";
  //     std::cout << "0x" << static_cast<uint32_t>(*i);

  //     std::cout.flags(saveFlags);
  //   }
  // std::cout << std::endl;

  BOOST_CHECK_EQUAL_COLLECTIONS(TestName, TestName+sizeof(TestName),
                                wire.begin(), wire.end());
}


BOOST_AUTO_TEST_CASE(Decode)
{
  Block block(TestName, sizeof(TestName));

  Name name(block);

  BOOST_CHECK_EQUAL(name.toUri(), "/local/ndn/prefix");
}

BOOST_AUTO_TEST_CASE(DecodeComponent)
{
  Block componentBlock(TestNameComponent, sizeof(TestNameComponent));
  name::Component nameComponent;
  BOOST_REQUIRE_NO_THROW(nameComponent.wireDecode(componentBlock));
  BOOST_CHECK_EQUAL(nameComponent.toUri(), "ndn");

  Block digestComponentBlock(TestDigestComponent, sizeof(TestDigestComponent));
  name::Component digestComponent;
  BOOST_REQUIRE_NO_THROW(digestComponent.wireDecode(digestComponentBlock));

  Block errorBlock(Component1, sizeof(Component1));
  name::Component errorComponent;
  BOOST_REQUIRE_THROW(errorComponent.wireDecode(errorBlock), name::Component::Error);
}

BOOST_AUTO_TEST_CASE(AppendsAndMultiEncode)
{
  Name name("/local");

  BOOST_CHECK_EQUAL_COLLECTIONS(name.wireEncode().begin(), name.wireEncode().end(),
                                Name1, Name1 + sizeof(Name1));

  name.append("ndn");

  BOOST_CHECK_EQUAL_COLLECTIONS(name.wireEncode().begin(), name.wireEncode().end(),
                                Name2, Name2 + sizeof(Name2));

  name.append("prefix");
  BOOST_CHECK_EQUAL_COLLECTIONS(name.wireEncode().begin(), name.wireEncode().end(),
                                TestName, TestName+sizeof(TestName));
}

BOOST_AUTO_TEST_CASE(ZeroLengthComponent)
{
  static const uint8_t compOctets[] {0x08, 0x00};
  Block compBlock(compOctets, sizeof(compOctets));
  name::Component comp;
  BOOST_REQUIRE_NO_THROW(comp.wireDecode(compBlock));
  BOOST_CHECK_EQUAL(comp.value_size(), 0);

  static const uint8_t nameOctets[] {0x07, 0x08, 0x08, 0x01, 0x41, 0x08, 0x00, 0x08, 0x01, 0x42};
  Block nameBlock(nameOctets, sizeof(nameOctets));
  static const std::string nameUri("/A/.../B");
  Name name;
  BOOST_REQUIRE_NO_THROW(name.wireDecode(nameBlock));
  BOOST_CHECK_EQUAL(name.toUri(), nameUri);
  Block nameEncoded = name.wireEncode();
  BOOST_CHECK(nameEncoded == nameBlock);

  Name name2;
  BOOST_REQUIRE_NO_THROW(name2 = std::move(Name(nameUri)));
  BOOST_CHECK_EQUAL(name2.toUri(), nameUri);
  Block name2Encoded = name2.wireEncode();
  BOOST_CHECK(name2Encoded == nameBlock);
}

BOOST_AUTO_TEST_CASE(AppendNumber)
{
  Name name;
  for (uint32_t i = 0; i < 10; i++)
    {
      name.appendNumber(i);
    }

  BOOST_CHECK_EQUAL(name.size(), 10);

  for (uint32_t i = 0; i < 10; i++)
    {
      BOOST_CHECK_EQUAL(name[i].toNumber(), i);
    }
}

class Numeric
{
public:
  typedef std::list<boost::tuple<function<name::Component(uint64_t)>,
                                 function<uint64_t(const name::Component&)>,
                                 function<Name&(Name&, uint64_t)>,
                                 Name/*expected*/,
                                 uint64_t/*value*/,
                                 function<bool(const name::Component&)> > > Dataset;

  Numeric()
  {
    dataset.push_back(boost::make_tuple(bind(&name::Component::fromNumberWithMarker,
                                             0xAA, _1),
                                        bind(&name::Component::toNumberWithMarker, _1, 0xAA),
                                        bind(&Name::appendNumberWithMarker, _1, 0xAA, _2),
                                        Name("/%AA%03%E8"),
                                        1000,
                                        bind(&name::Component::isNumberWithMarker, _1, 0xAA)));
    dataset.push_back(boost::make_tuple(&name::Component::fromSegment,
                                        bind(&name::Component::toSegment, _1),
                                        bind(&Name::appendSegment, _1, _2),
                                        Name("/%00%27%10"),
                                        10000,
                                        bind(&name::Component::isSegment, _1)));
    dataset.push_back(boost::make_tuple(&name::Component::fromSegmentOffset,
                                        bind(&name::Component::toSegmentOffset, _1),
                                        bind(&Name::appendSegmentOffset, _1, _2),
                                        Name("/%FB%00%01%86%A0"),
                                        100000,
                                        bind(&name::Component::isSegmentOffset, _1)));
    dataset.push_back(boost::make_tuple(&name::Component::fromVersion,
                                        bind(&name::Component::toVersion, _1),
                                        bind(static_cast<Name&(Name::*)(uint64_t)>(
                                               &Name::appendVersion), _1, _2),
                                        Name("/%FD%00%0FB%40"),
                                        1000000,
                                        bind(&name::Component::isVersion, _1)));
    dataset.push_back(boost::make_tuple(&name::Component::fromSequenceNumber,
                                        bind(&name::Component::toSequenceNumber, _1),
                                        bind(&Name::appendSequenceNumber, _1, _2),
                                        Name("/%FE%00%98%96%80"),
                                        10000000,
                                        bind(&name::Component::isSequenceNumber, _1)));
  }

  Dataset dataset;
};

class Timestamp
{
public:
  typedef std::list<boost::tuple<function<name::Component(const time::system_clock::TimePoint&)>,
                                 function<time::system_clock::TimePoint(const name::Component&)>,
                                 function<Name&(Name&, const time::system_clock::TimePoint&)>,
                                 Name/*expected*/,
                                 time::system_clock::TimePoint/*value*/,
                                 function<bool(const name::Component&)> > > Dataset;
  Timestamp()
  {
    dataset.push_back(boost::make_tuple(&name::Component::fromTimestamp,
                                        ndn::bind(&name::Component::toTimestamp, _1),
                                        ndn::bind(&Name::appendTimestamp, _1, _2),
                                        Name("/%FC%00%04%7BE%E3%1B%00%00"),
                                        time::getUnixEpoch() + time::days(14600/*40 years*/),
                                        bind(&name::Component::isTimestamp, _1)));
  }

  Dataset dataset;
};

typedef boost::mpl::vector<Numeric, Timestamp> ConventionsDatasets;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(NamingConventions, T, ConventionsDatasets, T)
{
  // // These octets are obtained by the snippet below.
  // // This check is intended to detect unexpected encoding change in the future.
  // for (typename T::Dataset::const_iterator it = this->dataset.begin();
  //      it != this->dataset.end(); ++it) {
  //   Name name;
  //   name.append(it->template get<0>()(it->template get<4>()));
  //   std::cout << name << std::endl;
  // }

  name::Component invalidComponent1;
  name::Component invalidComponent2("1234567890");

  for (typename T::Dataset::const_iterator it = this->dataset.begin();
       it != this->dataset.end(); ++it) {
    const Name& expected = it->template get<3>();
    BOOST_TEST_MESSAGE("Check " << expected[0].toUri());

    BOOST_CHECK_EQUAL(expected[0].isGeneric(), true);

    name::Component actualComponent = it->template get<0>()(it->template get<4>());
    BOOST_CHECK_EQUAL(actualComponent, expected[0]);

    Name actualName;
    it->template get<2>()(actualName, it->template get<4>());
    BOOST_CHECK_EQUAL(actualName, expected);

    BOOST_CHECK_EQUAL(it->template get<5>()(expected[0]), true);
    BOOST_REQUIRE_NO_THROW(it->template get<1>()(expected[0]));
    BOOST_CHECK_EQUAL(it->template get<1>()(expected[0]), it->template get<4>());

    BOOST_CHECK_EQUAL(it->template get<5>()(invalidComponent1), false);
    BOOST_CHECK_EQUAL(it->template get<5>()(invalidComponent2), false);

    BOOST_REQUIRE_THROW(it->template get<1>()(invalidComponent1), name::Component::Error);
    BOOST_REQUIRE_THROW(it->template get<1>()(invalidComponent2), name::Component::Error);
  }
}

BOOST_AUTO_TEST_CASE(GetSuccessor)
{
  BOOST_CHECK_EQUAL(Name("ndn:/%00%01/%01%02").getSuccessor(), Name("ndn:/%00%01/%01%03"));
  BOOST_CHECK_EQUAL(Name("ndn:/%00%01/%01%FF").getSuccessor(), Name("ndn:/%00%01/%02%00"));
  BOOST_CHECK_EQUAL(Name("ndn:/%00%01/%FF%FF").getSuccessor(), Name("ndn:/%00%01/%00%00%00"));
  BOOST_CHECK_EQUAL(Name().getSuccessor(), Name("ndn:/%00"));
}

BOOST_AUTO_TEST_CASE(Markers)
{
  Name name;
  uint64_t number;

  BOOST_REQUIRE_NO_THROW(number = name.appendSegment(30923).at(-1).toSegment());
  BOOST_CHECK_EQUAL(number, 30923);

  BOOST_REQUIRE_NO_THROW(number = name.appendSegmentOffset(589).at(-1).toSegmentOffset());
  BOOST_CHECK_EQUAL(number, 589);

  BOOST_REQUIRE_NO_THROW(number = name.appendVersion().at(-1).toVersion());

  BOOST_REQUIRE_NO_THROW(number = name.appendVersion(25912).at(-1).toVersion());
  BOOST_CHECK_EQUAL(number, 25912);

  const time::system_clock::TimePoint tp = time::system_clock::now();
  time::system_clock::TimePoint tp2;
  BOOST_REQUIRE_NO_THROW(tp2 = name.appendTimestamp(tp).at(-1).toTimestamp());
  BOOST_CHECK_LE(std::abs(time::duration_cast<time::microseconds>(tp2 - tp).count()), 1);

  BOOST_REQUIRE_NO_THROW(number = name.appendSequenceNumber(11676).at(-1).toSequenceNumber());
  BOOST_CHECK_EQUAL(number, 11676);
}

BOOST_AUTO_TEST_CASE(UnorderedMap)
{
  std::unordered_map<Name, int> map;
  Name name1("/1");
  Name name2("/2");
  Name name3("/3");
  map[name1] = 1;
  map[name2] = 2;
  map[name3] = 3;

  BOOST_CHECK_EQUAL(map[name1], 1);
  BOOST_CHECK_EQUAL(map[name2], 2);
  BOOST_CHECK_EQUAL(map[name3], 3);
}

BOOST_AUTO_TEST_CASE(ImplicitSha256Digest)
{
  Name n;

  static const uint8_t DIGEST[] = {0x28, 0xba, 0xd4, 0xb5, 0x27, 0x5b, 0xd3, 0x92,
                                   0xdb, 0xb6, 0x70, 0xc7, 0x5c, 0xf0, 0xb6, 0x6f,
                                   0x13, 0xf7, 0x94, 0x2b, 0x21, 0xe8, 0x0f, 0x55,
                                   0xc0, 0xe8, 0x6b, 0x37, 0x47, 0x53, 0xa5, 0x48,
                                   0x00, 0x00};

  BOOST_REQUIRE_NO_THROW(n.appendImplicitSha256Digest(DIGEST, 32));
  BOOST_REQUIRE_NO_THROW(n.appendImplicitSha256Digest(make_shared<Buffer>(DIGEST, 32)));
  BOOST_CHECK_EQUAL(n.get(0), n.get(1));

  BOOST_REQUIRE_THROW(n.appendImplicitSha256Digest(DIGEST, 34), name::Component::Error);
  BOOST_REQUIRE_THROW(n.appendImplicitSha256Digest(DIGEST, 30), name::Component::Error);

  n.append(DIGEST, 32);
  BOOST_CHECK_LT(n.get(0), n.get(2));
  BOOST_CHECK_EQUAL_COLLECTIONS(n.get(0).value_begin(), n.get(0).value_end(),
                                n.get(2).value_begin(), n.get(2).value_end());

  n.append(DIGEST + 1, 32);
  BOOST_CHECK_LT(n.get(0), n.get(3));

  n.append(DIGEST + 2, 32);
  BOOST_CHECK_LT(n.get(0), n.get(4));

  BOOST_CHECK_EQUAL(n.get(0).toUri(), "sha256digest="
                    "28bad4b5275bd392dbb670c75cf0b66f13f7942b21e80f55c0e86b374753a548");

  BOOST_CHECK_EQUAL(n.get(0).isImplicitSha256Digest(), true);
  BOOST_CHECK_EQUAL(n.get(2).isImplicitSha256Digest(), false);

  BOOST_CHECK_THROW(Name("/hello/sha256digest=hmm"), name::Component::Error);

  Name n2;
  // check canonical URI encoding (lower case)
  BOOST_CHECK_NO_THROW(n2 = Name("/hello/sha256digest="
                              "28bad4b5275bd392dbb670c75cf0b66f13f7942b21e80f55c0e86b374753a548"));
  BOOST_CHECK_EQUAL(n.get(0), n2.get(1));

  // will accept hex value in upper case too
  BOOST_CHECK_NO_THROW(n2 = Name("/hello/sha256digest="
                              "28BAD4B5275BD392DBB670C75CF0B66F13F7942B21E80F55C0E86B374753A548"));
  BOOST_CHECK_EQUAL(n.get(0), n2.get(1));

  // this is not valid sha256digest component, will be treated as generic component
  BOOST_CHECK_NO_THROW(n2 = Name("/hello/SHA256DIGEST="
                              "28BAD4B5275BD392DBB670C75CF0B66F13F7942B21E80F55C0E86B374753A548"));
  BOOST_CHECK_NE(n.get(0), n2.get(1));
  BOOST_CHECK(n2.get(1).isGeneric());
}

BOOST_AUTO_TEST_CASE(Compare)
{
  BOOST_CHECK_EQUAL( 0, Name("/A")  .compare(Name("/A")));
  BOOST_CHECK_EQUAL( 0, Name("/A")  .compare(Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/A")  .compare(Name("/B")));
  BOOST_CHECK_EQUAL( 1, Name("/B")  .compare(Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/A")  .compare(Name("/AA")));
  BOOST_CHECK_EQUAL( 1, Name("/AA") .compare(Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/A")  .compare(Name("/A/C")));
  BOOST_CHECK_EQUAL( 1, Name("/A/C").compare(Name("/A")));

  BOOST_CHECK_EQUAL( 0, Name("/Z/A/Y")  .compare(1, 1, Name("/A")));
  BOOST_CHECK_EQUAL( 0, Name("/Z/A/Y")  .compare(1, 1, Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/B")));
  BOOST_CHECK_EQUAL( 1, Name("/Z/B/Y")  .compare(1, 1, Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/AA")));
  BOOST_CHECK_EQUAL( 1, Name("/Z/AA/Y") .compare(1, 1, Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/A/C")));
  BOOST_CHECK_EQUAL( 1, Name("/Z/A/C/Y").compare(1, 2, Name("/A")));

  BOOST_CHECK_EQUAL( 0, Name("/Z/A")  .compare(1, Name::npos, Name("/A")));
  BOOST_CHECK_EQUAL( 0, Name("/Z/A")  .compare(1, Name::npos, Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A")  .compare(1, Name::npos, Name("/B")));
  BOOST_CHECK_EQUAL( 1, Name("/Z/B")  .compare(1, Name::npos, Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A")  .compare(1, Name::npos, Name("/AA")));
  BOOST_CHECK_EQUAL( 1, Name("/Z/AA") .compare(1, Name::npos, Name("/A")));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A")  .compare(1, Name::npos, Name("/A/C")));
  BOOST_CHECK_EQUAL( 1, Name("/Z/A/C").compare(1, Name::npos, Name("/A")));

  BOOST_CHECK_EQUAL( 0, Name("/Z/A/Y")  .compare(1, 1, Name("/X/A/W"),   1, 1));
  BOOST_CHECK_EQUAL( 0, Name("/Z/A/Y")  .compare(1, 1, Name("/X/A/W"),   1, 1));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/X/B/W"),   1, 1));
  BOOST_CHECK_EQUAL( 1, Name("/Z/B/Y")  .compare(1, 1, Name("/X/A/W"),   1, 1));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/X/AA/W"),  1, 1));
  BOOST_CHECK_EQUAL( 1, Name("/Z/AA/Y") .compare(1, 1, Name("/X/A/W"),   1, 1));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/X/A/C/W"), 1, 2));
  BOOST_CHECK_EQUAL( 1, Name("/Z/A/C/Y").compare(1, 2, Name("/X/A/W"),   1, 1));

  BOOST_CHECK_EQUAL( 0, Name("/Z/A/Y")  .compare(1, 1, Name("/X/A"),   1));
  BOOST_CHECK_EQUAL( 0, Name("/Z/A/Y")  .compare(1, 1, Name("/X/A"),   1));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/X/B"),   1));
  BOOST_CHECK_EQUAL( 1, Name("/Z/B/Y")  .compare(1, 1, Name("/X/A"),   1));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/X/AA"),  1));
  BOOST_CHECK_EQUAL( 1, Name("/Z/AA/Y") .compare(1, 1, Name("/X/A"),   1));
  BOOST_CHECK_EQUAL(-1, Name("/Z/A/Y")  .compare(1, 1, Name("/X/A/C"), 1));
  BOOST_CHECK_EQUAL( 1, Name("/Z/A/C/Y").compare(1, 2, Name("/X/A"),   1));
}

BOOST_AUTO_TEST_CASE(ZeroLengthComponentCompare)
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

BOOST_AUTO_TEST_CASE(CreateComponentWithIterators) // Bug #2490
{
  {
    std::vector<uint8_t> bytes = {1};
    name::Component c(bytes.begin(), bytes.end());
    BOOST_CHECK_EQUAL(c.value_size(), 1);
    BOOST_CHECK_EQUAL(c.size(), 3);
  }

  {
    std::list<uint8_t> bytes = {1, 2, 3, 4};
    name::Component c(bytes.begin(), bytes.end());
    BOOST_CHECK_EQUAL(c.value_size(), 4);
    BOOST_CHECK_EQUAL(c.size(), 6);
  }

  {
    std::vector<int8_t> bytes = {1};
    name::Component c(bytes.begin(), bytes.end());
    BOOST_CHECK_EQUAL(c.value_size(), 1);
    BOOST_CHECK_EQUAL(c.size(), 3);
  }

  {
    std::list<int8_t> bytes = {1, 2, 3, 4};
    name::Component c(bytes.begin(), bytes.end());
    BOOST_CHECK_EQUAL(c.value_size(), 4);
    BOOST_CHECK_EQUAL(c.size(), 6);
  }
}

BOOST_AUTO_TEST_CASE(NameWithSpaces)
{
  Name name("/ hello\t/\tworld \r\n");

  BOOST_CHECK_EQUAL("/hello/world", name);
  BOOST_CHECK_THROW(Name("/hello//world"), name::Component::Error);
}

BOOST_AUTO_TEST_CASE(Append)
{
  PartialName toAppend("/and");
  PartialName toAppend1("/beyond");
  {
    Name name("/hello/world");
    BOOST_CHECK_EQUAL("/hello/world/hello/world", name.append(name));
    BOOST_CHECK_EQUAL("/hello/world/hello/world", name);
  }
  {
    Name name("/hello/world");
    BOOST_CHECK_EQUAL("/hello/world/and", name.append(toAppend));
  }
  {
    Name name("/hello/world");
    BOOST_CHECK_EQUAL("/hello/world/and/beyond", name.append(toAppend).append(toAppend1));
  }
}

BOOST_AUTO_TEST_CASE(SubName)
{
  Name name("/hello/world");

  BOOST_CHECK_EQUAL("/hello/world", name.getSubName(0));
  BOOST_CHECK_EQUAL("/world", name.getSubName(1));
  BOOST_CHECK_EQUAL("/hello/", name.getSubName(0, 1));
}

BOOST_AUTO_TEST_CASE(SubNameNegativeIndex)
{
  Name name("/first/second/third/last");

  BOOST_CHECK_EQUAL("/last", name.getSubName(-1));
  BOOST_CHECK_EQUAL("/third/last", name.getSubName(-2));
  BOOST_CHECK_EQUAL("/second", name.getSubName(-3, 1));
}

BOOST_AUTO_TEST_CASE(SubNameOutOfRangeIndexes)
{
  Name name("/first/second/last");
  // No length
  BOOST_CHECK_EQUAL("/first/second/last", name.getSubName(-10));
  BOOST_CHECK_EQUAL("/", name.getSubName(10));

  // Starting after the max position
  BOOST_CHECK_EQUAL("/", name.getSubName(10, 1));
  BOOST_CHECK_EQUAL("/", name.getSubName(10, 10));

  // Not enough components
  BOOST_CHECK_EQUAL("/second/last", name.getSubName(1, 10));
  BOOST_CHECK_EQUAL("/last", name.getSubName(-1, 10));

  // Start before first
  BOOST_CHECK_EQUAL("/first/second", name.getSubName(-10, 2));
  BOOST_CHECK_EQUAL("/first/second/last", name.getSubName(-10, 10));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
