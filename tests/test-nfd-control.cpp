/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "management/control-response.hpp"

#include <boost/test/output_test_stream.hpp>

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestNfdControl)

const uint8_t TestControlResponse[] = {0x65, 0x17, 0x66, 0x02, 0x01, 0x94, 0x67, 0x11, 0x4e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64};

BOOST_AUTO_TEST_CASE (ControlResponseEncode)
{
  ndn::ControlResponse controlResponse(404, "Nothing not found");
  const Block &wire = controlResponse.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestControlResponse, TestControlResponse+sizeof(TestControlResponse),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE (ControlResponseDecode)
{
  ndn::ControlResponse controlResponse;
  
  BOOST_REQUIRE_NO_THROW(controlResponse.wireDecode(Block(TestControlResponse, sizeof(TestControlResponse))));

  BOOST_REQUIRE_EQUAL(controlResponse.getCode(), 404);
  BOOST_REQUIRE_EQUAL(controlResponse.getText(), "Nothing not found");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
