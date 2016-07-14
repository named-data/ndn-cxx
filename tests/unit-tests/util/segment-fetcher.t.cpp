/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
#include "security/validator-null.hpp"
#include "../../dummy-validator.hpp"
#include "data.hpp"
#include "encoding/block.hpp"

#include "boost-test.hpp"
#include "util/dummy-client-face.hpp"
#include "security/key-chain.hpp"
#include "lp/nack-header.hpp"
#include "../identity-management-time-fixture.hpp"
#include "../make-interest-data.hpp"

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
    , nErrors(0)
    , nData(0)
    , dataSize(0)
  {
  }

  shared_ptr<Data>
  makeDataSegment(const Name& baseName, uint64_t segment, bool isFinal)
  {
    const uint8_t buffer[] = "Hello, world!";

    auto data = make_shared<Data>(Name(baseName).appendSegment(segment));
    data->setContent(buffer, sizeof(buffer));

    if (isFinal) {
      data->setFinalBlockId(data->getName()[-1]);
    }
    data = signData(data);

    return data;
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
    ++nData;
    dataSize = data->size();
    dataString = std::string(reinterpret_cast<const char*>(data->get()));
  }

  void
  nackLastInterest(lp::NackReason nackReason)
  {
    const Interest& lastInterest = face.sentInterests.back();
    lp::Nack nack = makeNack(lastInterest, nackReason);
    face.receive(nack);
    advanceClocks(time::milliseconds(10));
  }

public:
  DummyClientFace face;

  uint32_t nErrors;
  uint32_t lastError;
  uint32_t nData;
  size_t dataSize;
  std::string dataString;
};

BOOST_FIXTURE_TEST_CASE(Timeout, Fixture)
{
  ValidatorNull nullValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::milliseconds(100)),
                        nullValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(1));
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);

  advanceClocks(time::milliseconds(98));
  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nData, 0);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  advanceClocks(time::milliseconds(1), 2);
  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::INTEREST_TIMEOUT));
  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
}


BOOST_FIXTURE_TEST_CASE(Basic, Fixture)
{
  ValidatorNull nullValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        nullValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(10));

  face.receive(*makeDataSegment("/hello/world/version0", 0, true));
  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nData, 1);

  BOOST_CHECK_EQUAL(dataSize, 14);

  const uint8_t buffer[] = "Hello, world!";
  std::string bufferString = std::string(reinterpret_cast<const char*>(buffer));

  BOOST_CHECK_EQUAL(dataString, bufferString);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  const Interest& interest = face.sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
}

BOOST_FIXTURE_TEST_CASE(NoSegmentInData, Fixture)
{
  ValidatorNull nullValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        nullValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(10));

  const uint8_t buffer[] = "Hello, world!";

  shared_ptr<Data> data = makeData("/hello/world/version0/no-segment");

  data->setContent(buffer, sizeof(buffer));

  face.receive(*data);
  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::DATA_HAS_NO_SEGMENT));
  BOOST_CHECK_EQUAL(nData, 0);
}

BOOST_FIXTURE_TEST_CASE(SegmentValidationFailure, Fixture)
{
  DummyRejectValidator rejectValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        rejectValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 0, true));
  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::SEGMENT_VALIDATION_FAIL));
  BOOST_CHECK_EQUAL(nData, 0);
}

BOOST_FIXTURE_TEST_CASE(Triple, Fixture)
{
  ValidatorNull nullValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        nullValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 0, false));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 1, false));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 2, true));

  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nData, 1);

  BOOST_CHECK_EQUAL(dataSize, 42);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 3);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  {
    const Interest& interest = face.sentInterests[0];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
  }

  {
    const Interest& interest = face.sentInterests[1];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world/version0/%00%01");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }

  {
    const Interest& interest = face.sentInterests[2];
    BOOST_CHECK_EQUAL(interest.getName(),  "/hello/world/version0/%00%02");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }
}

BOOST_FIXTURE_TEST_CASE(TripleWithInitialSegmentFetching, Fixture)
{
  ValidatorNull nullValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        nullValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 1, false));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 0, false));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 1, false));

  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 2, true));

  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nData, 1);

  BOOST_CHECK_EQUAL(dataSize, 42);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 4);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  {
    const Interest& interest = face.sentInterests[0];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
  }

  {
    const Interest& interest = face.sentInterests[1];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world/version0/%00%00");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }

  {
    const Interest& interest = face.sentInterests[2];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world/version0/%00%01");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }

  {
    const Interest& interest = face.sentInterests[3];
    BOOST_CHECK_EQUAL(interest.getName(),  "/hello/world/version0/%00%02");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }
}

BOOST_FIXTURE_TEST_CASE(MultipleSegmentFetching, Fixture)
{
  ValidatorNull nullValidator;
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        nullValidator,
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(10));

  for (uint64_t i = 0; i < 400; i++) {
    advanceClocks(time::milliseconds(10));
    face.receive(*makeDataSegment("/hello/world/version0", i, false));
  }
  advanceClocks(time::milliseconds(10));
  face.receive(*makeDataSegment("/hello/world/version0", 400, true));

  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nData, 1);
}

BOOST_FIXTURE_TEST_CASE(DuplicateNack, Fixture)
{
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        make_shared<ValidatorNull>(),
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));
  advanceClocks(time::milliseconds(10));

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
  SegmentFetcher::fetch(face, Interest("/hello/world", time::seconds(1000)),
                        make_shared<ValidatorNull>(),
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));
  advanceClocks(time::milliseconds(10));

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
  SegmentFetcher::fetch(face,
                        Interest(interestName),
                        make_shared<ValidatorNull>(),
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));

  advanceClocks(time::milliseconds(1000));

  for (uint64_t segmentNo = 0; segmentNo <= 3; segmentNo++) {
    if (segmentNo == 1) {
      while (nNacks--) {
        nackLastInterest(lp::NackReason::CONGESTION);
        advanceClocks(time::milliseconds(10));
      }
    }

    auto data = makeDataSegment(interestName, segmentNo, segmentNo == 3);
    face.receive(*data);
    advanceClocks(time::milliseconds(10));
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

BOOST_FIXTURE_TEST_CASE(ZeroComponentName, Fixture)
{
  SegmentFetcher::fetch(face, Interest("ndn:/"),
                        make_shared<ValidatorNull>(),
                        bind(&Fixture::onComplete, this, _1),
                        bind(&Fixture::onError, this, _1));
  advanceClocks(time::milliseconds(10));
  nackLastInterest(lp::NackReason::DUPLICATE);
  face.receive(*makeDataSegment("/hello/world", 0, true));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 2);
  BOOST_CHECK_EQUAL(face.sentInterests[0].getName(), ndn::Name("ndn:/"));
  BOOST_CHECK_EQUAL(face.sentInterests[1].getName(), ndn::Name("ndn:/"));
  BOOST_REQUIRE_EQUAL(nData, 1);
}

BOOST_AUTO_TEST_SUITE_END() // TestSegmentFetcher
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
