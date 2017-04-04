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

#include "mgmt/nfd/forwarder-status.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestForwarderStatus)

static ForwarderStatus
makeForwarderStatus()
{
  return ForwarderStatus()
      .setNfdVersion("0.5.1-14-g05dd444")
      .setStartTimestamp(time::fromUnixTimestamp(time::milliseconds(375193249325LL)))
      .setCurrentTimestamp(time::fromUnixTimestamp(time::milliseconds(886109034272LL)))
      .setNNameTreeEntries(1849943160)
      .setNFibEntries(621739748)
      .setNPitEntries(482129741)
      .setNMeasurementsEntries(1771725298)
      .setNCsEntries(1264968688)
      .setNInInterests(612811615)
      .setNInData(1843576050)
      .setNInNacks(1234)
      .setNOutInterests(952144445)
      .setNOutData(138198826)
      .setNOutNacks(4321);
}

BOOST_AUTO_TEST_CASE(Encode)
{
  ForwarderStatus status1 = makeForwarderStatus();
  Block wire = status1.wireEncode();

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x15, 0x65, 0x80, 0x11, 0x30, 0x2e, 0x35, 0x2e, 0x31, 0x2d, 0x31, 0x34,
    0x2d, 0x67, 0x30, 0x35, 0x64, 0x64, 0x34, 0x34, 0x34, 0x81, 0x08, 0x00,
    0x00, 0x00, 0x57, 0x5b, 0x42, 0xa6, 0x2d, 0x82, 0x08, 0x00, 0x00, 0x00,
    0xce, 0x50, 0x36, 0xd7, 0x20, 0x83, 0x04, 0x6e, 0x43, 0xe4, 0x78, 0x84,
    0x04, 0x25, 0x0e, 0xfe, 0xe4, 0x85, 0x04, 0x1c, 0xbc, 0xb7, 0x4d, 0x86,
    0x04, 0x69, 0x9a, 0x61, 0xf2, 0x87, 0x04, 0x4b, 0x65, 0xe3, 0xf0, 0x90,
    0x04, 0x24, 0x86, 0xc3, 0x5f, 0x91, 0x04, 0x6d, 0xe2, 0xbc, 0xf2, 0x97,
    0x02, 0x04, 0xd2, 0x92, 0x04, 0x38, 0xc0, 0x92, 0x3d, 0x93, 0x04, 0x08,
    0x3c, 0xbf, 0x2a, 0x98, 0x02, 0x10, 0xe1,
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  ForwarderStatus status2(wire);
  BOOST_CHECK_EQUAL(status1, status2);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  ForwarderStatus status1, status2;

  status1 = makeForwarderStatus();
  status2 = status1;
  BOOST_CHECK_EQUAL(status1, status2);

  status2.setNPitEntries(42);
  BOOST_CHECK_NE(status1, status2);
}

BOOST_AUTO_TEST_CASE(Print)
{
  ForwarderStatus status;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(status),
                    "GeneralStatus(NfdVersion: ,\n"
                    "              StartTimestamp: 0 nanoseconds since Jan 1, 1970,\n"
                    "              CurrentTimestamp: 0 nanoseconds since Jan 1, 1970,\n"
                    "              Counters: {NameTreeEntries: 0,\n"
                    "                         FibEntries: 0,\n"
                    "                         PitEntries: 0,\n"
                    "                         MeasurementsEntries: 0,\n"
                    "                         CsEntries: 0,\n"
                    "                         Interests: {in: 0, out: 0},\n"
                    "                         Data: {in: 0, out: 0},\n"
                    "                         Nacks: {in: 0, out: 0}}\n"
                    "              )");

  status = makeForwarderStatus();
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(status),
                    "GeneralStatus(NfdVersion: 0.5.1-14-g05dd444,\n"
                    "              StartTimestamp: 375193249325000000 nanoseconds since Jan 1, 1970,\n"
                    "              CurrentTimestamp: 886109034272000000 nanoseconds since Jan 1, 1970,\n"
                    "              Counters: {NameTreeEntries: 1849943160,\n"
                    "                         FibEntries: 621739748,\n"
                    "                         PitEntries: 482129741,\n"
                    "                         MeasurementsEntries: 1771725298,\n"
                    "                         CsEntries: 1264968688,\n"
                    "                         Interests: {in: 612811615, out: 952144445},\n"
                    "                         Data: {in: 1843576050, out: 138198826},\n"
                    "                         Nacks: {in: 1234, out: 4321}}\n"
                    "              )");
}

BOOST_AUTO_TEST_SUITE_END() // TestForwarderStatus
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
