/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-face-event-notification.hpp"

#include <boost/test/unit_test.hpp>

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(NfdFaceEventNotification)

BOOST_AUTO_TEST_CASE(Flags)
{
  FaceEventNotification notification;

  notification.setFlags(0);
  BOOST_CHECK_EQUAL(notification.isLocal(), false);
  BOOST_CHECK_EQUAL(notification.isOnDemand(), false);

  notification.setFlags(FACE_IS_LOCAL);
  BOOST_CHECK_EQUAL(notification.isLocal(), true);
  BOOST_CHECK_EQUAL(notification.isOnDemand(), false);

  notification.setFlags(FACE_IS_ON_DEMAND);
  BOOST_CHECK_EQUAL(notification.isLocal(), false);
  BOOST_CHECK_EQUAL(notification.isOnDemand(), true);

  notification.setFlags(FACE_IS_LOCAL | FACE_IS_ON_DEMAND);
  BOOST_CHECK_EQUAL(notification.isLocal(), true);
  BOOST_CHECK_EQUAL(notification.isOnDemand(), true);
}

BOOST_AUTO_TEST_CASE(EncodeCreated)
{
  FaceEventNotification notification1;
  notification1.setKind(FACE_EVENT_CREATED)
               .setFaceId(20)
               .setRemoteUri("tcp4://192.0.2.1:55555")
               .setLocalUri("tcp4://192.0.2.2:6363")
               .setFlags(FACE_IS_ON_DEMAND);
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = notification1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  //for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //  printf("0x%02x, ", *it);
  //}
  static const uint8_t expected[] = {
    0xc0, 0x38, 0xc1, 0x01, 0x01, 0x69, 0x01, 0x14, 0x72, 0x16, 0x74, 0x63,
    0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32,
    0x2e, 0x31, 0x3a, 0x35, 0x35, 0x35, 0x35, 0x35, 0x81, 0x15, 0x74, 0x63,
    0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32,
    0x2e, 0x32, 0x3a, 0x36, 0x33, 0x36, 0x33, 0xc2, 0x01, 0x02,
  };
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                  wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(FaceEventNotification(wire));
  FaceEventNotification notification2(wire);
  BOOST_CHECK_EQUAL(notification1.getKind(), notification2.getKind());
  BOOST_CHECK_EQUAL(notification1.getFaceId(), notification2.getFaceId());
  BOOST_CHECK_EQUAL(notification1.getRemoteUri(), notification2.getRemoteUri());
  BOOST_CHECK_EQUAL(notification1.getLocalUri(), notification2.getLocalUri());
  BOOST_CHECK_EQUAL(notification1.getFlags(), notification2.getFlags());

  std::ostringstream os;
  os << notification2;
  BOOST_CHECK_EQUAL(os.str(), "FaceEventNotification("
                              "Kind: created, "
                              "FaceID: 20, "
                              "RemoteUri: tcp4://192.0.2.1:55555, "
                              "LocalUri: tcp4://192.0.2.2:6363, "
                              "Flags: 2)");
}

BOOST_AUTO_TEST_CASE(EncodeDestroyed)
{
  FaceEventNotification notification1;
  notification1.setKind(FACE_EVENT_DESTROYED)
               .setFaceId(20)
               .setRemoteUri("tcp4://192.0.2.1:55555")
               .setLocalUri("tcp4://192.0.2.2:6363")
               .setFlags(FACE_IS_ON_DEMAND);
  Block wire;
  BOOST_REQUIRE_NO_THROW(wire = notification1.wireEncode());

  // These octets are obtained by the snippet below.
  // This check is intended to detect unexpected encoding change in the future.
  //for (Buffer::const_iterator it = wire.begin(); it != wire.end(); ++it) {
  //  printf("0x%02x, ", *it);
  //}
  static const uint8_t expected[] = {
    0xc0, 0x38, 0xc1, 0x01, 0x02, 0x69, 0x01, 0x14, 0x72, 0x16, 0x74, 0x63,
    0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32,
    0x2e, 0x31, 0x3a, 0x35, 0x35, 0x35, 0x35, 0x35, 0x81, 0x15, 0x74, 0x63,
    0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x30, 0x2e, 0x32,
    0x2e, 0x32, 0x3a, 0x36, 0x33, 0x36, 0x33, 0xc2, 0x01, 0x02,
  };
  BOOST_REQUIRE_EQUAL_COLLECTIONS(expected, expected + sizeof(expected),
                                  wire.begin(), wire.end());

  BOOST_REQUIRE_NO_THROW(FaceEventNotification(wire));
  FaceEventNotification notification2(wire);
  BOOST_CHECK_EQUAL(notification1.getKind(), notification2.getKind());
  BOOST_CHECK_EQUAL(notification1.getFaceId(), notification2.getFaceId());
  BOOST_CHECK_EQUAL(notification1.getRemoteUri(), notification2.getRemoteUri());
  BOOST_CHECK_EQUAL(notification1.getLocalUri(), notification2.getLocalUri());
  BOOST_CHECK_EQUAL(notification1.getFlags(), notification2.getFlags());

  std::ostringstream os;
  os << notification2;
  BOOST_CHECK_EQUAL(os.str(), "FaceEventNotification("
                              "Kind: destroyed, "
                              "FaceID: 20, "
                              "RemoteUri: tcp4://192.0.2.1:55555, "
                              "LocalUri: tcp4://192.0.2.2:6363, "
                              "Flags: 2)");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
