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

#include "ndn-cxx/mgmt/nfd/control-command.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"

namespace ndn::tests {

using namespace ndn::nfd;

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)
BOOST_AUTO_TEST_SUITE(TestControlCommand)

BOOST_AUTO_TEST_CASE(FaceCreateRequest)
{
  using Command = FaceCreateCommand;

  // good with required fields only
  ControlParameters p1;
  p1.setUri("tcp4://192.0.2.1:6363");
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("/PREFIX/faces/create").isPrefixOf(n1));

  // good with optional fields
  ControlParameters p2(p1);
  p2.setLocalUri("tcp4://192.0.2.2:32114")
    .setFacePersistency(FACE_PERSISTENCY_PERMANENT)
    .setBaseCongestionMarkingInterval(100_ms)
    .setDefaultCongestionThreshold(10000)
    .setMtu(8192)
    .setFlags(0x3)
    .setMask(0x1);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));

  // Uri is required
  ControlParameters p3;
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);

  // Name is forbidden
  ControlParameters p4(p1);
  p4.setName("/example");
  BOOST_CHECK_THROW(Command::validateRequest(p4), ArgumentError);

  // Flags and Mask must be specified together
  ControlParameters p5(p1);
  p5.setFlags(0x3);
  BOOST_CHECK_THROW(Command::validateRequest(p5), ArgumentError);

  // Flags and Mask must be specified together
  ControlParameters p6(p1);
  p6.setMask(0x1);
  BOOST_CHECK_THROW(Command::validateRequest(p6), ArgumentError);
}

BOOST_AUTO_TEST_CASE(FaceCreateResponse)
{
  using Command = FaceCreateCommand;

  // good
  ControlParameters p1;
  p1.setFaceId(3208)
    .setUri("tcp4://192.0.2.1:6363")
    .setLocalUri("tcp4://192.0.2.2:32114")
    .setFacePersistency(FACE_PERSISTENCY_PERMANENT)
    .setBaseCongestionMarkingInterval(500_ns)
    .setDefaultCongestionThreshold(12345)
    .setMtu(2048)
    .setFlags(0x3);
  BOOST_CHECK_NO_THROW(Command::validateResponse(p1));

  // Name is forbidden
  ControlParameters p2(p1);
  p2.setName("/example");
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // Mask is forbidden
  ControlParameters p3(p1);
  p3.setMask(0x1);
  BOOST_CHECK_THROW(Command::validateResponse(p3), ArgumentError);

  // FaceId must be valid
  ControlParameters p4(p1);
  p4.setFaceId(INVALID_FACE_ID);
  BOOST_CHECK_THROW(Command::validateResponse(p4), ArgumentError);

  // LocalUri is required
  ControlParameters p5(p1);
  p5.unsetLocalUri();
  BOOST_CHECK_THROW(Command::validateResponse(p5), ArgumentError);
}

BOOST_AUTO_TEST_CASE(FaceUpdate)
{
  using Command = FaceUpdateCommand;

  // FaceId must be valid
  ControlParameters p1;
  p1.setFaceId(0);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  p1.setFaceId(INVALID_FACE_ID);
  BOOST_CHECK_THROW(Command::validateResponse(p1), ArgumentError);

  // Persistency and Flags are required in response
  p1.setFaceId(1);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_THROW(Command::validateResponse(p1), ArgumentError);
  Command::applyDefaultsToRequest(p1);
  BOOST_CHECK_EQUAL(p1.getFaceId(), 1);

  // Good request, bad response (Mask is forbidden but present)
  ControlParameters p2;
  p2.setFaceId(1)
    .setFacePersistency(FACE_PERSISTENCY_PERSISTENT)
    .setBaseCongestionMarkingInterval(765_ns)
    .setDefaultCongestionThreshold(54321)
    .setMtu(8192)
    .setFlagBit(BIT_LOCAL_FIELDS_ENABLED, false);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // Flags without Mask (good response, bad request)
  p2.unsetMask();
  BOOST_CHECK_THROW(Command::validateRequest(p2), ArgumentError);
  BOOST_CHECK_NO_THROW(Command::validateResponse(p2));

  // FaceId is optional in request
  p2.setFlagBit(BIT_LOCAL_FIELDS_ENABLED, false);
  p2.unsetFaceId();
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));

  // Name is forbidden
  ControlParameters p3;
  p3.setFaceId(1)
    .setName("/ndn/name");
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p3), ArgumentError);

  // Uri is forbidden
  ControlParameters p4;
  p4.setFaceId(1)
    .setUri("tcp4://192.0.2.1");
  BOOST_CHECK_THROW(Command::validateRequest(p4), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p4), ArgumentError);

  // Empty request is valid, empty response is invalid
  ControlParameters p5;
  BOOST_CHECK_NO_THROW(Command::validateRequest(p5));
  BOOST_CHECK_THROW(Command::validateResponse(p5), ArgumentError);
  BOOST_CHECK(!p5.hasFaceId());

  // Default request, not valid response due to missing FacePersistency and Flags
  Command::applyDefaultsToRequest(p5);
  BOOST_REQUIRE(p5.hasFaceId());
  BOOST_CHECK_NO_THROW(Command::validateRequest(p5));
  BOOST_CHECK_THROW(Command::validateResponse(p5), ArgumentError);
  BOOST_CHECK_EQUAL(p5.getFaceId(), 0);
}

BOOST_AUTO_TEST_CASE(FaceDestroy)
{
  using Command = FaceDestroyCommand;

  // Uri is forbidden
  ControlParameters p1;
  p1.setUri("tcp4://192.0.2.1")
    .setFaceId(4);
  BOOST_CHECK_THROW(Command::validateRequest(p1), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p1), ArgumentError);

  // FaceId must be valid
  ControlParameters p2;
  p2.setFaceId(INVALID_FACE_ID);
  BOOST_CHECK_THROW(Command::validateRequest(p2), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // Good request, good response
  ControlParameters p3;
  p3.setFaceId(6);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p3));
  BOOST_CHECK_NO_THROW(Command::validateResponse(p3));
  Name n3 = Command::createRequest("/PREFIX", p3).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/faces/destroy").isPrefixOf(n3));
}

BOOST_AUTO_TEST_CASE(FibAddNextHop)
{
  using Command = FibAddNextHopCommand;

  // Cost required in response
  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_THROW(Command::validateResponse(p1), ArgumentError);
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/fib/add-nexthop").isPrefixOf(n1));

  // Good request, bad response (FaceId must be valid)
  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0)
    .setCost(6);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));
  p2.setFaceId(INVALID_FACE_ID);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // Default request
  Command::applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasCost());
  BOOST_CHECK_EQUAL(p1.getCost(), 0);

  // FaceId optional in request
  p1.unsetFaceId();
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  Command::applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasFaceId());
  BOOST_CHECK_EQUAL(p1.getFaceId(), 0);
}

BOOST_AUTO_TEST_CASE(FibRemoveNextHop)
{
  using Command = FibRemoveNextHopCommand;

  // Good request, good response
  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_NO_THROW(Command::validateResponse(p1));
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/fib/remove-nexthop").isPrefixOf(n1));

  // Good request, bad response (FaceId must be valid)
  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));
  p2.setFaceId(INVALID_FACE_ID);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // FaceId is optional in request
  p1.unsetFaceId();
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  Command::applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasFaceId());
  BOOST_CHECK_EQUAL(p1.getFaceId(), 0);
}

BOOST_AUTO_TEST_CASE(CsConfigRequest)
{
  using Command = CsConfigCommand;

  // good empty request
  ControlParameters p1;
  Command::validateRequest(p1);
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("/PREFIX/cs/config").isPrefixOf(n1));

  // good full request
  ControlParameters p2;
  p2.setCapacity(1574);
  p2.setFlagBit(BIT_CS_ENABLE_ADMIT, true);
  p2.setFlagBit(BIT_CS_ENABLE_SERVE, true);
  Command::validateRequest(p2);

  // bad request: Flags but no Mask
  ControlParameters p3(p2);
  p3.unsetMask();
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);

  // bad request: Mask but no Flags
  ControlParameters p4(p2);
  p4.unsetFlags();
  BOOST_CHECK_THROW(Command::validateRequest(p4), ArgumentError);

  // bad request: forbidden field
  ControlParameters p5(p2);
  p5.setName("/example");
  BOOST_CHECK_THROW(Command::validateRequest(p5), ArgumentError);
}

BOOST_AUTO_TEST_CASE(CsConfigResponse)
{
  using Command = CsConfigCommand;

  // bad empty response
  ControlParameters p1;
  BOOST_CHECK_THROW(Command::validateResponse(p1), ArgumentError);

  // bad response: Mask not allowed
  ControlParameters p2;
  p2.setCapacity(1574);
  p2.setFlagBit(BIT_CS_ENABLE_ADMIT, true);
  p2.setFlagBit(BIT_CS_ENABLE_SERVE, true);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // good response
  ControlParameters p3(p2);
  p3.unsetMask();
  Command::validateResponse(p3);
}

BOOST_AUTO_TEST_CASE(CsEraseRequest)
{
  using Command = CsEraseCommand;

  // good no-limit request
  ControlParameters p1;
  p1.setName("/u4LYPNU8Q");
  Command::validateRequest(p1);
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("/PREFIX/cs/erase").isPrefixOf(n1));

  // good limited request
  ControlParameters p2;
  p2.setName("/IMw1RaLF");
  p2.setCount(177);
  Command::validateRequest(p2);

  // bad request: zero entry
  ControlParameters p3;
  p3.setName("/ahMID1jcib");
  p3.setCount(0);
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);

  // bad request: forbidden field
  ControlParameters p4(p2);
  p4.setCapacity(278);
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);
}

BOOST_AUTO_TEST_CASE(CsEraseResponse)
{
  using Command = CsEraseCommand;

  // good normal response
  ControlParameters p1;
  p1.setName("/TwiIwCdR");
  p1.setCount(1);
  Command::validateResponse(p1);

  // good limit exceeded request
  ControlParameters p2;
  p2.setName("/NMsiy44pr");
  p2.setCapacity(360);
  p2.setCount(360);
  Command::validateResponse(p2);

  // good zero-entry response
  ControlParameters p3;
  p3.setName("/5f1LRPh1L");
  p3.setCount(0);
  Command::validateResponse(p3);

  // bad request: missing Count
  ControlParameters p4(p1);
  p4.unsetCount();
  BOOST_CHECK_THROW(Command::validateResponse(p4), ArgumentError);

  // bad request: zero capacity
  ControlParameters p5(p1);
  p5.setCapacity(0);
  BOOST_CHECK_THROW(Command::validateResponse(p5), ArgumentError);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceSet)
{
  using Command = StrategyChoiceSetCommand;

  // Good request, good response
  ControlParameters p1;
  p1.setName("ndn:/")
    .setStrategy("ndn:/strategy/P");
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_NO_THROW(Command::validateResponse(p1));
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/strategy-choice/set").isPrefixOf(n1));

  // Strategy is required in both requests and responses
  ControlParameters p2;
  p2.setName("ndn:/example");
  BOOST_CHECK_THROW(Command::validateRequest(p2), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceUnset)
{
  using Command = StrategyChoiceUnsetCommand;

  // Good request, good response
  ControlParameters p1;
  p1.setName("ndn:/example");
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_NO_THROW(Command::validateResponse(p1));
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/strategy-choice/unset").isPrefixOf(n1));

  // Strategy is forbidden
  ControlParameters p2;
  p2.setName("ndn:/example")
    .setStrategy("ndn:/strategy/P");
  BOOST_CHECK_THROW(Command::validateRequest(p2), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // Name must have at least one component
  ControlParameters p3;
  p3.setName("ndn:/");
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);
  BOOST_CHECK_THROW(Command::validateResponse(p3), ArgumentError);
}

BOOST_AUTO_TEST_CASE(RibRegister)
{
  using Command = RibRegisterCommand;

  // Good request, response missing many fields
  ControlParameters p1;
  p1.setName("ndn:/");
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_THROW(Command::validateResponse(p1), ArgumentError);
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/rib/register").isPrefixOf(n1));

  // Default request
  Command::applyDefaultsToRequest(p1);
  BOOST_REQUIRE(p1.hasOrigin());
  BOOST_CHECK_EQUAL(p1.getOrigin(), ROUTE_ORIGIN_APP);
  BOOST_REQUIRE(p1.hasCost());
  BOOST_CHECK_EQUAL(p1.getCost(), 0);
  BOOST_REQUIRE(p1.hasFlags());
  BOOST_CHECK_EQUAL(p1.getFlags(), static_cast<uint64_t>(ROUTE_FLAG_CHILD_INHERIT));
  BOOST_CHECK_EQUAL(p1.hasExpirationPeriod(), false);

  // Good request, good response
  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(2)
    .setCost(6);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));
  Command::applyDefaultsToRequest(p2);
  BOOST_CHECK_EQUAL(p2.hasExpirationPeriod(), false);
  BOOST_CHECK_NO_THROW(Command::validateResponse(p2));
}

BOOST_AUTO_TEST_CASE(RibUnregister)
{
  using Command = RibUnregisterCommand;

  // Good request, good response
  ControlParameters p1;
  p1.setName("ndn:/")
    .setFaceId(22)
    .setOrigin(ROUTE_ORIGIN_STATIC);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  BOOST_CHECK_NO_THROW(Command::validateResponse(p1));
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/rib/unregister").isPrefixOf(n1));

  // Good request, bad response (FaceId must be valid)
  ControlParameters p2;
  p2.setName("ndn:/example")
    .setFaceId(0)
    .setOrigin(ROUTE_ORIGIN_APP);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));
  p2.setFaceId(INVALID_FACE_ID);
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);

  // FaceId is optional in request, required in response
  p2.unsetFaceId();
  BOOST_CHECK_NO_THROW(Command::validateRequest(p2));
  BOOST_CHECK_THROW(Command::validateResponse(p2), ArgumentError);
}

BOOST_FIXTURE_TEST_CASE(RibAnnounce, KeyChainFixture)
{
  using Command = RibAnnounceCommand;

  // Good request
  PrefixAnnouncement pa1;
  pa1.setAnnouncedName("ndn:/");
  pa1.setExpiration(1_min);
  pa1.toData(m_keyChain);
  RibAnnounceParameters p1;
  p1.setPrefixAnnouncement(pa1);
  BOOST_CHECK_NO_THROW(Command::validateRequest(p1));
  Name n1 = Command::createRequest("/PREFIX", p1).getName();
  BOOST_CHECK(Name("ndn:/PREFIX/rib/announce").isPrefixOf(n1));

  // Good response
  ControlParameters p2;
  p2.setName("ndn:/")
    .setFaceId(22)
    .setOrigin(ndn::nfd::ROUTE_ORIGIN_PREFIXANN)
    .setCost(2048)
    .setFlags(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT)
    .setExpirationPeriod(1_min);
  BOOST_CHECK_NO_THROW(Command::validateResponse(p2));

  // Bad request (PrefixAnnouncement must be signed)
  PrefixAnnouncement pa2;
  pa2.setAnnouncedName("ndn:/");
  pa2.setExpiration(1_min);
  RibAnnounceParameters p3;
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);
  p3.setPrefixAnnouncement(pa2);
  BOOST_CHECK_THROW(Command::validateRequest(p3), ArgumentError);

  // Bad response (FaceId must be valid)
  ControlParameters p4;
  p4.setName("ndn:/")
    .setFaceId(0)
    .setOrigin(ndn::nfd::ROUTE_ORIGIN_PREFIXANN)
    .setCost(2048)
    .setFlags(ndn::nfd::ROUTE_FLAG_CHILD_INHERIT)
    .setExpirationPeriod(1_min);
  BOOST_CHECK_THROW(Command::validateResponse(p4), ArgumentError);
}

BOOST_AUTO_TEST_SUITE_END() // TestControlCommand
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace ndn::tests
