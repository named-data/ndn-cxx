/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-control-response.hpp"
#include "management/nfd-fib-management-options.hpp"
#include "management/nfd-face-management-options.hpp"

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


BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
