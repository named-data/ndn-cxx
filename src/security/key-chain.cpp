/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "key-chain.hpp"

#include "../util/random.hpp"
#include "../util/config-file.hpp"

#include "sec-public-info-sqlite3.hpp"

#ifdef NDN_CXX_HAVE_OSX_SECURITY
#include "sec-tpm-osx.hpp"
#endif // NDN_CXX_HAVE_OSX_SECURITY

#include "sec-tpm-file.hpp"

namespace ndn {
namespace security {

// Use a GUID as a magic number of KeyChain::DEFAULT_PREFIX identifier
const Name KeyChain::DEFAULT_PREFIX("/723821fd-f534-44b3-80d9-44bf5f58bbbb");
const Name KeyChain::DIGEST_SHA256_IDENTITY("/localhost/identity/digest-sha256");

// Note: cannot use default constructor, as it depends on static variables which may or may not be
// initialized at this point
const SigningInfo KeyChain::DEFAULT_SIGNING_INFO(SigningInfo::SIGNER_TYPE_NULL, Name(), SignatureInfo());

const RsaKeyParams KeyChain::DEFAULT_KEY_PARAMS;

const std::string DEFAULT_PIB_SCHEME = "pib-sqlite3";

#if defined(NDN_CXX_HAVE_OSX_SECURITY) and defined(NDN_CXX_WITH_OSX_KEYCHAIN)
const std::string DEFAULT_TPM_SCHEME = "tpm-osxkeychain";
#else
const std::string DEFAULT_TPM_SCHEME = "tpm-file";
#endif // defined(NDN_CXX_HAVE_OSX_SECURITY) and defined(NDN_CXX_WITH_OSX_KEYCHAIN)

// When static library is used, not everything is compiled into the resulting binary.
// Therefore, the following standard PIB and TPMs need to be registered here.
// http://stackoverflow.com/q/9459980/2150331
//
// Also, cannot use Type::SCHEME, as its value may be uninitialized
NDN_CXX_KEYCHAIN_REGISTER_PIB(SecPublicInfoSqlite3, "pib-sqlite3", "sqlite3");

#ifdef NDN_CXX_HAVE_OSX_SECURITY
NDN_CXX_KEYCHAIN_REGISTER_TPM(SecTpmOsx, "tpm-osxkeychain", "osx-keychain");
#endif // NDN_CXX_HAVE_OSX_SECURITY

NDN_CXX_KEYCHAIN_REGISTER_TPM(SecTpmFile, "tpm-file", "file");

template<class T>
struct Factory
{
  Factory(const std::string& canonicalName, const T& create)
    : canonicalName(canonicalName)
    , create(create)
  {
  }

  std::string canonicalName;
  T create;
};
typedef Factory<KeyChain::PibCreateFunc> PibFactory;
typedef Factory<KeyChain::TpmCreateFunc> TpmFactory;

static std::map<std::string, PibFactory>&
getPibFactories()
{
  static std::map<std::string, PibFactory> pibFactories;
  return pibFactories;
}

static std::map<std::string, TpmFactory>&
getTpmFactories()
{
  static std::map<std::string, TpmFactory> tpmFactories;
  return tpmFactories;
}

void
KeyChain::registerPibImpl(const std::string& canonicalName,
                          std::initializer_list<std::string> aliases,
                          KeyChain::PibCreateFunc createFunc)
{
  for (const std::string& alias : aliases) {
    getPibFactories().insert(make_pair(alias, PibFactory(canonicalName, createFunc)));
  }
}

void
KeyChain::registerTpmImpl(const std::string& canonicalName,
                          std::initializer_list<std::string> aliases,
                          KeyChain::TpmCreateFunc createFunc)
{
  for (const std::string& alias : aliases) {
    getTpmFactories().insert(make_pair(alias, TpmFactory(canonicalName, createFunc)));
  }
}

KeyChain::KeyChain()
  : m_pib(nullptr)
  , m_tpm(nullptr)
  , m_lastTimestamp(time::toUnixTimestamp(time::system_clock::now()))
{
  ConfigFile config;
  const ConfigFile::Parsed& parsed = config.getParsedConfiguration();

  std::string pibLocator = parsed.get<std::string>("pib", "");
  std::string tpmLocator = parsed.get<std::string>("tpm", "");

  initialize(pibLocator, tpmLocator, false);
}

KeyChain::KeyChain(const std::string& pibName,
                   const std::string& tpmName,
                   bool allowReset)
  : m_pib(nullptr)
  , m_tpm(nullptr)
  , m_lastTimestamp(time::toUnixTimestamp(time::system_clock::now()))
{
  initialize(pibName, tpmName, allowReset);
}

KeyChain::~KeyChain()
{
}

static inline std::tuple<std::string/*type*/, std::string/*location*/>
parseUri(const std::string& uri)
{
  size_t pos = uri.find(':');
  if (pos != std::string::npos) {
    return std::make_tuple(uri.substr(0, pos),
                           uri.substr(pos + 1));
  }
  else {
    return std::make_tuple(uri, "");
  }
}

std::string
KeyChain::getDefaultPibLocator()
{
  std::string defaultPibLocator = DEFAULT_PIB_SCHEME + ":";
  return defaultPibLocator;
}

static inline std::tuple<std::string/*type*/, std::string/*location*/>
getCanonicalPibLocator(const std::string& pibLocator)
{
  std::string pibScheme, pibLocation;
  std::tie(pibScheme, pibLocation) = parseUri(pibLocator);

  if (pibScheme.empty()) {
    pibScheme = DEFAULT_PIB_SCHEME;
  }

  auto pibFactory = getPibFactories().find(pibScheme);
  if (pibFactory == getPibFactories().end()) {
    BOOST_THROW_EXCEPTION(KeyChain::Error("PIB scheme '" + pibScheme + "' is not supported"));
  }
  pibScheme = pibFactory->second.canonicalName;

  return std::make_tuple(pibScheme, pibLocation);
}

unique_ptr<SecPublicInfo>
KeyChain::createPib(const std::string& pibLocator)
{
  BOOST_ASSERT(!getPibFactories().empty());

  std::string pibScheme, pibLocation;
  std::tie(pibScheme, pibLocation) = getCanonicalPibLocator(pibLocator);
  auto pibFactory = getPibFactories().find(pibScheme);
  BOOST_ASSERT(pibFactory != getPibFactories().end());
  return pibFactory->second.create(pibLocation);
}

std::string
KeyChain::getDefaultTpmLocator()
{
  std::string defaultTpmLocator = DEFAULT_TPM_SCHEME + ":";
  return defaultTpmLocator;
}

static inline std::tuple<std::string/*type*/, std::string/*location*/>
getCanonicalTpmLocator(const std::string& tpmLocator)
{
  std::string tpmScheme, tpmLocation;
  std::tie(tpmScheme, tpmLocation) = parseUri(tpmLocator);

  if (tpmScheme.empty()) {
    tpmScheme = DEFAULT_TPM_SCHEME;
  }
  auto tpmFactory = getTpmFactories().find(tpmScheme);
  if (tpmFactory == getTpmFactories().end()) {
    BOOST_THROW_EXCEPTION(KeyChain::Error("TPM scheme '" + tpmScheme + "' is not supported"));
  }
  tpmScheme = tpmFactory->second.canonicalName;

  return std::make_tuple(tpmScheme, tpmLocation);
}

unique_ptr<SecTpm>
KeyChain::createTpm(const std::string& tpmLocator)
{
  BOOST_ASSERT(!getTpmFactories().empty());

  std::string tpmScheme, tpmLocation;
  std::tie(tpmScheme, tpmLocation) = getCanonicalTpmLocator(tpmLocator);
  auto tpmFactory = getTpmFactories().find(tpmScheme);
  BOOST_ASSERT(tpmFactory != getTpmFactories().end());
  return tpmFactory->second.create(tpmLocation);
}

void
KeyChain::initialize(const std::string& pibLocator,
                     const std::string& tpmLocator,
                     bool allowReset)
{
  // PIB Locator
  std::string pibScheme, pibLocation;
  std::tie(pibScheme, pibLocation) = getCanonicalPibLocator(pibLocator);
  std::string canonicalPibLocator = pibScheme + ":" + pibLocation;

  // Create PIB
  m_pib = createPib(canonicalPibLocator);

  // TPM Locator
  std::string tpmScheme, tpmLocation;
  std::tie(tpmScheme, tpmLocation) = getCanonicalTpmLocator(tpmLocator);
  std::string canonicalTpmLocator = tpmScheme + ":" + tpmLocation;

  // Create TPM, checking that it matches to the previously associated one
  try {
    if (!allowReset &&
        !m_pib->getTpmLocator().empty() && m_pib->getTpmLocator() != canonicalTpmLocator)
      // Tpm mismatch, but we do not want to reset PIB
      BOOST_THROW_EXCEPTION(MismatchError("TPM locator supplied does not match TPM locator in PIB: "
                                          + m_pib->getTpmLocator() + " != " + canonicalTpmLocator));
  }
  catch (SecPublicInfo::Error&) {
    // TPM locator is not set in PIB yet.
  }

  // note that key mismatch may still happen if the TPM locator is initially set to a
  // wrong one or if the PIB was shared by more than one TPMs before.  This is due to the
  // old PIB does not have TPM info, new pib should not have this problem.
  m_tpm = createTpm(canonicalTpmLocator);
  m_pib->setTpmLocator(canonicalTpmLocator);
}

Name
KeyChain::createIdentity(const Name& identityName, const KeyParams& params)
{
  m_pib->addIdentity(identityName);

  Name keyName;
  try
    {
      keyName = m_pib->getDefaultKeyNameForIdentity(identityName);

      shared_ptr<PublicKey> key = m_pib->getPublicKey(keyName);

      if (key->getKeyType() != params.getKeyType())
        {
          keyName = generateKeyPair(identityName, true, params);
          m_pib->setDefaultKeyNameForIdentity(keyName);
        }
    }
  catch (SecPublicInfo::Error& e)
    {
      keyName = generateKeyPair(identityName, true, params);
      m_pib->setDefaultKeyNameForIdentity(keyName);
    }

  Name certName;
  try
    {
      certName = m_pib->getDefaultCertificateNameForKey(keyName);
    }
  catch (SecPublicInfo::Error& e)
    {
      shared_ptr<IdentityCertificate> selfCert = selfSign(keyName);
      m_pib->addCertificateAsIdentityDefault(*selfCert);
      certName = selfCert->getName();
    }

  return certName;
}

Name
KeyChain::generateRsaKeyPair(const Name& identityName, bool isKsk, uint32_t keySize)
{
  RsaKeyParams params(keySize);
  return generateKeyPair(identityName, isKsk, params);
}

Name
KeyChain::generateEcdsaKeyPair(const Name& identityName, bool isKsk, uint32_t keySize)
{
  EcdsaKeyParams params(keySize);
  return generateKeyPair(identityName, isKsk, params);
}

Name
KeyChain::generateRsaKeyPairAsDefault(const Name& identityName, bool isKsk, uint32_t keySize)
{
  RsaKeyParams params(keySize);

  Name keyName = generateKeyPair(identityName, isKsk, params);

  m_pib->setDefaultKeyNameForIdentity(keyName);

  return keyName;
}

Name
KeyChain::generateEcdsaKeyPairAsDefault(const Name& identityName, bool isKsk, uint32_t keySize)
{
  EcdsaKeyParams params(keySize);

  Name keyName = generateKeyPair(identityName, isKsk, params);

  m_pib->setDefaultKeyNameForIdentity(keyName);

  return keyName;
}


shared_ptr<IdentityCertificate>
KeyChain::prepareUnsignedIdentityCertificate(const Name& keyName,
  const Name& signingIdentity,
  const time::system_clock::TimePoint& notBefore,
  const time::system_clock::TimePoint& notAfter,
  const std::vector<CertificateSubjectDescription>& subjectDescription,
  const Name& certPrefix)
{
  shared_ptr<PublicKey> publicKey;
  try
    {
      publicKey = m_pib->getPublicKey(keyName);
    }
  catch (SecPublicInfo::Error& e)
    {
      return shared_ptr<IdentityCertificate>();
    }

  return prepareUnsignedIdentityCertificate(keyName, *publicKey, signingIdentity,
                                            notBefore, notAfter,
                                            subjectDescription, certPrefix);
}

shared_ptr<IdentityCertificate>
KeyChain::prepareUnsignedIdentityCertificate(const Name& keyName,
  const PublicKey& publicKey,
  const Name& signingIdentity,
  const time::system_clock::TimePoint& notBefore,
  const time::system_clock::TimePoint& notAfter,
  const std::vector<CertificateSubjectDescription>& subjectDescription,
  const Name& certPrefix)
{
  if (keyName.size() < 1)
    return shared_ptr<IdentityCertificate>();

  std::string keyIdPrefix = keyName.get(-1).toUri().substr(0, 4);
  if (keyIdPrefix != "ksk-" && keyIdPrefix != "dsk-")
    return shared_ptr<IdentityCertificate>();

  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();
  Name certName;

  if (certPrefix == KeyChain::DEFAULT_PREFIX)
    {
      // No certificate prefix hint, infer the prefix
      if (signingIdentity.isPrefixOf(keyName))
        certName.append(signingIdentity)
          .append("KEY")
          .append(keyName.getSubName(signingIdentity.size()))
          .append("ID-CERT")
          .appendVersion();
      else
        certName.append(keyName.getPrefix(-1))
          .append("KEY")
          .append(keyName.get(-1))
          .append("ID-CERT")
          .appendVersion();
    }
  else
    {
      // cert prefix hint is supplied, determine the cert name.
      if (certPrefix.isPrefixOf(keyName) && certPrefix != keyName)
        certName.append(certPrefix)
          .append("KEY")
          .append(keyName.getSubName(certPrefix.size()))
          .append("ID-CERT")
          .appendVersion();
      else
        return shared_ptr<IdentityCertificate>();
    }


  certificate->setName(certName);
  certificate->setNotBefore(notBefore);
  certificate->setNotAfter(notAfter);
  certificate->setPublicKeyInfo(publicKey);

  if (subjectDescription.empty())
    {
      CertificateSubjectDescription subjectName(oid::ATTRIBUTE_NAME, keyName.getPrefix(-1).toUri());
      certificate->addSubjectDescription(subjectName);
    }
  else
    {
      std::vector<CertificateSubjectDescription>::const_iterator sdIt =
        subjectDescription.begin();
      std::vector<CertificateSubjectDescription>::const_iterator sdEnd =
        subjectDescription.end();
      for(; sdIt != sdEnd; sdIt++)
        certificate->addSubjectDescription(*sdIt);
    }

  certificate->encode();

  return certificate;
}

std::tuple<Name, SignatureInfo>
KeyChain::prepareSignatureInfo(const SigningInfo& params)
{
  SignatureInfo sigInfo = params.getSignatureInfo();

  shared_ptr<IdentityCertificate> signingCert;

  switch (params.getSignerType()) {
  case SigningInfo::SIGNER_TYPE_NULL:
    {
      if (m_pib->getDefaultCertificate() == nullptr)
        setDefaultCertificateInternal();

      signingCert = m_pib->getDefaultCertificate();
      break;
    }
  case SigningInfo::SIGNER_TYPE_ID:
    {
      Name signingCertName;
      try {
        signingCertName = m_pib->getDefaultCertificateNameForIdentity(params.getSignerName());
      }
      catch (SecPublicInfo::Error&) {
        signingCertName = createIdentity(params.getSignerName());
      }

      signingCert = m_pib->getCertificate(signingCertName);

      break;
    }
  case SigningInfo::SIGNER_TYPE_KEY:
    {
      Name signingCertName;
      try {
        signingCertName = m_pib->getDefaultCertificateNameForKey(params.getSignerName());
      }
      catch (SecPublicInfo::Error&) {
        BOOST_THROW_EXCEPTION(Error("signing certificate does not exist"));
      }

      signingCert = m_pib->getCertificate(signingCertName);

      break;
    }
  case SigningInfo::SIGNER_TYPE_CERT:
    {
      signingCert = m_pib->getCertificate(params.getSignerName());
      if (signingCert == nullptr)
        BOOST_THROW_EXCEPTION(Error("signing certificate does not exist"));

      break;
    }
  case SigningInfo::SIGNER_TYPE_SHA256:
    {
      sigInfo.setSignatureType(tlv::DigestSha256);
      return std::make_tuple(DIGEST_SHA256_IDENTITY, sigInfo);
    }
  default:
    BOOST_THROW_EXCEPTION(Error("Unrecognized signer type"));
  }

  sigInfo.setSignatureType(getSignatureType(signingCert->getPublicKeyInfo().getKeyType(),
                                            params.getDigestAlgorithm()));
  sigInfo.setKeyLocator(KeyLocator(signingCert->getName().getPrefix(-1)));

  return std::make_tuple(signingCert->getPublicKeyName(), sigInfo);
}

void
KeyChain::sign(Data& data, const SigningInfo& params)
{
  signImpl(data, params);
}

void
KeyChain::sign(Interest& interest, const SigningInfo& params)
{
  signImpl(interest, params);
}

Block
KeyChain::sign(const uint8_t* buffer, size_t bufferLength, const SigningInfo& params)
{
  Name keyName;
  SignatureInfo sigInfo;
  std::tie(keyName, sigInfo) = prepareSignatureInfo(params);
  return pureSign(buffer, bufferLength, keyName, DIGEST_ALGORITHM_SHA256);
}

Signature
KeyChain::sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
{
  shared_ptr<IdentityCertificate> certificate = m_pib->getCertificate(certificateName);

  if (certificate == nullptr)
    BOOST_THROW_EXCEPTION(SecPublicInfo::Error("certificate does not exist"));

  Signature sig;

  // For temporary usage, we support SHA256 only, but will support more.
  sig.setValue(m_tpm->signInTpm(buffer, bufferLength,
                                certificate->getPublicKeyName(),
                                DIGEST_ALGORITHM_SHA256));

  return sig;
}

shared_ptr<IdentityCertificate>
KeyChain::selfSign(const Name& keyName)
{
  shared_ptr<PublicKey> pubKey;
  try {
    pubKey = m_pib->getPublicKey(keyName); // may throw an exception.
  }
  catch (SecPublicInfo::Error&) {
    return shared_ptr<IdentityCertificate>();
  }

  auto certificate = make_shared<IdentityCertificate>();

  Name certificateName = keyName.getPrefix(-1);
  certificateName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();

  certificate->setName(certificateName);
  certificate->setNotBefore(time::system_clock::now());
  certificate->setNotAfter(time::system_clock::now() + time::days(7300)); // ~20 years
  certificate->setPublicKeyInfo(*pubKey);
  certificate->addSubjectDescription(CertificateSubjectDescription(oid::ATTRIBUTE_NAME,
                                                                   keyName.toUri()));
  certificate->encode();

  certificate->setSignature(Signature(SignatureInfo()));

  selfSign(*certificate);
  return certificate;
}

void
KeyChain::selfSign(IdentityCertificate& cert)
{
  Name keyName = cert.getPublicKeyName();
  if (!m_tpm->doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
    BOOST_THROW_EXCEPTION(SecTpm::Error("Private key does not exist"));

  SignatureInfo sigInfo(cert.getSignature().getInfo());
  sigInfo.setKeyLocator(KeyLocator(cert.getName().getPrefix(-1)));
  sigInfo.setSignatureType(getSignatureType(cert.getPublicKeyInfo().getKeyType(),
                                            DIGEST_ALGORITHM_SHA256));

  signPacketWrapper(cert, Signature(sigInfo), keyName, DIGEST_ALGORITHM_SHA256);
}

shared_ptr<SecuredBag>
KeyChain::exportIdentity(const Name& identity, const std::string& passwordStr)
{
  if (!m_pib->doesIdentityExist(identity))
    BOOST_THROW_EXCEPTION(SecPublicInfo::Error("Identity does not exist"));

  Name keyName = m_pib->getDefaultKeyNameForIdentity(identity);

  ConstBufferPtr pkcs5;
  try
    {
      pkcs5 = m_tpm->exportPrivateKeyPkcs5FromTpm(keyName, passwordStr);
    }
  catch (SecTpm::Error& e)
    {
      BOOST_THROW_EXCEPTION(SecPublicInfo::Error("Fail to export PKCS5 of private key"));
    }

  shared_ptr<IdentityCertificate> cert;
  try
    {
      cert = m_pib->getCertificate(m_pib->getDefaultCertificateNameForKey(keyName));
    }
  catch (SecPublicInfo::Error& e)
    {
      cert = selfSign(keyName);
      m_pib->addCertificateAsIdentityDefault(*cert);
    }

  // make_shared on OSX 10.9 has some strange problem here
  shared_ptr<SecuredBag> secureBag(new SecuredBag(*cert, pkcs5));

  return secureBag;
}

void
KeyChain::importIdentity(const SecuredBag& securedBag, const std::string& passwordStr)
{
  Name certificateName = securedBag.getCertificate().getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);
  Name identity = keyName.getPrefix(-1);

  // Add identity
  m_pib->addIdentity(identity);

  // Add key
  m_tpm->importPrivateKeyPkcs5IntoTpm(keyName,
                                      securedBag.getKey()->buf(),
                                      securedBag.getKey()->size(),
                                      passwordStr);

  shared_ptr<PublicKey> pubKey = m_tpm->getPublicKeyFromTpm(keyName.toUri());
  // HACK! We should set key type according to the pkcs8 info.
  m_pib->addKey(keyName, *pubKey);
  m_pib->setDefaultKeyNameForIdentity(keyName);

  // Add cert
  m_pib->addCertificateAsIdentityDefault(securedBag.getCertificate());
}

void
KeyChain::setDefaultCertificateInternal()
{
  m_pib->refreshDefaultCertificate();

  if (!static_cast<bool>(m_pib->getDefaultCertificate()))
    {
      Name defaultIdentity;
      try
        {
          defaultIdentity = m_pib->getDefaultIdentity();
        }
      catch (SecPublicInfo::Error& e)
        {
          uint32_t random = random::generateWord32();
          defaultIdentity.append("tmp-identity")
            .append(reinterpret_cast<uint8_t*>(&random), 4);
        }
      createIdentity(defaultIdentity);
      m_pib->setDefaultIdentity(defaultIdentity);
      m_pib->refreshDefaultCertificate();
    }
}

Name
KeyChain::generateKeyPair(const Name& identityName, bool isKsk, const KeyParams& params)
{
  Name keyName = m_pib->getNewKeyName(identityName, isKsk);

  m_tpm->generateKeyPairInTpm(keyName.toUri(), params);

  shared_ptr<PublicKey> pubKey = m_tpm->getPublicKeyFromTpm(keyName.toUri());
  m_pib->addKey(keyName, *pubKey);

  return keyName;
}

void
KeyChain::signPacketWrapper(Data& data, const Signature& signature,
                            const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  data.setSignature(signature);

  EncodingBuffer encoder;
  data.wireEncode(encoder, true);

  Block sigValue = pureSign(encoder.buf(), encoder.size(), keyName, digestAlgorithm);

  data.wireEncode(encoder, sigValue);
}

void
KeyChain::signPacketWrapper(Interest& interest, const Signature& signature,
                            const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  time::milliseconds timestamp = time::toUnixTimestamp(time::system_clock::now());
  if (timestamp <= m_lastTimestamp)
    {
      timestamp = m_lastTimestamp + time::milliseconds(1);
    }

  Name signedName = interest.getName();
  signedName
    .append(name::Component::fromNumber(timestamp.count()))        // timestamp
    .append(name::Component::fromNumber(random::generateWord64())) // nonce
    .append(signature.getInfo());                                  // signatureInfo

  Block sigValue = pureSign(signedName.wireEncode().value(),
                            signedName.wireEncode().value_size(),
                            keyName,
                            digestAlgorithm);

  sigValue.encode();
  signedName.append(sigValue);                                     // signatureValue
  interest.setName(signedName);
}

Block
KeyChain::pureSign(const uint8_t* buf, size_t size,
                   const Name& keyName, DigestAlgorithm digestAlgorithm) const
{
  if (keyName == DIGEST_SHA256_IDENTITY)
    return Block(tlv::SignatureValue, crypto::sha256(buf, size));

  return m_tpm->signInTpm(buf, size, keyName, digestAlgorithm);
}

Signature
KeyChain::signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName)
{
  Name signingCertificateName;
  try
    {
      signingCertificateName = m_pib->getDefaultCertificateNameForIdentity(identityName);
    }
  catch (SecPublicInfo::Error& e)
    {
      signingCertificateName = createIdentity(identityName);
      // Ideally, no exception will be thrown out, unless something goes wrong in the TPM, which
      // is a fatal error.
    }

  // We either get or create the signing certificate, sign data! (no exception unless fatal error
  // in TPM)
  Signature sig;

  // For temporary usage, we support SHA256 only, but will support more.
  sig.setValue(sign(buffer, bufferLength, SigningInfo(SigningInfo::SIGNER_TYPE_CERT,
                                                      signingCertificateName)));

  return sig;
}

void
KeyChain::signWithSha256(Data& data)
{
  DigestSha256 sig;
  data.setSignature(sig);

  Block sigValue(tlv::SignatureValue,
                 crypto::sha256(data.wireEncode().value(),
                                data.wireEncode().value_size() -
                                data.getSignature().getValue().size()));
  data.setSignatureValue(sigValue);
}

void
KeyChain::signWithSha256(Interest& interest)
{
  DigestSha256 sig;

  time::milliseconds timestamp = time::toUnixTimestamp(time::system_clock::now());
  if (timestamp <= m_lastTimestamp)
    timestamp = m_lastTimestamp + time::milliseconds(1);

  Name signedName = interest.getName();
  signedName
    .append(name::Component::fromNumber(timestamp.count()))        // timestamp
    .append(name::Component::fromNumber(random::generateWord64())) // nonce
    .append(sig.getInfo());                                        // signatureInfo

  Block sigValue(tlv::SignatureValue,
                 crypto::sha256(signedName.wireEncode().value(),
                                signedName.wireEncode().value_size()));

  sigValue.encode();
  signedName.append(sigValue);                                     // signatureValue
  interest.setName(signedName);
}

void
KeyChain::deleteCertificate(const Name& certificateName)
{
  m_pib->deleteCertificateInfo(certificateName);
}

void
KeyChain::deleteKey(const Name& keyName)
{
  m_pib->deletePublicKeyInfo(keyName);
  m_tpm->deleteKeyPairInTpm(keyName);
}

void
KeyChain::deleteIdentity(const Name& identity)
{
  std::vector<Name> keyNames;
  m_pib->getAllKeyNamesOfIdentity(identity, keyNames, true);
  m_pib->getAllKeyNamesOfIdentity(identity, keyNames, false);

  m_pib->deleteIdentityInfo(identity);

  for (const auto& keyName : keyNames)
    m_tpm->deleteKeyPairInTpm(keyName);
}

tlv::SignatureTypeValue
KeyChain::getSignatureType(KeyType keyType, DigestAlgorithm digestAlgorithm)
{
  switch (keyType) {
    case KEY_TYPE_RSA:
      return tlv::SignatureSha256WithRsa;
    case KEY_TYPE_ECDSA:
      return tlv::SignatureSha256WithEcdsa;
    default:
      BOOST_THROW_EXCEPTION(Error("Unsupported key types"));
  }

}

} // namespace security
} // namespace ndn
