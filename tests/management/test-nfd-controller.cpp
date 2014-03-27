/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-controller.hpp"
#include "management/nfd-control-response.hpp"

#include "../transport/dummy-face.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(ManagementTestNfdController)

class CommandFixture
{
protected:
  CommandFixture()
    : face(makeDummyFace())
    , controller(*face)
    , commandSucceedCallback(bind(&CommandFixture::onCommandSucceed, this, _1))
    , commandFailCallback(bind(&CommandFixture::onCommandFail, this, _1, _2))
  {
  }

private:
  void
  onCommandSucceed(const ControlParameters& parameters)
  {
    commandSucceedHistory.push_back(boost::make_tuple(parameters));
  }

  void
  onCommandFail(uint32_t code, const std::string& reason)
  {
    commandFailHistory.push_back(boost::make_tuple(code, reason));
  }

protected:
  shared_ptr<DummyFace> face;
  Controller controller;
  KeyChain keyChain;

  Controller::CommandSucceedCallback commandSucceedCallback;
  typedef boost::tuple<ControlParameters> CommandSucceedArgs;
  std::vector<CommandSucceedArgs> commandSucceedHistory;

  Controller::CommandFailCallback commandFailCallback;
  typedef boost::tuple<uint32_t,std::string> CommandFailArgs;
  std::vector<CommandFailArgs> commandFailHistory;
};

BOOST_FIXTURE_TEST_CASE(CommandSuccess, CommandFixture)
{
  ControlParameters parameters;
  parameters.setUri("tcp://example.com");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters,
                         commandSucceedCallback,
                         commandFailCallback));
  face->processEvents(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 1);
  const Interest& commandInterest = face->m_sentInterests[0];

  FaceCreateCommand command;
  BOOST_CHECK(command.getPrefix().isPrefixOf(commandInterest.getName()));
  // 9 components: ndn:/localhost/nfd/face/create/<parameters>/<command Interest signature x4>
  BOOST_REQUIRE_EQUAL(commandInterest.getName().size(), 9);
  ControlParameters request;
  // 4th component: <parameters>
  BOOST_REQUIRE_NO_THROW(request.wireDecode(commandInterest.getName().at(4).blockFromValue()));
  BOOST_CHECK_NO_THROW(command.validateRequest(request));
  BOOST_CHECK_EQUAL(request.getUri(), parameters.getUri());

  ControlParameters responseBody;
  responseBody.setUri("tcp4://192.0.2.1:6363")
              .setFaceId(22);
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());

  Data responseData(commandInterest.getName());
  responseData.setContent(responsePayload.wireEncode());
  keyChain.sign(responseData);
  face->receive(responseData);
  face->processEvents(time::milliseconds(1));

  BOOST_CHECK_EQUAL(commandFailHistory.size(), 0);
  BOOST_REQUIRE_EQUAL(commandSucceedHistory.size(), 1);
  const ControlParameters& response = commandSucceedHistory[0].get<0>();
  BOOST_CHECK_EQUAL(response.getUri(), responseBody.getUri());
  BOOST_CHECK_EQUAL(response.getFaceId(), responseBody.getFaceId());
}

BOOST_FIXTURE_TEST_CASE(CommandInvalidRequest, CommandFixture)
{
  ControlParameters parameters;
  parameters.setName("ndn:/should-not-have-this-field");
  // Uri is missing

  BOOST_CHECK_THROW(controller.start<FaceCreateCommand>(
                      parameters,
                      commandSucceedCallback,
                      commandFailCallback),
                    ControlCommand::ArgumentError);
}

BOOST_FIXTURE_TEST_CASE(CommandErrorCode, CommandFixture)
{
  ControlParameters parameters;
  parameters.setUri("tcp://example.com");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters,
                         commandSucceedCallback,
                         commandFailCallback));
  face->processEvents(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 1);
  const Interest& commandInterest = face->m_sentInterests[0];

  ControlResponse responsePayload(401, "Not Authenticated");

  Data responseData(commandInterest.getName());
  responseData.setContent(responsePayload.wireEncode());
  keyChain.sign(responseData);
  face->receive(responseData);
  face->processEvents(time::milliseconds(1));

  BOOST_CHECK_EQUAL(commandSucceedHistory.size(), 0);
  BOOST_REQUIRE_EQUAL(commandFailHistory.size(), 1);
  BOOST_CHECK_EQUAL(commandFailHistory[0].get<0>(), 401);
}

BOOST_FIXTURE_TEST_CASE(CommandInvalidResponse, CommandFixture)
{
  ControlParameters parameters;
  parameters.setUri("tcp://example.com");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters,
                         commandSucceedCallback,
                         commandFailCallback));
  face->processEvents(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face->m_sentInterests.size(), 1);
  const Interest& commandInterest = face->m_sentInterests[0];

  ControlParameters responseBody;
  responseBody.setUri("tcp4://192.0.2.1:6363")
              .setName("ndn:/should-not-have-this-field");
  // FaceId is missing
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());

  Data responseData(commandInterest.getName());
  responseData.setContent(responsePayload.wireEncode());
  keyChain.sign(responseData);
  face->receive(responseData);
  face->processEvents(time::milliseconds(1));

  BOOST_CHECK_EQUAL(commandSucceedHistory.size(), 0);
  BOOST_REQUIRE_EQUAL(commandFailHistory.size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
