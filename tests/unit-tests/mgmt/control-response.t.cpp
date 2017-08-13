/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "mgmt/control-response.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace mgmt {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(TestControlResponse)

static const uint8_t WIRE[] = {
  0x65, 0x17, // ControlResponse
        0x66, 0x02, // StatusCode
              0x01, 0x94,
        0x67, 0x11, // StatusText
              0x4e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x6f, 0x74, 0x20,
              0x66, 0x6f, 0x75, 0x6e, 0x64};

BOOST_AUTO_TEST_CASE(Encode)
{
  ControlResponse cr(404, "Nothing not found");
  const Block& wire = cr.wireEncode();
  BOOST_CHECK_EQUAL_COLLECTIONS(WIRE, WIRE + sizeof(WIRE),
                                wire.begin(), wire.end());
}

BOOST_AUTO_TEST_CASE(Decode)
{
  ControlResponse cr(Block(WIRE, sizeof(WIRE)));
  BOOST_CHECK_EQUAL(cr.getCode(), 404);
  BOOST_CHECK_EQUAL(cr.getText(), "Nothing not found");
}

BOOST_AUTO_TEST_SUITE_END() // TestControlResponse
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace mgmt
} // namespace ndn
