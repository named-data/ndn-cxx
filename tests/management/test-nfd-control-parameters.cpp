/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-control-parameters.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(ManagementTestNfdControlParameters)

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

} // namespace nfd
} // namespace ndn
