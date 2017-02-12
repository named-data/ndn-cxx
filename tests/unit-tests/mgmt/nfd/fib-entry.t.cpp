/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "mgmt/nfd/fib-entry.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestFibEntry)

static FibEntry
makeFibEntry()
{
  std::vector<NextHopRecord> nexthops;
  for (size_t i = 1; i < 4; i++) {
    nexthops.push_back(NextHopRecord()
                       .setFaceId(i * 10)
                       .setCost(i * 100 + 100));
  }

  return FibEntry()
      .setPrefix("/this/is/a/test")
      .setNextHopRecords(nexthops.begin(), nexthops.end());
}

BOOST_AUTO_TEST_CASE(NextHopRecordEncode)
{
  NextHopRecord record1;
  record1.setFaceId(10)
      .setCost(200);
  const Block& wire = record1.wireEncode();

  static const uint8_t expected[] = {
    0x81, 0x06, 0x69, 0x01, 0x0a, 0x6a, 0x01, 0xc8
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  NextHopRecord record2(wire);
  BOOST_CHECK_EQUAL(record1, record2);
}

BOOST_AUTO_TEST_CASE(NextHopRecordEquality)
{
  NextHopRecord record1, record2;

  record1.setFaceId(10)
      .setCost(200);
  record2 = record1;
  BOOST_CHECK_EQUAL(record1, record2);

  record2.setFaceId(42);
  BOOST_CHECK_NE(record1, record2);

  record2 = record1;
  record2.setCost(42);
  BOOST_CHECK_NE(record1, record2);
}

BOOST_AUTO_TEST_CASE(FibEntryNoNextHopsEncode)
{
  FibEntry entry1;
  entry1.setPrefix("/this/is/a/test");
  BOOST_REQUIRE(entry1.getNextHopRecords().empty());
  const Block& wire = entry1.wireEncode();

  static const uint8_t expected[] = {
    0x80, 0x15, 0x07, 0x13, 0x08, 0x04, 0x74, 0x68, 0x69, 0x73,
    0x08, 0x02, 0x69, 0x73, 0x08, 0x01, 0x61, 0x08, 0x04, 0x74,
    0x65, 0x73, 0x74
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  FibEntry entry2(wire);
  BOOST_CHECK_EQUAL(entry1, entry2);
}

BOOST_AUTO_TEST_CASE(FibEntryEncode)
{
  FibEntry entry1 = makeFibEntry();
  NextHopRecord oneMore;
  oneMore.setFaceId(40);
  oneMore.setCost(500);
  entry1.addNextHopRecord(oneMore);
  const Block& wire = entry1.wireEncode();

  static const uint8_t expected[] = {
    0x80, 0x38, 0x07, 0x13, 0x08, 0x04, 0x74, 0x68, 0x69, 0x73, 0x08, 0x02, 0x69, 0x73, 0x08, 0x01,
    0x61, 0x08, 0x04, 0x74, 0x65, 0x73, 0x74, 0x81, 0x06, 0x69, 0x01, 0x0a, 0x6a, 0x01, 0xc8, 0x81,
    0x07, 0x69, 0x01, 0x14, 0x6a, 0x02, 0x01, 0x2c, 0x81, 0x07, 0x69, 0x01, 0x1e, 0x6a, 0x02, 0x01,
    0x90, 0x81, 0x07, 0x69, 0x01, 0x28, 0x6a, 0x02, 0x01, 0xf4
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  FibEntry entry2(wire);
  BOOST_CHECK_EQUAL(entry1, entry2);
}

BOOST_AUTO_TEST_CASE(FibEntryEquality)
{
  FibEntry entry1, entry2;
  BOOST_CHECK_EQUAL(entry1, entry2);

  entry1 = entry2 = makeFibEntry();
  BOOST_CHECK_EQUAL(entry1, entry2);
  BOOST_CHECK_EQUAL(entry2, entry1);

  entry2.setPrefix("/another/prefix");
  BOOST_CHECK_NE(entry1, entry2);

  entry2 = entry1;
  std::vector<NextHopRecord> empty;
  entry2.setNextHopRecords(empty.begin(), empty.end());
  BOOST_CHECK_NE(entry1, entry2);
  BOOST_CHECK_NE(entry2, entry1);

  entry2 = entry1;
  auto nh1 = NextHopRecord()
             .setFaceId(1)
             .setCost(1000);
  entry1.addNextHopRecord(nh1);
  BOOST_CHECK_NE(entry1, entry2);
  BOOST_CHECK_NE(entry2, entry1);

  auto nh42 = NextHopRecord()
              .setFaceId(42)
              .setCost(42);
  entry1.addNextHopRecord(nh42);
  entry2.addNextHopRecord(nh42)
      .addNextHopRecord(nh1);
  BOOST_CHECK_EQUAL(entry1, entry2); // order of NextHopRecords is irrelevant
  BOOST_CHECK_EQUAL(entry2, entry1);

  entry1 = entry2 = makeFibEntry();
  entry1.addNextHopRecord(nh1)
      .addNextHopRecord(nh42);
  entry2.addNextHopRecord(nh42)
      .addNextHopRecord(nh42);
  BOOST_CHECK_NE(entry1, entry2); // match each NextHopRecord at most once
  BOOST_CHECK_NE(entry2, entry1);
}

BOOST_AUTO_TEST_CASE(Print)
{
  NextHopRecord record;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(record),
                    "NextHopRecord(FaceId: 0, Cost: 0)");

  FibEntry entry;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(entry),
                    "FibEntry(Prefix: /,\n"
                    "         NextHops: []\n"
                    "         )");

  entry = makeFibEntry();
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(entry),
                    "FibEntry(Prefix: /this/is/a/test,\n"
                    "         NextHops: [NextHopRecord(FaceId: 10, Cost: 200),\n"
                    "                    NextHopRecord(FaceId: 20, Cost: 300),\n"
                    "                    NextHopRecord(FaceId: 30, Cost: 400)]\n"
                    "         )");
}

BOOST_AUTO_TEST_SUITE_END() // TestFibEntry
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
