/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "management/nfd-face-status.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(ManagementTestNfdFaceStatus)

BOOST_AUTO_TEST_CASE(Encode)
{
  FaceStatus status1;
  status1.setFaceId(100)
         .setRemoteUri("tcp4://192.0.2.1:6363")
         .setLocalUri("tcp4://192.0.2.2:55555")
         .setFlags(FACE_IS_ON_DEMAND)
         .setNInInterests(10)
         .setNInDatas(200)
         .setNOutInterests(3000)
         .setNOutDatas(4);

  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = status1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  //for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //  printf("0x%02x, ", *it);
  //}
  static const uint8_t expected[] = {
    0x80, 0x42, 0x69, 0x01, 0x64, 0x72, 0x15, 0x74, 0x63, 0x70, 0x34, 0x3a,
    0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x31, 0x3a,
    0x36, 0x33, 0x36, 0x33, 0x81, 0x16, 0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f,
    0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x32, 0x3a, 0x35,
    0x35, 0x35, 0x35, 0x35, 0xc2, 0x01, 0x02, 0x90, 0x01, 0x0a, 0x91, 0x01,
    0xc8, 0x92, 0x02, 0x0b, 0xb8, 0x93, 0x01, 0x04
  };
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                  wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(FaceStatus(wire));
  FaceStatus status2(wire);
  BOOST_CHECK_EQUAL(status1.getFaceId(), status2.getFaceId());
  BOOST_CHECK_EQUAL(status1.getRemoteUri(), status2.getRemoteUri());
  BOOST_CHECK_EQUAL(status1.getLocalUri(), status2.getLocalUri());
  BOOST_CHECK_EQUAL(status1.getFlags(), status2.getFlags());
  BOOST_CHECK_EQUAL(status1.getNInInterests(), status2.getNInInterests());
  BOOST_CHECK_EQUAL(status1.getNInDatas(), status2.getNInDatas());
  BOOST_CHECK_EQUAL(status1.getNOutInterests(), status2.getNOutInterests());
  BOOST_CHECK_EQUAL(status1.getNOutDatas(), status2.getNOutDatas());

  std::ostringstream os;
  os << status2;
  BOOST_CHECK_EQUAL(os.str(), "FaceStatus(FaceID: 100, "
                              "RemoteUri: tcp4://192.0.2.1:6363, "
                              "LocalUri: tcp4://192.0.2.2:55555, "
                              "Flags: 2, "
                              "Counters: 10|200|3000|4)");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
