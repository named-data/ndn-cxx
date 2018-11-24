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

#ifndef NDN_MGMT_NFD_CONTROLLER_HPP
#define NDN_MGMT_NFD_CONTROLLER_HPP

#include "ndn-cxx/mgmt/nfd/control-command.hpp"
#include "ndn-cxx/mgmt/nfd/control-response.hpp"
#include "ndn-cxx/mgmt/nfd/status-dataset.hpp"
#include "ndn-cxx/mgmt/nfd/command-options.hpp"
#include "ndn-cxx/security/command-interest-signer.hpp"
#include "ndn-cxx/security/validator-null.hpp"
#include "ndn-cxx/security/v2/key-chain.hpp"
#include "ndn-cxx/security/v2/validator.hpp"
#include "ndn-cxx/util/segment-fetcher.hpp"

namespace ndn {

class Face;

namespace nfd {

/**
 * \defgroup management Management
 * \brief Classes and data structures to manage NDN forwarder
 */

/**
 * \ingroup management
 * \brief NFD Management protocol client
 * \sa https://redmine.named-data.net/projects/nfd/wiki/Management
 */
class Controller : noncopyable
{
public:
  /** \brief a callback on command success
   */
  using CommandSucceedCallback = function<void(const ControlParameters&)>;

  /** \brief a callback on command failure
   */
  using CommandFailCallback = function<void(const ControlResponse&)>;

  /** \brief a callback on dataset retrieval failure
   */
  using DatasetFailCallback = function<void(uint32_t code, const std::string& reason)>;

  /** \brief construct a Controller that uses face for transport,
   *         and uses the passed KeyChain to sign commands
   */
  Controller(Face& face, KeyChain& keyChain,
             security::v2::Validator& validator = security::getAcceptAllValidator());

  ~Controller();

  /** \brief start command execution
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const CommandOptions& options = CommandOptions())
  {
    startCommand(make_shared<Command>(), parameters, onSuccess, onFailure, options);
  }

  /** \brief start dataset fetching
   */
  template<typename Dataset>
  std::enable_if_t<std::is_default_constructible<Dataset>::value>
  fetch(const std::function<void(typename Dataset::ResultType)>& onSuccess,
        const DatasetFailCallback& onFailure,
        const CommandOptions& options = CommandOptions())
  {
    fetchDataset(make_shared<Dataset>(), onSuccess, onFailure, options);
  }

  /** \brief start dataset fetching
   */
  template<typename Dataset, typename ParamType = typename Dataset::ParamType>
  void
  fetch(const ParamType& param,
        const std::function<void(typename Dataset::ResultType)>& onSuccess,
        const DatasetFailCallback& onFailure,
        const CommandOptions& options = CommandOptions())
  {
    fetchDataset(make_shared<Dataset>(param), onSuccess, onFailure, options);
  }

private:
  void
  startCommand(const shared_ptr<ControlCommand>& command,
               const ControlParameters& parameters,
               const CommandSucceedCallback& onSuccess,
               const CommandFailCallback& onFailure,
               const CommandOptions& options);

  void
  processCommandResponse(const Data& data,
                         const shared_ptr<ControlCommand>& command,
                         const CommandSucceedCallback& onSuccess,
                         const CommandFailCallback& onFailure);

  void
  processValidatedCommandResponse(const Data& data,
                                  const shared_ptr<ControlCommand>& command,
                                  const CommandSucceedCallback& onSuccess,
                                  const CommandFailCallback& onFailure);

  template<typename Dataset>
  void
  fetchDataset(shared_ptr<Dataset> dataset,
               const std::function<void(typename Dataset::ResultType)>& onSuccess,
               const DatasetFailCallback& onFailure,
               const CommandOptions& options);

  void
  fetchDataset(const Name& prefix,
               const std::function<void(ConstBufferPtr)>& processResponse,
               const DatasetFailCallback& onFailure,
               const CommandOptions& options);

  template<typename Dataset>
  void
  processDatasetResponse(shared_ptr<Dataset> dataset,
                         const std::function<void(typename Dataset::ResultType)>& onSuccess,
                         const DatasetFailCallback& onFailure,
                         ConstBufferPtr payload);

  void
  processDatasetFetchError(const DatasetFailCallback& onFailure, uint32_t code, std::string msg);

public:
  /** \brief error code for timeout
   */
  static const uint32_t ERROR_TIMEOUT;

  /** \brief error code for network Nack
   */
  static const uint32_t ERROR_NACK;

  /** \brief error code for response validation failure
   */
  static const uint32_t ERROR_VALIDATION;

  /** \brief error code for server error
   */
  static const uint32_t ERROR_SERVER;

  /** \brief inclusive lower bound of error codes
   */
  static const uint32_t ERROR_LBOUND;

protected:
  Face& m_face;
  KeyChain& m_keyChain;
  security::v2::Validator& m_validator;
  security::CommandInterestSigner m_signer;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PROTECTED:
  std::set<shared_ptr<util::SegmentFetcher>> m_fetchers;
};

template<typename Dataset>
void
Controller::fetchDataset(shared_ptr<Dataset> dataset,
                         const std::function<void(typename Dataset::ResultType)>& onSuccess,
                         const DatasetFailCallback& onFailure,
                         const CommandOptions& options)
{
  Name prefix = dataset->getDatasetPrefix(options.getPrefix());
  fetchDataset(prefix,
    [=, d = std::move(dataset)] (ConstBufferPtr p) {
      processDatasetResponse(std::move(d), onSuccess, onFailure, std::move(p));
    },
    onFailure, options);
}

template<typename Dataset>
void
Controller::processDatasetResponse(shared_ptr<Dataset> dataset,
                                   const std::function<void(typename Dataset::ResultType)>& onSuccess,
                                   const DatasetFailCallback& onFailure,
                                   ConstBufferPtr payload)
{
  typename Dataset::ResultType result;

  try {
    result = dataset->parseResult(std::move(payload));
  }
  catch (const tlv::Error& e) {
    if (onFailure)
      onFailure(ERROR_SERVER, e.what());
    return;
  }

  if (onSuccess)
    onSuccess(result);
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_CONTROLLER_HPP
