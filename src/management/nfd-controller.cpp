/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "nfd-controller.hpp"
#include "nfd-control-response.hpp"

namespace ndn {
namespace nfd {

Controller::Controller(Face& face)
  : m_face(face)
{
}

void
Controller::processCommandResponse(const Data& data,
                                   const shared_ptr<ControlCommand>& command,
                                   const CommandSucceedCallback& onSuccess,
                                   const CommandFailCallback& onFailure)
{
  /// \todo verify Data signature

  const uint32_t serverErrorCode = 500;

  ControlResponse response;
  try {
    response.wireDecode(data.getContent().blockFromValue());
  }
  catch (ndn::Tlv::Error& e) {
    if (static_cast<bool>(onFailure))
      onFailure(serverErrorCode, e.what());
    return;
  }

  uint32_t code = response.getCode();
  const uint32_t errorCodeLowerBound = 400;
  if (code >= errorCodeLowerBound) {
    if (static_cast<bool>(onFailure))
      onFailure(code, response.getText());
    return;
  }

  ControlParameters parameters;
  try {
    parameters.wireDecode(response.getBody());
  }
  catch (ndn::Tlv::Error& e) {
    if (static_cast<bool>(onFailure))
      onFailure(serverErrorCode, e.what());
    return;
  }

  try {
    command->validateResponse(parameters);
  }
  catch (ControlCommand::ArgumentError& e) {
    if (static_cast<bool>(onFailure))
      onFailure(serverErrorCode, e.what());
    return;
  }

  onSuccess(parameters);
}


void
Controller::selfRegisterPrefix(const Name& prefixToRegister,
                               const SuccessCallback& onSuccess,
                               const FailCallback&    onFail)
{
  const uint32_t selfFaceId = 0;

  ControlParameters parameters;
  parameters.setName(prefixToRegister)
            .setFaceId(selfFaceId);

  this->start<FibAddNextHopCommand>(parameters,
                                    bind(onSuccess),
                                    bind(onFail, _2));
}

void
Controller::selfDeregisterPrefix(const Name& prefixToDeRegister,
                                 const SuccessCallback& onSuccess,
                                 const FailCallback&    onFail)
{
  const uint32_t selfFaceId = 0;

  ControlParameters parameters;
  parameters.setName(prefixToDeRegister)
            .setFaceId(selfFaceId);

  this->start<FibRemoveNextHopCommand>(parameters,
                                       bind(onSuccess),
                                       bind(onFail, _2));
}

void
Controller::fibAddNextHop(const Name& prefix, uint64_t faceId, int cost,
                          const FibCommandSucceedCallback& onSuccess,
                          const FailCallback& onFail)
{
  BOOST_ASSERT(cost >= 0);

  ControlParameters parameters;
  parameters.setName(prefix)
            .setFaceId(faceId)
            .setCost(static_cast<uint64_t>(cost));

  this->start<FibAddNextHopCommand>(parameters,
                                    onSuccess,
                                    bind(onFail, _2));
}

void
Controller::fibRemoveNextHop(const Name& prefix, uint64_t faceId,
                             const FibCommandSucceedCallback& onSuccess,
                             const FailCallback& onFail)
{
  ControlParameters parameters;
  parameters.setName(prefix)
            .setFaceId(faceId);

  this->start<FibRemoveNextHopCommand>(parameters,
                                       onSuccess,
                                       bind(onFail, _2));
}

void
Controller::startFibCommand(const std::string& command,
                            const FibManagementOptions& options,
                            const FibCommandSucceedCallback& onSuccess,
                            const FailCallback& onFail)
{
  if (command == "add-nexthop") {
    this->start<FibAddNextHopCommand>(options,
                                      onSuccess,
                                      bind(onFail, _2));
  }
  else if (command == "remove-nexthop") {
    this->start<FibRemoveNextHopCommand>(options,
                                         onSuccess,
                                         bind(onFail, _2));
  }
  else {
    onFail("unknown command");
  }
}

void
Controller::startFaceCommand(const std::string& command,
                             const FaceManagementOptions& options,
                             const FaceCommandSucceedCallback& onSuccess,
                             const FailCallback& onFail)
{
  if (command == "create") {
    this->start<FaceCreateCommand>(options,
                                   onSuccess,
                                   bind(onFail, _2));
  }
  else if (command == "destroy") {
    this->start<FaceDestroyCommand>(options,
                                    onSuccess,
                                    bind(onFail, _2));
  }
  // enable-local-control and disable-local-control are not in legacy API.
  else {
    onFail("unknown command");
  }
}

void
Controller::startStrategyChoiceCommand(const std::string& command,
                                       const StrategyChoiceOptions& options,
                                       const StrategyChoiceCommandSucceedCallback& onSuccess,
                                       const FailCallback& onFail)
{
  if (command == "set") {
    this->start<StrategyChoiceSetCommand>(options,
                                          onSuccess,
                                          bind(onFail, _2));
  }
  else if (command == "unset") {
    this->start<StrategyChoiceUnsetCommand>(options,
                                            onSuccess,
                                            bind(onFail, _2));
  }
  else {
    onFail("unknown command");
  }
}


} // namespace nfd
} // namespace ndn
