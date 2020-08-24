/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#include "ndn-cxx/security/validator-config.hpp"
#include "ndn-cxx/security/certificate-fetcher-from-network.hpp"

namespace ndn {
namespace security {

ValidatorConfig::ValidatorConfig(std::unique_ptr<CertificateFetcher> fetcher,
                                 const CommandInterestOptions& ciOptions,
                                 const SignedInterestOptions& siOptions)
  : Validator(make_unique<ValidationPolicySignedInterest>(
                make_unique<ValidationPolicyCommandInterest>(
                  make_unique<ValidationPolicyConfig>(),
                  ciOptions),
                siOptions),
              std::move(fetcher))
  , m_policyConfig(static_cast<ValidationPolicyConfig&>(getPolicy().getInnerPolicy().getInnerPolicy()))
{
}

ValidatorConfig::ValidatorConfig(Face& face,
                                 const CommandInterestOptions& ciOptions,
                                 const SignedInterestOptions& siOptions)
  : ValidatorConfig(make_unique<CertificateFetcherFromNetwork>(face), ciOptions, siOptions)
{
}

void
ValidatorConfig::load(const std::string& filename)
{
  m_policyConfig.load(filename);
}

void
ValidatorConfig::load(const std::string& input, const std::string& filename)
{
  m_policyConfig.load(input, filename);
}

void
ValidatorConfig::load(std::istream& input, const std::string& filename)
{
  m_policyConfig.load(input, filename);
}

void
ValidatorConfig::load(const validator_config::ConfigSection& configSection,
                      const std::string& filename)
{
  m_policyConfig.load(configSection, filename);
}

} // namespace security
} // namespace ndn
