/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/mgmt/status-dataset-context.hpp"

#include "tests/test-common.hpp"

namespace ndn {
namespace mgmt {
namespace tests {

using namespace ndn::tests;

class StatusDatasetContextFixture
{
private:
  struct SendDataArgs
  {
    Name dataName;
    Block content;
    bool isFinalBlock;
  };

protected:
  StatusDatasetContextFixture()
    : interest(makeInterest("/test/context/interest"))
    , context(*interest,
              [this] (auto&&... args) {
                sendDataHistory.push_back({std::forward<decltype(args)>(args)...});
              },
              [this] (const auto& resp) {
                sendNackHistory.push_back(resp);
              })
  {
  }

  Name
  makeSegmentName(size_t segmentNo) const
  {
    auto name = context.getPrefix();
    return name.appendSegment(segmentNo);
  }

  ConstBufferPtr
  concatenateDataContent() const
  {
    auto buf = std::make_shared<Buffer>();
    for (const auto& args : sendDataHistory) {
      buf->insert(buf->end(), args.content.value_begin(), args.content.value_end());
    }
    return buf;
  }

protected:
  shared_ptr<Interest> interest;
  StatusDatasetContext context;
  std::vector<SendDataArgs> sendDataHistory;
  std::vector<ControlResponse> sendNackHistory;
};

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_FIXTURE_TEST_SUITE(TestStatusDatasetContext, StatusDatasetContextFixture)

BOOST_AUTO_TEST_SUITE(Prefix)

BOOST_AUTO_TEST_CASE(Get)
{
  Name dataName = context.getPrefix();
  BOOST_CHECK(dataName[-1].isVersion());
  BOOST_CHECK_EQUAL(dataName.getPrefix(-1), interest->getName());
}

BOOST_AUTO_TEST_CASE(SetValid)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  BOOST_CHECK_NO_THROW(context.setPrefix(validPrefix));
  BOOST_CHECK_EQUAL(context.getPrefix().getPrefix(-1), validPrefix);
  BOOST_CHECK(context.getPrefix()[-1].isVersion());

  // trailing version component is preserved
  validPrefix.appendVersion(42);
  BOOST_CHECK_NO_THROW(context.setPrefix(validPrefix));
  BOOST_CHECK_EQUAL(context.getPrefix(), validPrefix);
}

BOOST_AUTO_TEST_CASE(SetInvalid)
{
  // Interest name is not a prefix of invalidPrefix
  Name invalidPrefix = Name(interest->getName()).getPrefix(-1).append("/invalid");
  BOOST_CHECK_EXCEPTION(context.setPrefix(invalidPrefix), std::invalid_argument, [] (const auto& e) {
    return e.what() == "prefix must start with the Interest's name"s;
  });

  // invalidPrefix contains a segment component
  invalidPrefix = Name(interest->getName()).appendSegment(1);
  BOOST_CHECK_EXCEPTION(context.setPrefix(invalidPrefix), std::invalid_argument, [] (const auto& e) {
    return e.what() == "prefix must not contain a segment component"s;
  });
}

BOOST_AUTO_TEST_CASE(SetValidAfterAppend)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  context.append({0x12, 0x34});
  BOOST_CHECK_EXCEPTION(context.setPrefix(validPrefix), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call setPrefix() after append/end/reject"s;
  });
}

BOOST_AUTO_TEST_CASE(SetValidAfterEnd)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  context.end();
  BOOST_CHECK_EXCEPTION(context.setPrefix(validPrefix), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call setPrefix() after append/end/reject"s;
  });
}

BOOST_AUTO_TEST_CASE(SetValidAfterReject)
{
  Name validPrefix = Name(interest->getName()).append("/valid");
  context.reject();
  BOOST_CHECK_EXCEPTION(context.setPrefix(validPrefix), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call setPrefix() after append/end/reject"s;
  });
}

BOOST_AUTO_TEST_SUITE_END() // Prefix

BOOST_AUTO_TEST_SUITE(Respond)

BOOST_AUTO_TEST_CASE(Basic)
{
  const auto testBlock = makeStringBlock(tlv::Content, "TEST");
  context.append(testBlock);
  BOOST_CHECK(sendDataHistory.empty()); // end() not called yet

  context.end();
  BOOST_REQUIRE_EQUAL(sendDataHistory.size(), 1);

  const auto& args = sendDataHistory[0];
  BOOST_CHECK_EQUAL(args.dataName, makeSegmentName(0));
  BOOST_CHECK_EQUAL(args.content.blockFromValue(), testBlock);
  BOOST_CHECK_EQUAL(args.isFinalBlock, true);
}

BOOST_AUTO_TEST_CASE(Empty)
{
  context.append({});
  BOOST_TEST(sendDataHistory.empty()); // end() not called yet

  context.end();
  BOOST_TEST_REQUIRE(sendDataHistory.size() == 1);

  const auto& args = sendDataHistory[0];
  BOOST_TEST(args.dataName == makeSegmentName(0));
  BOOST_TEST(args.content.value_size() == 0);
  BOOST_TEST(args.isFinalBlock);
}

BOOST_AUTO_TEST_CASE(Large)
{
  const std::vector<uint8_t> big(10000, 'A');
  context.append(big);
  BOOST_CHECK_EQUAL(sendDataHistory.size(), 1);

  context.end();
  BOOST_REQUIRE_EQUAL(sendDataHistory.size(), 2);

  // check segment 0
  BOOST_CHECK_EQUAL(sendDataHistory[0].dataName, makeSegmentName(0));
  BOOST_CHECK_EQUAL(sendDataHistory[0].isFinalBlock, false);

  // check segment 1
  BOOST_CHECK_EQUAL(sendDataHistory[1].dataName, makeSegmentName(1));
  BOOST_CHECK_EQUAL(sendDataHistory[1].isFinalBlock, true);

  // check data content
  BOOST_TEST(*concatenateDataContent() == big, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(MultipleSmall)
{
  const size_t nBlocks = 1000;
  const auto contentBlock = makeStringBlock(0xFFFF, "Test Data Content");
  for (size_t i = 0 ; i < nBlocks ; i ++) {
    context.append(contentBlock);
  }
  context.end();

  BOOST_TEST_REQUIRE(sendDataHistory.size() == 3);
  BOOST_TEST(sendDataHistory[0].dataName == makeSegmentName(0));
  BOOST_TEST(!sendDataHistory[0].isFinalBlock);
  BOOST_TEST(sendDataHistory[1].dataName == makeSegmentName(1));
  BOOST_TEST(!sendDataHistory[1].isFinalBlock);
  BOOST_TEST(sendDataHistory[2].dataName == makeSegmentName(2));
  BOOST_TEST(sendDataHistory[2].isFinalBlock);

  Block contentMultiBlocks(tlv::Content, concatenateDataContent());
  contentMultiBlocks.parse();
  BOOST_TEST(contentMultiBlocks.elements().size() == nBlocks);
  for (const auto& element : contentMultiBlocks.elements()) {
    BOOST_TEST(element == contentBlock, boost::test_tools::per_element());
  }
}

BOOST_AUTO_TEST_SUITE_END() // Respond

BOOST_AUTO_TEST_CASE(Reject)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_REQUIRE_EQUAL(sendNackHistory.size(), 1);
  BOOST_CHECK_EQUAL(sendNackHistory[0].getCode(), 400);
}

class AbnormalStateTestFixture
{
protected:
  StatusDatasetContext context{Interest("/abnormal-state"), [] (auto&&...) {}, [] (auto&&...) {}};
};

BOOST_FIXTURE_TEST_SUITE(AbnormalState, AbnormalStateTestFixture)

BOOST_AUTO_TEST_CASE(AppendReject)
{
  BOOST_CHECK_NO_THROW(context.append({0x82, 0x01, 0x02}));
  BOOST_CHECK_EXCEPTION(context.reject(), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call reject() after append/end"s;
  });
}

BOOST_AUTO_TEST_CASE(AppendEndReject)
{
  BOOST_CHECK_NO_THROW(context.append({0x82, 0x01, 0x02}));
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_EXCEPTION(context.reject(), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call reject() after append/end"s;
  });
}

BOOST_AUTO_TEST_CASE(EndAppend)
{
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_EXCEPTION(context.append({0x82, 0x01, 0x02}), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call append() on a finalized context"s;
  });
}

BOOST_AUTO_TEST_CASE(EndEnd)
{
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_EXCEPTION(context.end(), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call end() on a finalized context"s;
  });
}

BOOST_AUTO_TEST_CASE(EndReject)
{
  BOOST_CHECK_NO_THROW(context.end());
  BOOST_CHECK_EXCEPTION(context.reject(), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call reject() after append/end"s;
  });
}

BOOST_AUTO_TEST_CASE(RejectAppend)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_EXCEPTION(context.append({0x82, 0x01, 0x02}), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call append() on a finalized context"s;
  });
}

BOOST_AUTO_TEST_CASE(RejectEnd)
{
  BOOST_CHECK_NO_THROW(context.reject());
  BOOST_CHECK_EXCEPTION(context.end(), std::logic_error, [] (const auto& e) {
    return e.what() == "cannot call end() on a finalized context"s;
  });
}

BOOST_AUTO_TEST_SUITE_END() // AbnormalState

BOOST_AUTO_TEST_SUITE_END() // TestStatusDatasetContext
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace mgmt
} // namespace ndn
