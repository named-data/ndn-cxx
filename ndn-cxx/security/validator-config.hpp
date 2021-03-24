/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_VALIDATOR_CONFIG_HPP
#define NDN_CXX_SECURITY_VALIDATOR_CONFIG_HPP

#include "ndn-cxx/security/validator.hpp"
#include "ndn-cxx/security/validation-policy-command-interest.hpp"
#include "ndn-cxx/security/validation-policy-config.hpp"
#include "ndn-cxx/security/validation-policy-signed-interest.hpp"

namespace ndn {
namespace security {

/**
 * @brief Helper for validator that uses SignedInterest + CommandInterest + Config policy and
 *        NetworkFetcher
 */
class ValidatorConfig : public Validator
{
public:
  using Validator::Validator;
  using SignedInterestOptions = ValidationPolicySignedInterest::Options;
  using CommandInterestOptions = ValidationPolicyCommandInterest::Options;

  explicit
  ValidatorConfig(std::unique_ptr<CertificateFetcher> fetcher,
                  const CommandInterestOptions& ciOptions = {},
                  const SignedInterestOptions& siOptions = {});

  explicit
  ValidatorConfig(Face& face,
                  const CommandInterestOptions& ciOptions = {},
                  const SignedInterestOptions& siOptions = {});

public: // helpers for ValidationPolicyConfig
  void
  load(const std::string& filename);

  void
  load(const std::string& input, const std::string& filename);

  void
  load(std::istream& input, const std::string& filename);

  void
  load(const validator_config::ConfigSection& configSection,
       const std::string& filename);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  ValidationPolicyConfig& m_policyConfig;
};

} // namespace security

using security::ValidatorConfig;

} // namespace ndn

#endif // NDN_CXX_SECURITY_VALIDATOR_CONFIG_HPP
