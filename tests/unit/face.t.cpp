/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/face.hpp"
#include "ndn-cxx/lp/tags.hpp"
#include "ndn-cxx/transport/tcp-transport.hpp"
#include "ndn-cxx/transport/unix-transport.hpp"
#include "ndn-cxx/util/dummy-client-face.hpp"
#include "ndn-cxx/util/scheduler.hpp"

#include "tests/boost-test.hpp"
#include "tests/make-interest-data.hpp"
#include "tests/unit/identity-management-time-fixture.hpp"

#include <boost/logic/tribool.hpp>

namespace ndn {
namespace tests {

using ndn::util::DummyClientFace;

struct WantPrefixRegReply;
struct NoPrefixRegReply;

template<typename PrefixRegReply = WantPrefixRegReply>
class FaceFixture : public IdentityManagementTimeFixture
{
public:
  FaceFixture()
    : face(io, m_keyChain, {true, !std::is_same<PrefixRegReply, NoPrefixRegReply>::value})
  {
    static_assert(std::is_same<PrefixRegReply, WantPrefixRegReply>::value ||
                  std::is_same<PrefixRegReply, NoPrefixRegReply>::value, "");
  }

  /** \brief Execute a prefix registration, and optionally check the name in callback.
   *  \return whether the prefix registration succeeded.
   */
  bool
  runPrefixReg(function<void(const RegisterPrefixSuccessCallback& success,
                             const RegisterPrefixFailureCallback& failure)> f)
  {
    boost::logic::tribool result = boost::logic::indeterminate;
    f([&] (const Name&) { result = true; },
      [&] (const Name&, const std::string&) { result = false; });

    advanceClocks(1_ms);
    BOOST_REQUIRE(!boost::logic::indeterminate(result));
    return static_cast<bool>(result);
  }

  /** \brief Execute a prefix unregistration, and optionally check the name in callback.
   *  \return whether the prefix unregistration succeeded.
   */
  bool
  runPrefixUnreg(function<void(const UnregisterPrefixSuccessCallback& success,
                               const UnregisterPrefixFailureCallback& failure)> f)
  {
    boost::logic::tribool result = boost::logic::indeterminate;
    f([&] { result = true; }, [&] (const std::string&) { result = false; });

    advanceClocks(1_ms);
    BOOST_REQUIRE(!boost::logic::indeterminate(result));
    return static_cast<bool>(result);
  }

public:
  DummyClientFace face;
};

BOOST_FIXTURE_TEST_SUITE(TestFace, FaceFixture<>)

BOOST_AUTO_TEST_SUITE(Consumer)

BOOST_AUTO_TEST_CASE(ExpressInterestData)
{
  size_t nData = 0;
  face.expressInterest(*makeInterest("/Hello/World", true, 50_ms),
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
  face.expressInterest(*makeInterest("/Hello/World/a/2", false, 50_ms),
                       bind([]{}),
                       bind([]{}),
                       bind([&nTimeouts] { ++nTimeouts; }));
  advanceClocks(200_ms, 5);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

BOOST_AUTO_TEST_CASE(ExpressMultipleInterestData)
{
  size_t nData = 0;

  face.expressInterest(*makeInterest("/Hello/World", true, 50_ms),
                       [&] (const Interest& i, const Data& d) {
                         ++nData;
                       },
                       bind([] { BOOST_FAIL("Unexpected Nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  face.expressInterest(*makeInterest("/Hello/World/a", true, 50_ms),
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
  face.expressInterest(*makeInterest("/Hello/World", true),
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
  face.expressInterest(*makeInterest("/Hello/World", false, 50_ms),
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
  face.expressInterest(*makeInterest("/Hello/World", false, 50_ms),
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

  auto interest = makeInterest("/Hello/World", false, 50_ms);

  face.expressInterest(*interest,
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

  auto interest = makeInterest("/Hello/World", false, 50_ms, 1);
  face.expressInterest(*interest,
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       [&] (const Interest& i, const lp::Nack& n) {
                         ++nNacks;
                       },
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  interest->setNonce(2);
  face.expressInterest(*interest,
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
  face.expressInterest(*makeInterest("/Hello/World"),
                       bind([] { BOOST_FAIL("Unexpected Data"); }),
                       nullptr,
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(1_ms);

  BOOST_CHECK_NO_THROW(do {
    face.receive(makeNack(face.sentInterests.at(0), lp::NackReason::DUPLICATE));
    advanceClocks(1_ms);
  } while (false));
}

BOOST_AUTO_TEST_CASE(CancelPendingInterestHandle)
{
  auto hdl = face.expressInterest(*makeInterest("/Hello/World", true, 50_ms),
                                  bind([] { BOOST_FAIL("Unexpected data"); }),
                                  bind([] { BOOST_FAIL("Unexpected nack"); }),
                                  bind([] { BOOST_FAIL("Unexpected timeout"); }));
  advanceClocks(10_ms);

  hdl.cancel();
  advanceClocks(10_ms);

  face.receive(*makeData("/Hello/World/%21"));
  advanceClocks(200_ms, 5);

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(RemoveAllPendingInterests)
{
  face.expressInterest(*makeInterest("/Hello/World/0", false, 50_ms),
                       bind([] { BOOST_FAIL("Unexpected data"); }),
                       bind([] { BOOST_FAIL("Unexpected nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  face.expressInterest(*makeInterest("/Hello/World/1", false, 50_ms),
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
    face2.expressInterest(*makeInterest("/Hello/World", false, 50_ms),
                          nullptr, nullptr, nullptr);
    advanceClocks(50_ms, 2);
  }

  advanceClocks(50_ms, 2); // should not crash

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(DataCallbackPutData) // Bug 4596
{
  face.expressInterest(*makeInterest("/localhost/notification/1"),
                       [&] (const Interest& i, const Data& d) {
                         face.put(*makeData("/chronosync/sampleDigest/1"));
                       }, nullptr, nullptr);
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(face.sentInterests.back().getName(), "/localhost/notification/1");

  face.receive(*makeInterest("/chronosync/sampleDigest", true));
  advanceClocks(10_ms);

  face.put(*makeData("/localhost/notification/1"));
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(face.sentData.back().getName(), "/chronosync/sampleDigest/1");
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
  face.setInterestFilter("/", [&] (const InterestFilter&, const Interest&) {
    hasInterest1 = true;
    // do not respond with Data right away, so Face must send Interest to forwarder
  });
  // second InterestFilter disallows loopback and should not receive Interest
  face.setInterestFilter(InterestFilter("/").allowLoopback(false),
    bind([] { BOOST_ERROR("Unexpected Interest on second InterestFilter"); }));

  face.expressInterest(*makeInterest("/A", true),
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

  face.receive(*makeInterest("/A", true));
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

  face.put(makeNack(*makeInterest("/unsolicited", false, DEFAULT_INTEREST_LIFETIME, 18645250),
                    lp::NackReason::NO_ROUTE));
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0); // unsolicited Nack would not be sent

  auto interest1 = makeInterest("/Hello/World", false, DEFAULT_INTEREST_LIFETIME, 14247162);
  face.receive(*interest1);
  auto interest2 = makeInterest("/another/prefix", false, DEFAULT_INTEREST_LIFETIME, 92203002);
  face.receive(*interest2);
  advanceClocks(10_ms);

  face.put(makeNack(*interest1, lp::NackReason::DUPLICATE));
  advanceClocks(10_ms);
  BOOST_REQUIRE_EQUAL(face.sentNacks.size(), 1);
  BOOST_CHECK_EQUAL(face.sentNacks[0].getReason(), lp::NackReason::DUPLICATE);
  BOOST_CHECK(face.sentNacks[0].getTag<lp::CongestionMarkTag>() == nullptr);

  auto nack = makeNack(*interest2, lp::NackReason::NO_ROUTE);
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

  auto interest = makeInterest("/A", false, DEFAULT_INTEREST_LIFETIME, 14333271);
  face.receive(*interest);
  advanceClocks(10_ms);
  BOOST_CHECK(hasInterest1);
  BOOST_CHECK(hasInterest2);

  // Nack from first destination is received, should wait for a response from the other destination
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);

  face.put(makeNack(*interest, lp::NackReason::NO_ROUTE)); // Nack from second destination
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 1); // sending Nack after both destinations Nacked
  BOOST_CHECK_EQUAL(face.sentNacks.at(0).getReason(), lp::NackReason::CONGESTION); // least severe reason

  face.put(makeNack(*interest, lp::NackReason::DUPLICATE));
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

  auto interest = makeInterest("/A", false, DEFAULT_INTEREST_LIFETIME, 28395852);
  face.expressInterest(*interest,
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

  face.receive(makeNack(*interest, lp::NackReason::NO_ROUTE));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(hasNack, true);
}

BOOST_AUTO_TEST_CASE(SetUnsetInterestFilter)
{
  size_t nInterests = 0;
  size_t nRegs = 0;
  auto hdl = face.setInterestFilter("/Hello/World",
                                    bind([&nInterests] { ++nInterests; }),
                                    bind([&nRegs] { ++nRegs; }),
                                    bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nRegs, 1);
  BOOST_CHECK_EQUAL(nInterests, 0);

  face.receive(*makeInterest("/Hello/World/%21"));
  advanceClocks(25_ms, 4);

  BOOST_CHECK_EQUAL(nRegs, 1);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(*makeInterest("/Bye/World/%21"));
  advanceClocks(10000_ms, 10);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(*makeInterest("/Hello/World/%21/2"));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nInterests, 2);

  // removing filter
  hdl.cancel();
  advanceClocks(25_ms, 4);

  face.receive(*makeInterest("/Hello/World/%21/3"));
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
  auto hdl = face.setInterestFilter("/Hello/World",
                                    bind([&nInterests] { ++nInterests; }),
                                    bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nInterests, 0);

  face.receive(*makeInterest("/Hello/World/%21"));
  advanceClocks(25_ms, 4);

  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(*makeInterest("/Bye/World/%21"));
  advanceClocks(10000_ms, 10);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(*makeInterest("/Hello/World/%21/2"));
  advanceClocks(25_ms, 4);
  BOOST_CHECK_EQUAL(nInterests, 2);

  // removing filter
  hdl.cancel();
  advanceClocks(25_ms, 4);

  face.receive(*makeInterest("/Hello/World/%21/3"));
  BOOST_CHECK_EQUAL(nInterests, 2);

  face.unsetInterestFilter(static_cast<const RegisteredPrefixId*>(nullptr));
  advanceClocks(25_ms, 4);

  face.unsetInterestFilter(static_cast<const InterestFilterId*>(nullptr));
  advanceClocks(25_ms, 4);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterFail, FaceFixture<NoPrefixRegReply>)
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

BOOST_FIXTURE_TEST_CASE(SetInterestFilterFailWithoutSuccessCallback, FaceFixture<NoPrefixRegReply>)
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

BOOST_FIXTURE_TEST_CASE(RegisterUnregisterPrefixFail, FaceFixture<NoPrefixRegReply>)
{
  BOOST_CHECK(!runPrefixReg([&] (const auto& success, const auto& failure) {
    face.registerPrefix("/Hello/World", success, failure);
    this->advanceClocks(5_s, 20); // wait for command timeout
  }));
}

BOOST_AUTO_TEST_CASE(RegisterUnregisterPrefixHandle)
{
  RegisteredPrefixHandle hdl;
  BOOST_CHECK(!runPrefixUnreg([&] (const auto& success, const auto& failure) {
    // despite the "undefined behavior" warning, we try not to crash, but no API guarantee for this
    hdl.unregister(success, failure);
  }));

  BOOST_CHECK(runPrefixReg([&] (const auto& success, const auto& failure) {
    hdl = face.registerPrefix("/Hello/World", success, failure);
  }));

  BOOST_CHECK(runPrefixUnreg([&] (const auto& success, const auto& failure) {
    hdl.unregister(success, failure);
  }));
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

  face.receive(*makeInterest("/Hello/World/%21"));
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

  BOOST_REQUIRE_THROW(face.receive(*makeInterest("/Hello/World/XXX/b/c")), InterestFilter::Error);
}

BOOST_AUTO_TEST_CASE(SetRegexFilter)
{
  size_t nInInterests = 0;
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind([&nInInterests] { ++nInInterests; }),
                         nullptr,
                         bind([] { BOOST_FAIL("Unexpected setInterestFilter failure"); }));

  advanceClocks(25_ms, 4);

  face.receive(*makeInterest("/Hello/World/a"));     // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face.receive(*makeInterest("/Hello/World/a/b"));   // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face.receive(*makeInterest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face.receive(*makeInterest("/Hello/World/a/b/d")); // should not match
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

  face.receive(*makeInterest("/Hello/World/a")); // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face.receive(*makeInterest("/Hello/World/a/b")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face.receive(*makeInterest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face.receive(*makeInterest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterNoReg, FaceFixture<NoPrefixRegReply>) // Bug 2318
{
  // This behavior is specific to DummyClientFace.
  // Regular Face won't accept incoming packets until something is sent.

  int hit = 0;
  face.setInterestFilter(Name("/"), bind([&hit] { ++hit; }));
  face.processEvents(time::milliseconds(-1));

  face.receive(*makeInterest("/A"));
  face.processEvents(time::milliseconds(-1));

  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_CASE(SetInterestFilterHandle)
{
  int hit = 0;
  auto hdl = face.setInterestFilter(Name("/"), bind([&hit] { ++hit; }));
  face.processEvents(-1_ms);

  face.receive(*makeInterest("/A"));
  face.processEvents(-1_ms);
  BOOST_CHECK_EQUAL(hit, 1);

  hdl.cancel();
  face.processEvents(-1_ms);

  face.receive(*makeInterest("/B"));
  face.processEvents(-1_ms);
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
