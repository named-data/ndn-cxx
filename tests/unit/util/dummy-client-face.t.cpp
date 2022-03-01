/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/util/dummy-client-face.hpp"
#include "ndn-cxx/mgmt/nfd/controller.hpp"

#include "tests/test-common.hpp"
#include "tests/unit/io-key-chain-fixture.hpp"

namespace ndn {
namespace util {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestDummyClientFace, IoKeyChainFixture)

BOOST_AUTO_TEST_CASE(ProcessEventsOverride)
{
  bool isOverrideInvoked = false;
  auto override = [&] (time::milliseconds timeout) {
    isOverrideInvoked = true;
    BOOST_CHECK_EQUAL(timeout, 200_ms);
  };

  DummyClientFace face(m_io, {false, false, override});
  face.processEvents(200_ms);
  BOOST_CHECK(isOverrideInvoked);
}

BOOST_AUTO_TEST_CASE(RegistrationReply)
{
  DummyClientFace::Options opts;
  opts.enableRegistrationReply = true;
  opts.registrationReplyFaceId = 3001;
  DummyClientFace face(m_io, m_keyChain, opts);

  ndn::nfd::Controller controller(face, m_keyChain);
  ndn::nfd::ControlParameters params;
  bool didRibRegisterSucceed = false;
  controller.start<ndn::nfd::RibRegisterCommand>(
    ndn::nfd::ControlParameters()
      .setName("/Q")
      .setOrigin(ndn::nfd::ROUTE_ORIGIN_NLSR)
      .setCost(2400)
      .setFlags(0),
    [&] (const ndn::nfd::ControlParameters& p) {
      BOOST_CHECK_EQUAL(p.getName(), "/Q");
      BOOST_CHECK_EQUAL(p.getFaceId(), 3001);
      BOOST_CHECK_EQUAL(p.getOrigin(), ndn::nfd::ROUTE_ORIGIN_NLSR);
      BOOST_CHECK_EQUAL(p.getCost(), 2400);
      BOOST_CHECK_EQUAL(p.getFlags(), 0);
      didRibRegisterSucceed = true;
    },
    [] (const ndn::nfd::ControlResponse& r) {
      BOOST_TEST_FAIL("RibRegisterCommand failed " << r);
    });
  advanceClocks(1_ms, 2);
  BOOST_CHECK(didRibRegisterSucceed);

  bool didRibUnregisterSucceed = false;
  controller.start<ndn::nfd::RibUnregisterCommand>(
    ndn::nfd::ControlParameters()
      .setName("/Q")
      .setOrigin(ndn::nfd::ROUTE_ORIGIN_NLSR),
    [&] (const ndn::nfd::ControlParameters& p) {
      BOOST_CHECK_EQUAL(p.getName(), "/Q");
      BOOST_CHECK_EQUAL(p.getFaceId(), 3001);
      BOOST_CHECK_EQUAL(p.getOrigin(), ndn::nfd::ROUTE_ORIGIN_NLSR);
      didRibUnregisterSucceed = true;
    },
    [] (const ndn::nfd::ControlResponse& r) {
      BOOST_TEST_FAIL("RibUnregisterCommand failed " << r);
    });
  advanceClocks(1_ms, 2);
  BOOST_CHECK(didRibUnregisterSucceed);
}

BOOST_AUTO_TEST_CASE(BroadcastLink)
{
  DummyClientFace face1(m_io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face2(m_io, m_keyChain, DummyClientFace::Options{true, true});
  face1.linkTo(face2);

  int nFace1Interest = 0;
  int nFace2Interest = 0;
  face1.setInterestFilter("/face1",
                          [&] (const InterestFilter&, const Interest& interest) {
                            BOOST_CHECK_EQUAL(interest.getName(), "/face1/data");
                            nFace1Interest++;
                            face1.put(ndn::tests::makeNack(interest, lp::NackReason::NO_ROUTE));
                          }, nullptr, nullptr);
  face2.setInterestFilter("/face2",
                          [&] (const InterestFilter&, const Interest& interest) {
                            BOOST_CHECK_EQUAL(interest.getName(), "/face2/data");
                            nFace2Interest++;
                            face2.put(*ndn::tests::makeData("/face2/data"));
                            return;
                          }, nullptr, nullptr);

  advanceClocks(25_ms, 4);

  int nFace1Data = 0;
  int nFace2Nack = 0;
  face1.expressInterest(*makeInterest("/face2/data"),
                        [&] (const Interest& i, const Data& d) {
                          BOOST_CHECK_EQUAL(d.getName(), "/face2/data");
                          nFace1Data++;
                        }, nullptr, nullptr);
  face2.expressInterest(*makeInterest("/face1/data"),
                        [&] (const Interest& i, const Data& d) {
                          BOOST_CHECK(false);
                        },
                        [&] (const Interest& i, const lp::Nack& n) {
                          BOOST_CHECK_EQUAL(n.getInterest().getName(), "/face1/data");
                          nFace2Nack++;
                        }, nullptr);

  advanceClocks(10_ms, 100);

  BOOST_CHECK_EQUAL(nFace1Data, 1);
  BOOST_CHECK_EQUAL(nFace2Nack, 1);
  BOOST_CHECK_EQUAL(nFace1Interest, 1);
  BOOST_CHECK_EQUAL(nFace2Interest, 1);
}

BOOST_AUTO_TEST_CASE(BroadcastLinkDestroy)
{
  DummyClientFace face1(m_io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face2(m_io, m_keyChain, DummyClientFace::Options{true, true});

  face1.linkTo(face2);
  face2.unlink();
  BOOST_CHECK(face1.m_bcastLink == nullptr);

  DummyClientFace face3(m_io, m_keyChain, DummyClientFace::Options{true, true});
  face1.linkTo(face2);
  face3.linkTo(face1);
  face2.unlink();
  BOOST_CHECK(face1.m_bcastLink != nullptr);
}

BOOST_AUTO_TEST_CASE(AlreadyLinkException)
{
  DummyClientFace face1(m_io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face2(m_io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face3(m_io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face4(m_io, m_keyChain, DummyClientFace::Options{true, true});

  face1.linkTo(face2);
  face3.linkTo(face4);

  BOOST_CHECK_THROW(face2.linkTo(face3), DummyClientFace::AlreadyLinkedError);
}

BOOST_AUTO_TEST_SUITE_END() // TestDummyClientFace
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
