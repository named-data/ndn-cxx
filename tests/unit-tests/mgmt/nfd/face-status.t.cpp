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

#include "mgmt/nfd/face-status.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestFaceStatus)

static FaceStatus
makeFaceStatus()
{
  return FaceStatus()
      .setFaceId(100)
      .setRemoteUri("tcp4://192.0.2.1:6363")
      .setLocalUri("tcp4://192.0.2.2:55555")
      .setFaceScope(FACE_SCOPE_LOCAL)
      .setFacePersistency(FACE_PERSISTENCY_ON_DEMAND)
      .setLinkType(LINK_TYPE_MULTI_ACCESS)
      .setExpirationPeriod(10_s)
      .setBaseCongestionMarkingInterval(5_ns)
      .setDefaultCongestionThreshold(7)
      .setNInInterests(10)
      .setNInData(200)
      .setNInNacks(1)
      .setNOutInterests(3000)
      .setNOutData(4)
      .setNOutNacks(2)
      .setNInBytes(1329719163)
      .setNOutBytes(999110448)
      .setFlags(0x7);
}

BOOST_AUTO_TEST_CASE(Encode)
{
  FaceStatus status1 = makeFaceStatus();
  Block wire = status1.wireEncode();

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x80, 0x67, 0x69, 0x01, 0x64, 0x72, 0x15, 0x74, 0x63, 0x70,
    0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e,
    0x32, 0x2e, 0x31, 0x3a, 0x36, 0x33, 0x36, 0x33, 0x81, 0x16,
    0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32,
    0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x32, 0x3a, 0x35, 0x35, 0x35,
    0x35, 0x35, 0x6d, 0x02, 0x27, 0x10, 0x84, 0x01, 0x01, 0x85,
    0x01, 0x01, 0x86, 0x01, 0x01, 0x87, 0x01, 0x05, 0x88, 0x01,
    0x07, 0x90, 0x01, 0x0a, 0x91, 0x01, 0xc8, 0x97, 0x01, 0x01,
    0x92, 0x02, 0x0b, 0xb8, 0x93, 0x01, 0x04, 0x98, 0x01, 0x02,
    0x94, 0x04, 0x4f, 0x41, 0xe7, 0x7b, 0x95, 0x04, 0x3b, 0x8d,
    0x37, 0x30, 0x6c, 0x01, 0x07,
  };
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  FaceStatus status2(wire);
  BOOST_CHECK_EQUAL(status1, status2);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  FaceStatus status1, status2;

  status1 = makeFaceStatus();
  status2 = status1;
  BOOST_CHECK_EQUAL(status1, status2);

  status2.setFaceId(42);
  BOOST_CHECK_NE(status1, status2);
}

BOOST_AUTO_TEST_CASE(Print)
{
  FaceStatus status;
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(status),
                    "Face(FaceId: 0,\n"
                    "     RemoteUri: ,\n"
                    "     LocalUri: ,\n"
                    "     ExpirationPeriod: infinite,\n"
                    "     FaceScope: non-local,\n"
                    "     FacePersistency: persistent,\n"
                    "     LinkType: point-to-point,\n"
                    "     Flags: 0x0,\n"
                    "     Counters: {Interests: {in: 0, out: 0},\n"
                    "                Data: {in: 0, out: 0},\n"
                    "                Nacks: {in: 0, out: 0},\n"
                    "                bytes: {in: 0, out: 0}}\n"
                    "     )");

  status = makeFaceStatus();
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(status),
                    "Face(FaceId: 100,\n"
                    "     RemoteUri: tcp4://192.0.2.1:6363,\n"
                    "     LocalUri: tcp4://192.0.2.2:55555,\n"
                    "     ExpirationPeriod: 10000 milliseconds,\n"
                    "     FaceScope: local,\n"
                    "     FacePersistency: on-demand,\n"
                    "     LinkType: multi-access,\n"
                    "     BaseCongestionMarkingInterval: 5 nanoseconds,\n"
                    "     DefaultCongestionThreshold: 7 bytes,\n"
                    "     Flags: 0x7,\n"
                    "     Counters: {Interests: {in: 10, out: 3000},\n"
                    "                Data: {in: 200, out: 4},\n"
                    "                Nacks: {in: 1, out: 2},\n"
                    "                bytes: {in: 1329719163, out: 999110448}}\n"
                    "     )");
}

BOOST_AUTO_TEST_CASE(ExpirationPeriod)
{
  FaceStatus status;
  BOOST_CHECK_EQUAL(status.hasExpirationPeriod(), false);

  status.setExpirationPeriod(1_min);
  BOOST_REQUIRE_EQUAL(status.hasExpirationPeriod(), true);
  BOOST_CHECK_EQUAL(status.getExpirationPeriod(), 1_min);

  status.unsetExpirationPeriod();
  BOOST_CHECK_EQUAL(status.hasExpirationPeriod(), false);
}

BOOST_AUTO_TEST_CASE(FlagBit)
{
  FaceStatus status;
  status.setFlags(0x7);
  BOOST_CHECK_EQUAL(status.getFlags(), 0x7);

  BOOST_CHECK(status.getFlagBit(0));
  BOOST_CHECK(status.getFlagBit(1));
  BOOST_CHECK(status.getFlagBit(2));
  BOOST_CHECK(!status.getFlagBit(3));

  status.setFlagBit(3, true);
  BOOST_CHECK_EQUAL(status.getFlags(), 0xf);
  BOOST_CHECK(status.getFlagBit(3));

  status.setFlagBit(1, false);
  BOOST_CHECK_EQUAL(status.getFlags(), 0xd);
  BOOST_CHECK(!status.getFlagBit(1));
}

BOOST_AUTO_TEST_SUITE_END() // TestFaceStatus
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
