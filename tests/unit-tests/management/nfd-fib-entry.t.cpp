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

#include "management/nfd-fib-entry.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdFibEntry)

const uint8_t TestNextHopRecord[] =
{
  0x81, 0x06, 0x69, 0x01, 0x0a, 0x6a, 0x01, 0xc8
};

const uint8_t TestFibEntryNoNextHops[] =
{
  0x80, 0x15, 0x07, 0x13, 0x08, 0x04, 0x74, 0x68, 0x69, 0x73,
  0x08, 0x02, 0x69, 0x73, 0x08, 0x01, 0x61, 0x08, 0x04, 0x74,
  0x65, 0x73, 0x74
};

const uint8_t TestFibEntry[] =
{
  0x80, 0x38, 0x07, 0x13, 0x08, 0x04, 0x74, 0x68, 0x69, 0x73, 0x08, 0x02, 0x69, 0x73, 0x08, 0x01,
  0x61, 0x08, 0x04, 0x74, 0x65, 0x73, 0x74, 0x81, 0x06, 0x69, 0x01, 0x0a, 0x6a, 0x01, 0xc8, 0x81,
  0x07, 0x69, 0x01, 0x14, 0x6a, 0x02, 0x01, 0x2c, 0x81, 0x07, 0x69, 0x01, 0x1e, 0x6a, 0x02, 0x01,
  0x90, 0x81, 0x07, 0x69, 0x01, 0x28, 0x6a, 0x02, 0x01, 0xf4
};

BOOST_AUTO_TEST_CASE(TestNextHopRecordEncode)
{
  NextHopRecord record;
  record.setFaceId(10);
  record.setCost(200);

  const Block& wire = record.wireEncode();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestNextHopRecord,
                                  TestNextHopRecord + sizeof(TestNextHopRecord),
                                  wire.begin(), wire.end());

}

BOOST_AUTO_TEST_CASE(TestNextHopRecordDecode)
{
  NextHopRecord record;

  BOOST_REQUIRE_NO_THROW(record.wireDecode(Block(TestNextHopRecord,
                                                sizeof(TestNextHopRecord))));
  BOOST_REQUIRE_EQUAL(record.getFaceId(), 10);
  BOOST_REQUIRE_EQUAL(record.getCost(), 200);
}

BOOST_AUTO_TEST_CASE(TestFibEntryNoNextHopEncode)
{
  FibEntry entry;
  entry.setPrefix("/this/is/a/test");

  const Block& wire = entry.wireEncode();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestFibEntryNoNextHops,
                                  TestFibEntryNoNextHops + sizeof(TestFibEntryNoNextHops),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(TestFibEntryNoNextHopsDecode)
{
  FibEntry entry;
  BOOST_REQUIRE_NO_THROW(entry.wireDecode(Block(TestFibEntryNoNextHops,
                                                sizeof(TestFibEntryNoNextHops))));

  BOOST_REQUIRE_EQUAL(entry.getPrefix(), "/this/is/a/test");
  BOOST_REQUIRE(entry.getNextHopRecords().empty());
}

BOOST_AUTO_TEST_CASE(TestFibEntryEncode)
{
  FibEntry entry;
  entry.setPrefix("/this/is/a/test");

  std::list<NextHopRecord> records;

  for (int i = 1; i < 4; i++)
    {
      NextHopRecord record;
      record.setFaceId(i * 10);
      record.setCost((i * 100) + 100);
      records.push_back(record);
    }

  entry.setNextHopRecords(records.begin(), records.end());

  NextHopRecord oneMore;
  oneMore.setFaceId(40);
  oneMore.setCost(500);

  entry.addNextHopRecord(oneMore);

  const Block& wire = entry.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(TestFibEntry,
                                TestFibEntry + sizeof(TestFibEntry),
                                wire.begin(), wire.end());

  // std::ofstream of("out.tmp");
  // of.write((const char*)entry.wireEncode().wire(),
  //          entry.wireEncode().size());
}

BOOST_AUTO_TEST_CASE(TestFibEntryDecode)
{
  FibEntry entry;
  BOOST_REQUIRE_NO_THROW(entry.wireDecode(Block(TestFibEntry,
                                                sizeof(TestFibEntry))));

  std::list<NextHopRecord> records = entry.getNextHopRecords();

  BOOST_CHECK_EQUAL(entry.getPrefix(), "/this/is/a/test");
  BOOST_CHECK_EQUAL(entry.getNextHopRecords().size(), 4);

  size_t value = 1;

  for (std::list<NextHopRecord>::const_iterator i = records.begin();
       i != records.end();
       ++i)
    {
      BOOST_CHECK_EQUAL(i->getFaceId(), value * 10);
      BOOST_CHECK_EQUAL(i->getCost(), (value * 100) + 100);
      ++value;
    }
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
