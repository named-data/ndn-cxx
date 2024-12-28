/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#include "ndn-cxx/mgmt/dispatcher.hpp"
#include "ndn-cxx/mgmt/nfd/control-command.hpp"
#include "ndn-cxx/util/dummy-client-face.hpp"

#include "tests/test-common.hpp"
#include "tests/unit/io-key-chain-fixture.hpp"

namespace ndn::tests {

using namespace ndn::mgmt;

class DispatcherFixture : public IoKeyChainFixture
{
protected:
  DummyClientFace face{m_io, m_keyChain, {true, true}};
  Dispatcher dispatcher{face, m_keyChain};
  InMemoryStorageFifo& storage{dispatcher.m_storage};
};

class VoidParameters : public ControlParametersBase
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
      NDN_THROW(tlv::Error("Expecting TLV type 128"));
  }
};

static Authorization
makeTestAuthorization()
{
  return [] (const Name&, const Interest& interest, const ControlParametersBase*,
             AcceptContinuation accept, RejectContinuation reject) {
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
  auto nop = [] (auto&&...) {};
  auto tautology = [] (auto&&...) { return true; };

  BOOST_CHECK_NO_THROW(dispatcher
                       .addControlCommand<VoidParameters>("test/1", makeAcceptAllAuthorization(),
                                                          tautology, nop));
  BOOST_CHECK_NO_THROW(dispatcher
                       .addControlCommand<VoidParameters>("test/2", makeAcceptAllAuthorization(),
                                                          tautology, nop));
  BOOST_CHECK_THROW(dispatcher
                    .addControlCommand<VoidParameters>("test", makeAcceptAllAuthorization(),
                                                       tautology, nop),
                    std::out_of_range);

  BOOST_CHECK_NO_THROW(dispatcher.addStatusDataset("status/1", makeAcceptAllAuthorization(), nop));
  BOOST_CHECK_NO_THROW(dispatcher.addStatusDataset("status/2", makeAcceptAllAuthorization(), nop));
  BOOST_CHECK_THROW(dispatcher.addStatusDataset("status", makeAcceptAllAuthorization(), nop),
                    std::out_of_range);

  BOOST_CHECK_NO_THROW(dispatcher.addNotificationStream("stream/1"));
  BOOST_CHECK_NO_THROW(dispatcher.addNotificationStream("stream/2"));
  BOOST_CHECK_THROW(dispatcher.addNotificationStream("stream"), std::out_of_range);

  BOOST_CHECK_NO_THROW(dispatcher.addTopPrefix("/root/1"));
  BOOST_CHECK_NO_THROW(dispatcher.addTopPrefix("/root/2"));
  BOOST_CHECK_THROW(dispatcher.addTopPrefix("/root"), std::out_of_range);

  BOOST_CHECK_THROW(dispatcher
                    .addControlCommand<VoidParameters>("test/3", makeAcceptAllAuthorization(),
                                                       tautology, nop),
                    std::domain_error);

  BOOST_CHECK_THROW(dispatcher.addStatusDataset("status/3", makeAcceptAllAuthorization(), nop),
                    std::domain_error);

  BOOST_CHECK_THROW(dispatcher.addNotificationStream("stream/3"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(AddRemoveTopPrefix)
{
  std::map<std::string, size_t> nCallbackCalled;
  dispatcher
    .addControlCommand<VoidParameters>("test/1", makeAcceptAllAuthorization(),
                                       [] (auto&&...) { return true; },
                                       [&nCallbackCalled] (auto&&...) { ++nCallbackCalled["test/1"]; });

  dispatcher
    .addControlCommand<VoidParameters>("test/2", makeAcceptAllAuthorization(),
                                       [] (auto&&...) { return true; },
                                       [&nCallbackCalled] (auto&&...) { ++nCallbackCalled["test/2"]; });

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

BOOST_AUTO_TEST_CASE(ControlCommandOld,
  * ut::description("test old-style ControlCommand registration"))
{
  size_t nCallbackCalled = 0;
  auto handler = [&nCallbackCalled] (auto&&...) { ++nCallbackCalled; };
  dispatcher.addControlCommand<VoidParameters>("test", makeTestAuthorization(),
                                               [] (auto&&...) { return true; }, std::move(handler));

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 0);

  face.receive(*makeInterest("/root/test/%80%00")); // returns 403
  face.receive(*makeInterest("/root/test/%80%00/invalid")); // returns 403
  face.receive(*makeInterest("/root/test/%80%00/silent")); // silently ignored
  face.receive(*makeInterest("/root/test/.../invalid")); // silently ignored (wrong format)
  face.receive(*makeInterest("/root/test/.../valid"));  // silently ignored (wrong format)
  advanceClocks(1_ms, 20);
  BOOST_CHECK_EQUAL(nCallbackCalled, 0);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 2);

  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 403);
  BOOST_CHECK_EQUAL(face.sentData[1].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[1].getContent().blockFromValue()).getCode(), 403);

  face.receive(*makeInterest("/root/test/%80%00/valid"));
  advanceClocks(1_ms, 10);
  BOOST_CHECK_EQUAL(nCallbackCalled, 1);
}

BOOST_AUTO_TEST_CASE(ControlCommandNew,
  * ut::description("test new-style ControlCommand registration"))
{
  size_t nHandlerCalled = 0;
  auto handler = [&nHandlerCalled] (auto&&...) { ++nHandlerCalled; };

  // test addControlCommand()
  dispatcher.addControlCommand<nfd::FaceCreateCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::FaceUpdateCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::FaceDestroyCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::FibAddNextHopCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::FibRemoveNextHopCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::CsConfigCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::CsEraseCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::StrategyChoiceSetCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::StrategyChoiceUnsetCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::RibRegisterCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addControlCommand<nfd::RibUnregisterCommand>(makeAcceptAllAuthorization(), handler);

  BOOST_CHECK_THROW(dispatcher.addControlCommand<nfd::CsConfigCommand>(makeAcceptAllAuthorization(),
                                                                       [] (auto&&...) {}),
                    std::out_of_range);

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 0);

  BOOST_CHECK_THROW(dispatcher.addControlCommand<nfd::CsConfigCommand>(makeAcceptAllAuthorization(),
                                                                       [] (auto&&...) {}),
                    std::domain_error);

  // we pick FaceDestroyCommand as an example for the following tests

  // malformed request (missing ControlParameters) => silently ignored
  auto baseName = Name("/root").append(nfd::FaceDestroyCommand::getName());
  auto interest = makeInterest(baseName);
  face.receive(*interest);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nHandlerCalled, 0);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  // ControlParameters present but invalid (missing required field) => reply with error 400
  nfd::ControlParameters params;
  interest->setName(Name(baseName).append(params.wireEncode()));
  face.receive(*interest);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nHandlerCalled, 0);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 400);

  // valid request
  params.setFaceId(42);
  interest->setName(Name(baseName).append(params.wireEncode()));
  face.receive(*interest);
  advanceClocks(1_ms);
  BOOST_CHECK_EQUAL(nHandlerCalled, 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 1);
}

BOOST_AUTO_TEST_CASE(ControlCommandResponse)
{
  auto handler = [] (const Name& prefix, const Interest& interest,
                     const ControlParametersBase&, const CommandContinuation& done) {
    BOOST_CHECK_EQUAL(prefix, "/root");
    BOOST_CHECK_EQUAL(interest.getName().getPrefix(3),
                      Name("/root").append(nfd::CsConfigCommand::getName()));
    done(ControlResponse(42, "the answer"));
  };

  // use CsConfigCommand as an example
  dispatcher.addControlCommand<nfd::CsConfigCommand>(makeAcceptAllAuthorization(), handler);
  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 0);

  nfd::ControlParameters params;
  auto interest = makeInterest(Name("/root")
                               .append(nfd::CsConfigCommand::getName())
                               .append(params.wireEncode()));
  face.receive(*interest);
  advanceClocks(1_ms, 10);

  BOOST_REQUIRE_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Blob);
  ControlResponse resp(face.sentData[0].getContent().blockFromValue());
  BOOST_CHECK_EQUAL(resp.getCode(), 42);
  BOOST_CHECK_EQUAL(resp.getText(), "the answer");
}

class StatefulParameters : public ControlParametersBase
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
    return {};
  }

  void
  wireDecode(const Block&) final
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

BOOST_AUTO_TEST_CASE(ControlCommandAsyncAuthorization,
  * ut::description("test for bug #4059"))
{
  AcceptContinuation authorizationAccept;
  auto authorization = [&authorizationAccept] (const Name&, const Interest&, const ControlParametersBase*,
                                               AcceptContinuation accept, RejectContinuation) {
    authorizationAccept = std::move(accept);
  };

  auto validateParameters = [] (const ControlParametersBase& params) {
    return dynamic_cast<const StatefulParameters&>(params).check();
  };

  size_t nCallbackCalled = 0;
  dispatcher.addControlCommand<StatefulParameters>("test", authorization, validateParameters,
                                                   [&nCallbackCalled] (auto&&...) { ++nCallbackCalled; });

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
  const Block smallBlock({0x81, 0x01, 0x01});
  const Block largeBlock = [] {
    Block b(129, std::make_shared<const Buffer>(3000));
    b.encode();
    return b;
  }();

  dispatcher.addStatusDataset("test/small",
                              makeTestAuthorization(),
                              [&smallBlock] (const Name&, const Interest&,
                                             StatusDatasetContext& context) {
                                context.append(smallBlock);
                                context.append(smallBlock);
                                context.append(smallBlock);
                                context.end();
                              });

  dispatcher.addStatusDataset("test/large",
                              makeTestAuthorization(),
                              [&largeBlock] (const Name&, const Interest&,
                                             StatusDatasetContext& context) {
                                context.append(largeBlock);
                                context.append(largeBlock);
                                context.append(largeBlock);
                                context.end();
                              });

  dispatcher.addStatusDataset("test/reject",
                              makeTestAuthorization(),
                              [] (const Name&, const Interest&, StatusDatasetContext& context) {
                                context.reject();
                              });

  dispatcher.addTopPrefix("/root");
  advanceClocks(1_ms);
  face.sentData.clear();

  face.receive(*makeInterest("/root/test/small/%80%00")); // returns 403
  face.receive(*makeInterest("/root/test/small/%80%00/invalid")); // returns 403
  face.receive(*makeInterest("/root/test/small/%80%00/silent")); // silently ignored
  advanceClocks(1_ms, 20);

  BOOST_REQUIRE_EQUAL(face.sentData.size(), 2);
  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 403);
  BOOST_CHECK_EQUAL(face.sentData[1].getContentType(), tlv::ContentType_Blob);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[1].getContent().blockFromValue()).getCode(), 403);

  face.sentData.clear();

  auto interestSmall = *makeInterest("/root/test/small/valid", true);
  face.receive(interestSmall);
  advanceClocks(1_ms, 10);

  // one data packet is generated and sent to both places
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(storage.size(), 1);

  auto fetchedData = storage.find(interestSmall);
  BOOST_REQUIRE(fetchedData != nullptr);
  BOOST_CHECK_EQUAL(face.sentData[0].wireEncode(), fetchedData->wireEncode());

  face.receive(*makeInterest(Name("/root/test/small/valid").appendVersion(10))); // should be ignored
  face.receive(*makeInterest(Name("/root/test/small/valid").appendSegment(20))); // should be ignored
  advanceClocks(1_ms, 10);
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 1);
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
  BOOST_REQUIRE_EQUAL(face.sentData.size(), 1);
  BOOST_REQUIRE_EQUAL(storage.size(), 2);

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
    size_t valueLength = encoder.prependBytes(dataInStorage[1].getContent().value_bytes());
    valueLength += encoder.prependBytes(dataInStorage[0].getContent().value_bytes());
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

  BOOST_REQUIRE_EQUAL(face.sentData.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData[0].getContentType(), tlv::ContentType_Nack);
  BOOST_CHECK_EQUAL(ControlResponse(face.sentData[0].getContent().blockFromValue()).getCode(), 400);
  BOOST_CHECK_EQUAL(storage.size(), 0); // the nack packet will not be inserted into the in-memory storage
}

BOOST_AUTO_TEST_CASE(NotificationStream)
{
  const Block block({0x82, 0x01, 0x02});
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
  BOOST_CHECK_EQUAL(face.sentData[0].getName(), "/root/test/seq=0");
  BOOST_CHECK_EQUAL(face.sentData[1].getName(), "/root/test/seq=1");
  BOOST_CHECK_EQUAL(face.sentData[2].getName(), "/root/test/seq=2");
  BOOST_CHECK_EQUAL(face.sentData[3].getName(), "/root/test/seq=3");

  BOOST_CHECK_EQUAL(face.sentData[0].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(face.sentData[1].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(face.sentData[2].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(face.sentData[3].getContent().blockFromValue(), block);

  // each version of notification will be sent to both places
  std::vector<Data> dataInStorage;
  std::copy(storage.begin(), storage.end(), std::back_inserter(dataInStorage));
  BOOST_REQUIRE_EQUAL(dataInStorage.size(), 4);
  BOOST_CHECK_EQUAL(dataInStorage[0].getName(), "/root/test/seq=0");
  BOOST_CHECK_EQUAL(dataInStorage[1].getName(), "/root/test/seq=1");
  BOOST_CHECK_EQUAL(dataInStorage[2].getName(), "/root/test/seq=2");
  BOOST_CHECK_EQUAL(dataInStorage[3].getName(), "/root/test/seq=3");

  BOOST_CHECK_EQUAL(dataInStorage[0].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(dataInStorage[1].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(dataInStorage[2].getContent().blockFromValue(), block);
  BOOST_CHECK_EQUAL(dataInStorage[3].getContent().blockFromValue(), block);
}

BOOST_AUTO_TEST_SUITE_END() // TestDispatcher
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace ndn::tests
