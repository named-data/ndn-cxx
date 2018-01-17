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

#include "security/v2/validation-policy-config.hpp"
#include "security/transform/base64-encode.hpp"
#include "security/transform/buffer-source.hpp"
#include "security/transform/stream-sink.hpp"
#include "util/logger.hpp"
#include "util/io.hpp"

#include "boost-test.hpp"
#include "validator-config/common.hpp"
#include "validator-fixture.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace validator_config {
namespace tests {

using namespace ndn::tests;
using namespace ndn::security::v2::tests;


BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_AUTO_TEST_SUITE(TestValidationPolicyConfig)

template<typename Packet>
class PacketName;

template<>
class PacketName<Interest>
{
public:
  static std::string
  getName()
  {
    return "interest";
  }
};

template<>
class PacketName<Data>
{
public:
  static std::string
  getName()
  {
    return "data";
  }
};

template<typename PacketType>
class ValidationPolicyConfigFixture : public HierarchicalValidatorFixture<ValidationPolicyConfig>
{
public:
  ValidationPolicyConfigFixture()
    : path(boost::filesystem::path(UNIT_TEST_CONFIG_PATH) / "security" / "v2" / "validation-policy-config")
  {
    boost::filesystem::create_directories(path);
    baseConfig = R"CONF(
        rule
        {
          id test-rule-id
          for )CONF" + PacketName<Packet>::getName() + R"CONF(
          filter
          {
            type name
            name )CONF" + identity.getName().toUri() + R"CONF(
            relation is-prefix-of
          }
          checker
          {
            type hierarchical
            sig-type rsa-sha256
          }
        }
      )CONF";
  }

  ~ValidationPolicyConfigFixture()
  {
    boost::filesystem::remove_all(path);
  }

public:
  using Packet = PacketType;

  const boost::filesystem::path path;
  std::string baseConfig;
};

template<typename PacketType>
class LoadStringWithFileAnchor : public ValidationPolicyConfigFixture<PacketType>
{
public:
  LoadStringWithFileAnchor()
  {
    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, false);

    this->saveCertificate(this->identity, (this->path / "identity.ndncert").string());
    this->policy.load(this->baseConfig + R"CONF(
        trust-anchor
        {
          type file
          file-name "trust-anchor.ndncert"
        }
      )CONF", (this->path / "test-config").string());

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
    BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, false);
  }
};

template<typename PacketType>
class LoadFileWithFileAnchor : public ValidationPolicyConfigFixture<PacketType>
{
public:
  LoadFileWithFileAnchor()
  {
    std::string configFile = (this->path / "config.conf").string();
    {
      std::ofstream config(configFile.c_str());
      config << this->baseConfig << R"CONF(
        trust-anchor
        {
          type file
          file-name "trust-anchor.ndncert"
        }
      )CONF";
    }
    this->saveCertificate(this->identity, (this->path / "identity.ndncert").string());

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, false);

    this->policy.load(configFile);

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
    BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, false);
  }
};

template<typename PacketType>
class LoadSectionWithFileAnchor : public ValidationPolicyConfigFixture<PacketType>
{
public:
  LoadSectionWithFileAnchor()
  {
    auto section = makeSection(this->baseConfig + R"CONF(
        trust-anchor
        {
          type file
          file-name "trust-anchor.ndncert"
        }
      )CONF");

    this->saveCertificate(this->identity, (this->path / "identity.ndncert").string());

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, false);

    this->policy.load(section, (this->path / "test-config").string());

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
    BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, false);
  }
};

template<typename PacketType>
class LoadStringWithBase64Anchor : public ValidationPolicyConfigFixture<PacketType>
{
public:
  LoadStringWithBase64Anchor()
  {
    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, false);

    std::ostringstream os;
    using namespace ndn::security::transform;
    const auto& cert = this->identity.getDefaultKey().getDefaultCertificate();
    bufferSource(cert.wireEncode().wire(), cert.wireEncode().size()) >> base64Encode(false) >> streamSink(os);

    this->policy.load(this->baseConfig + R"CONF(
        trust-anchor
        {
          type base64
          base64-string ")CONF" + os.str() + R"CONF("
        }
      )CONF", (this->path / "test-config").string());

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
    BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, false);
  }
};

class NoRefresh
{
public:
  static std::string
  getRefreshString()
  {
    return "";
  }
};

class Refresh1h
{
public:
  static std::string
  getRefreshString()
  {
    return "refresh 1h";
  }

  static time::milliseconds
  getRefreshTime()
  {
    return 1_h;
  }
};

class Refresh1m
{
public:
  static std::string
  getRefreshString()
  {
    return "refresh 1m";
  }

  static time::milliseconds
  getRefreshTime()
  {
    return 1_min;
  }
};

class Refresh1s
{
public:
  static std::string
  getRefreshString()
  {
    return "refresh 1s";
  }

  static time::milliseconds
  getRefreshTime()
  {
    return 1_s;
  }
};

template<typename PacketType, typename Refresh = NoRefresh>
class LoadStringWithDirAnchor : public ValidationPolicyConfigFixture<PacketType>
{
public:
  LoadStringWithDirAnchor()
  {
    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, false);

    boost::filesystem::create_directories(this->path / "keys");
    this->saveCertificate(this->identity, (this->path / "keys" / "identity.ndncert").string());

    this->policy.load(this->baseConfig + R"CONF(
        trust-anchor
        {
          type dir
          dir keys
          )CONF" + Refresh::getRefreshString() + R"CONF(
        }
      )CONF", (this->path / "test-config").string());

    BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
    BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, false);
  }
};

using DataPolicies = boost::mpl::vector<LoadStringWithFileAnchor<Data>,
                                        LoadFileWithFileAnchor<Data>,
                                        LoadSectionWithFileAnchor<Data>,
                                        LoadStringWithBase64Anchor<Data>,
                                        LoadStringWithDirAnchor<Data>,
                                        LoadStringWithDirAnchor<Data, Refresh1h>,
                                        LoadStringWithDirAnchor<Data, Refresh1m>,
                                        LoadStringWithDirAnchor<Data, Refresh1s>
                                        >;

using InterestPolicies = boost::mpl::vector<LoadStringWithFileAnchor<Interest>,
                                            LoadFileWithFileAnchor<Interest>,
                                            LoadSectionWithFileAnchor<Interest>,
                                            LoadStringWithBase64Anchor<Interest>,
                                            LoadStringWithDirAnchor<Interest>,
                                            LoadStringWithDirAnchor<Interest, Refresh1h>,
                                            LoadStringWithDirAnchor<Interest, Refresh1m>,
                                            LoadStringWithDirAnchor<Interest, Refresh1s>
                                            >;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateData, Policy, DataPolicies, Policy)
{
  BOOST_CHECK_EQUAL(this->policy.m_dataRules.size(), 1);
  BOOST_CHECK_EQUAL(this->policy.m_interestRules.size(), 0);

  using Packet = typename Policy::Packet;
  Packet unsignedPacket("/Security/V2/ValidatorFixture/Sub1/Sub2/Packet");

  Packet packet = unsignedPacket;
  VALIDATE_FAILURE(packet, "Unsigned");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_FAILURE(packet, "Policy doesn't accept Sha256Digest signature");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->identity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the anchor");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subIdentity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the policy-compliant cert");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->otherIdentity));
  VALIDATE_FAILURE(packet, "Should fail, as signed by the policy-violating cert");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subSelfSignedIdentity));
  VALIDATE_FAILURE(packet, "Should fail, because subSelfSignedIdentity is not a trust anchor");
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateInterest, Policy, InterestPolicies, Policy)
{
  BOOST_CHECK_EQUAL(this->policy.m_dataRules.size(), 0);
  BOOST_CHECK_EQUAL(this->policy.m_interestRules.size(), 1);

  using Packet = typename Policy::Packet;
  Packet unsignedPacket("/Security/V2/ValidatorFixture/Sub1/Sub2/Packet");

  Packet packet = unsignedPacket;
  VALIDATE_FAILURE(packet, "Unsigned");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_FAILURE(packet, "Policy doesn't accept Sha256Digest signature");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->identity));
  VALIDATE_SUCCESS(packet, "Should get accepted, as signed by the anchor");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subIdentity));
  VALIDATE_FAILURE(packet, "Should fail, as there is no matching rule for data");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->otherIdentity));
  VALIDATE_FAILURE(packet, "Should fail, as signed by the policy-violating cert");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subSelfSignedIdentity));
  VALIDATE_FAILURE(packet, "Should fail, because subSelfSignedIdentity is not a trust anchor");
}

BOOST_FIXTURE_TEST_CASE(Reload, HierarchicalValidatorFixture<ValidationPolicyConfig>)
{
  BOOST_CHECK_EQUAL(this->policy.m_isConfigured, false);
  this->policy.load(R"CONF(
      rule
      {
        id test-rule-data-id
        for data
        filter
        {
          type name
          name /foo/bar
          relation is-prefix-of
        }
        checker
        {
          type hierarchical
          sig-type rsa-sha256
        }
      }
      rule
      {
        id test-rule-interest-id
        for interest
        filter
        {
          type name
          name /foo/bar
          relation is-prefix-of
        }
        checker
        {
          type hierarchical
          sig-type rsa-sha256
        }
      }
      trust-anchor
      {
        type dir
        dir keys
        refresh 1h
      }
    )CONF", "test-config");
  BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
  BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, false);
  BOOST_CHECK_EQUAL(this->policy.m_dataRules.size(), 1);
  BOOST_CHECK_EQUAL(this->policy.m_interestRules.size(), 1);

  this->policy.load(R"CONF(
      trust-anchor
      {
        type any
      }
    )CONF", "test-config");
  BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
  BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, true);
  BOOST_CHECK_EQUAL(this->policy.m_dataRules.size(), 0);
  BOOST_CHECK_EQUAL(this->policy.m_interestRules.size(), 0);
}

using Packets = boost::mpl::vector<Interest, Data>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(TrustAnchorWildcard, Packet, Packets, ValidationPolicyConfigFixture<Packet>)
{
  this->policy.load(R"CONF(
      trust-anchor
      {
        type any
      }
    )CONF", "test-config");

  BOOST_CHECK_EQUAL(this->policy.m_isConfigured, true);
  BOOST_CHECK_EQUAL(this->policy.m_shouldBypass, true);
  BOOST_CHECK_EQUAL(this->policy.m_dataRules.size(), 0);
  BOOST_CHECK_EQUAL(this->policy.m_interestRules.size(), 0);

  Packet unsignedPacket("/Security/V2/ValidatorFixture/Sub1/Sub2/Packet");

  Packet packet = unsignedPacket;
  VALIDATE_SUCCESS(packet, "Policy should accept everything");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingWithSha256());
  VALIDATE_SUCCESS(packet, "Policy should accept everything");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->identity));
  VALIDATE_SUCCESS(packet, "Policy should accept everything");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subIdentity));
  VALIDATE_SUCCESS(packet, "Policy should accept everything");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->otherIdentity));
  VALIDATE_SUCCESS(packet, "Policy should accept everything");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subSelfSignedIdentity));
  VALIDATE_SUCCESS(packet, "Policy should accept everything");
}

using RefreshPolicies = boost::mpl::vector<Refresh1h, Refresh1m, Refresh1s>;

// Somehow, didn't work without this wrapper
template<typename RefreshPolicy>
class RefreshPolicyFixture : public LoadStringWithDirAnchor<Data, RefreshPolicy>
{
public:
};

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ValidateRefresh, Refresh, RefreshPolicies, RefreshPolicyFixture<Refresh>)
{
  using Packet = Data;
  Packet unsignedPacket("/Security/V2/ValidatorFixture/Sub1/Sub2/Packet");

  boost::filesystem::remove(this->path / "keys" / "identity.ndncert");
  this->advanceClocks(Refresh::getRefreshTime(), 3);

  Packet packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->identity));
  VALIDATE_FAILURE(packet, "Should fail, as the trust anchor should no longer exist");

  packet = unsignedPacket;
  this->m_keyChain.sign(packet, signingByIdentity(this->subIdentity));
  VALIDATE_FAILURE(packet, "Should fail, as the trust anchor should no longer exist");
}

BOOST_AUTO_TEST_SUITE_END() // TestValidationPolicyConfig
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace validator_config
} // namespace v2
} // namespace security
} // namespace ndn
