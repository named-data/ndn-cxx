/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "nfd-controller.hpp"
#include "nfd-control-response.hpp"
#include "../security/identity-certificate.hpp"

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

  if (static_cast<bool>(onSuccess))
    onSuccess(parameters);
}


} // namespace nfd
} // namespace ndn
