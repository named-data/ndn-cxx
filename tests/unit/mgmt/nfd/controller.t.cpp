/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/mgmt/nfd/controller.hpp"
#include "ndn-cxx/mgmt/nfd/control-response.hpp"

#include "tests/test-common.hpp"
#include "tests/unit/mgmt/nfd/controller-fixture.hpp"

namespace ndn::tests {

using namespace ndn::nfd;

BOOST_AUTO_TEST_SUITE(Mgmt)
BOOST_AUTO_TEST_SUITE(Nfd)

class CommandFixture : public ControllerFixture
{
protected:
  void
  respond(const ControlResponse& responsePayload)
  {
    auto responseData = makeData(face.sentInterests.at(0).getName());
    responseData->setContent(responsePayload.wireEncode());
    face.receive(*responseData);
    this->advanceClocks(1_ms);
  }

protected:
  CommandSuccessCallback succeedCallback = [this] (const auto& params) {
    succeeds.push_back(params);
  };
  std::vector<ControlParameters> succeeds;
};

// This test suite focuses on ControlCommand functionality of Controller.
// Individual commands are tested in control-command.t.cpp
// StatusDataset functionality is tested in status-dataset.t.cpp
BOOST_FIXTURE_TEST_SUITE(TestController, CommandFixture)

static ControlParameters
makeFaceCreateResponse()
{
  ControlParameters resp;
  resp.setFaceId(22)
      .setUri("tcp4://192.0.2.1:6363")
      .setLocalUri("tcp4://192.0.2.2:10847")
      .setFacePersistency(ndn::nfd::FacePersistency::FACE_PERSISTENCY_PERSISTENT)
      .setFlags(0x7);
  return resp;
}

BOOST_AUTO_TEST_CASE(Success)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback);
  this->advanceClocks(1_ms);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  BOOST_CHECK(Name("/localhost/nfd/faces/create").isPrefixOf(requestInterest.getName()));
  BOOST_CHECK(requestInterest.isSigned());
  BOOST_CHECK(requestInterest.isParametersDigestValid());
  BOOST_CHECK_EQUAL(requestInterest.getInterestLifetime(), CommandOptions::DEFAULT_TIMEOUT);

  // 6 components: /localhost/nfd/faces/create/<parameters>/params-sha256=...
  BOOST_REQUIRE_EQUAL(requestInterest.getName().size(), 6);
  ControlParameters requestParams(requestInterest.getName()[4].blockFromValue());
  FaceCreateCommand command;
  BOOST_CHECK_NO_THROW(command.validateRequest(requestParams));
  BOOST_CHECK_EQUAL(requestParams.getUri(), parameters.getUri());

  ControlParameters responseBody = makeFaceCreateResponse();
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());
  this->respond(responsePayload);

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
  this->advanceClocks(1_ms);

  ControlParameters responseBody = makeFaceCreateResponse();
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());
  this->respond(responsePayload);

  BOOST_CHECK_EQUAL(failCodes.size(), 0);
}

BOOST_AUTO_TEST_CASE(OptionsPrefix)
{
  ControlParameters parameters;
  parameters.setName("/ndn/com/example");
  parameters.setFaceId(400);

  CommandOptions options;
  options.setPrefix("/localhop/net/example/router1/nfd");

  controller.start<RibRegisterCommand>(parameters, succeedCallback, commandFailCallback, options);
  this->advanceClocks(1_ms);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  FaceCreateCommand command;
  BOOST_CHECK(Name("/localhop/net/example/router1/nfd/rib/register").isPrefixOf(requestInterest.getName()));
  BOOST_CHECK(requestInterest.isSigned());
  BOOST_CHECK(requestInterest.isParametersDigestValid());
}

BOOST_AUTO_TEST_CASE(InvalidRequest)
{
  ControlParameters parameters;
  parameters.setName("ndn:/should-not-have-this-field");
  // Uri is missing

  BOOST_CHECK_THROW(controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback),
                    ControlCommand::ArgumentError);
}

BOOST_AUTO_TEST_CASE(ValidationFailure)
{
  this->setValidationResult(false);

  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback);
  this->advanceClocks(1_ms);

  ControlParameters responseBody = makeFaceCreateResponse();
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());
  this->respond(responsePayload);

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_VALIDATION);
}

BOOST_AUTO_TEST_CASE(ErrorCode)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback);
  this->advanceClocks(1_ms);

  ControlResponse responsePayload(401, "Not Authenticated");
  this->respond(responsePayload);

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), 401);
}

BOOST_AUTO_TEST_CASE(InvalidResponse)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback);
  this->advanceClocks(1_ms);

  ControlParameters responseBody = makeFaceCreateResponse();
  responseBody.unsetFaceId() // FaceId is missing
              .setName("ndn:/should-not-have-this-field");
  ControlResponse responsePayload(201, "created");
  responsePayload.setBody(responseBody.wireEncode());
  this->respond(responsePayload);

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
}

BOOST_AUTO_TEST_CASE(Nack)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback);
  this->advanceClocks(1_ms);

  BOOST_REQUIRE_EQUAL(face.sentInterests.size(), 1);
  const Interest& requestInterest = face.sentInterests[0];

  auto responseNack = makeNack(requestInterest, lp::NackReason::NO_ROUTE);
  face.receive(responseNack);
  this->advanceClocks(1_ms);

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_NACK);
}

BOOST_AUTO_TEST_CASE(Timeout)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  CommandOptions options;
  options.setTimeout(50_ms);

  controller.start<FaceCreateCommand>(parameters, succeedCallback, commandFailCallback, options);
  this->advanceClocks(1_ms); // express Interest
  this->advanceClocks(51_ms); // timeout

  BOOST_REQUIRE_EQUAL(failCodes.size(), 1);
  BOOST_CHECK_EQUAL(failCodes.back(), Controller::ERROR_TIMEOUT);
}

BOOST_AUTO_TEST_CASE(FailureNoCallback)
{
  ControlParameters parameters;
  parameters.setUri("tcp4://192.0.2.1:6363");

  CommandOptions options;
  options.setTimeout(50_ms);

  controller.start<FaceCreateCommand>(parameters, succeedCallback, nullptr, options);
  this->advanceClocks(1_ms); // express Interest
  this->advanceClocks(51_ms); // timeout

  BOOST_CHECK_EQUAL(succeeds.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // TestController
BOOST_AUTO_TEST_SUITE_END() // Nfd
BOOST_AUTO_TEST_SUITE_END() // Mgmt

} // namespace ndn::tests
