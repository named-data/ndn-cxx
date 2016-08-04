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

#include "mgmt/status-dataset-context.hpp"
#include "boost-test.hpp"
#include "unit-tests/make-interest-data.hpp"

namespace ndn {
namespace mgmt {
namespace tests {

using namespace ndn::tests;

class StatusDatasetContextFixture
{
public:
  struct SendDataArgs
  {
    Name dataName;
    Block content;
    time::milliseconds imsFresh;
    bool isFinalBlock;
  };

  StatusDatasetContextFixture()
    : interest(makeInterest("/test/context/interest"))
    , contentBlock(makeStringBlock(tlv::Content, "/test/data/content"))
    , context(*interest,
              [this] (const Name& dataName, const Block& content,
                      time::milliseconds imsFresh, bool isFinalBlock) {
                SendDataArgs args{dataName, content, imsFresh, isFinalBlock};
                sendDataHistory.push_back(args);
              },
              [this] (const ControlResponse& resp) {
                sendNackHistory.push_back(resp);
              })
    , defaultImsFresh(time::milliseconds(1000))
  {
  }

  Name
  makeSegmentName(size_t segmentNo)
  {
    auto name = context.getPrefix();
    return name.appendSegment(segmentNo);
  }

  Block
  concatenateDataContent()
  {
    EncodingBuffer encoder;
    size_t valueLength = 0;
    for (auto args : sendDataHistory) {
      const auto& content = args.content;
      valueLength += encoder.appendByteArray(content.value(), content.value_size());
    }
    encoder.prependVarNumber(valueLength);
    encoder.prependVarNumber(tlv::Content);
    return encoder.block();
  }

public:
  std::vector<SendDataArgs> sendDataHistory;
  std::vector<ControlResponse> sendNackHistory;
  shared_ptr<Interest> interest;
  Block contentBlock;
  mgmt::StatusDatasetContext context;
  time::milliseconds defaultImsFresh;
};

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_FIXTURE_TEST_SUITE(TestStatusDatasetContext, StatusDatasetContextFixture)

BOOST_AUTO_TEST_CASE(GetPrefix)
{
  Name dataName = context.getPrefix();
  BOOST_CHECK(dataName[-1].isVersion());
  BOOST_CHECK(dataName.getPrefix(-1) == interest->getName());
}

BOOST_AUTO_TEST_SUITE(SetPrefix)

BOOST_AUTO_TEST_CASE(Valid)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  BOOST_CHECK_NO_THROW(context.setPrefix(validPrefix));
  BOOST_CHECK(context.getPrefix()[-1].isVersion());
  BOOST_CHECK(context.getPrefix().getPrefix(-1) == validPrefix);
}

BOOST_AUTO_TEST_CASE(Invalid)
{
  Name invalidPrefix = Name(interest->getName()).getPrefix(-1).append("/invalid");
  BOOST_CHECK_THROW(context.setPrefix(invalidPrefix), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ValidWithAppendCalled)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  context.append(contentBlock);
  BOOST_CHECK_THROW(context.setPrefix(validPrefix), std::domain_error);
}

BOOST_AUTO_TEST_CASE(ValidWithEndCalled)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  context.end();
  BOOST_CHECK_THROW(context.setPrefix(validPrefix), std::domain_error);
}

BOOST_AUTO_TEST_CASE(ValidWithRejectCalled)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  context.reject();
  BOOST_CHECK_THROW(context.setPrefix(validPrefix), std::domain_error);
}

BOOST_AUTO_TEST_SUITE_END() // SetPrefix

BOOST_AUTO_TEST_CASE(Expiry)
{
  // getExpiry & setExpiry
  auto period = time::milliseconds(9527);
  BOOST_CHECK_EQUAL(context.getExpiry(), time::milliseconds(1000));
  BOOST_CHECK_EQUAL(context.setExpiry(period).getExpiry(), period);
}

BOOST_AUTO_TEST_CASE(Respond)
{
  BOOST_CHECK_NO_THROW(context.append(contentBlock));
  BOOST_CHECK(sendDataHistory.empty()); // does not call end yet

  BOOST_CHECK_NO_THROW(context.end());

  BOOST_REQUIRE_EQUAL(sendDataHistory.size(), 1);
  const auto& args = sendDataHistory[0];

  BOOST_CHECK_EQUAL(args.dataName, makeSegmentName(0));
  BOOST_CHECK(args.content.blockFromValue() == contentBlock);
  BOOST_CHECK_EQUAL(args.imsFresh, defaultImsFresh);
  BOOST_CHECK_EQUAL(args.isFinalBlock, true);
}

BOOST_AUTO_TEST_CASE(RespondLarge)
{
  static Block largeBlock = [] () -> Block {
    EncodingBuffer encoder;
    size_t maxBlockSize = MAX_NDN_PACKET_SIZE >> 1;
    for (size_t i = 0; i < maxBlockSize; ++i) {
      encoder.prependByte(1);
    }
    encoder.prependVarNumber(maxBlockSize);
    encoder.prependVarNumber(tlv::Content);
    return encoder.block();
  }();

  BOOST_CHECK_NO_THROW(context.append(largeBlock));
  BOOST_CHECK_NO_THROW(context.end());

  // two segments are generated
  BOOST_REQUIRE_EQUAL(sendDataHistory.size(), 2);

  // check segment0
  BOOST_CHECK_EQUAL(sendDataHistory[0].dataName, makeSegmentName(0));
  BOOST_CHECK_EQUAL(sendDataHistory[0].imsFresh, defaultImsFresh);
  BOOST_CHECK_EQUAL(sendDataHistory[0].isFinalBlock, false);

  // check segment1
  BOOST_CHECK_EQUAL(sendDataHistory[1].dataName, makeSegmentName(1));
  BOOST_CHECK_EQUAL(sendDataHistory[1].imsFresh, defaultImsFresh);
  BOOST_CHECK_EQUAL(sendDataHistory[1].isFinalBlock, true);

  // check data content
  auto contentLargeBlock = concatenateDataContent();
  BOOST_CHECK_NO_THROW(contentLargeBlock.parse());
  BOOST_REQUIRE_EQUAL(contentLargeBlock.elements().size(), 1);
  BOOST_CHECK(contentLargeBlock.elements()[0] == largeBlock);
}

BOOST_AUTO_TEST_CASE(ResponseMultipleSmall)
{
  size_t nBlocks = 100;
  for (size_t i = 0 ; i < nBlocks ; i ++) {
    BOOST_CHECK_NO_THROW(context.append(contentBlock));
  }
  BOOST_CHECK_NO_THROW(context.end());

  // check data to in-memory storage
  BOOST_REQUIRE_EQUAL(sendDataHistory.size(), 1);
  BOOST_CHECK_EQUAL(sendDataHistory[0].dataName, makeSegmentName(0));
  BOOST_CHECK_EQUAL(sendDataHistory[0].imsFresh, defaultImsFresh);
  BOOST_CHECK_EQUAL(sendDataHistory[0].isFinalBlock, true);

  auto contentMultiBlocks = concatenateDataContent();
  BOOST_CHECK_NO_THROW(contentMultiBlocks.parse());
  BOOST_CHECK_EQUAL(contentMultiBlocks.elements().size(), nBlocks);
  for (auto&& element : contentMultiBlocks.elements()) {
    BOOST_CHECK(element == contentBlock);
  }
}

BOOST_AUTO_TEST_CASE(Reject)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_REQUIRE_EQUAL(sendNackHistory.size(), 1);
  BOOST_CHECK_EQUAL(sendNackHistory[0].getCode(), 400);
}

class AbnormalStateTestFixture
{
public:
  AbnormalStateTestFixture()
    : context(Interest("/abnormal-state"), bind([]{}), bind([]{}))
  {
  }

public:
  mgmt::StatusDatasetContext context;
};

BOOST_FIXTURE_TEST_SUITE(AbnormalState, AbnormalStateTestFixture)

BOOST_AUTO_TEST_CASE(AppendReject)
{
  BOOST_CHECK_NO_THROW(context.append(Block("\x82\x01\x02", 3)));
  BOOST_CHECK_THROW(context.reject(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(AppendEndReject)
{
  BOOST_CHECK_NO_THROW(context.append(Block("\x82\x01\x02", 3)));
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_THROW(context.reject(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(EndAppend)
{
  BOOST_CHECK_NO_THROW(context.end());
  // end, append -> error
  BOOST_CHECK_THROW(context.append(Block("\x82\x01\x02", 3)), std::domain_error);
}

BOOST_AUTO_TEST_CASE(EndEnd)
{
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_THROW(context.end(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(EndReject)
{
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_THROW(context.reject(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(RejectAppend)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_THROW(context.append(Block("\x82\x01\x02", 3)), std::domain_error);
}

BOOST_AUTO_TEST_CASE(RejectEnd)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_THROW(context.end(), std::domain_error);
}

BOOST_AUTO_TEST_SUITE_END() // AbnormalState
BOOST_AUTO_TEST_SUITE_END() // TestStatusDatasetContext
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace mgmt
} // namespace ndn
