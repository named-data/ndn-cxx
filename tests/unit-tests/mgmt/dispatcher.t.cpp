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

#include "mgmt/dispatcher.hpp"
#include "management/nfd-control-parameters.hpp"
#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"
#include "identity-management-fixture.hpp"
#include "unit-tests/unit-test-time-fixture.hpp"
#include "unit-tests/make-interest-data.hpp"

namespace ndn {
namespace mgmt {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(MgmtDispatcher)

class DispatcherFixture : public UnitTestTimeFixture
                        , public security::IdentityManagementFixture
{
public:
  DispatcherFixture()
    : face(util::makeDummyClientFace(io, {true, true}))
    , dispatcher(*face, m_keyChain, security::SigningInfo())
  {
  }

public:
  shared_ptr<util::DummyClientFace> face;
  mgmt::Dispatcher dispatcher;
};

class VoidParameters : public mgmt::ControlParameters
{
public:
  explicit
  VoidParameters(const Block& wire)
  {
    wireDecode(wire);
  }

  virtual Block
  wireEncode() const NDN_CXX_DECL_FINAL
  {
    return Block(128);
  }

  virtual void
  wireDecode(const Block& wire) NDN_CXX_DECL_FINAL
  {
    if (wire.type() != 128)
      throw tlv::Error("Expecting TLV type 128");
  }
};

static Authorization
makeTestAuthorization()
{
  return [] (const Name& prefix,
             const Interest& interest,
             const ControlParameters* params,
             AcceptContinuation accept,
             RejectContinuation reject) {
    if (interest.getName()[-1] == name::Component("valid")) {
      accept("");
    }
    else {
      if (interest.getName()[-1] == name::Component("silent")) {
        reject(RejectReply::SILENT);
      }
      else {
        reject(RejectReply::STATUS403);
      }
    }
  };
}

BOOST_FIXTURE_TEST_CASE(BasicUsageSemantics, DispatcherFixture)
{
  BOOST_CHECK_NO_THROW(dispatcher
                         .addControlCommand<VoidParameters>("test/1", makeAcceptAllAuthorization(),
                                                            bind([] { return true; }),
                                                            bind([]{})));
  BOOST_CHECK_NO_THROW(dispatcher
                         .addControlCommand<VoidParameters>("test/2", makeAcceptAllAuthorization(),
                                                            bind([] { return true; }),
                                                            bind([]{})));

  BOOST_CHECK_THROW(dispatcher
                      .addControlCommand<VoidParameters>("test", makeAcceptAllAuthorization(),
                                                         bind([] { return true; }),
                                                         bind([]{})),
                    std::out_of_range);

  BOOST_CHECK_NO_THROW(dispatcher.addStatusDataset("status/1",
                                                   makeAcceptAllAuthorization(), bind([]{})));
  BOOST_CHECK_NO_THROW(dispatcher.addStatusDataset("status/2",
                                                   makeAcceptAllAuthorization(), bind([]{})));
  BOOST_CHECK_THROW(dispatcher.addStatusDataset("status",
                                                makeAcceptAllAuthorization(), bind([]{})),
                    std::out_of_range);

  BOOST_CHECK_NO_THROW(dispatcher.addNotificationStream("stream/1"));
  BOOST_CHECK_NO_THROW(dispatcher.addNotificationStream("stream/2"));
  BOOST_CHECK_THROW(dispatcher.addNotificationStream("stream"), std::out_of_range);


  BOOST_CHECK_NO_THROW(dispatcher.addTopPrefix("/root/1"));
  BOOST_CHECK_NO_THROW(dispatcher.addTopPrefix("/root/2"));
  BOOST_CHECK_THROW(dispatcher.addTopPrefix("/root"), std::out_of_range);

  BOOST_CHECK_THROW(dispatcher
                      .addControlCommand<VoidParameters>("test/3", makeAcceptAllAuthorization(),
                                                         bind([] { return true; }),
                                                         bind([]{})),
                    std::domain_error);

  BOOST_CHECK_THROW(dispatcher.addStatusDataset("status/3",
                                                makeAcceptAllAuthorization(), bind([]{})),
                    std::domain_error);

  BOOST_CHECK_THROW(dispatcher.addNotificationStream("stream/3"), std::domain_error);
}

BOOST_FIXTURE_TEST_CASE(AddRemoveTopPrefix, DispatcherFixture)
{
  std::map<std::string, size_t> nCallbackCalled;
  dispatcher
    .addControlCommand<VoidParameters>("test/1", makeAcceptAllAuthorization(),
                                       bind([] { return true; }),
                                       bind([&nCallbackCalled] { ++nCallbackCalled["test/1"]; }));

  dispatcher
    .addControlCommand<VoidParameters>("test/2", makeAcceptAllAuthorization(),
                                       bind([] { return true; }),
                                       bind([&nCallbackCalled] { ++nCallbackCalled["test/2"]; }));

  face->receive(*util::makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 0);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 0);

  dispatcher.addTopPrefix("/root/1");
  advanceClocks(time::milliseconds(1));

  face->receive(*util::makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 1);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 0);

  face->receive(*util::makeInterest("/root/1/test/2/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 1);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 1);

  face->receive(*util::makeInterest("/root/2/test/1/%80%00"));
  face->receive(*util::makeInterest("/root/2/test/2/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 1);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 1);

  dispatcher.addTopPrefix("/root/2");
  advanceClocks(time::milliseconds(1));

  face->receive(*util::makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 2);

  face->receive(*util::makeInterest("/root/2/test/1/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 3);

  dispatcher.removeTopPrefix("/root/1");
  advanceClocks(time::milliseconds(1));

  face->receive(*util::makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 3);

  face->receive(*util::makeInterest("/root/2/test/1/%80%00"));
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 4);
}

BOOST_FIXTURE_TEST_CASE(ControlCommand, DispatcherFixture)
{
  size_t nCallbackCalled = 0;
  dispatcher
    .addControlCommand<VoidParameters>("test",
                                       makeTestAuthorization(),
                                       bind([] { return true; }),
                                       bind([&nCallbackCalled] { ++nCallbackCalled; }));

  dispatcher.addTopPrefix("/root");
  advanceClocks(time::milliseconds(1));
  face->sentDatas.clear();

  face->receive(*util::makeInterest("/root/test/%80%00")); // returns 403
  face->receive(*util::makeInterest("/root/test/%80%00/invalid")); // returns 403
  face->receive(*util::makeInterest("/root/test/%80%00/silent")); // silently ignored
  face->receive(*util::makeInterest("/root/test/.../invalid")); // silently ignored (wrong format)
  face->receive(*util::makeInterest("/root/test/.../valid"));  // silently ignored (wrong format)
  advanceClocks(time::milliseconds(1), 20);
  BOOST_CHECK_EQUAL(nCallbackCalled, 0);
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 2);

  BOOST_CHECK(face->sentDatas[0].getContentType() == tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face->sentDatas[0].getContent().blockFromValue()).getCode(), 403);
  BOOST_CHECK(face->sentDatas[1].getContentType() == tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face->sentDatas[1].getContent().blockFromValue()).getCode(), 403);

  face->receive(*util::makeInterest("/root/test/%80%00/valid"));
  advanceClocks(time::milliseconds(1), 10);
  BOOST_CHECK_EQUAL(nCallbackCalled, 1);
}

BOOST_FIXTURE_TEST_CASE(StatusDataset, DispatcherFixture)
{
  static Block smallBlock("\x81\x01\0x01", 3);
  static Block largeBlock = [] () -> Block {
    EncodingBuffer encoder;
    for (size_t i = 0; i < 2500; ++i) {
      encoder.prependByte(1);
    }
    encoder.prependVarNumber(2500);
    encoder.prependVarNumber(129);
    return encoder.block();
  }();

  dispatcher.addStatusDataset("test/small",
                              makeTestAuthorization(),
                              [] (const Name& prefix, const Interest& interest,
                                  StatusDatasetContext context) {
                                context.append(smallBlock);
                                context.append(smallBlock);
                                context.append(smallBlock);
                                context.end();
                              });

  dispatcher.addStatusDataset("test/large",
                              makeTestAuthorization(),
                              [] (const Name& prefix, const Interest& interest,
                                  StatusDatasetContext context) {
                                context.append(largeBlock);
                                context.append(largeBlock);
                                context.append(largeBlock);
                                context.end();
                              });

  dispatcher.addStatusDataset("test/reject",
                              makeTestAuthorization(),
                              [] (const Name& prefix, const Interest& interest,
                                  StatusDatasetContext context) {
                                context.reject();
                              });

  dispatcher.addTopPrefix("/root");
  advanceClocks(time::milliseconds(1));
  face->sentDatas.clear();

  face->receive(*util::makeInterest("/root/test/small/%80%00")); // returns 403
  face->receive(*util::makeInterest("/root/test/small/%80%00/invalid")); // returns 403
  face->receive(*util::makeInterest("/root/test/small/%80%00/silent")); // silently ignored
  advanceClocks(time::milliseconds(1), 20);
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 2);

  BOOST_CHECK(face->sentDatas[0].getContentType() == tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face->sentDatas[0].getContent().blockFromValue()).getCode(), 403);
  BOOST_CHECK(face->sentDatas[1].getContentType() == tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face->sentDatas[1].getContent().blockFromValue()).getCode(), 403);

  face->sentDatas.clear();
  face->receive(*util::makeInterest("/root/test/small/valid"));
  advanceClocks(time::milliseconds(1), 10);
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 1);

  face->receive(*util::makeInterest(Name("/root/test/small/valid").appendVersion(10))); // should be ignored
  face->receive(*util::makeInterest(Name("/root/test/small/valid").appendSegment(20))); // should be ignored
  advanceClocks(time::milliseconds(1), 10);
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 1);

  Block content = face->sentDatas[0].getContent();
  BOOST_CHECK_NO_THROW(content.parse());

  BOOST_CHECK_EQUAL(content.elements().size(), 3);
  BOOST_CHECK(content.elements()[0] == smallBlock);
  BOOST_CHECK(content.elements()[1] == smallBlock);
  BOOST_CHECK(content.elements()[2] == smallBlock);

  face->sentDatas.clear();
  face->receive(*util::makeInterest("/root/test/large/valid"));
  advanceClocks(time::milliseconds(1), 10);
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 2);

  const auto& datas = face->sentDatas;
  content = [&datas] () -> Block {
    EncodingBuffer encoder;
    size_t valueLength = encoder.prependByteArray(datas[1].getContent().value(),
                                                  datas[1].getContent().value_size());
    valueLength += encoder.prependByteArray(datas[0].getContent().value(),
                                            datas[0].getContent().value_size());
    encoder.prependVarNumber(valueLength);
    encoder.prependVarNumber(tlv::Content);
    return encoder.block();
  }();

  BOOST_CHECK_NO_THROW(content.parse());

  BOOST_CHECK_EQUAL(content.elements().size(), 3);
  BOOST_CHECK(content.elements()[0] == largeBlock);
  BOOST_CHECK(content.elements()[1] == largeBlock);
  BOOST_CHECK(content.elements()[2] == largeBlock);

  face->sentDatas.clear();
  face->receive(*util::makeInterest("/root/test/reject/%80%00/valid")); // returns nack
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 1);
  BOOST_CHECK(face->sentDatas[0].getContentType() == tlv::ContentType_Nack);
  BOOST_CHECK_EQUAL(ControlResponse(face->sentDatas[0].getContent().blockFromValue()).getCode(), 400);
}

BOOST_FIXTURE_TEST_CASE(NotificationStream, DispatcherFixture)
{
  static Block block("\x82\x01\x02", 3);

  auto post = dispatcher.addNotificationStream("test");

  post(block);
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 0);

  dispatcher.addTopPrefix("/root");
  advanceClocks(time::milliseconds(1));
  face->sentDatas.clear();

  post(block);
  advanceClocks(time::milliseconds(1));
  BOOST_CHECK_EQUAL(face->sentDatas.size(), 1);

  post(block);
  post(block);
  post(block);
  advanceClocks(time::milliseconds(1), 10);

  BOOST_CHECK_EQUAL(face->sentDatas.size(), 4);
  BOOST_CHECK_EQUAL(face->sentDatas[0].getName(), "/root/test/%FE%00");
  BOOST_CHECK_EQUAL(face->sentDatas[1].getName(), "/root/test/%FE%01");
  BOOST_CHECK_EQUAL(face->sentDatas[2].getName(), "/root/test/%FE%02");
  BOOST_CHECK_EQUAL(face->sentDatas[3].getName(), "/root/test/%FE%03");

  BOOST_CHECK(face->sentDatas[0].getContent().blockFromValue() == block);
  BOOST_CHECK(face->sentDatas[1].getContent().blockFromValue() == block);
  BOOST_CHECK(face->sentDatas[2].getContent().blockFromValue() == block);
  BOOST_CHECK(face->sentDatas[3].getContent().blockFromValue() == block);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace mgmt
} // namespace ndn
