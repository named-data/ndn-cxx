/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#include "ndn-cxx/mgmt/control-response.hpp"
#include "ndn-cxx/util/concepts.hpp"

#include "tests/boost-test.hpp"

namespace ndn::tests {

using namespace ndn::mgmt;

BOOST_CONCEPT_ASSERT((WireEncodable<ControlResponse>));
BOOST_CONCEPT_ASSERT((WireDecodable<ControlResponse>));
static_assert(std::is_convertible_v<ControlResponse::Error*, tlv::Error*>,
              "ControlResponse::Error must inherit from tlv::Error");

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(TestControlResponse)

const uint8_t WIRE[] = {
  0x65, 0x17, // ControlResponse
        0x66, 0x02, // StatusCode
              0x01, 0x94,
        0x67, 0x11, // StatusText
              0x4e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x6f, 0x74, 0x20,
              0x66, 0x6f, 0x75, 0x6e, 0x64};

BOOST_AUTO_TEST_CASE(Encode)
{
  ControlResponse cr1(404, "Nothing not found");
  BOOST_TEST(cr1.wireEncode() == WIRE, boost::test_tools::per_element());

  ControlResponse cr2;
  cr2.setCode(404);
  cr2.setText("Nothing not found");
  BOOST_TEST(cr2.wireEncode() == WIRE, boost::test_tools::per_element());

  ControlResponse cr3(cr1);
  cr3.setCode(405);
  BOOST_TEST(cr3.wireEncode() != Block{WIRE});

  ControlResponse cr4(cr1);
  cr4.setText("foo");
  BOOST_TEST(cr4.wireEncode() != Block{WIRE});
}

BOOST_AUTO_TEST_CASE(Decode)
{
  ControlResponse cr(Block{WIRE});
  BOOST_CHECK_EQUAL(cr.getCode(), 404);
  BOOST_CHECK_EQUAL(cr.getText(), "Nothing not found");

  // wrong outer TLV type
  BOOST_CHECK_EXCEPTION(cr.wireDecode("6406660201946700"_block), tlv::Error, [] (const auto& e) {
    return e.what() == "Expecting ControlResponse element, but TLV has type 100"sv;
  });

  // empty TLV
  BOOST_CHECK_EXCEPTION(cr.wireDecode("6500"_block), tlv::Error, [] (const auto& e) {
    return e.what() == "missing StatusCode sub-element"sv;
  });

  // missing StatusCode
  BOOST_CHECK_EXCEPTION(cr.wireDecode("65026700"_block), tlv::Error, [] (const auto& e) {
    return e.what() == "missing StatusCode sub-element"sv;
  });

  // missing StatusText
  BOOST_CHECK_EXCEPTION(cr.wireDecode("650466020194"_block), tlv::Error, [] (const auto& e) {
    return e.what() == "missing StatusText sub-element"sv;
  });
}

BOOST_AUTO_TEST_SUITE_END() // TestControlResponse
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace ndn::tests
