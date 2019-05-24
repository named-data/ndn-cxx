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

#include "ndn-cxx/util/segment-fetcher.hpp"

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/lp/nack.hpp"
#include "ndn-cxx/util/dummy-client-face.hpp"

#include "tests/boost-test.hpp"
#include "tests/make-interest-data.hpp"
#include "tests/unit/dummy-validator.hpp"
#include "tests/unit/identity-management-time-fixture.hpp"

#include <set>

namespace ndn {
namespace util {
namespace tests {

using namespace ndn::tests;

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
    data->setFreshnessPeriod(1_s);
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
  onComplete(ConstBufferPtr data)
  {
    ++nCompletions;
    dataSize = data->size();
    dataBuf = data;
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
  connectSignals(const shared_ptr<SegmentFetcher>& fetcher)
  {
    fetcher->onComplete.connect(bind(&Fixture::onComplete, this, _1));
    fetcher->onError.connect(bind(&Fixture::onError, this, _1));
  }

  void
  onInterest(const Interest& interest)
  {
    if (interest.getName().get(-1).isSegment()) {
      if (segmentsToDropOrNack.size() > 0 &&
          interest.getName().get(-1).toSegment() == segmentsToDropOrNack.front()) {
        segmentsToDropOrNack.pop();
        if (sendNackInsteadOfDropping) {
          lp::Nack nack = makeNack(interest, nackReason);
          face.receive(nack);
        }
        return;
      }

      auto data = makeDataSegment("/hello/world/version0",
                                  interest.getName().get(-1).toSegment(),
                                  interest.getName().get(-1).toSegment() == nSegments - 1);
      face.receive(*data);

      uniqSegmentsSent.insert(interest.getName().get(-1).toSegment());
      if (uniqSegmentsSent.size() == nSegments) {
        io.stop();
      }
    }
    else {
      if (segmentsToDropOrNack.size() > 0 &&
          segmentsToDropOrNack.front() == 0) {
        segmentsToDropOrNack.pop();
        if (sendNackInsteadOfDropping) {
          lp::Nack nack = makeNack(interest, nackReason);
          face.receive(nack);
        }
        return;
      }

      auto data = makeDataSegment("/hello/world/version0", defaultSegmentToSend, nSegments == 1);
      face.receive(*data);
      uniqSegmentsSent.insert(defaultSegmentToSend);
    }
  }

public:
  DummyClientFace face;
  std::set<uint64_t> uniqSegmentsSent;

  int nErrors = 0;
  uint32_t lastError = 0;
  int nCompletions = 0;
  size_t dataSize = 0;
  ConstBufferPtr dataBuf;

  // number of segments in fetched object
  uint64_t nSegments = 0;
  std::queue<uint64_t> segmentsToDropOrNack;
  bool sendNackInsteadOfDropping = false;
  lp::NackReason nackReason = lp::NackReason::NONE;
  // segment that is sent in response to an Interest w/o a segment component in its name
  uint64_t defaultSegmentToSend = 0;
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestSegmentFetcher, Fixture)

BOOST_AUTO_TEST_CASE(InvalidOptions)
{
  SegmentFetcher::Options options;
  options.mdCoef = 1.5;
  DummyValidator acceptValidator;
  BOOST_CHECK_THROW(SegmentFetcher::start(face, Interest("/hello/world"), acceptValidator, options),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ExceedMaxTimeout)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentTimedOut = 0;
  SegmentFetcher::Options options;
  options.maxTimeout = 100_ms;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator, options);
  connectSignals(fetcher);
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  advanceClocks(1_ms);
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getCanBePrefix(), true);

  advanceClocks(98_ms);
  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);

  advanceClocks(1_ms, 2);
  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::INTEREST_TIMEOUT));
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 1);
}

BOOST_AUTO_TEST_CASE(BasicSingleSegment)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  advanceClocks(10_ms);

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
}

BOOST_AUTO_TEST_CASE(ConstantCwnd)
{
  SegmentFetcher::Options options;
  options.useConstantCwnd = true;
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator, options);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);

  face.receive(*makeDataSegment("/hello/world/version0", 0, false));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 2);
  BOOST_CHECK_EQUAL(face.sentInterests.back().getName().get(-1).toSegment(), 1);
  face.receive(*makeDataSegment("/hello/world/version0", 1, false));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 3);
  BOOST_CHECK_EQUAL(face.sentInterests.back().getName().get(-1).toSegment(), 2);
  face.receive(*makeDataSegment("/hello/world/version0", 2, false));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 4);
  BOOST_CHECK_EQUAL(face.sentInterests.back().getName().get(-1).toSegment(), 3);
  face.receive(*makeDataSegment("/hello/world/version0", 3, false));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 5);
  BOOST_CHECK_EQUAL(face.sentInterests.back().getName().get(-1).toSegment(), 4);
  nackLastInterest(lp::NackReason::CONGESTION);

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 6);
  BOOST_CHECK_EQUAL(face.sentInterests.back().getName().get(-1).toSegment(), 4);
  face.receive(*makeDataSegment("/hello/world/version0", 4, true));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 5);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 5);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
}

BOOST_AUTO_TEST_CASE(BasicMultipleSegments)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  nSegments = 401;
  sendNackInsteadOfDropping = false;
  face.onSendInterest.connect(bind(&Fixture::onInterest, this, _1));

  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  face.processEvents(1_s);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(dataSize, 14 * 401);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
}

BOOST_AUTO_TEST_CASE(FirstSegmentNotZero)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  nSegments = 401;
  sendNackInsteadOfDropping = false;
  defaultSegmentToSend = 47;
  face.onSendInterest.connect(bind(&Fixture::onInterest, this, _1));

  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  face.processEvents(1_s);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(dataSize, 14 * 401);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
}

BOOST_AUTO_TEST_CASE(WindowSize)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));

  advanceClocks(10_ms); // T+10ms

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 10_ms);
  BOOST_CHECK_EQUAL(fetcher->m_retxQueue.size(), 0);
  BOOST_CHECK_EQUAL(fetcher->m_nextSegmentNum, 0);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, 1.0);
  BOOST_CHECK_EQUAL(fetcher->m_ssthresh, std::numeric_limits<double>::max());
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, 1);
  BOOST_CHECK_EQUAL(fetcher->m_nSegments, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nBytesReceived, 0);
  BOOST_CHECK_EQUAL(fetcher->m_highInterest, 0);
  BOOST_CHECK_EQUAL(fetcher->m_highData, 0);
  BOOST_CHECK_EQUAL(fetcher->m_recPoint, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nReceived, 0);
  BOOST_CHECK_EQUAL(fetcher->m_receivedSegments.size(), 0);
  BOOST_CHECK_EQUAL(fetcher->m_pendingSegments.size(), 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);

  double oldCwnd = fetcher->m_cwnd;
  double oldSsthresh = fetcher->m_ssthresh;
  uint64_t oldNextSegmentNum = fetcher->m_nextSegmentNum;

  face.receive(*makeDataSegment("/hello/world/version0", 0, false));

  advanceClocks(10_ms); //T+20ms

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 10_ms);
  BOOST_CHECK_EQUAL(fetcher->m_retxQueue.size(), 0);
  BOOST_CHECK_EQUAL(fetcher->m_versionedDataName, "/hello/world/version0");
  // +2 below because m_nextSegmentNum will be incremented in the receive callback if segment 0 is
  // the first received
  BOOST_CHECK_EQUAL(fetcher->m_nextSegmentNum, oldNextSegmentNum + fetcher->m_options.aiStep + 2);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, oldCwnd + fetcher->m_options.aiStep);
  BOOST_CHECK_EQUAL(fetcher->m_ssthresh, oldSsthresh);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, oldCwnd + fetcher->m_options.aiStep);
  BOOST_CHECK_EQUAL(fetcher->m_nSegments, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nBytesReceived, 14);
  BOOST_CHECK_EQUAL(fetcher->m_highInterest, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_highData, 0);
  BOOST_CHECK_EQUAL(fetcher->m_recPoint, 0);
  BOOST_CHECK_EQUAL(fetcher->m_receivedSegments.size(), 1);
  BOOST_CHECK_EQUAL(fetcher->m_pendingSegments.size(), fetcher->m_cwnd);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1 + fetcher->m_cwnd);

  oldCwnd = fetcher->m_cwnd;
  oldNextSegmentNum = fetcher->m_nextSegmentNum;

  face.receive(*makeDataSegment("/hello/world/version0", 2, false));

  advanceClocks(10_ms); //T+30ms

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 10_ms);
  BOOST_CHECK_EQUAL(fetcher->m_retxQueue.size(), 0);
  BOOST_CHECK_EQUAL(fetcher->m_versionedDataName, "/hello/world/version0");
  BOOST_CHECK_EQUAL(fetcher->m_nextSegmentNum, oldNextSegmentNum + fetcher->m_options.aiStep + 1);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, oldCwnd + fetcher->m_options.aiStep);
  BOOST_CHECK_EQUAL(fetcher->m_ssthresh, oldSsthresh);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, fetcher->m_cwnd);
  BOOST_CHECK_EQUAL(fetcher->m_nSegments, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nBytesReceived, 28);
  BOOST_CHECK_EQUAL(fetcher->m_highInterest, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_highData, 2);
  BOOST_CHECK_EQUAL(fetcher->m_recPoint, 0);
  BOOST_CHECK_EQUAL(fetcher->m_receivedSegments.size(), 2);
  BOOST_CHECK_EQUAL(fetcher->m_pendingSegments.size(), fetcher->m_cwnd);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 2 + fetcher->m_cwnd);

  oldCwnd = fetcher->m_cwnd;
  oldNextSegmentNum = fetcher->m_nextSegmentNum;

  face.receive(*makeDataSegment("/hello/world/version0", 1, false));

  advanceClocks(10_ms); //T+40ms

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 10_ms);
  BOOST_CHECK_EQUAL(fetcher->m_retxQueue.size(), 0);
  BOOST_CHECK_EQUAL(fetcher->m_versionedDataName, "/hello/world/version0");
  BOOST_CHECK_EQUAL(fetcher->m_nextSegmentNum, oldNextSegmentNum + fetcher->m_options.aiStep + 1);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, oldCwnd + fetcher->m_options.aiStep);
  BOOST_CHECK_EQUAL(fetcher->m_ssthresh, oldSsthresh);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, fetcher->m_cwnd);
  BOOST_CHECK_EQUAL(fetcher->m_nSegments, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nBytesReceived, 42);
  BOOST_CHECK_EQUAL(fetcher->m_highInterest, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_highData, 2);
  BOOST_CHECK_EQUAL(fetcher->m_recPoint, 0);
  BOOST_CHECK_EQUAL(fetcher->m_receivedSegments.size(), 3);
  BOOST_CHECK_EQUAL(fetcher->m_pendingSegments.size(), fetcher->m_cwnd);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 3 + fetcher->m_cwnd);

  oldCwnd = fetcher->m_cwnd;
  oldSsthresh = fetcher->m_ssthresh;
  oldNextSegmentNum = fetcher->m_nextSegmentNum;
  size_t oldSentInterestsSize = face.sentInterests.size();

  nackLastInterest(lp::NackReason::CONGESTION); //T+50ms

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 20_ms);
  BOOST_CHECK_EQUAL(fetcher->m_retxQueue.size(), 1);
  BOOST_CHECK_EQUAL(fetcher->m_versionedDataName, "/hello/world/version0");
  BOOST_CHECK_EQUAL(fetcher->m_nextSegmentNum, oldNextSegmentNum);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, oldCwnd / 2.0);
  BOOST_CHECK_EQUAL(fetcher->m_ssthresh, oldCwnd / 2.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, oldCwnd - 1);
  BOOST_CHECK_EQUAL(fetcher->m_nSegments, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nBytesReceived, 42);
  BOOST_CHECK_EQUAL(fetcher->m_highInterest, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_highData, 2);
  BOOST_CHECK_EQUAL(fetcher->m_recPoint, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_receivedSegments.size(), 3);
  // The Nacked segment will remain in pendingSegments, so the size of the structure doesn't change
  BOOST_CHECK_EQUAL(fetcher->m_pendingSegments.size(), oldCwnd);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), oldSentInterestsSize);

  advanceClocks(10_ms); //T+60ms

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 30_ms);
  BOOST_CHECK_EQUAL(fetcher->m_retxQueue.size(), 1);
  BOOST_CHECK_EQUAL(fetcher->m_versionedDataName, "/hello/world/version0");
  BOOST_CHECK_EQUAL(fetcher->m_nextSegmentNum, oldNextSegmentNum);
  BOOST_CHECK_EQUAL(fetcher->m_cwnd, oldCwnd / 2.0);
  BOOST_CHECK_EQUAL(fetcher->m_ssthresh, oldCwnd / 2.0);
  BOOST_CHECK_EQUAL(fetcher->m_nSegmentsInFlight, oldCwnd - 1);
  BOOST_CHECK_EQUAL(fetcher->m_nSegments, 0);
  BOOST_CHECK_EQUAL(fetcher->m_nBytesReceived, 42);
  BOOST_CHECK_EQUAL(fetcher->m_highInterest, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_highData, 2);
  BOOST_CHECK_EQUAL(fetcher->m_recPoint, fetcher->m_nextSegmentNum - 1);
  BOOST_CHECK_EQUAL(fetcher->m_receivedSegments.size(), 3);
  BOOST_CHECK_EQUAL(fetcher->m_pendingSegments.size(), oldCwnd);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), oldSentInterestsSize);

  // Properly end test case
  lp::Nack nack = makeNack(face.sentInterests[face.sentInterests.size() - 2], lp::NackReason::NO_ROUTE);
  face.receive(nack);
  advanceClocks(10_ms); //T+70ms

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::NACK_ERROR));
  BOOST_CHECK_EQUAL(nCompletions, 0);
}

BOOST_AUTO_TEST_CASE(MissingSegmentNum)
{
  DummyValidator acceptValidator;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);

  advanceClocks(10_ms);

  const uint8_t buffer[] = "Hello, world!";
  auto data = makeData("/hello/world/version0/no-segment");
  data->setFreshnessPeriod(1_s);
  data->setContent(buffer, sizeof(buffer));

  face.receive(*data);
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::DATA_HAS_NO_SEGMENT));
  BOOST_CHECK_EQUAL(nCompletions, 0);
}

BOOST_AUTO_TEST_CASE(MoreSegmentsThanNSegments)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  advanceClocks(10_ms);

  face.receive(*makeDataSegment("/hello/world/version0", 0, false));
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);

  face.receive(*makeDataSegment("/hello/world/version0", 1, false));
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);

  face.receive(*makeDataSegment("/hello/world/version0", 2, false));
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);

  face.receive(*makeDataSegment("/hello/world/version0", 3, false));
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);

  auto data4 = makeDataSegment("/hello/world/version0", 4, false);
  data4->setFinalBlock(name::Component::fromSegment(2));
  face.receive(*data4);
  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(dataSize, 14 * 3);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 5);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 5);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
}

BOOST_AUTO_TEST_CASE(DuplicateNack)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  nSegments = 401;
  segmentsToDropOrNack.push(0);
  segmentsToDropOrNack.push(200);
  sendNackInsteadOfDropping = true;
  nackReason = lp::NackReason::DUPLICATE;
  face.onSendInterest.connect(bind(&Fixture::onInterest, this, _1));

  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  face.processEvents(1_s);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(dataSize, 14 * 401);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 2);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
}

BOOST_AUTO_TEST_CASE(CongestionNack)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  nSegments = 401;
  segmentsToDropOrNack.push(0);
  segmentsToDropOrNack.push(200);
  sendNackInsteadOfDropping = true;
  nackReason = lp::NackReason::CONGESTION;
  face.onSendInterest.connect(bind(&Fixture::onInterest, this, _1));

  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  face.processEvents(1_s);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(dataSize, 14 * 401);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 401);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 2);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
}

BOOST_AUTO_TEST_CASE(OtherNackReason)
{
  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;
  segmentsToDropOrNack.push(0);
  sendNackInsteadOfDropping = true;
  nackReason = lp::NackReason::NO_ROUTE;
  face.onSendInterest.connect(bind(&Fixture::onInterest, this, _1));

  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
                                                             acceptValidator);
  connectSignals(fetcher);
  fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
  fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
  fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
  fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));

  face.processEvents(1_s);

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(nCompletions, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(ValidationFailure)
{
  DummyValidator validator;
  validator.getPolicy().setResultCallback([] (const Name& name) {
      return name.at(-1).toSegment() % 2 == 0;
    });
  shared_ptr<SegmentFetcher> fetcher = SegmentFetcher::start(face, Interest("/hello/world"),
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

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 100_ms);

  face.receive(*data1);

  advanceClocks(10_ms, 10);

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 100_ms);

  face.receive(*data2);

  advanceClocks(10_ms, 10);

  BOOST_CHECK_EQUAL(fetcher->m_timeLastSegmentReceived, time::steady_clock::now() - 200_ms);
  BOOST_CHECK_EQUAL(nRecvSegments, 2);
  BOOST_CHECK_EQUAL(nValidatedSegments, 1);
  BOOST_CHECK_EQUAL(nErrors, 1);
}

BOOST_AUTO_TEST_CASE(Stop)
{
  DummyValidator acceptValidator;

  auto fetcher = SegmentFetcher::start(face, Interest("/hello/world"), acceptValidator);
  connectSignals(fetcher);
  BOOST_CHECK_EQUAL(fetcher.use_count(), 2);

  fetcher->stop();
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(fetcher.use_count(), 1);

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));
  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 0);

  fetcher.reset();
  BOOST_CHECK_EQUAL(fetcher.use_count(), 0);

  // Make sure we can re-assign w/o any complains from ASan
  fetcher = SegmentFetcher::start(face, Interest("/hello/world"), acceptValidator);
  connectSignals(fetcher);
  BOOST_CHECK_EQUAL(fetcher.use_count(), 2);

  advanceClocks(10_ms);

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));

  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(fetcher.use_count(), 1);

  // Stop from callback
  bool fetcherStopped = false;

  fetcher = SegmentFetcher::start(face, Interest("/hello/world"), acceptValidator);
  fetcher->afterSegmentReceived.connect([&fetcher, &fetcherStopped] (const Data& data) {
                                          fetcherStopped = true;
                                          fetcher->stop();
                                        });
  BOOST_CHECK_EQUAL(fetcher.use_count(), 2);

  advanceClocks(10_ms);

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));

  advanceClocks(10_ms);
  BOOST_CHECK(fetcherStopped);
  BOOST_CHECK_EQUAL(fetcher.use_count(), 1);
}

BOOST_AUTO_TEST_CASE(Lifetime)
{
  // BasicSingleSegment, but with scoped fetcher

  DummyValidator acceptValidator;
  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;

  weak_ptr<SegmentFetcher> weakFetcher;
  {
    auto fetcher = SegmentFetcher::start(face, Interest("/hello/world"), acceptValidator);
    weakFetcher = fetcher;
    connectSignals(fetcher);

    fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
    fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
    fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
    fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));
  }

  advanceClocks(10_ms);
  BOOST_CHECK_EQUAL(weakFetcher.expired(), false);

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));

  advanceClocks(10_ms);

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 1);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 0);
  BOOST_CHECK_EQUAL(weakFetcher.expired(), true);
}

BOOST_AUTO_TEST_CASE(OutOfScopeTimeout)
{
  DummyValidator acceptValidator;
  SegmentFetcher::Options options;
  options.maxTimeout = 3000_ms;

  size_t nAfterSegmentReceived = 0;
  size_t nAfterSegmentValidated = 0;
  size_t nAfterSegmentNacked = 0;
  size_t nAfterSegmentTimedOut = 0;

  weak_ptr<SegmentFetcher> weakFetcher;
  {
    auto fetcher = SegmentFetcher::start(face, Interest("/localhost/nfd/faces/list"),
                                         acceptValidator, options);
    weakFetcher = fetcher;
    connectSignals(fetcher);
    fetcher->afterSegmentReceived.connect(bind([&nAfterSegmentReceived] { ++nAfterSegmentReceived; }));
    fetcher->afterSegmentValidated.connect(bind([&nAfterSegmentValidated] { ++nAfterSegmentValidated; }));
    fetcher->afterSegmentNacked.connect(bind([&nAfterSegmentNacked] { ++nAfterSegmentNacked; }));
    fetcher->afterSegmentTimedOut.connect(bind([&nAfterSegmentTimedOut] { ++nAfterSegmentTimedOut; }));
  }

  advanceClocks(500_ms, 7);
  BOOST_CHECK_EQUAL(weakFetcher.expired(), true);

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(nCompletions, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentReceived, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentValidated, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentNacked, 0);
  BOOST_CHECK_EQUAL(nAfterSegmentTimedOut, 2);
}

BOOST_AUTO_TEST_CASE(UncanceledPendingInterestBug) // Bug #4770
{
  DummyValidator acceptValidator;
  auto fetcher = SegmentFetcher::start(face, Interest("/hello/world"), acceptValidator);
  connectSignals(fetcher);

  // Fetcher will send the first interest immediately
  // and the second interest after 1 second by default
  advanceClocks(1100_ms);

  // Face will give data to the fetcher twice if the first interest is not canceled.
  // isFinal=false to keep fetcher alive so that it can receive the second data.
  face.receive(*makeDataSegment("/hello/world/version0", 0, false));

  advanceClocks(1100_ms);

  face.receive(*makeDataSegment("/hello/world/version0", 1, true));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nCompletions, 1);
}

BOOST_AUTO_TEST_SUITE_END() // TestSegmentFetcher
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
