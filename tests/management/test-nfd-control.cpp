/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-control-response.hpp"
#include "management/nfd-fib-management-options.hpp"
#include "management/nfd-face-management-options.hpp"
#include "management/nfd-face-event-notification.hpp"
#include "management/nfd-face-status.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

using namespace std;

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(TestNfdControl)

const uint8_t TestControlResponse[] = {0x65, 0x17,
                                         0x66, 0x02, 0x01, 0x94, 0x67, 0x11, 0x4e, 0x6f, 0x74,
                                         0x68, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x6f, 0x74, 0x20,
                                         0x66, 0x6f, 0x75, 0x6e, 0x64};

const uint8_t TestFibManagementOptions[] = {
  0x68, 0x49, 0x07, 0x16, 0x08, 0x09, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68,
  0x6f, 0x73, 0x74, 0x08, 0x03, 0x72, 0x65, 0x67, 0x08, 0x04, 0x74, 0x65,
  0x73, 0x74, 0x69, 0x01, 0x00, 0x6a, 0x01, 0x01, 0x6b, 0x29, 0x07, 0x27,
  0x08, 0x09, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74, 0x08,
  0x03, 0x6e, 0x66, 0x64, 0x08, 0x08, 0x73, 0x74, 0x72, 0x61, 0x74, 0x65,
  0x67, 0x79, 0x08, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2d, 0x77, 0x6f,
  0x72, 0x6c, 0x64
};

const uint8_t TestFaceManagementOptions[] = {
  0x68, 0x1e, 0x69, 0x01, 0x0a, 0x72, 0x19, 0x74, 0x63, 0x70, 0x3a, 0x2f,
  0x2f, 0x31, 0x2e, 0x31, 0x2e, 0x31, 0x2e, 0x31, 0x2f, 0x68, 0x65, 0x6c,
  0x6c, 0x6f, 0x2f, 0x77, 0x6f, 0x72, 0x6c, 0x64
};

const uint8_t TestFaceEventNotification[] = {
  0xc0, 0x20, 0xc1, 0x01, 0x01, 0x69, 0x01, 0x64, 0x72, 0x15, 0x74, 0x63,
  0x70, 0x34, 0x3a, 0x2f, 0x2f, 0x31, 0x32, 0x37, 0x2e, 0x30, 0x2e, 0x30,
  0x2e, 0x31, 0x3a, 0x36, 0x33, 0x36, 0x33, 0xc2, 0x01, 0x03
};

const uint8_t TestFaceStatus[] = {
  0x80, 0x27, 0x69, 0x01, 0x64, 0x72, 0x15, 0x74, 0x63, 0x70, 0x34, 0x3a,
  0x2f, 0x2f, 0x31, 0x32, 0x37, 0x2e, 0x30, 0x2e, 0x30, 0x2e, 0x31, 0x3a,
  0x36, 0x33, 0x36, 0x33, 0x91, 0x01, 0x0a, 0x90, 0x01, 0x14, 0x92, 0x01,
  0x1e, 0x93, 0x02, 0x01, 0x90
};

// ControlResponse

BOOST_AUTO_TEST_CASE(ControlResponseEncode)
{
  ControlResponse controlResponse(404, "Nothing not found");
  const Block &wire = controlResponse.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestControlResponse, TestControlResponse+sizeof(TestControlResponse),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(ControlResponseDecode)
{
  ControlResponse controlResponse;

  BOOST_REQUIRE_NO_THROW(controlResponse.wireDecode(Block(TestControlResponse, sizeof(TestControlResponse))));

  BOOST_REQUIRE_EQUAL(controlResponse.getCode(), 404);
  BOOST_REQUIRE_EQUAL(controlResponse.getText(), "Nothing not found");
}

// FibManagementOptions

BOOST_AUTO_TEST_CASE(FibManagementOptionsEncoding)
{
  Name n("/localhost/reg/test");
  FibManagementOptions opt;

  opt.setName(n);
  opt.setFaceId(0);
  opt.setCost(1);
  opt.setStrategy("/localhost/nfd/strategy/hello-world");

  const Block& blk = opt.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestFibManagementOptions,
                                  TestFibManagementOptions + sizeof(TestFibManagementOptions),
                                  blk.begin(), blk.end());
}

BOOST_AUTO_TEST_CASE(FibManagementOptionsFastEncoding)
{
  Name n("/localhost/reg/test");
  FibManagementOptions opt;

  opt.setName(n);
  opt.setFaceId(0);
  opt.setCost(1);
  opt.setStrategy("/localhost/nfd/strategy/hello-world");

  EncodingBuffer blk;

  BOOST_REQUIRE_NO_THROW(opt.wireEncode(blk));

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestFibManagementOptions,
                                  TestFibManagementOptions + sizeof(TestFibManagementOptions),
                                  blk.begin(), blk.end());

  EncodingBuffer blk2(4);

  BOOST_REQUIRE_NO_THROW(opt.wireEncode(blk2));

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestFibManagementOptions,
                                  TestFibManagementOptions + sizeof(TestFibManagementOptions),
                                  blk2.begin(), blk2.end());
}

BOOST_AUTO_TEST_CASE(FibManagementOptionsDecoding)
{
  Block blk(TestFibManagementOptions, sizeof(TestFibManagementOptions));
  Name n("/localhost/reg/test");
  FibManagementOptions opt;

  BOOST_REQUIRE_NO_THROW(opt.wireDecode(blk));

  BOOST_CHECK_EQUAL(opt.getName(), n);
  BOOST_CHECK_EQUAL(opt.getFaceId(), 0);
  BOOST_CHECK_EQUAL(opt.getCost(), 1);
  BOOST_CHECK_EQUAL(opt.getStrategy().toUri(), "/localhost/nfd/strategy/hello-world");
}

BOOST_AUTO_TEST_CASE(FaceManagementOptionsFastEncoding)
{
  FaceManagementOptions opt;

  opt.setFaceId(10);
  opt.setUri("tcp://1.1.1.1/hello/world");

  BOOST_REQUIRE_NO_THROW(opt.wireEncode ());

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestFaceManagementOptions,
                                  TestFaceManagementOptions + sizeof(TestFaceManagementOptions),
                                  opt.wireEncode().begin(), opt.wireEncode().end());
}

BOOST_AUTO_TEST_CASE(FaceManagementOptionsDecoding)
{
  Block blk(TestFaceManagementOptions, sizeof(TestFaceManagementOptions));
  FaceManagementOptions opt;

  BOOST_REQUIRE_NO_THROW(opt.wireDecode(blk));

  BOOST_CHECK_EQUAL(opt.getFaceId(), 10);
  BOOST_CHECK_EQUAL(opt.getUri(), "tcp://1.1.1.1/hello/world");
}


BOOST_AUTO_TEST_CASE(FaceEventNotificationEncodingDecoding)
{
  FaceEventKind expectedKind = FACE_EVENT_CREATED;
  std::string expectedUri("tcp4://127.0.0.1:6363");
  uint64_t expectedFaceId = 100;
  uint64_t expectedFlags = 3;

  {
    FaceEventNotification faceEvent(expectedKind, expectedFaceId, expectedUri, expectedFlags);
    BOOST_REQUIRE_NO_THROW(faceEvent.wireEncode());

    BOOST_REQUIRE_EQUAL_COLLECTIONS(TestFaceEventNotification,
                                    TestFaceEventNotification + sizeof(TestFaceEventNotification),
                                    faceEvent.wireEncode().begin(), faceEvent.wireEncode().end());

    std::ostringstream os;
    os << faceEvent;
    BOOST_CHECK_EQUAL(os.str(), "FaceEventNotification(Kind: created, FaceID: 100, Uri: tcp4://127.0.0.1:6363, Flags: 3)");
  }

  {
    Block blk(TestFaceEventNotification, sizeof(TestFaceEventNotification));
    FaceEventNotification faceEvent(blk);

    BOOST_CHECK_EQUAL(faceEvent.getEventKind(), expectedKind);
    BOOST_CHECK_EQUAL(faceEvent.getFaceId(), expectedFaceId);
    BOOST_CHECK_EQUAL(faceEvent.getUri(), expectedUri);
    BOOST_CHECK_EQUAL(faceEvent.getFlags(), expectedFlags);
    BOOST_CHECK(faceEvent.isLocal());
    BOOST_CHECK(faceEvent.isOnDemand());
  }
}

BOOST_AUTO_TEST_CASE(FaceStatusEncodingDecoding)
{
  {
    FaceStatus faceStatus(100, "tcp4://127.0.0.1:6363", 10, 20, 30, 400);
    BOOST_REQUIRE_NO_THROW(faceStatus.wireEncode());

    BOOST_CHECK_EQUAL_COLLECTIONS(TestFaceStatus,
                                  TestFaceStatus + sizeof(TestFaceStatus),
                                  faceStatus.wireEncode().begin(), faceStatus.wireEncode().end());

    std::ostringstream os;
    os << faceStatus;
    BOOST_CHECK_EQUAL(os.str(), "FaceStatus(FaceID: 100, Uri: tcp4://127.0.0.1:6363, "
                      "Counters: 10|20|30|400)");
  }

  {
    Block block(TestFaceStatus, sizeof(TestFaceStatus));
    BOOST_REQUIRE_NO_THROW((FaceStatus(block)));

    FaceStatus faceStatus(block);

    BOOST_CHECK_EQUAL(faceStatus.getFaceId(), 100);
    BOOST_CHECK_EQUAL(faceStatus.getUri(), "tcp4://127.0.0.1:6363");

    BOOST_CHECK_EQUAL(faceStatus.getInInterest(),  10);
    BOOST_CHECK_EQUAL(faceStatus.getInData(),      20);
    BOOST_CHECK_EQUAL(faceStatus.getOutInterest(), 30);
    BOOST_CHECK_EQUAL(faceStatus.getOutData(),     400);
  }
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
