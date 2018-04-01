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

#include "mgmt/nfd/status-dataset.hpp"
#include "mgmt/nfd/controller.hpp"

#include "controller-fixture.hpp"
#include "make-interest-data.hpp"

namespace ndn {
namespace nfd {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)

class ControllerStatusDatasetFixture : public ControllerFixture
{
protected:
  /** \brief send one WireEncodable as Data reply
   *  \param prefix dataset prefix without version and segment
   *  \param payload payload block
   *  \note payload must fit in one Data
   */
  template<typename T>
  void
  sendDataset(const Name& prefix, const T& payload)
  {
    BOOST_CONCEPT_ASSERT((WireEncodable<T>));

    auto data = this->prepareDatasetReply(prefix);
    data->setContent(payload.wireEncode());
    face.receive(*signData(data));
  }

  /** \brief send two WireEncodables as Data reply
   *  \param prefix dataset prefix without version and segment
   *  \param payload1 first vector item
   *  \param payload2 second vector item
   *  \note all payloads must fit in one Data
   */
  template<typename T1, typename T2>
  void
  sendDataset(const Name& prefix, const T1& payload1, const T2& payload2)
  {
    // The test suite allows up to two items, and put them in the same Data packet,
    // because this test suite focuses on Controller::fetch<StatusDataset>,
    // and is not intended to cover SegmentFetcher behavior.

    BOOST_CONCEPT_ASSERT((WireEncodable<T1>));
    BOOST_CONCEPT_ASSERT((WireEncodable<T2>));

    ndn::encoding::EncodingBuffer buffer;
    payload2.wireEncode(buffer);
    payload1.wireEncode(buffer);

    auto data = this->prepareDatasetReply(prefix);
    data->setContent(buffer.buf(), buffer.size());
    face.receive(*signData(data));
  }

private:
  shared_ptr<Data>
  prepareDatasetReply(const Name& prefix)
  {
    Name name = prefix;
    name.appendVersion().appendSegment(0);

    // These warnings assist in debugging a `hasResult` check failure.
    // They usually indicate a misspelled prefix or incorrect timing in the test case.
    if (face.sentInterests.size() < 1) {
      BOOST_WARN_MESSAGE(false, "no Interest expressed");
    }
    else {
      BOOST_WARN_MESSAGE(face.sentInterests.back().getName().isPrefixOf(name),
                         "last Interest " << face.sentInterests.back().getName() <<
                         " cannot be satisfied by this Data " << name);
    }

    auto data = make_shared<Data>(name);
    data->setFinalBlock(name[-1]);
    return data;
  }
};

BOOST_FIXTURE_TEST_SUITE(TestStatusDataset, ControllerStatusDatasetFixture)

BOOST_AUTO_TEST_SUITE(Failures)

BOOST_AUTO_TEST_CASE(Timeout)
{
  CommandOptions options;
  options.setTimeout(3000_ms);
  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    datasetFailCallback,
    options);
  this->advanceClocks(500_ms, 7);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_TIMEOUT);
}

BOOST_AUTO_TEST_CASE(DataHasNoSegment)
{
  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  face.receive(*makeData("/localhost/nfd/faces/list/%FD%00"));
  this->advanceClocks(500_ms);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_SERVER);
}

BOOST_AUTO_TEST_CASE(ValidationFailure)
{
  this->setValidationResult(false);

  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  FaceStatus payload;
  payload.setFaceId(5744);
  this->sendDataset("/localhost/nfd/faces/list", payload);
  this->advanceClocks(500_ms);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_VALIDATION);
}

BOOST_AUTO_TEST_CASE(Nack)
{
  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  face.receive(lp::Nack(face.sentInterests.back()));
  this->advanceClocks(500_ms);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_NACK);
}

BOOST_AUTO_TEST_CASE(ParseError1)
{
  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  Name payload; // Name is not valid FaceStatus
  this->sendDataset("/localhost/nfd/faces/list", payload);
  this->advanceClocks(500_ms);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_SERVER);
}

BOOST_AUTO_TEST_CASE(ParseError2)
{
  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  FaceStatus payload1;
  payload1.setFaceId(10930);
  Name payload2; // Name is not valid FaceStatus
  this->sendDataset("/localhost/nfd/faces/list", payload1, payload2);
  this->advanceClocks(500_ms);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_SERVER);
}

BOOST_AUTO_TEST_SUITE_END() // Failures

BOOST_AUTO_TEST_SUITE(NoCallback)

BOOST_AUTO_TEST_CASE(Success)
{
  controller.fetch<FaceDataset>(
    nullptr,
    datasetFailCallback);
  this->advanceClocks(500_ms);

  FaceStatus payload;
  payload.setFaceId(2577);
  this->sendDataset("/localhost/nfd/faces/list", payload);
  BOOST_CHECK_NO_THROW(this->advanceClocks(500_ms));

  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(Failure)
{
  CommandOptions options;
  options.setTimeout(3000_ms);
  controller.fetch<FaceDataset>(
    [] (const std::vector<FaceStatus>& result) { BOOST_FAIL("fetchDataset should not succeed"); },
    nullptr,
    options);
  BOOST_CHECK_NO_THROW(this->advanceClocks(500_ms, 7));
}

BOOST_AUTO_TEST_SUITE_END() // NoCallback

BOOST_AUTO_TEST_SUITE(Datasets)

BOOST_AUTO_TEST_CASE(StatusGeneral)
{
  bool hasResult = false;
  controller.fetch<ForwarderGeneralStatusDataset>(
    [&hasResult] (const ForwarderStatus& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.getNfdVersion(), "0.4.2");
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  ForwarderStatus payload;
  payload.setNfdVersion("0.4.2");
  this->sendDataset("/localhost/nfd/status/general", payload);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(FaceList)
{
  bool hasResult = false;
  controller.fetch<FaceDataset>(
    [&hasResult] (const std::vector<FaceStatus>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 2);
      BOOST_CHECK_EQUAL(result.front().getFaceId(), 24485);
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  FaceStatus payload1;
  payload1.setFaceId(24485);
  FaceStatus payload2;
  payload2.setFaceId(12987);
  this->sendDataset("/localhost/nfd/faces/list", payload1, payload2);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(FaceQuery)
{
  FaceQueryFilter filter;
  filter.setUriScheme("udp4");
  bool hasResult = false;
  controller.fetch<FaceQueryDataset>(
    filter,
    [&hasResult] (const std::vector<FaceStatus>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 1);
      BOOST_CHECK_EQUAL(result.front().getFaceId(), 8795);
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  Name prefix("/localhost/nfd/faces/query");
  prefix.append(filter.wireEncode());
  FaceStatus payload;
  payload.setFaceId(8795);
  this->sendDataset(prefix, payload);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(FaceQueryWithOptions)
{
  FaceQueryFilter filter;
  filter.setUriScheme("udp4");
  CommandOptions options;
  options.setTimeout(3000_ms);
  bool hasResult = false;
  controller.fetch<FaceQueryDataset>(
    filter,
    [&hasResult] (const std::vector<FaceStatus>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 1);
      BOOST_CHECK_EQUAL(result.front().getFaceId(), 14022);
    },
    datasetFailCallback,
    options);
  this->advanceClocks(500_ms);

  Name prefix("/localhost/nfd/faces/query");
  prefix.append(filter.wireEncode());
  FaceStatus payload;
  payload.setFaceId(14022);
  this->sendDataset(prefix, payload);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(FaceChannels)
{
  bool hasResult = false;
  controller.fetch<ChannelDataset>(
    [&hasResult] (const std::vector<ChannelStatus>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 2);
      BOOST_CHECK_EQUAL(result.front().getLocalUri(), "tcp4://192.0.2.1:6363");
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  ChannelStatus payload1;
  payload1.setLocalUri("tcp4://192.0.2.1:6363");
  ChannelStatus payload2;
  payload2.setLocalUri("udp4://192.0.2.1:6363");
  this->sendDataset("/localhost/nfd/faces/channels", payload1, payload2);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(FibList)
{
  bool hasResult = false;
  controller.fetch<FibDataset>(
    [&hasResult] (const std::vector<FibEntry>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 2);
      BOOST_CHECK_EQUAL(result.front().getPrefix(), "/wYs7fzYcfG");
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  FibEntry payload1;
  payload1.setPrefix("/wYs7fzYcfG");
  FibEntry payload2;
  payload2.setPrefix("/LKvmnzY5S");
  this->sendDataset("/localhost/nfd/fib/list", payload1, payload2);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(CsInfo)
{
  using ndn::nfd::CsInfo;

  bool hasResult = false;
  controller.fetch<CsInfoDataset>(
    [&hasResult] (const CsInfo& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.getNHits(), 4539);
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  CsInfo payload;
  payload.setNHits(4539);
  this->sendDataset("/localhost/nfd/cs/info", payload);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(StrategyChoiceList)
{
  bool hasResult = false;
  controller.fetch<StrategyChoiceDataset>(
    [&hasResult] (const std::vector<StrategyChoice>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 2);
      BOOST_CHECK_EQUAL(result.front().getName(), "/8MLz6N3B");
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  StrategyChoice payload1;
  payload1.setName("/8MLz6N3B");
  StrategyChoice payload2;
  payload2.setName("/svqcBu0YwU");
  this->sendDataset("/localhost/nfd/strategy-choice/list", payload1, payload2);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(RibList)
{
  bool hasResult = false;
  controller.fetch<RibDataset>(
    [&hasResult] (const std::vector<RibEntry>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 2);
      BOOST_CHECK_EQUAL(result.front().getName(), "/zXxBth97ee");
    },
    datasetFailCallback);
  this->advanceClocks(500_ms);

  RibEntry payload1;
  payload1.setName("/zXxBth97ee");
  RibEntry payload2;
  payload2.setName("/rJ8CvUpr4G");
  this->sendDataset("/localhost/nfd/rib/list", payload1, payload2);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(RibListWithOptions)
{
  CommandOptions options;
  options.setPrefix("/localhop/nfd");
  bool hasResult = false;
  controller.fetch<RibDataset>(
    [&hasResult] (const std::vector<RibEntry>& result) {
      hasResult = true;
      BOOST_CHECK_EQUAL(result.size(), 1);
      BOOST_CHECK_EQUAL(result.front().getName(), "/e6L5K4ascd");
    },
    datasetFailCallback,
    options);
  this->advanceClocks(500_ms);

  RibEntry payload;
  payload.setName("/e6L5K4ascd");
  this->sendDataset("/localhop/nfd/rib/list", payload);
  this->advanceClocks(500_ms);

  BOOST_CHECK(hasResult);
  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // Datasets

BOOST_AUTO_TEST_SUITE_END() // TestStatusDataset
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
