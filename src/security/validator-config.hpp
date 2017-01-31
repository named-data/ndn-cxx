/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_SECURITY_VALIDATOR_CONFIG_HPP
#define NDN_SECURITY_VALIDATOR_CONFIG_HPP

#include "v2/validator.hpp"
#include "v2/validation-policy-command-interest.hpp"
#include "v2/validation-policy-config.hpp"

namespace ndn {
namespace security {

/**
 * @brief Helper for validator that uses CommandInterest + Config policy and NetworkFetcher
 */
class ValidatorConfig : public v2::Validator
{
public:
  using v2::Validator::Validator;
  using Options = v2::ValidationPolicyCommandInterest::Options;

  explicit
  ValidatorConfig(std::unique_ptr<v2::CertificateFetcher> fetcher, const Options& options = Options());

  explicit
  ValidatorConfig(Face& face, const Options& options = Options());

public: // helpers for ValidationPolicyConfig
  void
  load(const std::string& filename);

  void
  load(const std::string& input, const std::string& filename);

  void
  load(std::istream& input, const std::string& filename);

  void
  load(const v2::validator_config::ConfigSection& configSection,
       const std::string& filename);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  v2::ValidationPolicyConfig& m_policyConfig;
};

} // namespace security

using security::ValidatorConfig;

} // namespace ndn

#endif // NDN_SECURITY_VALIDATOR_CONFIG_HPP
