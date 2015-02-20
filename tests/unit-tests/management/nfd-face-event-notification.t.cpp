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

#include "management/nfd-face-event-notification.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdFaceEventNotification)

BOOST_AUTO_TEST_CASE(Traits)
{
  FaceEventNotification notification;

  BOOST_CHECK_EQUAL(notification.getFaceScope(), FACE_SCOPE_NON_LOCAL);
  BOOST_CHECK_EQUAL(notification.getFacePersistency(), FACE_PERSISTENCY_PERSISTENT);
  BOOST_CHECK_EQUAL(notification.getLinkType(), LINK_TYPE_POINT_TO_POINT);

  notification.setFaceScope(FACE_SCOPE_LOCAL);
  BOOST_CHECK_EQUAL(notification.getFaceScope(), FACE_SCOPE_LOCAL);
  BOOST_CHECK_EQUAL(notification.getFacePersistency(), FACE_PERSISTENCY_PERSISTENT);
  BOOST_CHECK_EQUAL(notification.getLinkType(), LINK_TYPE_POINT_TO_POINT);

  notification.setFacePersistency(FACE_PERSISTENCY_ON_DEMAND);
  BOOST_CHECK_EQUAL(notification.getFaceScope(), FACE_SCOPE_LOCAL);
  BOOST_CHECK_EQUAL(notification.getFacePersistency(), FACE_PERSISTENCY_ON_DEMAND);
  BOOST_CHECK_EQUAL(notification.getLinkType(), LINK_TYPE_POINT_TO_POINT);

  notification.setFacePersistency(FACE_PERSISTENCY_PERMANENT);
  BOOST_CHECK_EQUAL(notification.getFaceScope(), FACE_SCOPE_LOCAL);
  BOOST_CHECK_EQUAL(notification.getFacePersistency(), FACE_PERSISTENCY_PERMANENT);
  BOOST_CHECK_EQUAL(notification.getLinkType(), LINK_TYPE_POINT_TO_POINT);

  notification.setLinkType(LINK_TYPE_MULTI_ACCESS);
  BOOST_CHECK_EQUAL(notification.getFaceScope(), FACE_SCOPE_LOCAL);
  BOOST_CHECK_EQUAL(notification.getFacePersistency(), FACE_PERSISTENCY_PERMANENT);
  BOOST_CHECK_EQUAL(notification.getLinkType(), LINK_TYPE_MULTI_ACCESS);
}

BOOST_AUTO_TEST_CASE(EncodeCreated)
{
  FaceEventNotification notification1;
  notification1.setKind(FACE_EVENT_CREATED)
               .setFaceId(20)
               .setRemoteUri("tcp4://192.0.2.1:55555")
               .setLocalUri("tcp4://192.0.2.2:6363")
               .setFaceScope(FACE_SCOPE_LOCAL)
               .setFacePersistency(FACE_PERSISTENCY_ON_DEMAND)
               .setLinkType(LINK_TYPE_MULTI_ACCESS);
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = notification1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0xc0, 0x3e, 0xc1, 0x01, 0x01, 0x69, 0x01, 0x14, 0x72, 0x16,
    0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32,
    0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x31, 0x3a, 0x35, 0x35, 0x35,
    0x35, 0x35, 0x81, 0x15, 0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f,
    0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x32,
    0x3a, 0x36, 0x33, 0x36, 0x33, 0x84, 0x01, 0x01, 0x85, 0x01,
    0x01, 0x86, 0x01, 0x01,
  };
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                  wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(FaceEventNotification(wire));
  FaceEventNotification notification2(wire);
  BOOST_CHECK_EQUAL(notification1.getKind(), notification2.getKind());
  BOOST_CHECK_EQUAL(notification1.getFaceId(), notification2.getFaceId());
  BOOST_CHECK_EQUAL(notification1.getRemoteUri(), notification2.getRemoteUri());
  BOOST_CHECK_EQUAL(notification1.getLocalUri(), notification2.getLocalUri());
  BOOST_CHECK_EQUAL(notification1.getFaceScope(), notification2.getFaceScope());
  BOOST_CHECK_EQUAL(notification1.getFacePersistency(), notification2.getFacePersistency());
  BOOST_CHECK_EQUAL(notification1.getLinkType(), notification2.getLinkType());

  std::ostringstream os;
  os << notification2;
  BOOST_CHECK_EQUAL(os.str(), "FaceEventNotification("
                              "Kind: created, "
                              "FaceID: 20, "
                              "RemoteUri: tcp4://192.0.2.1:55555, "
                              "LocalUri: tcp4://192.0.2.2:6363, "
                              "FaceScope: local, "
                              "FacePersistency: on-demand, "
                              "LinkType: multi-access)");
}

BOOST_AUTO_TEST_CASE(EncodeDestroyed)
{
  FaceEventNotification notification1;
  notification1.setKind(FACE_EVENT_DESTROYED)
               .setFaceId(20)
               .setRemoteUri("tcp4://192.0.2.1:55555")
               .setLocalUri("tcp4://192.0.2.2:6363")
               .setFaceScope(FACE_SCOPE_LOCAL)
               .setFacePersistency(FACE_PERSISTENCY_ON_DEMAND)
               .setLinkType(LINK_TYPE_MULTI_ACCESS);
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = notification1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  // for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //   printf("0x%02x, ", *it);
  // }
  static const uint8_t expected[] = {
    0xc0, 0x3e, 0xc1, 0x01, 0x02, 0x69, 0x01, 0x14, 0x72, 0x16,
    0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32,
    0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x31, 0x3a, 0x35, 0x35, 0x35,
    0x35, 0x35, 0x81, 0x15, 0x74, 0x63, 0x70, 0x34, 0x3a, 0x2f,
    0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32, 0x2e, 0x32,
    0x3a, 0x36, 0x33, 0x36, 0x33, 0x84, 0x01, 0x01, 0x85, 0x01,
    0x01, 0x86, 0x01, 0x01,
  };
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                  wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(FaceEventNotification(wire));
  FaceEventNotification notification2(wire);
  BOOST_CHECK_EQUAL(notification1.getKind(), notification2.getKind());
  BOOST_CHECK_EQUAL(notification1.getFaceId(), notification2.getFaceId());
  BOOST_CHECK_EQUAL(notification1.getRemoteUri(), notification2.getRemoteUri());
  BOOST_CHECK_EQUAL(notification1.getLocalUri(), notification2.getLocalUri());
  BOOST_CHECK_EQUAL(notification1.getFaceScope(), notification2.getFaceScope());
  BOOST_CHECK_EQUAL(notification1.getFacePersistency(), notification2.getFacePersistency());
  BOOST_CHECK_EQUAL(notification1.getLinkType(), notification2.getLinkType());

  std::ostringstream os;
  os << notification2;
  BOOST_CHECK_EQUAL(os.str(), "FaceEventNotification("
                              "Kind: destroyed, "
                              "FaceID: 20, "
                              "RemoteUri: tcp4://192.0.2.1:55555, "
                              "LocalUri: tcp4://192.0.2.2:6363, "
                              "FaceScope: local, "
                              "FacePersistency: on-demand, "
                              "LinkType: multi-access)");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
