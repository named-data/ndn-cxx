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

#include "management/nfd-face-query-filter.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdFaceQueryFilter)

BOOST_AUTO_TEST_CASE(Encode)
{
  FaceQueryFilter filter1;
  BOOST_CHECK_EQUAL(filter1.hasFaceId(), false);
  BOOST_CHECK_EQUAL(filter1.hasUriScheme(), false);
  BOOST_CHECK_EQUAL(filter1.hasRemoteUri(), false);
  BOOST_CHECK_EQUAL(filter1.hasLocalUri(), false);
  BOOST_CHECK_EQUAL(filter1.hasFaceScope(), false);
  BOOST_CHECK_EQUAL(filter1.hasFacePersistency(), false);
  BOOST_CHECK_EQUAL(filter1.hasLinkType(), false);

  filter1.setFaceId(100)
         .setUriScheme("tcp4")
         .setRemoteUri("tcp4://192.0.2.1:6363")
         .setLocalUri("tcp4://192.0.2.2:55555")
         .setFaceScope(FACE_SCOPE_LOCAL)
         .setFacePersistency(FACE_PERSISTENCY_ON_DEMAND)
         .setLinkType(LINK_TYPE_MULTI_ACCESS);

  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = filter1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //  printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0x96, 0x41, 0x69, 0x01, 0x64, 0x83, 0x04, 0x74, 0x63, 0x70,
    0x34, 0x72, 0x15, 0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f, 0x2f,
    0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x31, 0x3a,
    0x36, 0x33, 0x36, 0x33, 0x81, 0x16, 0x74, 0x63, 0x70, 0x34,
    0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32,
    0x2e, 0x32, 0x3a, 0x35, 0x35, 0x35, 0x35, 0x35, 0x84, 0x01,
    0x01, 0x85, 0x01, 0x01, 0x86, 0x01, 0x01,
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(FaceQueryFilter(wire));

  FaceQueryFilter filter2(wire);
  BOOST_CHECK_EQUAL(filter1.getFaceId(), filter2.getFaceId());
  BOOST_CHECK_EQUAL(filter1.getUriScheme(), filter2.getUriScheme());
  BOOST_CHECK_EQUAL(filter1.getRemoteUri(), filter2.getRemoteUri());
  BOOST_CHECK_EQUAL(filter1.getLocalUri(), filter2.getLocalUri());
  BOOST_CHECK_EQUAL(filter1.getFaceScope(), filter2.getFaceScope());
  BOOST_CHECK_EQUAL(filter1.getFacePersistency(), filter2.getFacePersistency());
  BOOST_CHECK_EQUAL(filter1.getLinkType(), filter2.getLinkType());

  std::ostringstream os;
  os << filter2;
  BOOST_CHECK_EQUAL(os.str(), "FaceQueryFilter(FaceID: 100,\n"
                              "UriScheme: tcp4,\n"
                              "RemoteUri: tcp4://192.0.2.1:6363,\n"
                              "LocalUri: tcp4://192.0.2.2:55555,\n"
                              "FaceScope: local,\n"
                              "FacePersistency: on-demand,\n"
                              "LinkType: multi-access,\n"
                              ")");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
