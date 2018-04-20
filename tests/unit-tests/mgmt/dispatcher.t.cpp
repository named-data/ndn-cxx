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

#include "mgmt/dispatcher.hpp"
#include "mgmt/nfd/control-parameters.hpp"
#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"
#include "make-interest-data.hpp"
#include "../identity-management-time-fixture.hpp"

namespace ndn {
namespace mgmt {
namespace tests {

using namespace ndn::tests;

class DispatcherFixture : public IdentityManagementTimeFixture
{
public:
  DispatcherFixture()
    : face(io, m_keyChain, {true, true})
    , dispatcher(face, m_keyChain, security::SigningInfo())
    , storage(dispatcher.m_storage)
  {
  }

public:
  util::DummyClientFace face;
  mgmt::Dispatcher dispatcher;
  InMemoryStorageFifo& storage;
};

class VoidParameters : public mgmt::ControlParameters
{
public:
  explicit
  VoidParameters(const Block& wire)
  {
    wireDecode(wire);
  }

  Block
  wireEncode() const final
  {
    return Block(128);
  }

  void
  wireDecode(const Block& wire) final
  {
    if (wire.type() != 128)
      BOOST_THROW_EXCEPTION(tlv::Error("Expecting TLV type 128"));
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

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_FIXTURE_TEST_SUITE(TestDispatcher, DispatcherFixture)

BOOST_AUTO_TEST_CASE(Basic)
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

BOOST_AUTO_TEST_CASE(AddRemoveTopPrefix)
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

  face.receive(*makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 0);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 0);

  dispatcher.addTopPrefix("/root/1");
  advanceClocks(1_ms);

  face.receive(*makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 1);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 0);

  face.receive(*makeInterest("/root/1/test/2/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 1);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 1);

  face.receive(*makeInterest("/root/2/test/1/%80%00"));
  face.receive(*makeInterest("/root/2/test/2/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 1);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/2"], 1);

  dispatcher.addTopPrefix("/root/2");
  advanceClocks(1_ms);

  face.receive(*makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 2);

  face.receive(*makeInterest("/root/2/test/1/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 3);

  dispatcher.removeTopPrefix("/root/1");
  advanceClocks(1_ms);

  face.receive(*makeInterest("/root/1/test/1/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 3);

  face.receive(*makeInterest("/root/2/test/1/%80%00"));
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled["test/1"], 4);
}

BOOST_AUTO_TEST_CASE(ControlCommand)
{
  size_t nCallbackCalled = 0;
  dispatcher
    .addControlCommand<VoidParameters>("test",
                                       makeTestAuthorization(),
                                       bind([] { return true; }),
                                       bind([&nCallbackCalled] { ++nCallbackCalled; }));

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  face.sentData.clear();

  face.receive(*makeInterest("/root/test/%80%00")); // returns 403
  face.receive(*makeInterest("/root/test/%80%00/invalid")); // returns 403
  face.receive(*makeInterest("/root/test/%80%00/silent")); // silently ignored
  face.receive(*makeInterest("/root/test/.../invalid")); // silently ignored (wrong format)
  face.receive(*makeInterest("/root/test/.../valid"));  // silently ignored (wrong format)
  advanceClocks(1_ms, 20);
  BOOST_CHECK_EQUAL(nCallbackCalled, 0);
  BOOST_CHECK_EQUAL(face.sentData.size(), 2);

  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 403);
  BOOST_CHECK_EQUAL(face.sentData[1].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[1].getContent().blockFromValue()).getCode(), 403);

  face.receive(*makeInterest("/root/test/%80%00/valid"));
  advanceClocks(1_ms, 10);
  BOOST_CHECK_EQUAL(nCallbackCalled, 1);
}

class StatefulParameters : public mgmt::ControlParameters
{
public:
  explicit
  StatefulParameters(const Block& wire)
  {
    wireDecode(wire);
  }

  Block
  wireEncode() const final
  {
    return Block();
  }

  void
  wireDecode(const Block& wire) final
  {
    m_state = EXPECTED_STATE;
  }

  bool
  check() const
  {
    return m_state == EXPECTED_STATE;
  }

private:
  static constexpr int EXPECTED_STATE = 12602;
  int m_state = 0;
};

BOOST_AUTO_TEST_CASE(ControlCommandAsyncAuthorization) // Bug 4059
{
  AcceptContinuation authorizationAccept;
  auto authorization =
    [&authorizationAccept] (const Name& prefix, const Interest& interest, const ControlParameters* params,
        AcceptContinuation accept, RejectContinuation reject) {
      authorizationAccept = accept;
    };

  auto validateParameters =
    [] (const ControlParameters& params) {
      return dynamic_cast<const StatefulParameters&>(params).check();
    };

  size_t nCallbackCalled = 0;
  dispatcher
    .addControlCommand<StatefulParameters>("test",
                                           authorization,
                                           validateParameters,
                                           bind([&nCallbackCalled] { ++nCallbackCalled; }));

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);

  face.receive(*makeInterest("/root/test/%80%00"));
  BOOST_CHECK_EQUAL(nCallbackCalled, 0);
  BOOST_REQUIRE(authorizationAccept != nullptr);

  advanceClocks(1_ms);
  authorizationAccept("");
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nCallbackCalled, 1);
}

BOOST_AUTO_TEST_CASE(StatusDataset)
{
  const uint8_t smallBuf[] = {0x81, 0x01, 0x01};
  const Block smallBlock(smallBuf, sizeof(smallBuf));
  Block largeBlock;
  {
    EncodingBuffer encoder;
    for (size_t i = 0; i < 2500; ++i) {
      encoder.prependByte(1);
    }
    encoder.prependVarNumber(2500);
    encoder.prependVarNumber(129);
    largeBlock = encoder.block();
  }

  dispatcher.addStatusDataset("test/small",
                              makeTestAuthorization(),
                              [&smallBlock] (const Name& prefix, const Interest& interest,
                                             StatusDatasetContext& context) {
                                context.append(smallBlock);
                                context.append(smallBlock);
                                context.append(smallBlock);
                                context.end();
                              });

  dispatcher.addStatusDataset("test/large",
                              makeTestAuthorization(),
                              [&largeBlock] (const Name& prefix, const Interest& interest,
                                             StatusDatasetContext& context) {
                                context.append(largeBlock);
                                context.append(largeBlock);
                                context.append(largeBlock);
                                context.end();
                              });

  dispatcher.addStatusDataset("test/reject",
                              makeTestAuthorization(),
                              [] (const Name& prefix, const Interest& interest,
                                  StatusDatasetContext& context) {
                                context.reject();
                              });

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  face.sentData.clear();

  face.receive(*makeInterest("/root/test/small/%80%00")); // returns 403
  face.receive(*makeInterest("/root/test/small/%80%00/invalid")); // returns 403
  face.receive(*makeInterest("/root/test/small/%80%00/silent")); // silently ignored
  advanceClocks(1_ms, 20);
  BOOST_CHECK_EQUAL(face.sentData.size(), 2);

  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 403);
  BOOST_CHECK_EQUAL(face.sentData[1].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[1].getContent().blockFromValue()).getCode(), 403);

  face.sentData.clear();

  auto interestSmall = *makeInterest("/root/test/small/valid");
  face.receive(interestSmall);
  advanceClocks(1_ms, 10);

  // one data packet is generated and sent to both places
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(storage.size(), 1);

  auto fetchedData = storage.find(interestSmall);
  BOOST_REQUIRE(fetchedData != nullptr);
  BOOST_CHECK_EQUAL(face.sentData[0].wireEncode(), fetchedData->wireEncode());

  face.receive(*makeInterest(Name("/root/test/small/valid").appendVersion(10))); // should be ignored
  face.receive(*makeInterest(Name("/root/test/small/valid").appendSegment(20))); // should be ignored
  advanceClocks(1_ms, 10);
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(storage.size(), 1);

  Block content = face.sentData[0].getContent();
  BOOST_CHECK_NO_THROW(content.parse());

  BOOST_REQUIRE_EQUAL(content.elements().size(), 3);
  BOOST_CHECK_EQUAL(content.elements()[0], smallBlock);
  BOOST_CHECK_EQUAL(content.elements()[1], smallBlock);
  BOOST_CHECK_EQUAL(content.elements()[2], smallBlock);

  storage.erase("/", true); // clear the storage
  face.sentData.clear();
  face.receive(*makeInterest("/root/test/large/valid"));
  advanceClocks(1_ms, 10);

  // two data packets are generated, the first one will be sent to both places
  // while the second one will only be inserted into the in-memory storage
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(storage.size(), 2);

  // segment0 should be sent through the face
  const auto& component = face.sentData[0].getName().at(-1);
  BOOST_CHECK(component.isSegment());
  BOOST_CHECK_EQUAL(component.toSegment(), 0);

  std::vector<Data> dataInStorage;
  std::copy(storage.begin(), storage.end(), std::back_inserter(dataInStorage));

  // the Data sent through the face should be the same as the first Data in the storage
  BOOST_CHECK_EQUAL(face.sentData[0].getName(), dataInStorage[0].getName());
  BOOST_CHECK_EQUAL(face.sentData[0].getContent(), dataInStorage[0].getContent());

  content = [&dataInStorage] () -> Block {
    EncodingBuffer encoder;
    size_t valueLength = encoder.prependByteArray(dataInStorage[1].getContent().value(),
                                                  dataInStorage[1].getContent().value_size());
    valueLength += encoder.prependByteArray(dataInStorage[0].getContent().value(),
                                            dataInStorage[0].getContent().value_size());
    encoder.prependVarNumber(valueLength);
    encoder.prependVarNumber(tlv::Content);
    return encoder.block();
  }();

  BOOST_CHECK_NO_THROW(content.parse());
  BOOST_REQUIRE_EQUAL(content.elements().size(), 3);
  BOOST_CHECK_EQUAL(content.elements()[0], largeBlock);
  BOOST_CHECK_EQUAL(content.elements()[1], largeBlock);
  BOOST_CHECK_EQUAL(content.elements()[2], largeBlock);

  storage.erase("/", true);// clear the storage
  face.sentData.clear();
  face.receive(*makeInterest("/root/test/reject/%80%00/valid")); // returns nack
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Nack);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 400);
  BOOST_CHECK_EQUAL(storage.size(), 0); // the nack packet will not be inserted into the in-memory storage
}

BOOST_AUTO_TEST_CASE(NotificationStream)
{
  const uint8_t buf[] = {0x82, 0x01, 0x02};
  const Block block(buf, sizeof(buf));
  auto post = dispatcher.addNotificationStream("test");

  post(block);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  face.sentData.clear();

  post(block);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(storage.size(), 1);

  post(block);
  post(block);
  post(block);
  advanceClocks(1_ms, 10);

  BOOST_REQUIRE_EQUAL(face.sentData.size(), 4);
  BOOST_CHECK_EQUAL(face.sentData[0].getName(), "/root/test/%FE%00");
  BOOST_CHECK_EQUAL(face.sentData[1].getName(), "/root/test/%FE%01");
  BOOST_CHECK_EQUAL(face.sentData[2].getName(), "/root/test/%FE%02");
  BOOST_CHECK_EQUAL(face.sentData[3].getName(), "/root/test/%FE%03");

  BOOST_CHECK_EQUAL(face.sentData[0].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(face.sentData[1].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(face.sentData[2].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(face.sentData[3].getContent().blockFromValue(), block);

  // each version of notification will be sent to both places
  std::vector<Data> dataInStorage;
  std::copy(storage.begin(), storage.end(), std::back_inserter(dataInStorage));
  BOOST_REQUIRE_EQUAL(dataInStorage.size(), 4);
  BOOST_CHECK_EQUAL(dataInStorage[0].getName(), "/root/test/%FE%00");
  BOOST_CHECK_EQUAL(dataInStorage[1].getName(), "/root/test/%FE%01");
  BOOST_CHECK_EQUAL(dataInStorage[2].getName(), "/root/test/%FE%02");
  BOOST_CHECK_EQUAL(dataInStorage[3].getName(), "/root/test/%FE%03");

  BOOST_CHECK_EQUAL(dataInStorage[0].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(dataInStorage[1].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(dataInStorage[2].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(dataInStorage[3].getContent().blockFromValue(), block);
}

BOOST_AUTO_TEST_SUITE_END() // TestDispatcher
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace mgmt
} // namespace ndn
