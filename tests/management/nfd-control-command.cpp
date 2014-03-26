/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-control-command.hpp"
// Having a separate compilation unit is necessary to ensure .hpp can compile on its own.

#include <boost/test/unit_test.hpp>

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(NfdControlCommand)

BOOST_AUTO_TEST_CASE(FaceCreate)
{
  FaceCreateCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/faces/create");

  ControlParameters p1;
  p1.setUri("tcp4://192.0.2.1")
    .setFaceId(4);
  BOOST_CHECK_THROW(command.validateRequest(p1), ControlCommand::ArgumentError);
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));

  ControlParameters p2;
  p2.setName("ndn:/example");
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  ControlParameters p3;
  p3.setUri("tcp4://192.0.2.1")
    .setFaceId(0);
  BOOST_CHECK_THROW(command.validateResponse(p3), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(FaceDestroy)
{
  FaceDestroyCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/faces/destroy");

  ControlParameters p1;
  p1.setUri("tcp4://192.0.2.1")
    .setFaceId(4);
  BOOST_CHECK_THROW(command.validateRequest(p1), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p1), ControlCommand::ArgumentError);

  ControlParameters p2;
  p2.setFaceId(0);
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  ControlParameters p3;
  p3.setFaceId(6);
  BOOST_CHECK_NO_THROW(command.validateRequest(p3));
  BOOST_CHECK_NO_THROW(command.validateResponse(p3));
}

BOOST_AUTO_TEST_CASE(FaceEnableLocalControl)
{
  FaceEnableLocalControlCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/faces/enable-local-control");

  ControlParameters p1;
  p1.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));

  ControlParameters p2;
  p2.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID)
    .setFaceId(9);
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(FaceDisableLocalControl)
{
  FaceDisableLocalControlCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/faces/disable-local-control");

  ControlParameters p1;
  p1.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));

  ControlParameters p2;
  p2.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID)
    .setFaceId(9);
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(FibAddNextHop)
{
  FibAddNextHopCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/fib/add-nexthop");

  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_THROW(command.validateResponse(p1), ControlCommand::ArgumentError);

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0)
    .setCost(6);
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  command.applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasCost());
  BOOST_CHECK_EQUAL(p1.getCost(), 0);
}

BOOST_AUTO_TEST_CASE(FibRemoveNextHop)
{
  FibRemoveNextHopCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/fib/remove-nexthop");

  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0);
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceSet)
{
  StrategyChoiceSetCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/strategy-choice/set");

  ControlParameters p1;
  p1.setName("ndn:/")
    .setStrategy("ndn:/strategy/P");
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));

  ControlParameters p2;
  p2.setName("ndn:/example");
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceUnset)
{
  StrategyChoiceUnsetCommand command;
  BOOST_CHECK_EQUAL(command.getPrefix(), "ndn:/localhost/nfd/strategy-choice/unset");

  ControlParameters p1;
  p1.setName("ndn:/example");
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setStrategy("ndn:/strategy/P");
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  ControlParameters p3;
  p3.setName("ndn:/");
  BOOST_CHECK_THROW(command.validateRequest(p3), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p3), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
