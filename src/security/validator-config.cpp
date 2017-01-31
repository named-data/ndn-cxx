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

#include "validator-config.hpp"
#include "v2/certificate-fetcher-from-network.hpp"

namespace ndn {
namespace security {

ValidatorConfig::ValidatorConfig(std::unique_ptr<v2::CertificateFetcher> fetcher, const Options& options)
  : v2::Validator(make_unique<v2::ValidationPolicyCommandInterest>(make_unique<v2::ValidationPolicyConfig>(),
                                                                   options),
                  std::move(fetcher))
  , m_policyConfig(static_cast<v2::ValidationPolicyConfig&>(getPolicy().getInnerPolicy()))
{
}

ValidatorConfig::ValidatorConfig(Face& face, const Options& options)
  : ValidatorConfig(make_unique<v2::CertificateFetcherFromNetwork>(face), options)
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
ValidatorConfig::load(const v2::validator_config::ConfigSection& configSection,
                      const std::string& filename)
{
  m_policyConfig.load(configSection, filename);
}

} // namespace security
} // namespace ndn
