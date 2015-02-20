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

#include "management/nfd-control-parameters.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdControlParameters)

BOOST_AUTO_TEST_CASE(FaceOptions)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  Block wire = parameters.wireEncode();

  ControlParameters decoded(wire);
  BOOST_CHECK_EQUAL(decoded.getUri(), "tcp4://192.0.2.1:6363");

  BOOST_CHECK_EQUAL(decoded.hasName(), false);
  BOOST_CHECK_EQUAL(decoded.hasFaceId(), false);
  BOOST_CHECK_EQUAL(decoded.hasLocalControlFeature(), false);
  BOOST_CHECK_EQUAL(decoded.hasOrigin(), false);
  BOOST_CHECK_EQUAL(decoded.hasCost(), false);
  BOOST_CHECK_EQUAL(decoded.hasFlags(), false);
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
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
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
  BOOST_CHECK_EQUAL(decoded.hasExpirationPeriod(), false);
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
  BOOST_CHECK_EQUAL(decoded.hasStrategy(), false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
