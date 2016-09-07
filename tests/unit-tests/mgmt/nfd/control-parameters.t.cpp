/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

BOOST_AUTO_TEST_CASE(FaceOptions)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");
  parameters.setFacePersistency(ndn::nfd::FacePersistency::FACE_PERSISTENCY_PERSISTENT);
  parameters.setFlags(1);
  parameters.setMask(1);
  Block wire = parameters.wireEncode();

  ControlParameters decoded(wire);
  BOOST_CHECK_EQUAL(decoded.getUri(), "tcp4://192.0.2.1:6363");
  BOOST_CHECK_EQUAL(decoded.getFacePersistency(), ndn::nfd::FacePersistency::FACE_PERSISTENCY_PERSISTENT);
  BOOST_CHECK_EQUAL(decoded.getFlags(), 1);
  BOOST_CHECK_EQUAL(decoded.getMask(), 1);

  BOOST_CHECK_EQUAL(decoded.hasName(), false);
  BOOST_CHECK_EQUAL(decoded.hasFaceId(), false);
  BOOST_CHECK_EQUAL(decoded.hasLocalControlFeature(), false);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), false);
  BOOST_CHECK_EQUAL(decoded.hasCost(), false);
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
}

BOOST_AUTO_TEST_CASE(FaceLocalControlOptions)
{
  ControlParameters parameters;
  parameters.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID);

  Block wire = parameters.wireEncode();

  ControlParameters decoded(wire);
  BOOST_CHECK_EQUAL(decoded.getLocalControlFeature(), LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID);

  BOOST_CHECK_EQUAL(decoded.hasName(), false);
  BOOST_CHECK_EQUAL(decoded.hasFaceId(), false);
  BOOST_CHECK_EQUAL(decoded.hasUri(), false);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), false);
  BOOST_CHECK_EQUAL(decoded.hasCost(), false);
  BOOST_CHECK_EQUAL(decoded.hasFlags(), false);
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(decoded.hasFacePersistency(), false);
}

BOOST_AUTO_TEST_CASE(FibOptions)
{
  ControlParameters parameters;
  parameters.setName("ndn:/example")
            .setFaceId(4)
            .setCost(555);

  Block wire = parameters.wireEncode();

  ControlParameters decoded(wire);
  BOOST_CHECK_EQUAL(decoded.getName(), Name("ndn:/example"));
  BOOST_CHECK_EQUAL(decoded.getFaceId(), 4);
  BOOST_CHECK_EQUAL(decoded.getCost(), 555);

  BOOST_CHECK_EQUAL(decoded.hasUri(), false);
  BOOST_CHECK_EQUAL(decoded.hasLocalControlFeature(), false);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), false);
  BOOST_CHECK_EQUAL(decoded.hasFlags(), false);
  BOOST_CHECK_EQUAL(decoded.hasMask(), false);
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(decoded.hasFacePersistency(), false);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceOptions)
{
  ControlParameters parameters;
  parameters.setName("ndn:/")
            .setStrategy("ndn:/strategy/A");

  Block wire = parameters.wireEncode();

  ControlParameters decoded(wire);
  BOOST_CHECK_EQUAL(decoded.getName(), Name("ndn:/"));
  BOOST_CHECK_EQUAL(decoded.getStrategy(), Name("ndn:/strategy/A"));

  BOOST_CHECK_EQUAL(decoded.hasFaceId(), false);
  BOOST_CHECK_EQUAL(decoded.hasUri(), false);
  BOOST_CHECK_EQUAL(decoded.hasLocalControlFeature(), false);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), false);
  BOOST_CHECK_EQUAL(decoded.hasCost(), false);
  BOOST_CHECK_EQUAL(decoded.hasFlags(), false);
  BOOST_CHECK_EQUAL(decoded.hasMask(), false);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
  BOOST_CHECK_EQUAL(decoded.hasFacePersistency(), false);
}

BOOST_AUTO_TEST_CASE(RibOptions)
{
  ControlParameters parameters;
  parameters.setName("ndn:/example")
            .setFaceId(4)
            .setOrigin(128)
            .setCost(6)
            .setFlags(0x01)
            .setExpirationPeriod(time::milliseconds(1800000));

  Block wire = parameters.wireEncode();

  ControlParameters decoded(wire);
  BOOST_CHECK_EQUAL(decoded.getName(), Name("ndn:/example"));
  BOOST_CHECK_EQUAL(decoded.getFaceId(), 4);
  BOOST_CHECK_EQUAL(decoded.getOrigin(), 128);
  BOOST_CHECK_EQUAL(decoded.getCost(), 6);
  BOOST_CHECK_EQUAL(decoded.getFlags(), 0x01);
  BOOST_CHECK_EQUAL(decoded.getExpirationPeriod(), time::milliseconds(1800000));

  BOOST_CHECK_EQUAL(decoded.hasUri(), false);
  BOOST_CHECK_EQUAL(decoded.hasLocalControlFeature(), false);
  BOOST_CHECK_EQUAL(decoded.hasMask(), false);
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
  BOOST_CHECK_EQUAL(decoded.hasFacePersistency(), false);
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
