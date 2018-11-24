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

#include "ndn-cxx/mgmt/nfd/controller.hpp"
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/security/v2/key-chain.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace nfd {

using ndn::util::SegmentFetcher;

const uint32_t Controller::ERROR_TIMEOUT = 10060; // WinSock ESAETIMEDOUT
const uint32_t Controller::ERROR_NACK = 10800; // 10000 + TLV-TYPE of Nack header
const uint32_t Controller::ERROR_VALIDATION = 10021; // 10000 + TLS1_ALERT_DECRYPTION_FAILED
const uint32_t Controller::ERROR_SERVER = 500;
const uint32_t Controller::ERROR_LBOUND = 400;

Controller::Controller(Face& face, KeyChain& keyChain, security::v2::Validator& validator)
  : m_face(face)
  , m_keyChain(keyChain)
  , m_validator(validator)
  , m_signer(keyChain)
{
}

Controller::~Controller()
{
  for (const auto& sp : m_fetchers) {
    sp->stop();
  }
}

void
Controller::startCommand(const shared_ptr<ControlCommand>& command,
                         const ControlParameters& parameters,
                         const CommandSucceedCallback& onSuccess,
                         const CommandFailCallback& onFailure,
                         const CommandOptions& options)
{
  Name requestName = command->getRequestName(options.getPrefix(), parameters);
  Interest interest = m_signer.makeCommandInterest(requestName, options.getSigningInfo());
  interest.setInterestLifetime(options.getTimeout());

  m_face.expressInterest(interest,
    [=] (const Interest&, const Data& data) {
      processCommandResponse(data, command, onSuccess, onFailure);
    },
    [=] (const Interest&, const lp::Nack&) {
      if (onFailure)
        onFailure(ControlResponse(Controller::ERROR_NACK, "network Nack received"));
    },
    [=] (const Interest&) {
      if (onFailure)
        onFailure(ControlResponse(Controller::ERROR_TIMEOUT, "request timed out"));
    });
}

void
Controller::processCommandResponse(const Data& data,
                                   const shared_ptr<ControlCommand>& command,
                                   const CommandSucceedCallback& onSuccess,
                                   const CommandFailCallback& onFailure)
{
  m_validator.validate(data,
    [=] (const Data& data) {
      processValidatedCommandResponse(data, command, onSuccess, onFailure);
    },
    [=] (const Data&, const auto& error) {
      if (onFailure)
        onFailure(ControlResponse(ERROR_VALIDATION, boost::lexical_cast<std::string>(error)));
    }
  );
}

void
Controller::processValidatedCommandResponse(const Data& data,
                                            const shared_ptr<ControlCommand>& command,
                                            const CommandSucceedCallback& onSuccess,
                                            const CommandFailCallback& onFailure)
{
  ControlResponse response;
  try {
    response.wireDecode(data.getContent().blockFromValue());
  }
  catch (const tlv::Error& e) {
    if (onFailure)
      onFailure(ControlResponse(ERROR_SERVER, e.what()));
    return;
  }

  uint32_t code = response.getCode();
  if (code >= ERROR_LBOUND) {
    if (onFailure)
      onFailure(response);
    return;
  }

  ControlParameters parameters;
  try {
    parameters.wireDecode(response.getBody());
  }
  catch (const tlv::Error& e) {
    if (onFailure)
      onFailure(ControlResponse(ERROR_SERVER, e.what()));
    return;
  }

  try {
    command->validateResponse(parameters);
  }
  catch (const ControlCommand::ArgumentError& e) {
    if (onFailure)
      onFailure(ControlResponse(ERROR_SERVER, e.what()));
    return;
  }

  if (onSuccess)
    onSuccess(parameters);
}

void
Controller::fetchDataset(const Name& prefix,
                         const std::function<void(ConstBufferPtr)>& processResponse,
                         const DatasetFailCallback& onFailure,
                         const CommandOptions& options)
{
  SegmentFetcher::Options fetcherOptions;
  fetcherOptions.maxTimeout = options.getTimeout();

  auto fetcher = SegmentFetcher::start(m_face, Interest(prefix), m_validator, fetcherOptions);
  if (processResponse) {
    fetcher->onComplete.connect(processResponse);
  }
  if (onFailure) {
    fetcher->onError.connect([=] (uint32_t code, const std::string& msg) {
      processDatasetFetchError(onFailure, code, msg);
    });
  }

  auto it = m_fetchers.insert(fetcher).first;
  fetcher->onComplete.connect([this, it] (ConstBufferPtr) { m_fetchers.erase(it); });
  fetcher->onError.connect([this, it] (uint32_t, const std::string&) { m_fetchers.erase(it); });
}

void
Controller::processDatasetFetchError(const DatasetFailCallback& onFailure,
                                     uint32_t code, std::string msg)
{
  BOOST_ASSERT(onFailure);

  switch (static_cast<SegmentFetcher::ErrorCode>(code)) {
    // It's intentional to cast as SegmentFetcher::ErrorCode, and to not have a 'default' clause.
    // This forces the switch statement to handle every defined SegmentFetcher::ErrorCode,
    // and breaks compilation if it does not.
    case SegmentFetcher::ErrorCode::INTEREST_TIMEOUT:
      onFailure(ERROR_TIMEOUT, msg);
      break;
    case SegmentFetcher::ErrorCode::DATA_HAS_NO_SEGMENT:
    case SegmentFetcher::ErrorCode::FINALBLOCKID_NOT_SEGMENT:
      onFailure(ERROR_SERVER, msg);
      break;
    case SegmentFetcher::ErrorCode::SEGMENT_VALIDATION_FAIL:
      /// \todo When SegmentFetcher exposes validator error code, Controller::ERROR_VALIDATION
      ///       should be replaced with a range that corresponds to validator error codes.
      onFailure(ERROR_VALIDATION, msg);
      break;
    case SegmentFetcher::ErrorCode::NACK_ERROR:
      onFailure(ERROR_NACK, msg);
      break;
  }
}

} // namespace nfd
} // namespace ndn
