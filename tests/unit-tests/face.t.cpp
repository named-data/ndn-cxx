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

#include "face.hpp"
#include "lp/tags.hpp"
#include "transport/tcp-transport.hpp"
#include "transport/unix-transport.hpp"
#include "util/dummy-client-face.hpp"
#include "util/scheduler.hpp"

#include "boost-test.hpp"
#include "identity-management-time-fixture.hpp"
#include "test-home-fixture.hpp"
#include "make-interest-data.hpp"

namespace ndn {
namespace tests {

using ndn::util::DummyClientFace;

class FaceFixture : public IdentityManagementTimeFixture
{
public:
  explicit
  FaceFixture(bool enableRegistrationReply = true)
    : face(io, m_keyChain, {true, enableRegistrationReply})
  {
  }

public:
  DummyClientFace face;
};

class FacesNoRegistrationReplyFixture : public FaceFixture
{
public:
  FacesNoRegistrationReplyFixture()
    : FaceFixture(false)
  {
  }
};

BOOST_FIXTURE_TEST_SUITE(TestFace, FaceFixture)

BOOST_AUTO_TEST_SUITE(Consumer)

BOOST_AUTO_TEST_CASE(ExpressInterestData)
{
  size_t nData = 0;
  face.expressInterest(Interest("/Hello/World", 50_ms),
                       [&] (const Interest& i, const Data& d) {
                         BOOST_CHECK(i.getName().isPrefixOf(d.getName()));
                         BOOST_CHECK_EQUAL(i.getName(), "/Hello/World");
                         ++nData;
                       },
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  advanceClocks(40_ms);

  face.receive(*makeData("/Bye/World/a"));
  face.receive(*makeData("/Hello/World/a"));

  advanceClocks(50_ms, 2);

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  size_t nTimeouts = 0;
  face.expressInterest(Interest("/Hello/World/a/2", 50_ms),
                       bind([]{}),
                       bind([]{}),
                       bind([&nTimeouts] { ++nTimeouts; }));
  advanceClocks(200_ms, 5);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

BOOST_AUTO_TEST_CASE(ExpressMultipleInterestData)
{
  size_t nData = 0;

  face.expressInterest(Interest("/Hello/World", 50_ms),
                       [&] (const Interest& i, const Data& d) {
                         ++nData;
                       },
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  face.expressInterest(Interest("/Hello/World/a", 50_ms),
                       [&] (const Interest& i, const Data& d) {
                         ++nData;
                       },
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  advanceClocks(40_ms);

  face.receive(*makeData("/Hello/World/a/b"));

  advanceClocks(50_ms, 2);

  BOOST_CHECK_EQUAL(nData, 2);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 2);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
}

BOOST_AUTO_TEST_CASE(ExpressInterestEmptyDataCallback)
{
  face.expressInterest(Interest("/Hello/World"),
                       nullptr,
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(1_ms);

  BOOST_CHECK_NO_THROW(do {
    face.receive(*makeData("/Hello/World/a"));
    advanceClocks(1_ms);
  } while (false));
}

BOOST_AUTO_TEST_CASE(ExpressInterestTimeout)
{
  size_t nTimeouts = 0;
  face.expressInterest(Interest("/Hello/World", 50_ms),
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       [&nTimeouts] (const Interest& i) {
                         BOOST_CHECK_EQUAL(i.getName(), "/Hello/World");
                         ++nTimeouts;
                       });

  advanceClocks(200_ms, 5);

  BOOST_CHECK_EQUAL(nTimeouts, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);
}

BOOST_AUTO_TEST_CASE(ExpressInterestEmptyTimeoutCallback)
{
  face.expressInterest(Interest("/Hello/World", 50_ms),
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       nullptr);
  advanceClocks(40_ms);

  BOOST_CHECK_NO_THROW(do {
    advanceClocks(6_ms, 2);
  } while (false));
}

BOOST_AUTO_TEST_CASE(ExpressInterestNack)
{
  size_t nNacks = 0;

  Interest interest("/Hello/World", 50_ms);

  face.expressInterest(interest,
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       [&] (const Interest& i, const lp::Nack& n) {
                         BOOST_CHECK(i.getName().isPrefixOf(n.getInterest().getName()));
                         BOOST_CHECK_EQUAL(i.getName(), "/Hello/World");
                         BOOST_CHECK_EQUAL(n.getReason(), lp::NackReason::DUPLICATE);
                         ++nNacks;
                       },
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  advanceClocks(40_ms);

  face.receive(makeNack(face.sentInterests.at(0), lp::NackReason::DUPLICATE));

  advanceClocks(50_ms, 2);

  BOOST_CHECK_EQUAL(nNacks, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(ExpressMultipleInterestNack)
{
  size_t nNacks = 0;

  Interest interest("/Hello/World", 50_ms);
  interest.setNonce(1);

  face.expressInterest(interest,
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       [&] (const Interest& i, const lp::Nack& n) {
                         ++nNacks;
                       },
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  interest.setNonce(2);
  face.expressInterest(interest,
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       [&] (const Interest& i, const lp::Nack& n) {
                         ++nNacks;
                       },
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  advanceClocks(40_ms);

  face.receive(makeNack(face.sentInterests.at(1), lp::NackReason::DUPLICATE));

  advanceClocks(50_ms, 2);

  BOOST_CHECK_EQUAL(nNacks, 2);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 2);
}

BOOST_AUTO_TEST_CASE(ExpressInterestEmptyNackCallback)
{
  face.expressInterest(Interest("/Hello/World"),
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       nullptr,
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(1_ms);

  BOOST_CHECK_NO_THROW(do {
    face.receive(makeNack(face.sentInterests.at(0), lp::NackReason::DUPLICATE));
    advanceClocks(1_ms);
  } while (false));
}

BOOST_AUTO_TEST_CASE(RemovePendingInterest)
{
  const PendingInterestId* interestId =
    face.expressInterest(Interest("/Hello/World", 50_ms),
                         bind([] { BOOST_FAIL("Unexpected data"); }),
                         bind([] { BOOST_FAIL("Unexpected nack"); }),
                         bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(10_ms);

  face.removePendingInterest(interestId);
  advanceClocks(10_ms);

  face.receive(*makeData("/Hello/World/%21"));
  advanceClocks(200_ms, 5);

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(RemoveAllPendingInterests)
{
  face.expressInterest(Interest("/Hello/World/0", 50_ms),
                       bind([] { BOOST_FAIL("Unexpected data"); }),
                       bind([] { BOOST_FAIL("Unexpected nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  face.expressInterest(Interest("/Hello/World/1", 50_ms),
                       bind([] { BOOST_FAIL("Unexpected data"); }),
                       bind([] { BOOST_FAIL("Unexpected nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  advanceClocks(10_ms);

  face.removeAllPendingInterests();
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(face.getNPendingInterests(), 0);

  face.receive(*makeData("/Hello/World/0"));
  face.receive(*makeData("/Hello/World/1"));
  advanceClocks(200_ms, 5);
}

BOOST_AUTO_TEST_CASE(DestructionWithoutCancellingPendingInterests) // Bug #2518
{
  {
    DummyClientFace face2(io, m_keyChain);
    face2.expressInterest(Interest("/Hello/World", 50_ms),
                          nullptr, nullptr, nullptr);
    advanceClocks(50_ms, 2);
  }

  advanceClocks(50_ms, 2); // should not crash

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Consumer

BOOST_AUTO_TEST_SUITE(Producer)

BOOST_AUTO_TEST_CASE(PutData)
{
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  Data data("/4g7xxcuEow/KFvK5Kf2m");
  signData(data);
  face.put(data);

  lp::CachePolicy cachePolicy;
  cachePolicy.setPolicy(lp::CachePolicyType::NO_CACHE);
  data.setTag(make_shared<lp::CachePolicyTag>(cachePolicy));
  data.setTag(make_shared<lp::CongestionMarkTag>(1));
  face.put(data);

  advanceClocks(10_ms);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 2);
  BOOST_CHECK(face.sentData[0].getTag<lp::CachePolicyTag>() == nullptr);
  BOOST_CHECK(face.sentData[0].getTag<lp::CongestionMarkTag>() == nullptr);
  BOOST_CHECK(face.sentData[1].getTag<lp::CachePolicyTag>() != nullptr);
  BOOST_CHECK(face.sentData[1].getTag<lp::CongestionMarkTag>() != nullptr);
}

BOOST_AUTO_TEST_CASE(PutDataLoopback)
{
  bool hasInterest1 = false, hasData = false;

  // first InterestFilter allows loopback and should receive Interest
  face.setInterestFilter("/", [&] (const InterestFilter&, const Interest& interest) {
    hasInterest1 = true;
    // do not respond with Data right away, so Face must send Interest to forwarder
  });
  // second InterestFilter disallows loopback and should not receive Interest
  face.setInterestFilter(InterestFilter("/").allowLoopback(false),
    bind([] { BOOST_ERROR("Unexpected Interest on second InterestFilter"); }));

  face.expressInterest(Interest("/A"),
                       bind([&] { hasData = true; }),
                       bind([] { BOOST_FAIL("Unexpected nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(hasInterest1, true); // Interest looped back
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1); // Interest sent to forwarder
  BOOST_CHECK_EQUAL(hasData, false); // waiting for Data

  face.put(*makeData("/A/B")); // first InterestFilter responds with Data
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(hasData, true);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0); // do not spill Data to forwarder
}

BOOST_AUTO_TEST_CASE(PutMultipleData)
{
  bool hasInterest1 = false;
  // register two Interest destinations
  face.setInterestFilter("/", bind([&] {
    hasInterest1 = true;
    // sending Data right away from the first destination, don't care whether Interest goes to second destination
    face.put(*makeData("/A/B"));
  }));
  face.setInterestFilter("/", bind([]{}));
  advanceClocks(10_ms);

  face.receive(*makeInterest("/A"));
  advanceClocks(10_ms);
  BOOST_CHECK(hasInterest1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.at(0).getName(), "/A/B");

  face.put(*makeData("/A/C"));
  BOOST_CHECK_EQUAL(face.sentData.size(), 1); // additional Data are ignored
}

BOOST_AUTO_TEST_CASE(PutNack)
{
  face.setInterestFilter("/", bind([]{})); // register one Interest destination so that face can accept Nacks
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);

  face.put(makeNack("/unsolicited", 18645250, lp::NackReason::NO_ROUTE));
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0); // unsolicited Nack would not be sent

  face.receive(*makeInterest("/Hello/World", 14247162));
  face.receive(*makeInterest("/another/prefix", 92203002));
  advanceClocks(10_ms);

  face.put(makeNack("/Hello/World", 14247162, lp::NackReason::DUPLICATE));
  advanceClocks(10_ms);
  BOOST_REQUIRE_EQUAL(face.sentNacks.size(), 1);
  BOOST_CHECK_EQUAL(face.sentNacks[0].getReason(), lp::NackReason::DUPLICATE);
  BOOST_CHECK(face.sentNacks[0].getTag<lp::CongestionMarkTag>() == nullptr);

  auto nack = makeNack("/another/prefix", 92203002, lp::NackReason::NO_ROUTE);
  nack.setTag(make_shared<lp::CongestionMarkTag>(1));
  face.put(nack);
  advanceClocks(10_ms);
  BOOST_REQUIRE_EQUAL(face.sentNacks.size(), 2);
  BOOST_CHECK_EQUAL(face.sentNacks[1].getReason(), lp::NackReason::NO_ROUTE);
  BOOST_CHECK(face.sentNacks[1].getTag<lp::CongestionMarkTag>() != nullptr);
}

BOOST_AUTO_TEST_CASE(PutMultipleNack)
{
  bool hasInterest1 = false, hasInterest2 = false;
  // register two Interest destinations
  face.setInterestFilter("/", [&] (const InterestFilter&, const Interest& interest) {
    hasInterest1 = true;
    // sending Nack right away from the first destination, Interest should still go to second destination
    face.put(makeNack(interest, lp::NackReason::CONGESTION));
  });
  face.setInterestFilter("/", bind([&] { hasInterest2 = true; }));
  advanceClocks(10_ms);

  face.receive(*makeInterest("/A", 14333271));
  advanceClocks(10_ms);
  BOOST_CHECK(hasInterest1);
  BOOST_CHECK(hasInterest2);

  // Nack from first destination is received, should wait for a response from the other destination
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);

  face.put(makeNack("/A", 14333271, lp::NackReason::NO_ROUTE)); // Nack from second destination
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 1); // sending Nack after both destinations Nacked
  BOOST_CHECK_EQUAL(face.sentNacks.at(0).getReason(), lp::NackReason::CONGESTION); // least severe reason

  face.put(makeNack("/A", 14333271, lp::NackReason::DUPLICATE));
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 1); // additional Nacks are ignored
}

BOOST_AUTO_TEST_CASE(PutMultipleNackLoopback)
{
  bool hasInterest1 = false, hasNack = false;

  // first InterestFilter allows loopback and should receive Interest
  face.setInterestFilter("/", [&] (const InterestFilter&, const Interest& interest) {
    hasInterest1 = true;
    face.put(makeNack(interest, lp::NackReason::CONGESTION));
  });
  // second InterestFilter disallows loopback and should not receive Interest
  face.setInterestFilter(InterestFilter("/").allowLoopback(false),
    bind([] { BOOST_ERROR("Unexpected Interest on second InterestFilter"); }));

  face.expressInterest(*makeInterest("/A", 28395852),
                       bind([] { BOOST_FAIL("Unexpected data"); }),
                       [&] (const Interest&, const lp::Nack& nack) {
                         hasNack = true;
                         BOOST_CHECK_EQUAL(nack.getReason(), lp::NackReason::CONGESTION);
                       },
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(hasInterest1, true); // Interest looped back
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1); // Interest sent to forwarder
  BOOST_CHECK_EQUAL(hasNack, false); // waiting for Nack from forwarder

  face.receive(makeNack("/A", 28395852, lp::NackReason::NO_ROUTE));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(hasNack, true);
}

BOOST_AUTO_TEST_CASE(SetUnsetInterestFilter)
{
  size_t nInterests = 0;
  size_t nRegs = 0;
  const RegisteredPrefixId* regPrefixId =
    face.setInterestFilter("/Hello/World",
                           bind([&nInterests] { ++nInterests; }),
                           bind([&nRegs] { ++nRegs; }),
                           bind([] {  BOOST_FAIL("Unexpected setInterestFilter failure"); }));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nRegs, 1);
  BOOST_CHECK_EQUAL(nInterests, 0);

  face.receive(Interest("/Hello/World/%21"));
  advanceClocks(25_ms, 4);

  BOOST_CHECK_EQUAL(nRegs, 1);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Bye/World/%21"));
  advanceClocks(10000_ms, 10);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Hello/World/%21/2"));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nInterests, 2);

  // removing filter
  face.unsetInterestFilter(regPrefixId);
  advanceClocks(25_ms, 4);

  face.receive(Interest("/Hello/World/%21/3"));
  BOOST_CHECK_EQUAL(nInterests, 2);

  face.unsetInterestFilter(static_cast<const RegisteredPrefixId*>(nullptr));
  advanceClocks(25_ms, 4);

  face.unsetInterestFilter(static_cast<const InterestFilterId*>(nullptr));
  advanceClocks(25_ms, 4);
}

BOOST_AUTO_TEST_CASE(SetInterestFilterEmptyInterestCallback)
{
  face.setInterestFilter("/A", nullptr);
  advanceClocks(1_ms);

  BOOST_CHECK_NO_THROW(do {
    face.receive(*makeInterest("/A/1"));
    advanceClocks(1_ms);
  } while (false));
}

BOOST_AUTO_TEST_CASE(SetUnsetInterestFilterWithoutSucessCallback)
{
  size_t nInterests = 0;
  const RegisteredPrefixId* regPrefixId =
    face.setInterestFilter("/Hello/World",
                           bind([&nInterests] { ++nInterests; }),
                           bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nInterests, 0);

  face.receive(Interest("/Hello/World/%21"));
  advanceClocks(25_ms, 4);

  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Bye/World/%21"));
  advanceClocks(10000_ms, 10);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Hello/World/%21/2"));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nInterests, 2);

  // removing filter
  face.unsetInterestFilter(regPrefixId);
  advanceClocks(25_ms, 4);

  face.receive(Interest("/Hello/World/%21/3"));
  BOOST_CHECK_EQUAL(nInterests, 2);

  face.unsetInterestFilter(static_cast<const RegisteredPrefixId*>(nullptr));
  advanceClocks(25_ms, 4);

  face.unsetInterestFilter(static_cast<const InterestFilterId*>(nullptr));
  advanceClocks(25_ms, 4);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterFail, FacesNoRegistrationReplyFixture)
{
  // don't enable registration reply
  size_t nRegFailed = 0;
  face.setInterestFilter("/Hello/World",
                         bind([] { BOOST_FAIL("Unexpected Interest"); }),
                         bind([] { BOOST_FAIL("Unexpected success of setInterestFilter"); }),
                         bind([&nRegFailed] { ++nRegFailed; }));

  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nRegFailed, 0);

  advanceClocks(2000_ms, 5);
  BOOST_CHECK_EQUAL(nRegFailed, 1);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterFailWithoutSuccessCallback, FacesNoRegistrationReplyFixture)
{
  // don't enable registration reply
  size_t nRegFailed = 0;
  face.setInterestFilter("/Hello/World",
                         bind([] { BOOST_FAIL("Unexpected Interest"); }),
                         bind([&nRegFailed] { ++nRegFailed; }));

  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nRegFailed, 0);

  advanceClocks(2000_ms, 5);
  BOOST_CHECK_EQUAL(nRegFailed, 1);
}

BOOST_AUTO_TEST_CASE(RegisterUnregisterPrefix)
{
  size_t nRegSuccesses = 0;
  const RegisteredPrefixId* regPrefixId =
    face.registerPrefix("/Hello/World",
                        bind([&nRegSuccesses] { ++nRegSuccesses; }),
                        bind([] { BOOST_FAIL("Unexpected registerPrefix failure"); }));

  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  size_t nUnregSuccesses = 0;
  face.unregisterPrefix(regPrefixId,
                        bind([&nUnregSuccesses] { ++nUnregSuccesses; }),
                        bind([] { BOOST_FAIL("Unexpected unregisterPrefix failure"); }));

  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nUnregSuccesses, 1);
}

BOOST_FIXTURE_TEST_CASE(RegisterUnregisterPrefixFail, FacesNoRegistrationReplyFixture)
{
  // don't enable registration reply
  size_t nRegFailures = 0;
  face.registerPrefix("/Hello/World",
                      bind([] { BOOST_FAIL("Unexpected registerPrefix success"); }),
                      bind([&nRegFailures] { ++nRegFailures; }));

  advanceClocks(5000_ms, 20);
  BOOST_CHECK_EQUAL(nRegFailures, 1);
}

BOOST_AUTO_TEST_CASE(SimilarFilters)
{
  size_t nInInterests1 = 0;
  face.setInterestFilter("/Hello/World",
                         bind([&nInInterests1] { ++nInInterests1; }),
                         nullptr,
                         bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  size_t nInInterests2 = 0;
  face.setInterestFilter("/Hello",
                         bind([&nInInterests2] { ++nInInterests2; }),
                         nullptr,
                         bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  size_t nInInterests3 = 0;
  face.setInterestFilter("/Los/Angeles/Lakers",
                         bind([&nInInterests3] { ++nInInterests3; }),
                         nullptr,
                         bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  advanceClocks(25_ms, 4);

  face.receive(Interest("/Hello/World/%21"));
  advanceClocks(25_ms, 4);

  BOOST_CHECK_EQUAL(nInInterests1, 1);
  BOOST_CHECK_EQUAL(nInInterests2, 1);
  BOOST_CHECK_EQUAL(nInInterests3, 0);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterError)
{
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         [] (const Name&, const Interest&) {
                           BOOST_FAIL("InterestFilter::Error should have been triggered");
                         },
                         nullptr,
                         bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  advanceClocks(25_ms, 4);

  BOOST_REQUIRE_THROW(face.receive(Interest("/Hello/World/XXX/b/c")), InterestFilter::Error);
}

BOOST_AUTO_TEST_CASE(SetRegexFilter)
{
  size_t nInInterests = 0;
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind([&nInInterests] { ++nInInterests; }),
                         nullptr,
                         bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  advanceClocks(25_ms, 4);

  face.receive(Interest("/Hello/World/a"));     // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face.receive(Interest("/Hello/World/a/b"));   // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face.receive(Interest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face.receive(Interest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterAndRegister)
{
  size_t nInInterests = 0;
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind([&nInInterests] { ++nInInterests; }));

  size_t nRegSuccesses = 0;
  face.registerPrefix("/Hello/World",
                      bind([&nRegSuccesses] { ++nRegSuccesses; }),
                      bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  face.receive(Interest("/Hello/World/a")); // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face.receive(Interest("/Hello/World/a/b")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face.receive(Interest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face.receive(Interest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterNoReg, FacesNoRegistrationReplyFixture) // Bug 2318
{
  // This behavior is specific to DummyClientFace.
  // Regular Face won't accept incoming packets until something is sent.

  int hit = 0;
  face.setInterestFilter(Name("/"), bind([&hit] { ++hit; }));
  face.processEvents(time::milliseconds(-1));

  auto interest = make_shared<Interest>("/A");
  face.receive(*interest);
  face.processEvents(time::milliseconds(-1));

  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_SUITE_END() // Producer

BOOST_AUTO_TEST_SUITE(IoRoutines)

BOOST_AUTO_TEST_CASE(ProcessEvents)
{
  face.processEvents(time::milliseconds(-1)); // io_service::reset()/poll() inside

  size_t nRegSuccesses = 0;
  face.registerPrefix("/Hello/World",
                      bind([&nRegSuccesses] { ++nRegSuccesses; }),
                      bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  // io_service::poll() without reset
  face.getIoService().poll();
  BOOST_CHECK_EQUAL(nRegSuccesses, 0);

  face.processEvents(time::milliseconds(-1)); // io_service::reset()/poll() inside
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);
}

BOOST_AUTO_TEST_CASE(DestroyWithoutProcessEvents) // Bug 3248
{
  auto face2 = make_unique<Face>(io);
  face2.reset();

  io.poll(); // should not crash

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // IoRoutines

BOOST_AUTO_TEST_SUITE(Transport)

using ndn::Transport;

struct PibDirWithDefaultTpm
{
  const std::string PATH = "build/keys-with-default-tpm";
};

BOOST_FIXTURE_TEST_CASE(FaceTransport, IdentityManagementTimeFixture)
{
  BOOST_CHECK(Face().getTransport() != nullptr);

  BOOST_CHECK(Face(shared_ptr<Transport>()).getTransport() != nullptr);
  BOOST_CHECK(Face(shared_ptr<Transport>(), io).getTransport() != nullptr);
  BOOST_CHECK(Face(shared_ptr<Transport>(), io, m_keyChain).getTransport() != nullptr);

  auto transport = make_shared<TcpTransport>("localhost", "6363"); // no real io operations will be scheduled
  BOOST_CHECK(Face(transport).getTransport() == transport);
  BOOST_CHECK(Face(transport, io).getTransport() == transport);
  BOOST_CHECK(Face(transport, io, m_keyChain).getTransport() == transport);
}

class WithEnv : private IdentityManagementTimeFixture
{
public:
  WithEnv()
  {
    if (getenv("NDN_CLIENT_TRANSPORT") != nullptr) {
      m_oldTransport = getenv("NDN_CLIENT_TRANSPORT");
      unsetenv("NDN_CLIENT_TRANSPORT");
    }
  }

  void
  configure(const std::string& faceUri)
  {
    setenv("NDN_CLIENT_TRANSPORT", faceUri.c_str(), true);
  }

  ~WithEnv()
  {
    if (!m_oldTransport.empty()) {
      setenv("NDN_CLIENT_TRANSPORT", m_oldTransport.c_str(), true);
    }
    else {
      unsetenv("NDN_CLIENT_TRANSPORT");
    }
  }

private:
  std::string m_oldTransport;
};

class WithConfig : private TestHomeFixture<DefaultPibDir>
{
public:
  void
  configure(const std::string& faceUri)
  {
    createClientConf({"transport=" + faceUri});
  }
};

class WithEnvAndConfig : public WithEnv, public WithConfig
{
};

typedef boost::mpl::vector<WithEnv, WithConfig> ConfigOptions;

BOOST_FIXTURE_TEST_CASE(NoConfig, WithEnvAndConfig) // fixture configures test HOME and PIB/TPM path
{
  shared_ptr<Face> face;
  BOOST_REQUIRE_NO_THROW(face = make_shared<Face>());
  BOOST_CHECK(dynamic_pointer_cast<UnixTransport>(face->getTransport()) != nullptr);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Unix, T, ConfigOptions, T)
{
  this->configure("unix://some/path");

  shared_ptr<Face> face;
  BOOST_REQUIRE_NO_THROW(face = make_shared<Face>());
  BOOST_CHECK(dynamic_pointer_cast<UnixTransport>(face->getTransport()) != nullptr);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(Tcp, T, ConfigOptions, T)
{
  this->configure("tcp://127.0.0.1:6000");

  shared_ptr<Face> face;
  BOOST_REQUIRE_NO_THROW(face = make_shared<Face>());
  BOOST_CHECK(dynamic_pointer_cast<TcpTransport>(face->getTransport()) != nullptr);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(WrongTransport, T, ConfigOptions, T)
{
  this->configure("wrong-transport:");

  BOOST_CHECK_THROW(make_shared<Face>(), ConfigFile::Error);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(WrongUri, T, ConfigOptions, T)
{
  this->configure("wrong-uri");

  BOOST_CHECK_THROW(make_shared<Face>(), ConfigFile::Error);
}

BOOST_FIXTURE_TEST_CASE(EnvOverride, WithEnvAndConfig)
{
  this->WithEnv::configure("tcp://127.0.0.1:6000");
  this->WithConfig::configure("unix://some/path");

  shared_ptr<Face> face;
  BOOST_REQUIRE_NO_THROW(face = make_shared<Face>());
  BOOST_CHECK(dynamic_pointer_cast<TcpTransport>(face->getTransport()) != nullptr);
}

BOOST_FIXTURE_TEST_CASE(ExplicitTransport, WithEnvAndConfig)
{
  this->WithEnv::configure("wrong-uri");
  this->WithConfig::configure("wrong-transport:");

  auto transport = make_shared<UnixTransport>("unix://some/path");
  shared_ptr<Face> face;
  BOOST_REQUIRE_NO_THROW(face = make_shared<Face>(transport));
  BOOST_CHECK(dynamic_pointer_cast<UnixTransport>(face->getTransport()) != nullptr);
}

BOOST_AUTO_TEST_SUITE_END() // Transport

BOOST_AUTO_TEST_SUITE_END() // TestFace

} // namespace tests
} // namespace ndn
