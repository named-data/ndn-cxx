/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2015,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mgmt/status-dataset-context.hpp"
#include "boost-test.hpp"
#include "unit-tests/make-interest-data.hpp"

namespace ndn {
namespace mgmt {
namespace tests {

class StatusDatasetContextFixture
{
public:
  StatusDatasetContextFixture()
    : interest(util::makeInterest("/test/context/interest"))
    , contentBlock(makeStringBlock(tlv::Content, "/test/data/content"))
    , context(*interest, bind(&StatusDatasetContextFixture::sendData, this, _1, _2, _3))
  {
  }

  void
  sendData(const Name& dataName, const Block& content, const MetaInfo& info)
  {
    sentData.push_back(Data(dataName).setContent(content).setMetaInfo(info));
  }

  Block
  concatenate()
  {
    EncodingBuffer encoder;
    size_t valueLength = 0;
    for (auto data : sentData) {
       valueLength += encoder.appendByteArray(data.getContent().value(),
                                              data.getContent().value_size());
    }
    encoder.prependVarNumber(valueLength);
    encoder.prependVarNumber(tlv::Content);
    return encoder.block();
  }

public:
  std::vector<Data> sentData;
  shared_ptr<Interest> interest;
  Block contentBlock;
  mgmt::StatusDatasetContext context;
};

BOOST_FIXTURE_TEST_SUITE(MgmtStatusDatasetContext, StatusDatasetContextFixture)

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
  BOOST_CHECK(sentData.empty()); // does not call end yet

  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_EQUAL(sentData.size(), 1);
  BOOST_CHECK_EQUAL(sentData[0].getName()[-1].toSegment(), 0);
  BOOST_CHECK_EQUAL(sentData[0].getName().getPrefix(-1), context.getPrefix());
  BOOST_CHECK_EQUAL(sentData[0].getFinalBlockId().toSegment(), 0);
  BOOST_CHECK(sentData[0].getContent().blockFromValue() == contentBlock);
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
  BOOST_CHECK_EQUAL(sentData.size(), 2);
  BOOST_CHECK_EQUAL(sentData[0].getName()[-1].toSegment(), 0);
  BOOST_CHECK_EQUAL(sentData[0].getName().getPrefix(-1), context.getPrefix());
  BOOST_CHECK_EQUAL(sentData[1].getName()[-1].toSegment(), 1);
  BOOST_CHECK_EQUAL(sentData[1].getName().getPrefix(-1), context.getPrefix());
  BOOST_CHECK_EQUAL(sentData[1].getFinalBlockId().toSegment(), 1);

  auto contentLargeBlock = concatenate();
  BOOST_CHECK_NO_THROW(contentLargeBlock.parse());
  BOOST_CHECK_EQUAL(contentLargeBlock.elements().size(), 1);
  BOOST_CHECK(contentLargeBlock.elements()[0] == largeBlock);
}

BOOST_AUTO_TEST_CASE(ResponseMultipleSmall)
{
  size_t nBlocks = 100;
  for (size_t i = 0 ; i < nBlocks ; i ++) {
    BOOST_CHECK_NO_THROW(context.append(contentBlock));
  }
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_EQUAL(sentData.size(), 1);
  BOOST_CHECK_EQUAL(sentData[0].getName()[-1].toSegment(), 0);
  BOOST_CHECK_EQUAL(sentData[0].getName().getPrefix(-1), context.getPrefix());
  BOOST_CHECK_EQUAL(sentData[0].getFinalBlockId().toSegment(), 0);

  auto contentMultiBlocks = concatenate();
  BOOST_CHECK_NO_THROW(contentMultiBlocks.parse());
  BOOST_CHECK_EQUAL(contentMultiBlocks.elements().size(), nBlocks);
  for (auto&& element : contentMultiBlocks.elements()) {
    BOOST_CHECK(element == contentBlock);
  }
}

BOOST_AUTO_TEST_CASE(Reject)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_EQUAL(sentData.size(), 1);
  BOOST_CHECK(sentData[0].getContentType() == tlv::ContentType_Nack);
  BOOST_CHECK_EQUAL(ControlResponse(sentData[0].getContent().blockFromValue()).getCode(), 400);
}

BOOST_AUTO_TEST_SUITE(AbnormalState)

BOOST_AUTO_TEST_CASE(AppendReject)
{
  mgmt::StatusDatasetContext context(Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.append(Block("\x82\x01\x02", 3)));
  BOOST_CHECK_THROW(context.reject(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(AppendEndReject)
{
  mgmt::StatusDatasetContext context(Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.append(Block("\x82\x01\x02", 3)));
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_THROW(context.reject(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(EndAppend)
{
  mgmt::StatusDatasetContext context (Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.end());
  // end, append -> error
  BOOST_CHECK_THROW(context.append(Block("\x82\x01\x02", 3)), std::domain_error);
}

BOOST_AUTO_TEST_CASE(EndEnd)
{
  mgmt::StatusDatasetContext context(Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_THROW(context.end(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(EndReject)
{
  mgmt::StatusDatasetContext context(Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_THROW(context.reject(), std::domain_error);
}

BOOST_AUTO_TEST_CASE(RejectAppend)
{
  mgmt::StatusDatasetContext context(Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_THROW(context.append(Block("\x82\x01\x02", 3)), std::domain_error);
}

BOOST_AUTO_TEST_CASE(RejectEnd)
{
  mgmt::StatusDatasetContext context(Interest("/abnormal-state"), bind([]{}));
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_THROW(context.end(), std::domain_error);
}

BOOST_AUTO_TEST_SUITE_END() // AbnormalState


BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace mgmt
} // namespace ndn
