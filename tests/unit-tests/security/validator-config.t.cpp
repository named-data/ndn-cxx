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

#include "security/validator-config.hpp"
#include "security/v2/certificate-fetcher-offline.hpp"
#include "util/dummy-client-face.hpp"

#include "boost-test.hpp"
#include "identity-management-fixture.hpp"
#include "v2/validator-config/common.hpp"

namespace ndn {
namespace security {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestValidatorConfig, IdentityManagementFixture)

// This test only for API, actual tests are in ValidationPolicyConfig and corresponding CertificateFetchers

BOOST_AUTO_TEST_CASE(Construct)
{
  util::DummyClientFace face;

  ValidatorConfig v1(face);
  BOOST_CHECK_EQUAL(v1.m_policyConfig.m_isConfigured, false);

  ValidatorConfig v2(make_unique<v2::CertificateFetcherOffline>());
  BOOST_CHECK_EQUAL(v2.m_policyConfig.m_isConfigured, false);
}

class ValidatorConfigFixture : public IdentityManagementFixture
{
public:
  ValidatorConfigFixture()
    : path(boost::filesystem::path(UNIT_TEST_CONFIG_PATH) / "security" / "validator-config")
    , validator(make_unique<v2::CertificateFetcherOffline>())
  {
    boost::filesystem::create_directories(path);
    config = R"CONF(
        trust-anchor
        {
          type any
        }
      )CONF";
    configFile = (this->path / "config.conf").string();
    std::ofstream f(configFile.c_str());
    f << config;
  }

  ~ValidatorConfigFixture()
  {
    boost::filesystem::remove_all(path);
  }

public:
  const boost::filesystem::path path;
  std::string config;
  std::string configFile;
  ValidatorConfig validator;
};

BOOST_FIXTURE_TEST_SUITE(Loads, ValidatorConfigFixture)

BOOST_AUTO_TEST_CASE(FromFile)
{
  validator.load(configFile);
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);

  // should reload policy
  validator.load(configFile);
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);
}

BOOST_AUTO_TEST_CASE(FromString)
{
  validator.load(config, "config-file-from-string");
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);

  // should reload policy
  validator.load(config, "config-file-from-string");
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);
}

BOOST_AUTO_TEST_CASE(FromIstream)
{
  std::istringstream is(config);
  validator.load(is, "config-file-from-istream");
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);

  // should reload policy
  std::istringstream is2(config);
  validator.load(is2, "config-file-from-istream");
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);
}

BOOST_AUTO_TEST_CASE(FromSection)
{
  validator.load(v2::validator_config::tests::makeSection(config), "config-file-from-section");
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);

  // should reload policy
  validator.load(v2::validator_config::tests::makeSection(config), "config-file-from-section");
  BOOST_CHECK_EQUAL(validator.m_policyConfig.m_isConfigured, true);
}

BOOST_AUTO_TEST_SUITE_END() // Loads

BOOST_AUTO_TEST_SUITE_END() // TestValidatorConfig
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
