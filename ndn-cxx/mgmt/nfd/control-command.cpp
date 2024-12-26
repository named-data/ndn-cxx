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

#include "ndn-cxx/mgmt/nfd/control-command.hpp"

namespace ndn::nfd {

ControlParametersCommandFormat::ControlParametersCommandFormat()
  : m_required(CONTROL_PARAMETER_UBOUND)
  , m_optional(CONTROL_PARAMETER_UBOUND)
{
}

void
ControlParametersCommandFormat::validate(const ControlParameters& parameters) const
{
  const auto& presentFields = parameters.getPresentFields();

  for (size_t i = 0; i < CONTROL_PARAMETER_UBOUND; ++i) {
    bool isPresent = presentFields[i];
    if (m_required[i]) {
      if (!isPresent) {
        NDN_THROW(ArgumentError(std::string(CONTROL_PARAMETER_FIELD[i]) + " is required but missing"));
      }
    }
    else if (isPresent && !m_optional[i]) {
      NDN_THROW(ArgumentError(std::string(CONTROL_PARAMETER_FIELD[i]) + " is forbidden but present"));
    }
  }

  if (m_optional[CONTROL_PARAMETER_FLAGS] && m_optional[CONTROL_PARAMETER_MASK]) {
    if (parameters.hasFlags() != parameters.hasMask()) {
      NDN_THROW(ArgumentError("Flags must be accompanied by Mask"));
    }
  }
}

void
ControlParametersCommandFormat::encode(Interest& interest, const ControlParameters& params) const
{
  auto name = interest.getName();
  name.append(params.wireEncode());
  interest.setName(name);
}

const FaceCreateCommand::RequestFormat FaceCreateCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_URI)
    .optional(CONTROL_PARAMETER_LOCAL_URI)
    .optional(CONTROL_PARAMETER_FLAGS)
    .optional(CONTROL_PARAMETER_MASK)
    .optional(CONTROL_PARAMETER_FACE_PERSISTENCY)
    .optional(CONTROL_PARAMETER_BASE_CONGESTION_MARKING_INTERVAL)
    .optional(CONTROL_PARAMETER_DEFAULT_CONGESTION_THRESHOLD)
    .optional(CONTROL_PARAMETER_MTU);
const FaceCreateCommand::ResponseFormat FaceCreateCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_FACE_ID)
    .required(CONTROL_PARAMETER_URI)
    .required(CONTROL_PARAMETER_LOCAL_URI)
    .required(CONTROL_PARAMETER_FLAGS)
    .required(CONTROL_PARAMETER_FACE_PERSISTENCY)
    .optional(CONTROL_PARAMETER_BASE_CONGESTION_MARKING_INTERVAL)
    .optional(CONTROL_PARAMETER_DEFAULT_CONGESTION_THRESHOLD)
    .optional(CONTROL_PARAMETER_MTU);

void
FaceCreateCommand::applyDefaultsToRequestImpl(ControlParameters& parameters)
{
  if (!parameters.hasFacePersistency()) {
    parameters.setFacePersistency(FacePersistency::FACE_PERSISTENCY_PERSISTENT);
  }
}

void
FaceCreateCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

const FaceUpdateCommand::RequestFormat FaceUpdateCommand::s_requestFormat =
    RequestFormat()
    .optional(CONTROL_PARAMETER_FACE_ID)
    .optional(CONTROL_PARAMETER_FLAGS)
    .optional(CONTROL_PARAMETER_MASK)
    .optional(CONTROL_PARAMETER_FACE_PERSISTENCY)
    .optional(CONTROL_PARAMETER_BASE_CONGESTION_MARKING_INTERVAL)
    .optional(CONTROL_PARAMETER_DEFAULT_CONGESTION_THRESHOLD)
    .optional(CONTROL_PARAMETER_MTU);
const FaceUpdateCommand::ResponseFormat FaceUpdateCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_FACE_ID)
    .required(CONTROL_PARAMETER_FLAGS)
    .required(CONTROL_PARAMETER_FACE_PERSISTENCY)
    .optional(CONTROL_PARAMETER_BASE_CONGESTION_MARKING_INTERVAL)
    .optional(CONTROL_PARAMETER_DEFAULT_CONGESTION_THRESHOLD)
    .optional(CONTROL_PARAMETER_MTU);

void
FaceUpdateCommand::applyDefaultsToRequestImpl(ControlParameters& parameters)
{
  if (!parameters.hasFaceId()) {
    parameters.setFaceId(0);
  }
}

void
FaceUpdateCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

const FaceDestroyCommand::RequestFormat FaceDestroyCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_FACE_ID);
const FaceDestroyCommand::ResponseFormat FaceDestroyCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_FACE_ID);

void
FaceDestroyCommand::validateRequestImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

void
FaceDestroyCommand::validateResponseImpl(const ControlParameters& parameters)
{
  validateRequestImpl(parameters);
}

const FibAddNextHopCommand::RequestFormat FibAddNextHopCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME)
    .optional(CONTROL_PARAMETER_FACE_ID)
    .optional(CONTROL_PARAMETER_COST);
const FibAddNextHopCommand::ResponseFormat FibAddNextHopCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME)
    .required(CONTROL_PARAMETER_FACE_ID)
    .required(CONTROL_PARAMETER_COST);

void
FibAddNextHopCommand::applyDefaultsToRequestImpl(ControlParameters& parameters)
{
  if (!parameters.hasFaceId()) {
    parameters.setFaceId(0);
  }
  if (!parameters.hasCost()) {
    parameters.setCost(0);
  }
}

void
FibAddNextHopCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

const FibRemoveNextHopCommand::RequestFormat FibRemoveNextHopCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME)
    .optional(CONTROL_PARAMETER_FACE_ID);
const FibRemoveNextHopCommand::ResponseFormat FibRemoveNextHopCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME)
    .required(CONTROL_PARAMETER_FACE_ID);

void
FibRemoveNextHopCommand::applyDefaultsToRequestImpl(ControlParameters& parameters)
{
  if (!parameters.hasFaceId()) {
    parameters.setFaceId(0);
  }
}

void
FibRemoveNextHopCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

const CsConfigCommand::RequestFormat CsConfigCommand::s_requestFormat =
    RequestFormat()
    .optional(CONTROL_PARAMETER_CAPACITY)
    .optional(CONTROL_PARAMETER_FLAGS)
    .optional(CONTROL_PARAMETER_MASK);
const CsConfigCommand::ResponseFormat CsConfigCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_CAPACITY)
    .required(CONTROL_PARAMETER_FLAGS);

const CsEraseCommand::RequestFormat CsEraseCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME)
    .optional(CONTROL_PARAMETER_COUNT);
const CsEraseCommand::ResponseFormat CsEraseCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME)
    .optional(CONTROL_PARAMETER_CAPACITY)
    .required(CONTROL_PARAMETER_COUNT);

void
CsEraseCommand::validateRequestImpl(const ControlParameters& parameters)
{
  if (parameters.hasCount() && parameters.getCount() == 0) {
    NDN_THROW(ArgumentError("Count must be positive"));
  }
}

void
CsEraseCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.hasCapacity() && parameters.getCapacity() == 0) {
    NDN_THROW(ArgumentError("Capacity must be positive"));
  }
}

const StrategyChoiceSetCommand::RequestFormat StrategyChoiceSetCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME)
    .required(CONTROL_PARAMETER_STRATEGY);
const StrategyChoiceSetCommand::ResponseFormat StrategyChoiceSetCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME)
    .required(CONTROL_PARAMETER_STRATEGY);

const StrategyChoiceUnsetCommand::RequestFormat StrategyChoiceUnsetCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME);
const StrategyChoiceUnsetCommand::ResponseFormat StrategyChoiceUnsetCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME);

void
StrategyChoiceUnsetCommand::validateRequestImpl(const ControlParameters& parameters)
{
  if (parameters.getName().empty()) {
    NDN_THROW(ArgumentError("Name must not be ndn:/"));
  }
}

void
StrategyChoiceUnsetCommand::validateResponseImpl(const ControlParameters& parameters)
{
  validateRequestImpl(parameters);
}

const RibRegisterCommand::RequestFormat RibRegisterCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME)
    .optional(CONTROL_PARAMETER_FACE_ID)
    .optional(CONTROL_PARAMETER_ORIGIN)
    .optional(CONTROL_PARAMETER_COST)
    .optional(CONTROL_PARAMETER_FLAGS)
    .optional(CONTROL_PARAMETER_EXPIRATION_PERIOD);
const RibRegisterCommand::ResponseFormat RibRegisterCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME)
    .required(CONTROL_PARAMETER_FACE_ID)
    .required(CONTROL_PARAMETER_ORIGIN)
    .required(CONTROL_PARAMETER_COST)
    .required(CONTROL_PARAMETER_FLAGS)
    .optional(CONTROL_PARAMETER_EXPIRATION_PERIOD);

void
RibRegisterCommand::applyDefaultsToRequestImpl(ControlParameters& parameters)
{
  if (!parameters.hasFaceId()) {
    parameters.setFaceId(0);
  }
  if (!parameters.hasOrigin()) {
    parameters.setOrigin(ROUTE_ORIGIN_APP);
  }
  if (!parameters.hasCost()) {
    parameters.setCost(0);
  }
  if (!parameters.hasFlags()) {
    parameters.setFlags(ROUTE_FLAG_CHILD_INHERIT);
  }
}

void
RibRegisterCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

const RibUnregisterCommand::RequestFormat RibUnregisterCommand::s_requestFormat =
    RequestFormat()
    .required(CONTROL_PARAMETER_NAME)
    .optional(CONTROL_PARAMETER_FACE_ID)
    .optional(CONTROL_PARAMETER_ORIGIN);
const RibUnregisterCommand::ResponseFormat RibUnregisterCommand::s_responseFormat =
    ResponseFormat()
    .required(CONTROL_PARAMETER_NAME)
    .required(CONTROL_PARAMETER_FACE_ID)
    .required(CONTROL_PARAMETER_ORIGIN);

void
RibUnregisterCommand::applyDefaultsToRequestImpl(ControlParameters& parameters)
{
  if (!parameters.hasFaceId()) {
    parameters.setFaceId(0);
  }
  if (!parameters.hasOrigin()) {
    parameters.setOrigin(ROUTE_ORIGIN_APP);
  }
}

void
RibUnregisterCommand::validateResponseImpl(const ControlParameters& parameters)
{
  if (parameters.getFaceId() == INVALID_FACE_ID) {
    NDN_THROW(ArgumentError("FaceId must be valid"));
  }
}

} // namespace ndn::nfd
