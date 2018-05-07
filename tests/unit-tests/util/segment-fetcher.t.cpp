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

#include "util/segment-fetcher.hpp"

#include "data.hpp"
#include "lp/nack.hpp"
#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"
#include "dummy-validator.hpp"
#include "make-interest-data.hpp"
#include "../identity-management-time-fixture.hpp"

namespace ndn {
namespace util {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestSegmentFetcher)

class Fixture : public IdentityManagementTimeFixture
{
public:
  Fixture()
    : face(io, m_keyChain)
  {
  }

  static shared_ptr<Data>
  makeDataSegment(const Name& baseName, uint64_t segment, bool isFinal)
  {
    const uint8_t buffer[] = "Hello, world!";

    auto data = make_shared<Data>(Name(baseName).appendSegment(segment));
    data->setContent(buffer, sizeof(buffer));
    if (isFinal) {
      data->setFinalBlock(data->getName()[-1]);
    }

    return signData(data);
  }

  void
  onError(uint32_t errorCode)
  {
    ++nErrors;
    lastError = errorCode;
  }

  void
  onComplete(const ConstBufferPtr& data)
  {
    ++nCompletions;
    dataSize = data->size();
    dataString = std::string(data->get<char>());
  }

  void
  nackLastInterest(lp::NackReason nackReason)
  {
    const Interest& lastInterest = face.sentInterests.back();
    lp::Nack nack = makeNack(lastInterest, nackReason);
    face.receive(nack);
    advanceClocks(10_ms);
  }

  void
  connectSignals(shared_ptr<SegmentFetcher> fetcher)
  {
    fetcher->onComplete.connect(bind(&Fixture::onComplete, this, _1));
    fetcher->onError.connect(bind(&Fixture::onError, this, _1));
  }

public:
  DummyClientFace face;

  int nErrors = 0;
  uint32_t lastError = 0;
  int nCompletions = 0;
  size_t dataSize = 0;
  std::string dataString;
};

BOOST_FIXTURE_TEST_CASE(Timeout, Fixture)
{
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world", 100_ms),
                                                             acceptValidator);
  connectSignals(fetcher);

  advanceClocks(1_ms);
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);

  advanceClocks(98_ms);
  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  advanceClocks(1_ms, 2);
  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::INTEREST_TIMEOUT));
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(Basic, Fixture)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world", 1000_s),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));

  advanceClocks(10_ms);

  for (uint64_t i = 0; i < 400; i++) {
    advanceClocks(10_ms);
    face.receive(*makeDataSegment("/hello/world/version0", i, false));
  }
  advanceClocks(10_ms);
  face.receive(*makeDataSegment("/hello/world/version0", 400, true));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 401);
}

BOOST_FIXTURE_TEST_CASE(MissingSegmentNum, Fixture)
{
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world", 1000_s),
                                                             acceptValidator);
  connectSignals(fetcher);

  advanceClocks(10_ms);

  const uint8_t buffer[] = "Hello, world!";
  auto data = makeData("/hello/world/version0/no-segment");
  data->setContent(buffer, sizeof(buffer));

  face.receive(*data);
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::DATA_HAS_NO_SEGMENT));
  BOOST_CHECK_EQUAL(nCompletions, 0);
}

BOOST_FIXTURE_TEST_CASE(DuplicateNack, Fixture)
{
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world", 1000_s),
                                                             acceptValidator);
  connectSignals(fetcher);
  advanceClocks(10_ms);

  // receive nack for the original interest
  nackLastInterest(lp::NackReason::DUPLICATE);

  // receive nack due to Duplication for the reexpressed interests
  for (uint32_t i = 1; i <= SegmentFetcher::MAX_INTEREST_REEXPRESS; ++i) {
    nackLastInterest(lp::NackReason::DUPLICATE);
  }

  BOOST_CHECK_EQUAL(face.sentInterests.size(), (SegmentFetcher::MAX_INTEREST_REEXPRESS + 1));
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::NACK_ERROR));
}

BOOST_FIXTURE_TEST_CASE(CongestionNack, Fixture)
{
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world", 1000_s),
                                                             acceptValidator);
  connectSignals(fetcher);
  advanceClocks(10_ms);

  // receive nack for the original interest
  nackLastInterest(lp::NackReason::CONGESTION);

  // receive nack due to Congestion for the reexpressed interests
  for (uint32_t i = 1; i <= SegmentFetcher::MAX_INTEREST_REEXPRESS; ++i) {
    nackLastInterest(lp::NackReason::CONGESTION);
  }

  BOOST_CHECK_EQUAL(face.sentInterests.size(), (SegmentFetcher::MAX_INTEREST_REEXPRESS + 1));
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::NACK_ERROR));
}

BOOST_FIXTURE_TEST_CASE(SegmentZero, Fixture)
{
  int nNacks = 2;

  ndn::Name interestName("ndn:/A");
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest(interestName),
                                                             acceptValidator);
  connectSignals(fetcher);

  advanceClocks(1000_ms);

  for (uint64_t segmentNo = 0; segmentNo <= 3; segmentNo++) {
    if (segmentNo == 1) {
      while (nNacks--) {
        nackLastInterest(lp::NackReason::CONGESTION);
        advanceClocks(10_ms);
      }
    }

    auto data = makeDataSegment(interestName, segmentNo, segmentNo == 3);
    face.receive(*data);
    advanceClocks(10_ms);
  }

  // Total number of sent interests should be 6: one interest for segment zero and segment one each,
  // two re-expressed interests for segment one after getting nack twice, and two interests for
  // segment two and three
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 6);

  BOOST_CHECK_EQUAL(face.sentInterests[0].getName(), ndn::Name("ndn:/A"));
  BOOST_CHECK_EQUAL(face.sentInterests[1].getName(), ndn::Name("ndn:/A/%00%01"));
  BOOST_CHECK_EQUAL(face.sentInterests[2].getName(), ndn::Name("ndn:/A/%00%01"));
  BOOST_CHECK_EQUAL(face.sentInterests[3].getName(), ndn::Name("ndn:/A/%00%01"));
  BOOST_CHECK_EQUAL(face.sentInterests[4].getName(), ndn::Name("ndn:/A/%00%02"));
  BOOST_CHECK_EQUAL(face.sentInterests[5].getName(), ndn::Name("ndn:/A/%00%03"));
}

BOOST_FIXTURE_TEST_CASE(SingleSegment, Fixture)
{
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("ndn:/", 1000_s),
                                                             acceptValidator);
  connectSignals(fetcher);
  advanceClocks(10_ms);
  nackLastInterest(lp::NackReason::DUPLICATE);
  face.receive(*makeDataSegment("/hello/world", 0, true));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 2);
  BOOST_CHECK_EQUAL(face.sentInterests[0].getName(), ndn::Name("ndn:/"));
  BOOST_CHECK_EQUAL(face.sentInterests[1].getName(), ndn::Name("ndn:/"));
  BOOST_REQUIRE_EQUAL(nCompletions, 1);
}

BOOST_FIXTURE_TEST_CASE(ValidationFailure, Fixture)
{
  DummyValidator validator;
  validator.getPolicy().setResultCallback([] (const Name& name) {
      return name.at(-1).toSegment() % 2 == 0;
    });
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world", 1000_s),
                                                             validator);
  connectSignals(fetcher);

  auto data1 = makeDataSegment("/hello/world", 0, false);
  auto data2 = makeDataSegment("/hello/world", 1, true);

  size_t nRecvSegments = 0;
  fetcher->afterSegmentReceived.connect([&nRecvSegments] (const Data& receivedSegment) {
      ++nRecvSegments;
    });

  size_t nValidatedSegments = 0;
  fetcher->afterSegmentValidated.connect([&nValidatedSegments] (const Data& validatedSegment) {
      ++nValidatedSegments;
    });

  advanceClocks(10_ms, 10);

  face.receive(*data1);

  advanceClocks(10_ms, 10);

  face.receive(*data2);

  advanceClocks(10_ms, 10);

  BOOST_CHECK_EQUAL(nRecvSegments, 2);
  BOOST_CHECK_EQUAL(nValidatedSegments, 1);
  BOOST_CHECK_EQUAL(nErrors, 1);
}

// Tests deprecated `fetch` API that uses callbacks instead of signals. This test case will be
// removed when this API is removed.
BOOST_FIXTURE_TEST_CASE(DeprecatedFetch, Fixture)
{
  DummyValidator acceptValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", 1000_s),
                        acceptValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(10_ms);

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);

  BOOST_CHECK_EQUAL(dataSize, 14);

  const uint8_t buffer[] = "Hello, world!";
  std::string bufferString(reinterpret_cast<const char*>(buffer));

  BOOST_CHECK_EQUAL(dataString, bufferString);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
}

// Tests deprecated `fetch` API that uses callbacks instead of signals (with an accepting shared_ptr
// Validator). This test case will be removed when this API is removed.
BOOST_FIXTURE_TEST_CASE(DeprecatedFetchSharedPtrComplete, Fixture)
{
  auto acceptValidator = make_shared<DummyValidator>(true);
  SegmentFetcher::fetch(face, Interest("/hello/world", 1000_s),
                        acceptValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  weak_ptr<DummyValidator> weakValidator = acceptValidator;
  BOOST_CHECK(!weakValidator.expired());
  acceptValidator.reset();
  BOOST_CHECK(!weakValidator.expired());

  advanceClocks(10_ms);

  BOOST_CHECK(!weakValidator.expired());
  face.receive(*makeDataSegment("/hello/world/version0", 0, true));

  advanceClocks(10_ms);

  BOOST_CHECK(weakValidator.expired());
  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);

  BOOST_CHECK_EQUAL(dataSize, 14);

  const uint8_t buffer[] = "Hello, world!";
  std::string bufferString(reinterpret_cast<const char*>(buffer));

  BOOST_CHECK_EQUAL(dataString, bufferString);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
}

// Tests deprecated `fetch` API that uses callbacks instead of signals (with a rejecting shared_ptr
// Validator). This test case will be removed when this API is removed.
BOOST_FIXTURE_TEST_CASE(DeprecatedFetchSharedPtrError, Fixture)
{
  auto acceptValidator = make_shared<DummyValidator>(false);
  SegmentFetcher::fetch(face, Interest("/hello/world", 1000_s),
                        acceptValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  weak_ptr<DummyValidator> weakValidator = acceptValidator;
  BOOST_CHECK(!weakValidator.expired());
  acceptValidator.reset();
  BOOST_CHECK(!weakValidator.expired());

  advanceClocks(10_ms);

  BOOST_CHECK(!weakValidator.expired());
  face.receive(*makeDataSegment("/hello/world/version0", 0, true));

  advanceClocks(10_ms);

  BOOST_CHECK(weakValidator.expired());
  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(nCompletions, 0);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
}

BOOST_AUTO_TEST_SUITE_END() // TestSegmentFetcher
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
