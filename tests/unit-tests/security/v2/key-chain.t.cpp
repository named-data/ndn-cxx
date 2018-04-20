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

#include "security/v2/key-chain.hpp"
#include "security/signing-helpers.hpp"
#include "security/verification-helpers.hpp"

#include "boost-test.hpp"
#include "unit-tests/test-home-env-saver.hpp"
#include "identity-management-fixture.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)
BOOST_FIXTURE_TEST_SUITE(TestKeyChain, TestHomeEnvSaver)

template<class Path>
class TestHomeAndPibFixture : public TestHomeFixture<Path>
{
public:
  TestHomeAndPibFixture()
  {
    unsetenv("NDN_CLIENT_PIB");
    unsetenv("NDN_CLIENT_TPM");
  }

  ~TestHomeAndPibFixture()
  {
    const_cast<std::string&>(KeyChain::getDefaultPibLocator()).clear();
    const_cast<std::string&>(KeyChain::getDefaultTpmLocator()).clear();
  }
};

struct PibPathConfigFileHome
{
  const std::string PATH = "build/config-file-home/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorNormalConfig, TestHomeAndPibFixture<PibPathConfigFileHome>)
{
  createClientConf({"pib=pib-memory:", "tpm=tpm-memory:"});

  BOOST_REQUIRE_NO_THROW(KeyChain());

  KeyChain keyChain;
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-memory:");
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-memory:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-memory:");
}

struct PibPathConfigFileEmptyHome
{
  const std::string PATH = "build/config-file-empty-home/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorEmptyConfig, TestHomeAndPibFixture<PibPathConfigFileEmptyHome>)
{
  createClientConf({"pib=pib-memory:"});

#if defined(NDN_CXX_HAVE_OSX_FRAMEWORKS)
  std::string oldHOME;
  if (std::getenv("OLD_HOME"))
    oldHOME = std::getenv("OLD_HOME");

  std::string HOME;
  if (std::getenv("HOME"))
    HOME = std::getenv("HOME");

  if (!oldHOME.empty())
    setenv("HOME", oldHOME.c_str(), 1);
  else
    unsetenv("HOME");
#endif

  BOOST_REQUIRE_NO_THROW(KeyChain());
  KeyChain keyChain;
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-memory:");

#if defined(NDN_CXX_HAVE_OSX_FRAMEWORKS)
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-osxkeychain:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-osxkeychain:");
#else
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-file:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-file:");
#endif

#if defined(NDN_CXX_HAVE_OSX_FRAMEWORKS)
  if (!HOME.empty())
    setenv("HOME", HOME.c_str(), 1);
  else
    unsetenv("HOME");

  if (!oldHOME.empty())
    setenv("OLD_HOME", oldHOME.c_str(), 1);
  else
    unsetenv("OLD_HOME");
#endif
}

struct PibPathConfigFileEmpty2Home
{
  const std::string PATH = "build/config-file-empty2-home/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorEmpty2Config, TestHomeAndPibFixture<PibPathConfigFileEmpty2Home>)
{
  createClientConf({"tpm=tpm-memory:"});

  BOOST_REQUIRE_NO_THROW(KeyChain());

  KeyChain keyChain;
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-sqlite3:");
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-memory:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-memory:");
}

struct PibPathConfigFileMalformedHome
{
  const std::string PATH = "build/config-file-malformed-home/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorMalConfig, TestHomeAndPibFixture<PibPathConfigFileMalformedHome>)
{
  createClientConf({"pib=lord", "tpm=ring"});

  BOOST_REQUIRE_THROW(KeyChain(), KeyChain::Error); // Wrong configuration. Error expected.
}

struct PibPathConfigFileMalformed2Home
{
  const std::string PATH = "build/config-file-malformed2-home/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorMal2Config, TestHomeAndPibFixture<PibPathConfigFileMalformed2Home>)
{
  createClientConf({"pib=pib-sqlite3:%PATH%", "tpm=just-wrong"});

  BOOST_REQUIRE_THROW(KeyChain(), KeyChain::Error); // Wrong configuration. Error expected.
}

BOOST_AUTO_TEST_CASE(KeyChainWithCustomTpmAndPib)
{
  BOOST_REQUIRE_NO_THROW((KeyChain("pib-memory", "tpm-memory")));
  BOOST_REQUIRE_NO_THROW((KeyChain("pib-memory:", "tpm-memory:")));
  BOOST_REQUIRE_NO_THROW((KeyChain("pib-memory:/something", "tpm-memory:/something")));

  KeyChain keyChain("pib-memory", "tpm-memory");
  BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-memory:");
  BOOST_CHECK_EQUAL(keyChain.getPib().getTpmLocator(), "tpm-memory:");
  BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-memory:");
}

BOOST_FIXTURE_TEST_CASE(Management, IdentityManagementFixture)
{
  Name identityName("/test/id");
  Name identity2Name("/test/id2");

  BOOST_CHECK_EQUAL(m_keyChain.getPib().getIdentities().size(), 0);
  BOOST_REQUIRE_THROW(m_keyChain.getPib().getDefaultIdentity(), Pib::Error);

  // Create identity
  Identity id = m_keyChain.createIdentity(identityName);
  BOOST_CHECK(id);
  BOOST_CHECK(m_keyChain.getPib().getIdentities().find(identityName) != m_keyChain.getPib().getIdentities().end());
  // The first added identity becomes the default identity
  BOOST_REQUIRE_NO_THROW(m_keyChain.getPib().getDefaultIdentity());
  // The default key of the added identity must exist
  Key key;
  BOOST_REQUIRE_NO_THROW(key = id.getDefaultKey());
  // The default certificate of the default key must exist
  BOOST_REQUIRE_NO_THROW(key.getDefaultCertificate());

  // Delete key
  Name key1Name = key.getName();
  BOOST_CHECK_NO_THROW(id.getKey(key1Name));
  BOOST_CHECK_EQUAL(id.getKeys().size(), 1);
  m_keyChain.deleteKey(id, key);
  // The key instance should not be valid any more
  BOOST_CHECK(!key);
  BOOST_CHECK_THROW(id.getKey(key1Name), Pib::Error);
  BOOST_CHECK_EQUAL(id.getKeys().size(), 0);

  // Create another key
  m_keyChain.createKey(id);
  // The added key becomes the default key.
  BOOST_REQUIRE_NO_THROW(id.getDefaultKey());
  Key key2 = id.getDefaultKey();
  BOOST_REQUIRE(key2);
  BOOST_CHECK_NE(key2.getName(), key1Name);
  BOOST_CHECK_EQUAL(id.getKeys().size(), 1);
  BOOST_REQUIRE_NO_THROW(key2.getDefaultCertificate());

  // Create the third key
  Key key3 = m_keyChain.createKey(id);
  BOOST_CHECK_NE(key3.getName(), key2.getName());
  // The added key will not be the default key, because the default key already exists
  BOOST_CHECK_EQUAL(id.getDefaultKey().getName(), key2.getName());
  BOOST_CHECK_EQUAL(id.getKeys().size(), 2);
  BOOST_REQUIRE_NO_THROW(key3.getDefaultCertificate());

  // Delete cert
  BOOST_CHECK_EQUAL(key3.getCertificates().size(), 1);
  Certificate key3Cert1 = *key3.getCertificates().begin();
  Name key3CertName = key3Cert1.getName();
  m_keyChain.deleteCertificate(key3, key3CertName);
  BOOST_CHECK_EQUAL(key3.getCertificates().size(), 0);
  BOOST_REQUIRE_THROW(key3.getDefaultCertificate(), Pib::Error);

  // Add cert
  m_keyChain.addCertificate(key3, key3Cert1);
  BOOST_CHECK_EQUAL(key3.getCertificates().size(), 1);
  BOOST_REQUIRE_NO_THROW(key3.getDefaultCertificate());
  m_keyChain.addCertificate(key3, key3Cert1); // overwriting the cert should work
  BOOST_CHECK_EQUAL(key3.getCertificates().size(), 1);
  // Add another cert
  Certificate key3Cert2 = key3Cert1;
  Name key3Cert2Name = key3.getName();
  key3Cert2Name.append("Self");
  key3Cert2Name.appendVersion();
  key3Cert2.setName(key3Cert2Name);
  m_keyChain.addCertificate(key3, key3Cert2);
  BOOST_CHECK_EQUAL(key3.getCertificates().size(), 2);

  // Default certificate setting
  BOOST_CHECK_EQUAL(key3.getDefaultCertificate().getName(), key3CertName);
  m_keyChain.setDefaultCertificate(key3, key3Cert2);
  BOOST_CHECK_EQUAL(key3.getDefaultCertificate().getName(), key3Cert2Name);

  // Default key setting
  BOOST_CHECK_EQUAL(id.getDefaultKey().getName(), key2.getName());
  m_keyChain.setDefaultKey(id, key3);
  BOOST_CHECK_EQUAL(id.getDefaultKey().getName(), key3.getName());

  // Default identity setting
  Identity id2 = m_keyChain.createIdentity(identity2Name);
  BOOST_CHECK_EQUAL(m_keyChain.getPib().getDefaultIdentity().getName(), id.getName());
  m_keyChain.setDefaultIdentity(id2);
  BOOST_CHECK_EQUAL(m_keyChain.getPib().getDefaultIdentity().getName(), id2.getName());

  // Delete identity
  m_keyChain.deleteIdentity(id);
  // The identity instance should not be valid any more
  BOOST_CHECK(!id);
  BOOST_REQUIRE_THROW(m_keyChain.getPib().getIdentity(identityName), Pib::Error);
  BOOST_CHECK(m_keyChain.getPib().getIdentities().find(identityName) == m_keyChain.getPib().getIdentities().end());
}

BOOST_FIXTURE_TEST_CASE(GeneralSigningInterface, IdentityManagementFixture)
{
  Identity id = addIdentity("/id");
  Key key = id.getDefaultKey();
  Certificate cert = key.getDefaultCertificate();

  std::list<SigningInfo> signingInfos = {
    SigningInfo(),

    SigningInfo(SigningInfo::SIGNER_TYPE_ID, id.getName()),
    signingByIdentity(id.getName()),

    SigningInfo(id),
    signingByIdentity(id),

    SigningInfo(SigningInfo::SIGNER_TYPE_KEY, key.getName()),
    signingByKey(key.getName()),

    SigningInfo(key),
    signingByKey(key),

    SigningInfo(SigningInfo::SIGNER_TYPE_CERT, cert.getName()),
    signingByCertificate(cert.getName()),
    signingByCertificate(cert),

    SigningInfo(SigningInfo::SIGNER_TYPE_SHA256),
    signingWithSha256()
  };

  for (const auto& signingInfo : signingInfos) {
    BOOST_TEST_MESSAGE("SigningInfo: " << signingInfo);
    Data data("/data");
    Interest interest("/interest");

    if (signingInfo.getSignerType() == SigningInfo::SIGNER_TYPE_NULL) {
      m_keyChain.sign(data);
      m_keyChain.sign(interest);
    }
    else {
      m_keyChain.sign(data, signingInfo);
      m_keyChain.sign(interest, signingInfo);
    }

    Signature interestSignature(interest.getName()[-2].blockFromValue(), interest.getName()[-1].blockFromValue());

    if (signingInfo.getSignerType() == SigningInfo::SIGNER_TYPE_SHA256) {
      BOOST_CHECK_EQUAL(data.getSignature().getType(), tlv::DigestSha256);
      BOOST_CHECK_EQUAL(interestSignature.getType(), tlv::DigestSha256);

      BOOST_CHECK(verifyDigest(data, DigestAlgorithm::SHA256));
      BOOST_CHECK(verifyDigest(interest, DigestAlgorithm::SHA256));
    }
    else {
      BOOST_CHECK_EQUAL(data.getSignature().getType(), tlv::SignatureSha256WithEcdsa);
      BOOST_CHECK_EQUAL(interestSignature.getType(), tlv::SignatureSha256WithEcdsa);

      BOOST_CHECK_EQUAL(data.getSignature().getKeyLocator().getName(), cert.getName().getPrefix(-2));
      BOOST_CHECK_EQUAL(interestSignature.getKeyLocator().getName(), cert.getName().getPrefix(-2));

      BOOST_CHECK(verifySignature(data, key));
      BOOST_CHECK(verifySignature(interest, key));
    }
  }
}

BOOST_FIXTURE_TEST_CASE(PublicKeySigningDefaults, IdentityManagementFixture)
{
  Data data("/test/data");

  // Identity will be created with generated key and self-signed cert with default parameters
  BOOST_CHECK_THROW(m_keyChain.sign(data, signingByIdentity("/non-existing/identity")), KeyChain::InvalidSigningInfoError);

  // Create identity with EC key and the corresponding self-signed certificate
  Identity id = addIdentity("/ndn/test/ec", EcKeyParams());
  BOOST_CHECK_NO_THROW(m_keyChain.sign(data, signingByIdentity(id.getName())));
  BOOST_CHECK_EQUAL(data.getSignature().getType(),
                    KeyChain::getSignatureType(EcKeyParams().getKeyType(), DigestAlgorithm::SHA256));
  BOOST_CHECK(id.getName().isPrefixOf(data.getSignature().getKeyLocator().getName()));

  // Create identity with RSA key and the corresponding self-signed certificate
  id = addIdentity("/ndn/test/rsa", RsaKeyParams());
  BOOST_CHECK_NO_THROW(m_keyChain.sign(data, signingByIdentity(id.getName())));
  BOOST_CHECK_EQUAL(data.getSignature().getType(),
                    KeyChain::getSignatureType(RsaKeyParams().getKeyType(), DigestAlgorithm::SHA256));
  BOOST_CHECK(id.getName().isPrefixOf(data.getSignature().getKeyLocator().getName()));
}

BOOST_FIXTURE_TEST_CASE(ExportImport, IdentityManagementFixture)
{
  Identity id = addIdentity("/TestKeyChain/ExportIdentity/");
  Certificate cert = id.getDefaultKey().getDefaultCertificate();

  shared_ptr<SafeBag> exported = m_keyChain.exportSafeBag(cert, "1234", 4);
  Block block = exported->wireEncode();

  m_keyChain.deleteIdentity(id);
  BOOST_CHECK_THROW(m_keyChain.exportSafeBag(cert, "1234", 4), KeyChain::Error);

  BOOST_CHECK_EQUAL(m_keyChain.getTpm().hasKey(cert.getKeyName()), false);
  BOOST_CHECK_EQUAL(m_keyChain.getPib().getIdentities().size(), 0);

  SafeBag imported;
  imported.wireDecode(block);
  m_keyChain.importSafeBag(imported, "1234", 4);
  BOOST_CHECK_THROW(m_keyChain.importSafeBag(imported, "1234", 4), KeyChain::Error);

  BOOST_CHECK_EQUAL(m_keyChain.getTpm().hasKey(cert.getKeyName()), true);
  BOOST_CHECK_EQUAL(m_keyChain.getPib().getIdentities().size(), 1);
  BOOST_REQUIRE_NO_THROW(m_keyChain.getPib().getIdentity(cert.getIdentity()));
  Identity newId = m_keyChain.getPib().getIdentity(cert.getIdentity());
  BOOST_CHECK_EQUAL(newId.getKeys().size(), 1);
  BOOST_REQUIRE_NO_THROW(newId.getKey(cert.getKeyName()));
  Key newKey = newId.getKey(cert.getKeyName());
  BOOST_CHECK_EQUAL(newKey.getCertificates().size(), 1);
  BOOST_REQUIRE_NO_THROW(newKey.getCertificate(cert.getName()));

  m_keyChain.deleteIdentity(newId);
  BOOST_CHECK_EQUAL(m_keyChain.getPib().getIdentities().size(), 0);
  BOOST_CHECK_EQUAL(m_keyChain.getTpm().hasKey(cert.getKeyName()), false);
}

BOOST_FIXTURE_TEST_CASE(SelfSignedCertValidity, IdentityManagementFixture)
{
  Certificate cert = addIdentity("/Security/V2/TestKeyChain/SelfSignedCertValidity")
                       .getDefaultKey()
                       .getDefaultCertificate();
  BOOST_CHECK(cert.isValid());
  BOOST_CHECK(cert.isValid(time::system_clock::now() + 10 * 365_days));
  BOOST_CHECK_GT(cert.getValidityPeriod().getPeriod().second, time::system_clock::now() + 10 * 365_days);
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyChain
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
