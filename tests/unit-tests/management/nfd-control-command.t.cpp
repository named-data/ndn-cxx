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

#include "management/nfd-control-command.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdControlCommand)

BOOST_AUTO_TEST_CASE(FaceCreate)
{
  FaceCreateCommand command;

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

  ControlParameters p4;
  p4.setUri("tcp4://192.0.2.1:6363");
  Name n4;
  BOOST_CHECK_NO_THROW(n4 = command.getRequestName("/PREFIX", p4));
  BOOST_CHECK(Name("ndn:/PREFIX/faces/create").isPrefixOf(n4));
}

BOOST_AUTO_TEST_CASE(FaceDestroy)
{
  FaceDestroyCommand command;

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
  Name n3;
  BOOST_CHECK_NO_THROW(n3 = command.getRequestName("/PREFIX", p3));
  BOOST_CHECK(Name("ndn:/PREFIX/faces/destroy").isPrefixOf(n3));
}

BOOST_AUTO_TEST_CASE(FaceEnableLocalControl)
{
  FaceEnableLocalControlCommand command;

  ControlParameters p1;
  p1.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/faces/enable-local-control").isPrefixOf(n1));

  ControlParameters p2;
  p2.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID)
    .setFaceId(9);
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  ControlParameters p3;
  p3.setLocalControlFeature(static_cast<LocalControlFeature>(666));
  BOOST_CHECK_THROW(command.validateRequest(p3), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p3), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(FaceDisableLocalControl)
{
  FaceDisableLocalControlCommand command;

  ControlParameters p1;
  p1.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/faces/disable-local-control").isPrefixOf(n1));

  ControlParameters p2;
  p2.setLocalControlFeature(LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID)
    .setFaceId(9);
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  ControlParameters p3;
  p3.setLocalControlFeature(static_cast<LocalControlFeature>(666));
  BOOST_CHECK_THROW(command.validateRequest(p3), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p3), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(FibAddNextHop)
{
  FibAddNextHopCommand command;

  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_THROW(command.validateResponse(p1), ControlCommand::ArgumentError);
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/fib/add-nexthop").isPrefixOf(n1));

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0)
    .setCost(6);
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  command.applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasCost());
  BOOST_CHECK_EQUAL(p1.getCost(), 0);

  p1.unsetFaceId();
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  command.applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasFaceId());
  BOOST_CHECK_EQUAL(p1.getFaceId(), 0);
}

BOOST_AUTO_TEST_CASE(FibRemoveNextHop)
{
  FibRemoveNextHopCommand command;

  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/fib/remove-nexthop").isPrefixOf(n1));

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0);
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  p1.unsetFaceId();
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  command.applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasFaceId());
  BOOST_CHECK_EQUAL(p1.getFaceId(), 0);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceSet)
{
  StrategyChoiceSetCommand command;

  ControlParameters p1;
  p1.setName("ndn:/")
    .setStrategy("ndn:/strategy/P");
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/strategy-choice/set").isPrefixOf(n1));

  ControlParameters p2;
  p2.setName("ndn:/example");
  BOOST_CHECK_THROW(command.validateRequest(p2), ControlCommand::ArgumentError);
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceUnset)
{
  StrategyChoiceUnsetCommand command;

  ControlParameters p1;
  p1.setName("ndn:/example");
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/strategy-choice/unset").isPrefixOf(n1));

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

BOOST_AUTO_TEST_CASE(RibRegister)
{
  RibRegisterCommand command;

  ControlParameters p1;
  p1.setName("ndn:/");
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_THROW(command.validateResponse(p1), ControlCommand::ArgumentError);
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/rib/register").isPrefixOf(n1));

  command.applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasOrigin());
  BOOST_CHECK_EQUAL(p1.getOrigin(), static_cast<uint64_t>(ROUTE_ORIGIN_APP));
  BOOST_REQUIRE(p1.hasCost());
  BOOST_CHECK_EQUAL(p1.getCost(), 0);
  BOOST_REQUIRE(p1.hasFlags());
  BOOST_CHECK_EQUAL(p1.getFlags(), static_cast<uint64_t>(ROUTE_FLAG_CHILD_INHERIT));
  BOOST_CHECK_EQUAL(p1.hasExpirationPeriod(), false);

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(2)
    .setCost(6);
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  command.applyDefaultsToRequest(p2);
  BOOST_CHECK_EQUAL(p2.hasExpirationPeriod(), false);
  BOOST_CHECK_NO_THROW(command.validateResponse(p2));
}

BOOST_AUTO_TEST_CASE(RibUnregister)
{
  RibUnregisterCommand command;

  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22)
    .setOrigin(ROUTE_ORIGIN_STATIC);
  BOOST_CHECK_NO_THROW(command.validateRequest(p1));
  BOOST_CHECK_NO_THROW(command.validateResponse(p1));
  Name n1;
  BOOST_CHECK_NO_THROW(n1 = command.getRequestName("/PREFIX", p1));
  BOOST_CHECK(Name("ndn:/PREFIX/rib/unregister").isPrefixOf(n1));

  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0)
    .setOrigin(ROUTE_ORIGIN_APP);
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);

  p2.unsetFaceId();
  BOOST_CHECK_NO_THROW(command.validateRequest(p2));
  BOOST_CHECK_THROW(command.validateResponse(p2), ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
