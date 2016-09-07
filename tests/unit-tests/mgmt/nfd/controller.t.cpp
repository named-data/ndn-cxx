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

#include "mgmt/nfd/controller.hpp"
#include "mgmt/nfd/control-response.hpp"

#include <boost/tuple/tuple.hpp>

#include "controller-fixture.hpp"
#include "../../make-interest-data.hpp"

namespace ndn {
namespace nfd {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)

class CommandFixture : public ControllerFixture
{
protected:
  CommandFixture()
    : succeedCallback(bind(&CommandFixture::succeed, this, _1))
  {
  }

private:
  void
  succeed(const ControlParameters& parameters)
  {
    succeeds.push_back(parameters);
  }

protected:
  Controller::CommandSucceedCallback succeedCallback;
  std::vector<ControlParameters> succeeds;
};

// This test suite focuses on ControlCommand functionality of Controller.
// Individual commands are tested in nfd-control-command.t.cpp
// StatusDataset functionality is tested in nfd-status-dataset.t.cpp
BOOST_FIXTURE_TEST_SUITE(TestController, CommandFixture)

BOOST_AUTO_TEST_CASE(Success)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters, succeedCallback, commandFailCallback));
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  FaceCreateCommand command;
  BOOST_CHECK(Name("/localhost/nfd/faces/create").isPrefixOf(requestInterest.getName()));
  // 9 components: ndn:/localhost/nfd/faces/create/<parameters>/<signed Interest x4>
  BOOST_REQUIRE_EQUAL(requestInterest.getName().size(), 9);
  ControlParameters request;
  // 4th component: <parameters>
  BOOST_REQUIRE_NO_THROW(request.wireDecode(requestInterest.getName().at(4).blockFromValue()));
  BOOST_CHECK_NO_THROW(command.validateRequest(request));
  BOOST_CHECK_EQUAL(request.getUri(), parameters.getUri());
  BOOST_CHECK_EQUAL(requestInterest.getInterestLifetime(), CommandOptions::DEFAULT_TIMEOUT);

  ControlParameters responseBody;
  responseBody.setUri("tcp4://192.0.2.1:6363")
              .setFaceId(22)
              .setFacePersistency(ndn::nfd::FacePersistency::FACE_PERSISTENCY_PERSISTENT);
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());

  auto responseData = makeData(requestInterest.getName());
  responseData->setContent(responsePayload.wireEncode());
  face.receive(*responseData);

  this->advanceClocks(time::milliseconds(1));

  BOOST_CHECK_EQUAL(failCodes.size(), 0);
  BOOST_REQUIRE_EQUAL(succeeds.size(), 1);
  BOOST_CHECK_EQUAL(succeeds.back().getUri(), responseBody.getUri());
  BOOST_CHECK_EQUAL(succeeds.back().getFaceId(), responseBody.getFaceId());
}

BOOST_AUTO_TEST_CASE(SuccessNoCallback)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  controller.start<FaceCreateCommand>(parameters, nullptr, commandFailCallback);
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  ControlParameters responseBody;
  responseBody.setUri("tcp4://192.0.2.1:6363")
              .setFaceId(22)
              .setFacePersistency(ndn::nfd::FacePersistency::FACE_PERSISTENCY_PERSISTENT);
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());

  auto responseData = makeData(requestInterest.getName());
  responseData->setContent(responsePayload.wireEncode());
  face.receive(*responseData);

  BOOST_CHECK_NO_THROW(this->advanceClocks(time::milliseconds(1)));

  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(OptionsPrefix)
{
  ControlParameters parameters;
  parameters.setName("/ndn/com/example");
  parameters.setFaceId(400);

  CommandOptions options;
  options.setPrefix("/localhop/net/example/router1/nfd");

  BOOST_CHECK_NO_THROW(controller.start<RibRegisterCommand>(
                         parameters, succeedCallback, commandFailCallback, options));
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  FaceCreateCommand command;
  BOOST_CHECK(Name("/localhop/net/example/router1/nfd/rib/register").isPrefixOf(
              requestInterest.getName()));
}

BOOST_AUTO_TEST_CASE(InvalidRequest)
{
  ControlParameters parameters;
  parameters.setName("ndn:/should-not-have-this-field");
  // Uri is missing

  BOOST_CHECK_THROW(controller.start<FaceCreateCommand>(
                      parameters, succeedCallback, commandFailCallback),
                    ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(ValidationFailure)
{
  this->setValidationResult(false);

  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters, succeedCallback, commandFailCallback));
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  ControlParameters responseBody;
  responseBody.setUri("tcp4://192.0.2.1:6363")
              .setFaceId(22)
              .setFacePersistency(ndn::nfd::FacePersistency::FACE_PERSISTENCY_PERSISTENT);
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());

  auto responseData = makeData(requestInterest.getName());
  responseData->setContent(responsePayload.wireEncode());
  face.receive(*responseData);
  this->advanceClocks(time::milliseconds(1));

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_VALIDATION);
}

BOOST_AUTO_TEST_CASE(ErrorCode)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters, succeedCallback, commandFailCallback));
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  ControlResponse responsePayload(401, "Not Authenticated");

  auto responseData = makeData(requestInterest.getName());
  responseData->setContent(responsePayload.wireEncode());
  face.receive(*responseData);
  this->advanceClocks(time::milliseconds(1));

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), 401);
}

BOOST_AUTO_TEST_CASE(InvalidResponse)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters, succeedCallback, commandFailCallback));
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  ControlParameters responseBody;
  responseBody.setUri("tcp4://192.0.2.1:6363")
              .setName("ndn:/should-not-have-this-field");
  // FaceId is missing
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());

  auto responseData = makeData(requestInterest.getName());
  responseData->setContent(responsePayload.wireEncode());
  face.receive(*responseData);
  this->advanceClocks(time::milliseconds(1));

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
}

BOOST_AUTO_TEST_CASE(Nack)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters, succeedCallback, commandFailCallback));
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  auto responseNack = makeNack(requestInterest, lp::NackReason::NO_ROUTE);
  face.receive(responseNack);
  this->advanceClocks(time::milliseconds(1));

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_NACK);
}

BOOST_AUTO_TEST_CASE(Timeout)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  CommandOptions options;
  options.setTimeout(time::milliseconds(50));

  BOOST_CHECK_NO_THROW(controller.start<FaceCreateCommand>(
                         parameters, succeedCallback, commandFailCallback, options));
  this->advanceClocks(time::milliseconds(1), 101); // Face's PIT granularity is 100ms

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_TIMEOUT);
}

BOOST_AUTO_TEST_CASE(FailureNoCallback)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  CommandOptions options;
  options.setTimeout(time::milliseconds(50));

  controller.start<FaceCreateCommand>(parameters, succeedCallback, nullptr, options);
  BOOST_CHECK_NO_THROW(this->advanceClocks(time::milliseconds(100), 10));
  // timeout

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // TestController
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace tests
} // namespace nfd
} // namespace ndn
