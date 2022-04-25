/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/key-chain.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"
#include "tests/unit/clock-fixture.hpp"
#include "tests/unit/test-home-env-saver.hpp"

#include <boost/mpl/vector.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
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
    try {
      const_cast<std::string&>(KeyChain::getDefaultPibLocator()).clear();
    }
    catch (const KeyChain::Error&) {
      // ignore
    }

    try {
      const_cast<std::string&>(KeyChain::getDefaultTpmLocator()).clear();
    }
    catch (const KeyChain::Error&) {
      // ignore
    }
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

BOOST_FIXTURE_TEST_CASE(ConstructorEmptyConfig2, TestHomeAndPibFixture<PibPathConfigFileEmpty2Home>)
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

BOOST_FIXTURE_TEST_CASE(ConstructorBadConfig, TestHomeAndPibFixture<PibPathConfigFileMalformedHome>)
{
  createClientConf({"pib=lord", "tpm=ring"});
  BOOST_CHECK_THROW(KeyChain(), KeyChain::Error); // Wrong configuration. Error expected.
}

struct PibPathConfigFileMalformed2Home
{
  const std::string PATH = "build/config-file-malformed2-home/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorBadConfig2, TestHomeAndPibFixture<PibPathConfigFileMalformed2Home>)
{
  createClientConf({"pib=pib-sqlite3:%PATH%", "tpm=just-wrong"});
  BOOST_CHECK_THROW(KeyChain(), KeyChain::Error); // Wrong configuration. Error expected.
}

struct PibPathConfigFileNonCanonicalTpm
{
  const std::string PATH = "build/config-file-non-canonical-tpm/";
};

BOOST_FIXTURE_TEST_CASE(ConstructorNonCanonicalTpm, TestHomeAndPibFixture<PibPathConfigFileNonCanonicalTpm>) // Bug 4297
{
  createClientConf({"pib=pib-sqlite3:", "tpm=tpm-file"});

  {
    KeyChain keyChain;
    keyChain.createIdentity("/test");
    BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-sqlite3:");
    BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-file:");
  }

  {
    KeyChain keyChain;
    BOOST_CHECK_EQUAL(keyChain.getPib().getPibLocator(), "pib-sqlite3:");
    BOOST_CHECK_EQUAL(keyChain.getTpm().getTpmLocator(), "tpm-file:");
    BOOST_CHECK(keyChain.getPib().getIdentities().find("/test") != keyChain.getPib().getIdentities().end());
  }
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

BOOST_FIXTURE_TEST_CASE(SigningWithCorruptedPibTpm, KeyChainFixture)
{
  Identity id = m_keyChain.createIdentity("/test");

  Data data("/foobar");
  BOOST_CHECK_NO_THROW(m_keyChain.sign(data, signingByIdentity(id)));

  // now, "corrupting TPM"
  const_cast<Tpm&>(m_keyChain.getTpm()).deleteKey(id.getDefaultKey().getName());

  BOOST_CHECK_NO_THROW(id.getDefaultKey());
  BOOST_CHECK_THROW(m_keyChain.sign(data, signingByIdentity(id)), KeyChain::InvalidSigningInfoError);
}

BOOST_FIXTURE_TEST_CASE(SigningWithNonExistingIdentity, KeyChainFixture)
{
  Data data("/test/data");
  BOOST_CHECK_THROW(m_keyChain.sign(data, signingByIdentity("/non-existing/identity")),
                    KeyChain::InvalidSigningInfoError);
}

BOOST_FIXTURE_TEST_CASE(Management, KeyChainFixture)
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
  BOOST_CHECK_NO_THROW(m_keyChain.getPib().getDefaultIdentity());
  // The default key of the added identity must exist
  Key key = id.getDefaultKey();
  // The default certificate of the default key must exist
  BOOST_CHECK_NO_THROW(key.getDefaultCertificate());

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
  Key key2 = id.getDefaultKey();
  BOOST_REQUIRE(key2);
  BOOST_CHECK_NE(key2.getName(), key1Name);
  BOOST_CHECK_EQUAL(id.getKeys().size(), 1);
  BOOST_CHECK_NO_THROW(key2.getDefaultCertificate());

  // Create the third key
  Key key3 = m_keyChain.createKey(id);
  BOOST_CHECK_NE(key3.getName(), key2.getName());
  // The added key will not be the default key, because the default key already exists
  BOOST_CHECK_EQUAL(id.getDefaultKey().getName(), key2.getName());
  BOOST_CHECK_EQUAL(id.getKeys().size(), 2);
  BOOST_CHECK_NO_THROW(key3.getDefaultCertificate());

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
  BOOST_CHECK_NO_THROW(key3.getDefaultCertificate());
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
  // Add empty cert
  Certificate key3Cert3 = key3Cert1;
  key3Cert3.unsetContent();
  BOOST_CHECK_THROW(m_keyChain.addCertificate(key3, key3Cert3), std::invalid_argument);

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
  BOOST_CHECK_THROW(m_keyChain.getPib().getIdentity(identityName), Pib::Error);
  BOOST_CHECK(m_keyChain.getPib().getIdentities().find(identityName) == m_keyChain.getPib().getIdentities().end());
}

struct DataPkt
{
  Data packet{"/data"};
  SignedInterestFormat sigFormat = SignedInterestFormat::V02; // irrelevant for Data

  SignatureInfo
  getSignatureInfo() const
  {
    return packet.getSignatureInfo();
  }
};

struct InterestV02Pkt
{
  Interest packet{"/interest02"};
  SignedInterestFormat sigFormat = SignedInterestFormat::V02;

  SignatureInfo
  getSignatureInfo() const
  {
    return SignatureInfo(packet.getName()[signed_interest::POS_SIG_INFO].blockFromValue());
  }
};

struct InterestV03Pkt
{
  Interest packet{"/interest03"};
  SignedInterestFormat sigFormat = SignedInterestFormat::V03;

  SignatureInfo
  getSignatureInfo() const
  {
    return packet.getSignatureInfo().value(); // use .value() for checked access
  }
};

template<typename KeyParams>
struct DefaultIdentity
{
  Identity
  operator()(KeyChain& keyChain) const
  {
    auto id = keyChain.createIdentity("/id", KeyParams());
    BOOST_ASSERT(keyChain.getPib().getDefaultIdentity() == id);
    return id;
  }
};

template<typename KeyParams>
struct NonDefaultIdentity
{
  Identity
  operator()(KeyChain& keyChain) const
  {
    auto id = keyChain.createIdentity("/id");
    auto id2 = keyChain.createIdentity("/id2", KeyParams());
    BOOST_ASSERT(keyChain.getPib().getDefaultIdentity() == id);
    return id2;
  }
};

template<typename KeyParams>
struct DefaultKey
{
  Key
  operator()(KeyChain&, const Identity& id) const
  {
    auto key = id.getDefaultKey();
    BOOST_ASSERT(key.getKeyType() == KeyParams().getKeyType());
    return key;
  }
};

template<typename KeyParams>
struct NonDefaultKey
{
  Key
  operator()(KeyChain& keyChain, const Identity& id) const
  {
    auto key2 = keyChain.createKey(id, KeyParams());
    BOOST_ASSERT(id.getDefaultKey() != key2);
    return key2;
  }
};

template<typename PacketType,
         template<typename> class IdentityMaker = DefaultIdentity,
         template<typename> class KeyMaker = DefaultKey,
         typename AsymmetricKeyParams = EcKeyParams,
         uint32_t SignatureTypeTlvValue = tlv::SignatureSha256WithEcdsa>
struct AsymmetricSigningBase : protected KeyChainFixture, protected PacketType
{
  const Identity id = IdentityMaker<AsymmetricKeyParams>()(m_keyChain);
  const Key key = KeyMaker<AsymmetricKeyParams>()(m_keyChain, id);
  const Certificate cert = key.getDefaultCertificate();

  const uint32_t expectedSigType = SignatureTypeTlvValue;
  const bool shouldHaveKeyLocator = true;
  const optional<KeyLocator> expectedKeyLocator = cert.getName();

  bool
  verify(const SigningInfo&) const
  {
    return verifySignature(this->packet, key);
  }
};

template<typename PacketType,
         typename AsymmetricKeyParams,
         uint32_t SignatureTypeTlvValue>
struct AsymmetricSigning : protected AsymmetricSigningBase<PacketType, DefaultIdentity, DefaultKey,
                                                           AsymmetricKeyParams, SignatureTypeTlvValue>
{
  const std::vector<SigningInfo> signingInfos = {
    SigningInfo(),
    SigningInfo(""),

    SigningInfo(this->id),
    SigningInfo(SigningInfo::SIGNER_TYPE_ID, this->id.getName()),
    SigningInfo("id:" + this->id.getName().toUri()),
    signingByIdentity(this->id),
    signingByIdentity(this->id.getName()),

    SigningInfo(this->key),
    SigningInfo(SigningInfo::SIGNER_TYPE_KEY, this->key.getName()),
    SigningInfo("key:" + this->key.getName().toUri()),
    signingByKey(this->key),
    signingByKey(this->key.getName()),

    SigningInfo(SigningInfo::SIGNER_TYPE_CERT, this->cert.getName()),
    SigningInfo("cert:" + this->cert.getName().toUri()),
    signingByCertificate(this->cert),
    signingByCertificate(this->cert.getName()),
  };
};

template<typename PacketType>
using RsaSigning = AsymmetricSigning<PacketType, RsaKeyParams, tlv::SignatureSha256WithRsa>;

template<typename PacketType>
using EcdsaSigning = AsymmetricSigning<PacketType, EcKeyParams, tlv::SignatureSha256WithEcdsa>;

template<typename PacketType>
struct SigningWithNonDefaultIdentity : protected AsymmetricSigningBase<PacketType, NonDefaultIdentity>
{
  const std::vector<SigningInfo> signingInfos = {
    signingByIdentity(this->id),
    signingByIdentity(this->id.getName()),
    signingByKey(this->key),
    signingByCertificate(this->cert),
  };
};

template<typename PacketType>
struct SigningWithNonDefaultKey : protected AsymmetricSigningBase<PacketType, NonDefaultIdentity, NonDefaultKey>
{
  const std::vector<SigningInfo> signingInfos = {
    signingByKey(this->key),
    signingByKey(this->key.getName()),
    signingByCertificate(this->cert),
  };
};

template<typename PacketType,
         DigestAlgorithm DigestAlgo = DigestAlgorithm::SHA256,
         uint32_t SignatureTypeTlvValue = tlv::SignatureHmacWithSha256>
struct HmacSigning : protected KeyChainFixture, protected PacketType
{
  const std::vector<SigningInfo> signingInfos = {
    SigningInfo(SigningInfo::SIGNER_TYPE_HMAC, m_keyChain.createHmacKey()),
    SigningInfo("hmac-sha256:QjM3NEEyNkE3MTQ5MDQzN0FBMDI0RTRGQURENUI0OTdGREZGMUE4RUE2RkYxMkY2RkI2NUFGMjcyMEI1OUNDRg=="),
  };

  const uint32_t expectedSigType = SignatureTypeTlvValue;
  const bool shouldHaveKeyLocator = true;
  const optional<KeyLocator> expectedKeyLocator = nullopt; // don't check KeyLocator value

  bool
  verify(const SigningInfo& si) const
  {
    return verifySignature(this->packet, m_keyChain.getTpm(), si.getSignerName(), DigestAlgo);
  }
};

template<typename PacketType>
struct Sha256Signing : protected KeyChainFixture, protected PacketType
{
  const std::vector<SigningInfo> signingInfos = {
    SigningInfo(SigningInfo::SIGNER_TYPE_SHA256),
    SigningInfo("id:" + SigningInfo::getDigestSha256Identity().toUri()),
    signingWithSha256()
  };

  const uint32_t expectedSigType = tlv::DigestSha256;
  const bool shouldHaveKeyLocator = false;
  const optional<KeyLocator> expectedKeyLocator = nullopt;

  bool
  verify(const SigningInfo&) const
  {
    return verifySignature(this->packet, nullopt);
  }
};

using SigningTests = boost::mpl::vector<
  RsaSigning<DataPkt>,
  RsaSigning<InterestV02Pkt>,
  RsaSigning<InterestV03Pkt>,
  EcdsaSigning<DataPkt>,
  EcdsaSigning<InterestV02Pkt>,
  EcdsaSigning<InterestV03Pkt>,
#if OPENSSL_VERSION_NUMBER < 0x30000000L // FIXME #5154
  HmacSigning<DataPkt>,
  HmacSigning<InterestV02Pkt>,
  HmacSigning<InterestV03Pkt>,
#endif
  Sha256Signing<DataPkt>,
  Sha256Signing<InterestV02Pkt>,
  Sha256Signing<InterestV03Pkt>,
  SigningWithNonDefaultIdentity<DataPkt>,
  SigningWithNonDefaultIdentity<InterestV03Pkt>,
  SigningWithNonDefaultKey<DataPkt>,
  SigningWithNonDefaultKey<InterestV03Pkt>
>;

BOOST_FIXTURE_TEST_CASE_TEMPLATE(SigningInterface, T, SigningTests, T)
{
  BOOST_TEST_CONTEXT("Packet = " << this->packet.getName()) {
    for (auto signingInfo : this->signingInfos) {
      signingInfo.setSignedInterestFormat(this->sigFormat);

      BOOST_TEST_CONTEXT("SigningInfo = " << signingInfo) {
        this->m_keyChain.sign(this->packet, signingInfo);

        auto sigInfo = this->getSignatureInfo();
        BOOST_CHECK_EQUAL(sigInfo.getSignatureType(), this->expectedSigType);
        BOOST_CHECK_EQUAL(sigInfo.hasKeyLocator(), this->shouldHaveKeyLocator);
        if (this->expectedKeyLocator) {
          BOOST_CHECK_EQUAL(sigInfo.getKeyLocator(), *this->expectedKeyLocator);
        }
        BOOST_CHECK(this->verify(signingInfo));
      }
    }
  }
}

class MakeCertificateFixture : public ClockFixture
{
public:
  MakeCertificateFixture()
    : requesterKeyChain("pib-memory:", "tpm-memory:")
    , signerKeyChain("pib-memory:", "tpm-memory:")
  {
    m_systemClock->setNow(time::fromIsoString("20091117T203458,651387237").time_since_epoch());

    requester = requesterKeyChain.createIdentity("/requester").getDefaultKey();
    Name signerIdentityName("/signer");
    signerKey = signerKeyChain.createIdentity(signerIdentityName).getDefaultKey();
    signerParams = signingByIdentity(signerIdentityName);
  }

  void
  checkKeyLocatorName(const Certificate& cert, optional<Name> klName = nullopt) const
  {
    auto kl = cert.getKeyLocator();
    if (!kl.has_value()) {
      BOOST_ERROR("KeyLocator is missing");
      return;
    }
    BOOST_CHECK_EQUAL(kl->getName(),
                      klName.value_or(signerKey.getDefaultCertificate().getName()));
  }

  void
  checkCertFromDefaults(const Certificate& cert) const
  {
    BOOST_CHECK(Certificate::isValidName(cert.getName()));
    BOOST_CHECK_EQUAL(cert.getKeyName(), requester.getName());
    BOOST_CHECK_EQUAL(cert.getName()[-2], name::Component("NA"));
    BOOST_CHECK(cert.getName()[-1].isVersion());

    BOOST_CHECK_EQUAL(cert.getContentType(), tlv::ContentType_Key);
    BOOST_CHECK_EQUAL(cert.getFreshnessPeriod(), 1_h);

    BOOST_TEST(cert.getContent().value_bytes() == requester.getPublicKey(),
               boost::test_tools::per_element());

    checkKeyLocatorName(cert);

    BOOST_CHECK(cert.isValid());
    auto vp = cert.getValidityPeriod().getPeriod();
    BOOST_CHECK_EQUAL(vp.first, time::fromIsoString("20091117T203458"));
    BOOST_CHECK_EQUAL(vp.second, time::fromIsoString("20101117T203458"));

    auto adBlock = cert.getSignatureInfo().getCustomTlv(tlv::AdditionalDescription);
    BOOST_CHECK(!adBlock.has_value());
  }

public:
  KeyChain requesterKeyChain;
  pib::Key requester;

  KeyChain signerKeyChain;
  pib::Key signerKey;
  Name signerCertificateName;
  SigningInfo signerParams;
};

BOOST_FIXTURE_TEST_SUITE(MakeCertificate, MakeCertificateFixture)

BOOST_AUTO_TEST_CASE(DefaultsFromKey)
{
  auto cert = signerKeyChain.makeCertificate(requester, signerParams);
  checkCertFromDefaults(cert);
}

BOOST_AUTO_TEST_CASE(DefaultsFromCert)
{
  auto cert = signerKeyChain.makeCertificate(requester.getDefaultCertificate(), signerParams);
  checkCertFromDefaults(cert);
}

BOOST_AUTO_TEST_CASE(Options)
{
  MakeCertificateOptions opts;
  opts.issuerId = name::Component::fromEscapedString("ISSUER");
  opts.version = 41218268;
  opts.freshnessPeriod = 321_s;
  opts.validity.emplace(time::fromIsoString("20060702T150405"),
                        time::fromIsoString("20160702T150405"));

  SignatureInfo sigInfo;
  sigInfo.setKeyLocator(signerKey.getName());
  sigInfo.setValidityPeriod(ValidityPeriod(time::fromIsoString("20060102T150405"),
                                           time::fromIsoString("20160102T150405")));
  sigInfo.addCustomTlv(Block(0xF0));
  signerParams.setSignatureInfo(sigInfo);

  auto cert = signerKeyChain.makeCertificate(requester, signerParams, opts);

  BOOST_CHECK_EQUAL(cert.getName(),
                    Name(requester.getName()).append(PartialName("ISSUER/v=41218268")));
  BOOST_CHECK_EQUAL(cert.getFreshnessPeriod(), 321_s);
  checkKeyLocatorName(cert, signerKey.getName());

  auto vp = cert.getValidityPeriod().getPeriod();
  BOOST_CHECK_EQUAL(vp.first, time::fromIsoString("20060702T150405"));
  BOOST_CHECK_EQUAL(vp.second, time::fromIsoString("20160702T150405"));

  BOOST_CHECK(cert.getSignatureInfo().getCustomTlv(0xF0).has_value());
}

BOOST_AUTO_TEST_CASE(ErrSigner)
{
  signerParams = signingByIdentity("/nonexistent");
  BOOST_CHECK_THROW(signerKeyChain.makeCertificate(requester, signerParams), KeyChain::Error);
}

BOOST_AUTO_TEST_CASE(ErrZeroFreshness)
{
  MakeCertificateOptions opts;
  opts.freshnessPeriod = 0_ms;
  BOOST_CHECK_THROW(signerKeyChain.makeCertificate(requester, signerParams, opts),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ErrNegativeFreshness)
{
  MakeCertificateOptions opts;
  opts.freshnessPeriod = -1_ms;
  BOOST_CHECK_THROW(signerKeyChain.makeCertificate(requester, signerParams, opts),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ErrContent)
{
  Certificate request(requester.getDefaultCertificate());
  const auto& oldContent = request.getContent();
  std::vector<uint8_t> content(oldContent.value_begin(), oldContent.value_end());
  content[0] ^= 0x80;
  request.setContent(content);
  BOOST_CHECK_THROW(signerKeyChain.makeCertificate(request, signerParams), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // MakeCertificate

BOOST_FIXTURE_TEST_CASE(ImportPrivateKey, KeyChainFixture)
{
  const Name keyName("/test/device2");
  const uint8_t rawKey[] = "nPSNOHyZKsg2WLqHAs7MXGb0sjQb4zCT";
  auto key = make_shared<transform::PrivateKey>();
  key->loadRaw(KeyType::HMAC, rawKey);

  m_keyChain.importPrivateKey(keyName, key);
  BOOST_CHECK_EQUAL(m_keyChain.getTpm().hasKey(keyName), true);
  BOOST_CHECK_THROW(m_keyChain.importPrivateKey(keyName, key), KeyChain::Error);
}

BOOST_FIXTURE_TEST_CASE(ExportImport, KeyChainFixture)
{
  Identity id = m_keyChain.createIdentity("/TestKeyChain/ExportIdentity");
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
  Identity newId = m_keyChain.getPib().getIdentity(cert.getIdentity());
  BOOST_CHECK_EQUAL(newId.getKeys().size(), 1);
  Key newKey = newId.getKey(cert.getKeyName());
  BOOST_CHECK_EQUAL(newKey.getCertificates().size(), 1);
  BOOST_CHECK_NO_THROW(newKey.getCertificate(cert.getName()));

  m_keyChain.deleteIdentity(newId);
  BOOST_CHECK_EQUAL(m_keyChain.getPib().getIdentities().size(), 0);
  BOOST_CHECK_EQUAL(m_keyChain.getTpm().hasKey(cert.getKeyName()), false);
}

BOOST_FIXTURE_TEST_CASE(SelfSignedCertValidity, KeyChainFixture)
{
  Certificate cert = m_keyChain.createIdentity("/Security/TestKeyChain/SelfSignedCertValidity")
                       .getDefaultKey()
                       .getDefaultCertificate();
  BOOST_CHECK(cert.isValid());
  BOOST_CHECK(cert.isValid(time::system_clock::now() + 10 * 365_days));
  BOOST_CHECK_GT(cert.getValidityPeriod().getPeriod().second, time::system_clock::now() + 10 * 365_days);
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyChain
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn
