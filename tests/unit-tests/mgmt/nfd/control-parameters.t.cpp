/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "mgmt/nfd/control-parameters.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestControlParameters)

BOOST_AUTO_TEST_CASE(Fields)
{
  ControlParameters input;

  ControlParameters decoded(input.wireEncode());
  BOOST_CHECK_EQUAL(decoded.hasName(), false);
  BOOST_CHECK_EQUAL(decoded.hasFaceId(), false);
  BOOST_CHECK_EQUAL(decoded.hasUri(), false);
  BOOST_CHECK_EQUAL(decoded.hasLocalUri(), false);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), false);
  BOOST_CHECK_EQUAL(decoded.hasCost(), false);
  BOOST_CHECK_EQUAL(decoded.hasCapacity(), false);
  BOOST_CHECK_EQUAL(decoded.hasCount(), false);
  BOOST_CHECK_EQUAL(decoded.hasFlags(), false);
  BOOST_CHECK_EQUAL(decoded.hasMask(), false);
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(decoded.hasFacePersistency(), false);

  input.setName("/name");
  input.setFaceId(2634);
  input.setUri("udp4://192.0.2.1:6363");
  input.setLocalUri("udp4://192.0.2.2:6363");
  input.setOrigin(ROUTE_ORIGIN_NLSR);
  input.setCost(1388);
  input.setCapacity(2632);
  input.setCount(3100);
  input.setFlags(0xAFC4);
  input.setMask(0xF7A1);
  input.setStrategy("/strategy-name");
  input.setExpirationPeriod(1800000_ms);
  input.setFacePersistency(FacePersistency::FACE_PERSISTENCY_PERSISTENT);

  decoded.wireDecode(input.wireEncode());
  BOOST_CHECK_EQUAL(decoded.hasName(), true);
  BOOST_CHECK_EQUAL(decoded.hasFaceId(), true);
  BOOST_CHECK_EQUAL(decoded.hasUri(), true);
  BOOST_CHECK_EQUAL(decoded.hasLocalUri(), true);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), true);
  BOOST_CHECK_EQUAL(decoded.hasCost(), true);
  BOOST_CHECK_EQUAL(decoded.hasCapacity(), true);
  BOOST_CHECK_EQUAL(decoded.hasCount(), true);
  BOOST_CHECK_EQUAL(decoded.hasFlags(), true);
  BOOST_CHECK_EQUAL(decoded.hasMask(), true);
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), true);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), true);
  BOOST_CHECK_EQUAL(decoded.hasFacePersistency(), true);

  BOOST_CHECK_EQUAL(decoded.getName(), "/name");
  BOOST_CHECK_EQUAL(decoded.getFaceId(), 2634);
  BOOST_CHECK_EQUAL(decoded.getUri(), "udp4://192.0.2.1:6363");
  BOOST_CHECK_EQUAL(decoded.getLocalUri(), "udp4://192.0.2.2:6363");
  BOOST_CHECK_EQUAL(decoded.getOrigin(), ROUTE_ORIGIN_NLSR);
  BOOST_CHECK_EQUAL(decoded.getCost(), 1388);
  BOOST_CHECK_EQUAL(decoded.getCapacity(), 2632);
  BOOST_CHECK_EQUAL(decoded.getCount(), 3100);
  BOOST_CHECK_EQUAL(decoded.getFlags(), 0xAFC4);
  BOOST_CHECK_EQUAL(decoded.getMask(), 0xF7A1);
  BOOST_CHECK_EQUAL(decoded.getStrategy(), "/strategy-name");
  BOOST_CHECK_EQUAL(decoded.getExpirationPeriod(), 1800000_ms);
  BOOST_CHECK_EQUAL(decoded.getFacePersistency(), FacePersistency::FACE_PERSISTENCY_PERSISTENT);

  input.unsetName();
  input.unsetFaceId();
  input.unsetUri();
  input.unsetLocalUri();
  input.unsetOrigin();
  input.unsetCost();
  input.unsetCapacity();
  input.unsetCount();
  input.unsetFlags();
  input.unsetMask();
  input.unsetStrategy();
  input.unsetExpirationPeriod();
  input.unsetFacePersistency();
  BOOST_CHECK_EQUAL(input.hasName(), false);
  BOOST_CHECK_EQUAL(input.hasFaceId(), false);
  BOOST_CHECK_EQUAL(input.hasUri(), false);
  BOOST_CHECK_EQUAL(input.hasLocalUri(), false);
  BOOST_CHECK_EQUAL(input.hasOrigin(), false);
  BOOST_CHECK_EQUAL(input.hasCost(), false);
  BOOST_CHECK_EQUAL(input.hasCapacity(), false);
  BOOST_CHECK_EQUAL(input.hasCount(), false);
  BOOST_CHECK_EQUAL(input.hasFlags(), false);
  BOOST_CHECK_EQUAL(input.hasMask(), false);
  BOOST_CHECK_EQUAL(input.hasStrategy(), false);
  BOOST_CHECK_EQUAL(input.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(input.hasFacePersistency(), false);
}

BOOST_AUTO_TEST_CASE(FlagsAndMask)
{
  ControlParameters p;

  BOOST_CHECK(!p.hasFlags());
  BOOST_CHECK(!p.hasMask());
  BOOST_CHECK(!p.hasFlagBit(0));
  BOOST_CHECK(!p.getFlagBit(0));

  // Set bit 2 to true (change Mask)
  p.setFlagBit(2, true);
  // 2^2 = 4
  BOOST_CHECK_EQUAL(p.getFlags(), 4);
  BOOST_CHECK_EQUAL(p.getMask(), 4);
  BOOST_CHECK(p.hasFlagBit(2));
  BOOST_CHECK(p.getFlagBit(2));
  BOOST_CHECK(!p.hasFlagBit(1));
  BOOST_CHECK(!p.getFlagBit(1));

  // Set bit 3 to true (no change to Mask)
  p.setFlagBit(3, true, false);
  // 2^3 + 2^2 = 12
  BOOST_CHECK_EQUAL(p.getFlags(), 12);
  // 2^2 = 4
  BOOST_CHECK_EQUAL(p.getMask(), 4);
  BOOST_CHECK(!p.hasFlagBit(3));
  BOOST_CHECK(p.getFlagBit(3));
  BOOST_CHECK(p.hasFlagBit(2));
  BOOST_CHECK(p.getFlagBit(2));

  // Set bit 1 to false (change Mask)
  p.setFlagBit(1, false);
  // 2^3 + 2^2 = 12
  BOOST_CHECK_EQUAL(p.getFlags(), 12);
  // 2^2 + 2^1 = 6
  BOOST_CHECK_EQUAL(p.getMask(), 6);
  BOOST_CHECK(!p.hasFlagBit(3));
  BOOST_CHECK(p.getFlagBit(3));
  BOOST_CHECK(p.hasFlagBit(2));
  BOOST_CHECK(p.getFlagBit(2));
  BOOST_CHECK(p.hasFlagBit(1));
  BOOST_CHECK(!p.getFlagBit(1));

  // Set bit 2 to false (change Mask)
  p.setFlagBit(2, false);
  // 2^3 = 8
  BOOST_CHECK_EQUAL(p.getFlags(), 8);
  // 2^2 + 2^1 = 6
  BOOST_CHECK_EQUAL(p.getMask(), 6);
  BOOST_CHECK(!p.hasFlagBit(3));
  BOOST_CHECK(p.getFlagBit(3));
  BOOST_CHECK(p.hasFlagBit(2));
  BOOST_CHECK(!p.getFlagBit(2));
  BOOST_CHECK(p.hasFlagBit(1));
  BOOST_CHECK(!p.getFlagBit(1));

  // Set bit 0 to true (change Mask)
  p.setFlagBit(0, true);
  // 2^3 + 2^0 = 9
  BOOST_CHECK_EQUAL(p.getFlags(), 9);
  // 2^2 + 2^1 + 2^0 = 7
  BOOST_CHECK_EQUAL(p.getMask(), 7);
  BOOST_CHECK(p.hasFlagBit(0));
  BOOST_CHECK(p.getFlagBit(0));

  // Unset bit 0
  p.unsetFlagBit(0);
  BOOST_REQUIRE(p.hasFlags());
  BOOST_REQUIRE(p.hasMask());
  // 2^3 + 2^0 = 9
  BOOST_CHECK_EQUAL(p.getFlags(), 9);
  // 2^2 + 2^1 = 6
  BOOST_CHECK_EQUAL(p.getMask(), 6);
  BOOST_CHECK(p.hasFlagBit(1));
  BOOST_CHECK(!p.hasFlagBit(0));
  BOOST_CHECK(p.getFlagBit(0));

  // Unset bit 3 (already unset in Mask, so no change)
  p.unsetFlagBit(3);
  BOOST_REQUIRE(p.hasFlags());
  BOOST_REQUIRE(p.hasMask());
  // 2^3 + 2^0 = 9
  BOOST_CHECK_EQUAL(p.getFlags(), 9);
  // 2^2 + 2^1 = 6
  BOOST_CHECK_EQUAL(p.getMask(), 6);
  BOOST_CHECK(!p.hasFlagBit(3));
  BOOST_CHECK(p.getFlagBit(3));

  // Unset bit 2
  p.unsetFlagBit(2);
  BOOST_REQUIRE(p.hasFlags());
  BOOST_REQUIRE(p.hasMask());
  // 2^3 + 2^0 = 9
  BOOST_CHECK_EQUAL(p.getFlags(), 9);
  // 2^1 = 2
  BOOST_CHECK_EQUAL(p.getMask(), 2);
  BOOST_CHECK(!p.hasFlagBit(2));
  BOOST_CHECK(!p.getFlagBit(2));

  // Unset bit 1
  // Flags and Mask fields will be deleted as Mask is now 0
  p.unsetFlagBit(1);
  BOOST_CHECK(!p.hasFlags());
  BOOST_CHECK(!p.hasMask());
  BOOST_CHECK(!p.hasFlagBit(3));
  BOOST_CHECK(!p.getFlagBit(3));
  BOOST_CHECK(!p.hasFlagBit(2));
  BOOST_CHECK(!p.getFlagBit(2));
  BOOST_CHECK(!p.hasFlagBit(1));
  BOOST_CHECK(!p.getFlagBit(1));
  BOOST_CHECK(!p.hasFlagBit(0));
  BOOST_CHECK(!p.getFlagBit(0));

  BOOST_CHECK_THROW(p.setFlagBit(64, true), std::out_of_range);
  BOOST_CHECK_THROW(p.unsetFlagBit(64), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END() // TestControlParameters
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
