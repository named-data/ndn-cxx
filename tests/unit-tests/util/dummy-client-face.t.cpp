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

#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"
#include "../identity-management-time-fixture.hpp"
#include "make-interest-data.hpp"

namespace ndn {
namespace util {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestDummyClientFace, ndn::tests::IdentityManagementTimeFixture)

BOOST_AUTO_TEST_CASE(ProcessEventsOverride)
{
  bool isOverrideInvoked = false;
  auto override = [&] (time::milliseconds timeout) {
    isOverrideInvoked = true;
    BOOST_CHECK_EQUAL(timeout, 200_ms);
  };

  DummyClientFace face(io, {false, false, override});
  face.processEvents(200_ms);
  BOOST_CHECK(isOverrideInvoked);
}

BOOST_AUTO_TEST_CASE(BroadcastLink)
{
  DummyClientFace face1(io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face2(io, m_keyChain, DummyClientFace::Options{true, true});
  face1.linkTo(face2);

  int nFace1Interest = 0;
  int nFace2Interest = 0;
  face1.setInterestFilter("/face1",
                          [&] (const InterestFilter&, const Interest& interest) {
                            BOOST_CHECK_EQUAL(interest.getName().toUri(), "/face1/data");
                            nFace1Interest++;
                            face1.put(ndn::tests::makeNack(interest, lp::NackReason::NO_ROUTE));
                          }, nullptr, nullptr);
  face2.setInterestFilter("/face2",
                          [&] (const InterestFilter&, const Interest& interest) {
                            BOOST_CHECK_EQUAL(interest.getName().toUri(), "/face2/data");
                            nFace2Interest++;
                            face2.put(*ndn::tests::makeData("/face2/data"));
                            return;
                          }, nullptr, nullptr);

  advanceClocks(25_ms, 4);

  int nFace1Data = 0;
  int nFace2Nack = 0;
  face1.expressInterest(Interest("/face2/data"),
                        [&] (const Interest& i, const Data& d) {
                          BOOST_CHECK_EQUAL(d.getName().toUri(), "/face2/data");
                          nFace1Data++;
                        }, nullptr, nullptr);
  face2.expressInterest(Interest("/face1/data"),
                        [&] (const Interest& i, const Data& d) {
                          BOOST_CHECK(false);
                        },
                        [&] (const Interest& i, const lp::Nack& n) {
                          BOOST_CHECK_EQUAL(n.getInterest().getName().toUri(), "/face1/data");
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
  DummyClientFace face1(io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face2(io, m_keyChain, DummyClientFace::Options{true, true});

  face1.linkTo(face2);
  face2.unlink();
  BOOST_CHECK(face1.m_bcastLink == nullptr);

  DummyClientFace face3(io, m_keyChain, DummyClientFace::Options{true, true});
  face1.linkTo(face2);
  face3.linkTo(face1);
  face2.unlink();
  BOOST_CHECK(face1.m_bcastLink != nullptr);
}

BOOST_AUTO_TEST_CASE(AlreadyLinkException)
{
  DummyClientFace face1(io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face2(io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face3(io, m_keyChain, DummyClientFace::Options{true, true});
  DummyClientFace face4(io, m_keyChain, DummyClientFace::Options{true, true});

  face1.linkTo(face2);
  face3.linkTo(face4);

  BOOST_CHECK_THROW(face2.linkTo(face3), DummyClientFace::AlreadyLinkedError);
}

BOOST_AUTO_TEST_SUITE_END() // TestDummyClientFace
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
