/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "../face.hpp"

// NRD
#include "nrd-controller.hpp"
#include "nrd-prefix-reg-options.hpp"

// NFD
#include "nfd-control-response.hpp"

namespace ndn {
namespace nrd {

Controller::Controller(Face& face)
  : m_face(face)
{
}

void
Controller::selfRegisterPrefix(const Name& prefixToRegister,
                               const SuccessCallback& onSuccess,
                               const FailCallback&    onFail)
{
  startCommand("register",
               PrefixRegOptions()
                 .setName(prefixToRegister)
                 .setFaceId(0) // self-registration
                 .setCost(0),
               bind(onSuccess), onFail);
}

void
Controller::selfDeregisterPrefix(const Name& prefixToRegister,
                                 const SuccessCallback& onSuccess,
                                 const FailCallback&    onFail)
{
  startCommand("unregister",
               PrefixRegOptions()
                 .setName(prefixToRegister)
                 .setFaceId(0), // self-registration
               bind(onSuccess), onFail);
}

void
Controller::registerPrefix(const PrefixRegOptions& options,
                           const CommandSucceedCallback& onSuccess,
                           const FailCallback& onFail)
{
  startCommand("register", options, onSuccess, onFail);
}

void
Controller::unregisterPrefix(const PrefixRegOptions& options,
                             const CommandSucceedCallback& onSuccess,
                             const FailCallback& onFail)
{
  startCommand("unregister", options, onSuccess, onFail);
}

void
Controller::advertisePrefix(const PrefixRegOptions& options,
                            const CommandSucceedCallback& onSuccess,
                            const FailCallback& onFail)
{
  startCommand("advertise", options, onSuccess, onFail);
}

void
Controller::withdrawPrefix(const PrefixRegOptions& options,
                            const CommandSucceedCallback& onSuccess,
                            const FailCallback& onFail)
{
  startCommand("withdraw", options, onSuccess, onFail);
}

void
Controller::startCommand(const std::string& command,
                         const PrefixRegOptions& options,
                         const CommandSucceedCallback& onSuccess,
                         const FailCallback& onFail)
{
  Name commandInterestName("/localhost/nrd");
  commandInterestName
    .append(command)
    .append(options.wireEncode());

  Interest commandInterest(commandInterestName);
  m_commandInterestGenerator.generate(commandInterest);

  m_face.expressInterest(commandInterest,
                         bind(&Controller::processCommandResponse, this, _2,
                              onSuccess, onFail),
                         bind(onFail, "Command Interest timed out"));
}

void
Controller::processCommandResponse(Data& data,
                                   const CommandSucceedCallback& onSuccess,
                                   const FailCallback& onFail)
{
  /// \todo Add validation of incoming Data

  try
    {
      nfd::ControlResponse response(data.getContent().blockFromValue());
      if (response.getCode() != 200)
        return onFail(response.getText());

      PrefixRegOptions options(response.getBody());
      return onSuccess(options);
    }
  catch(ndn::Tlv::Error& e)
    {
      if (static_cast<bool>(onFail))
        return onFail(e.what());
    }
}

} // namespace nrd
} // namespace ndn
