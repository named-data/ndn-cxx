/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "boost-test.hpp"
#include "../dummy-client-face.hpp"
#include "security/key-chain.hpp"

namespace ndn {
namespace util {
namespace tests {

BOOST_AUTO_TEST_SUITE(UtilSegmentFetcher)

class Fixture
{
public:
  Fixture()
    : face(::ndn::tests::makeDummyClientFace())
    , nErrors(0)
    , nDatas(0)
    , dataSize(0)
  {
  }

  shared_ptr<Data>
  makeData(const Name& baseName, uint64_t segment, bool isFinal)
  {
    const uint8_t buffer[] = "Hello, world!";

    shared_ptr<Data> data = make_shared<Data>(Name(baseName).appendSegment(segment));
    data->setContent(buffer, sizeof(buffer));

    if (isFinal)
      data->setFinalBlockId(data->getName()[-1]);
    keyChain.sign(*data);

    return data;
  }

  void
  onError(uint32_t errorCode)
  {
    ++nErrors;
    lastError = errorCode;
  }

  void
  onData(const ConstBufferPtr& data)
  {
    ++nDatas;
    dataSize = data->size();
  }


public:
  shared_ptr<ndn::tests::DummyClientFace> face;
  KeyChain keyChain;

  uint32_t nErrors;
  uint32_t lastError;
  uint32_t nDatas;
  size_t dataSize;
};

BOOST_FIXTURE_TEST_CASE(Timeout, Fixture)
{
  SegmentFetcher::fetch(*face, Interest("/hello/world", time::milliseconds(100)),
                        DontVerifySegment(),
                        bind(&Fixture::onData, this, _1),
                        bind(&Fixture::onError, this, _1));

  face->processEvents(time::seconds(1));

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::INTEREST_TIMEOUT));
  BOOST_CHECK_EQUAL(nDatas, 0);
  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face->m_sentDatas.size(), 0);

  const Interest& interest = face->m_sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
}


BOOST_FIXTURE_TEST_CASE(Basic, Fixture)
{

  SegmentFetcher::fetch(*face, Interest("/hello/world", time::seconds(1000)),
                        DontVerifySegment(),
                        bind(&Fixture::onData, this, _1),
                        bind(&Fixture::onError, this, _1));


  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 0, true));
  face->processEvents(time::milliseconds(-100));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nDatas, 1);

  BOOST_CHECK_EQUAL(dataSize, 14);

  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face->m_sentDatas.size(), 0);

  const Interest& interest = face->m_sentInterests[0];
  BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
  BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
  BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
}

BOOST_FIXTURE_TEST_CASE(NoSegmentInData, Fixture)
{

  SegmentFetcher::fetch(*face, Interest("/hello/world", time::seconds(1000)),
                        DontVerifySegment(),
                        bind(&Fixture::onData, this, _1),
                        bind(&Fixture::onError, this, _1));

  face->processEvents(time::milliseconds(-100));

  const uint8_t buffer[] = "Hello, world!";

  shared_ptr<Data> data = make_shared<Data>("/hello/world/version0/no-segment");
  data->setContent(buffer, sizeof(buffer));
  keyChain.sign(*data);

  face->receive(*data);
  face->processEvents(time::milliseconds(-100));

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::DATA_HAS_NO_SEGMENT));
  BOOST_CHECK_EQUAL(nDatas, 0);
}

bool
failValidation(const Data& data)
{
  return false;
}

BOOST_FIXTURE_TEST_CASE(SegmentValidationFailure, Fixture)
{

  SegmentFetcher::fetch(*face, Interest("/hello/world", time::seconds(1000)),
                        &failValidation,
                        bind(&Fixture::onData, this, _1),
                        bind(&Fixture::onError, this, _1));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 0, true));
  face->processEvents(time::milliseconds(-100));

  BOOST_CHECK_EQUAL(nErrors, 1);
  BOOST_CHECK_EQUAL(lastError, static_cast<uint32_t>(SegmentFetcher::SEGMENT_VERIFICATION_FAIL));
  BOOST_CHECK_EQUAL(nDatas, 0);
}


BOOST_FIXTURE_TEST_CASE(Triple, Fixture)
{
  KeyChain keyChain;

  SegmentFetcher::fetch(*face, Interest("/hello/world", time::seconds(1000)),
                        DontVerifySegment(),
                        bind(&Fixture::onData, this, _1),
                        bind(&Fixture::onError, this, _1));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 0, false));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 1, false));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 2, true));

  face->processEvents(time::milliseconds(-100));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nDatas, 1);

  BOOST_CHECK_EQUAL(dataSize, 42);

  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 3);
  BOOST_CHECK_EQUAL(face->m_sentDatas.size(), 0);

  {
    const Interest& interest = face->m_sentInterests[0];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
  }

  {
    const Interest& interest = face->m_sentInterests[1];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world/version0/%00%01");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }

  {
    const Interest& interest = face->m_sentInterests[2];
    BOOST_CHECK_EQUAL(interest.getName(),  "/hello/world/version0/%00%02");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }
}

BOOST_FIXTURE_TEST_CASE(TripleWithInitialSegmentFetching, Fixture)
{
  KeyChain keyChain;

  SegmentFetcher::fetch(*face, Interest("/hello/world", time::seconds(1000)),
                        DontVerifySegment(),
                        bind(&Fixture::onData, this, _1),
                        bind(&Fixture::onError, this, _1));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 1, false));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 0, false));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 1, false));

  face->processEvents(time::milliseconds(-100));
  face->receive(*makeData("/hello/world/version0", 2, true));

  face->processEvents(time::milliseconds(-100));

  BOOST_CHECK_EQUAL(nErrors, 0);
  BOOST_CHECK_EQUAL(nDatas, 1);

  BOOST_CHECK_EQUAL(dataSize, 42);

  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 4);
  BOOST_CHECK_EQUAL(face->m_sentDatas.size(), 0);

  {
    const Interest& interest = face->m_sentInterests[0];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), true);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 1);
  }

  {
    const Interest& interest = face->m_sentInterests[1];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world/version0/%00%00");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }

  {
    const Interest& interest = face->m_sentInterests[2];
    BOOST_CHECK_EQUAL(interest.getName(), "/hello/world/version0/%00%01");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }

  {
    const Interest& interest = face->m_sentInterests[3];
    BOOST_CHECK_EQUAL(interest.getName(),  "/hello/world/version0/%00%02");
    BOOST_CHECK_EQUAL(interest.getMustBeFresh(), false);
    BOOST_CHECK_EQUAL(interest.getChildSelector(), 0);
  }
}


BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace util
} // namespace ndn
