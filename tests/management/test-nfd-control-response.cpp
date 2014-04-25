/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "management/nfd-control-response.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(ManagementTestNfdControlResponse)

const uint8_t TestControlResponse[] = {0x65, 0x17,
                                       0x66, 0x02, 0x01, 0x94, 0x67, 0x11, 0x4e, 0x6f, 0x74,
                                       0x68, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x6f, 0x74, 0x20,
                                       0x66, 0x6f, 0x75, 0x6e, 0x64};

// ControlResponse

BOOST_AUTO_TEST_CASE(ControlResponseEncode)
{
  ControlResponse controlResponse(404, "Nothing not found");
  const Block &wire = controlResponse.wireEncode();

  BOOST_REQUIRE_EQUAL_COLLECTIONS(TestControlResponse,
                                  TestControlResponse+sizeof(TestControlResponse),
                                  wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(ControlResponseDecode)
{
  ControlResponse controlResponse;

  BOOST_REQUIRE_NO_THROW(controlResponse.wireDecode(Block(TestControlResponse,
                                                          sizeof(TestControlResponse))));

  BOOST_REQUIRE_EQUAL(controlResponse.getCode(), 404);
  BOOST_REQUIRE_EQUAL(controlResponse.getText(), "Nothing not found");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
