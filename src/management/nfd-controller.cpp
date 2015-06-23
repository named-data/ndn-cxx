/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "nfd-controller.hpp"
#include "nfd-control-response.hpp"

namespace ndn {
namespace nfd {

const uint32_t Controller::ERROR_TIMEOUT = 10060;
const uint32_t Controller::ERROR_SERVER = 500;
const uint32_t Controller::ERROR_LBOUND = 400;

Controller::Controller(Face& face, KeyChain& keyChain)
  : m_face(face)
  , m_keyChain(keyChain)
{
}

void
Controller::startCommand(const shared_ptr<ControlCommand>& command,
                         const ControlParameters& parameters,
                         const CommandSucceedCallback& onSuccess,
                         const CommandFailCallback& onFailure,
                         const CommandOptions& options)
{
  Name requestName = command->getRequestName(options.getPrefix(), parameters);
  Interest interest(requestName);
  interest.setInterestLifetime(options.getTimeout());
  m_keyChain.sign(interest, options.getSigningInfo());

  m_face.expressInterest(interest,
                         bind(&Controller::processCommandResponse, this, _2,
                              command, onSuccess, onFailure),
                         bind(onFailure, ERROR_TIMEOUT, "request timed out"));
}

void
Controller::processCommandResponse(const Data& data,
                                   const shared_ptr<ControlCommand>& command,
                                   const CommandSucceedCallback& onSuccess,
                                   const CommandFailCallback& onFailure)
{
  /// \todo verify Data signature

  ControlResponse response;
  try {
    response.wireDecode(data.getContent().blockFromValue());
  }
  catch (tlv::Error& e) {
    if (static_cast<bool>(onFailure))
      onFailure(ERROR_SERVER, e.what());
    return;
  }

  uint32_t code = response.getCode();
  if (code >= ERROR_LBOUND) {
    if (static_cast<bool>(onFailure))
      onFailure(code, response.getText());
    return;
  }

  ControlParameters parameters;
  try {
    parameters.wireDecode(response.getBody());
  }
  catch (tlv::Error& e) {
    if (static_cast<bool>(onFailure))
      onFailure(ERROR_SERVER, e.what());
    return;
  }

  try {
    command->validateResponse(parameters);
  }
  catch (ControlCommand::ArgumentError& e) {
    if (static_cast<bool>(onFailure))
      onFailure(ERROR_SERVER, e.what());
    return;
  }

  if (static_cast<bool>(onSuccess))
    onSuccess(parameters);
}

} // namespace nfd
} // namespace ndn
