/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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
#include "ndn-cxx/security/signing-helpers.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"

#include "ndn-cxx/security/pib/impl/pib-memory.hpp"
#include "ndn-cxx/security/pib/impl/pib-sqlite3.hpp"

#include "ndn-cxx/security/tpm/impl/back-end-file.hpp"
#include "ndn-cxx/security/tpm/impl/back-end-mem.hpp"
#ifdef NDN_CXX_HAVE_OSX_FRAMEWORKS
#include "ndn-cxx/security/tpm/impl/back-end-osx.hpp"
#endif // NDN_CXX_HAVE_OSX_FRAMEWORKS

#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/digest-filter.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"

#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/util/config-file.hpp"
#include "ndn-cxx/util/logger.hpp"
#include "ndn-cxx/util/random.hpp"

#include <boost/lexical_cast.hpp>
#include <cstdlib>  // for std::getenv()

namespace ndn::security {

NDN_LOG_INIT(ndn.security.KeyChain);

// When static library is used, not everything is compiled into the resulting binary.
// Therefore, the following standard PIB and TPMs need to be registered here.
// http://stackoverflow.com/q/9459980/2150331

namespace pib {
NDN_CXX_KEYCHAIN_REGISTER_PIB_BACKEND(PibSqlite3);
NDN_CXX_KEYCHAIN_REGISTER_PIB_BACKEND(PibMemory);
} // namespace pib

namespace tpm {
#ifdef NDN_CXX_HAVE_OSX_FRAMEWORKS
NDN_CXX_KEYCHAIN_REGISTER_TPM_BACKEND(BackEndOsx);
#endif // NDN_CXX_HAVE_OSX_FRAMEWORKS
NDN_CXX_KEYCHAIN_REGISTER_TPM_BACKEND(BackEndFile);
NDN_CXX_KEYCHAIN_REGISTER_TPM_BACKEND(BackEndMem);
} // namespace tpm

const name::Component SELF{"self"};

KeyChain::PibFactories&
KeyChain::getPibFactories()
{
  static PibFactories pibFactories;
  return pibFactories;
}

KeyChain::TpmFactories&
KeyChain::getTpmFactories()
{
  static TpmFactories tpmFactories;
  return tpmFactories;
}

static const auto&
getDefaultPibScheme()
{
  return pib::PibSqlite3::getScheme();
}

static const auto&
getDefaultTpmScheme()
{
#if defined(NDN_CXX_HAVE_OSX_FRAMEWORKS) && defined(NDN_CXX_WITH_OSX_KEYCHAIN)
  return tpm::BackEndOsx::getScheme();
#else
  return tpm::BackEndFile::getScheme();
#endif // defined(NDN_CXX_HAVE_OSX_FRAMEWORKS) && defined(NDN_CXX_WITH_OSX_KEYCHAIN)
}

const KeyParams&
KeyChain::getDefaultKeyParams()
{
  static EcKeyParams keyParams;
  return keyParams;
}

//

class KeyChain::Locator
{
public:
  [[nodiscard]] bool
  empty() const
  {
    return scheme.empty();
  }

  [[nodiscard]] std::string
  canonical() const
  {
    return scheme + ':' + location;
  }

  friend bool
  operator==(const Locator& lhs, const Locator& rhs)
  {
    return lhs.scheme == rhs.scheme && lhs.location == rhs.location;
  }

public:
  std::string scheme;
  std::string location;
};

KeyChain::Locator KeyChain::s_defaultPibLocator;
KeyChain::Locator KeyChain::s_defaultTpmLocator;

//

KeyChain::KeyChain()
  : KeyChain(getDefaultPibLocator(), getDefaultTpmLocator(), true)
{
}

KeyChain::KeyChain(const std::string& pibLocator, const std::string& tpmLocator, bool allowReset)
  : KeyChain(parseAndCheckPibLocator(pibLocator),
             parseAndCheckTpmLocator(tpmLocator),
             allowReset)
{
}

KeyChain::KeyChain(Locator pibLocator, Locator tpmLocator, bool allowReset)
{
  // Create PIB
  auto pibFactory = getPibFactories().find(pibLocator.scheme);
  BOOST_ASSERT(pibFactory != getPibFactories().end());
  m_pib.reset(new Pib(pibLocator.canonical(), pibFactory->second(pibLocator.location)));

  // Figure out the TPM Locator
  std::string oldTpmLocator = m_pib->getTpmLocator();
  if (pibLocator == getDefaultPibLocator()) {
    // Default PIB must use default TPM
    if (!oldTpmLocator.empty() && oldTpmLocator != getDefaultTpmLocator().canonical()) {
      m_pib->reset();
      tpmLocator = getDefaultTpmLocator();
    }
  }
  else {
    // non-default PIB check consistency
    if (!oldTpmLocator.empty() && oldTpmLocator != tpmLocator.canonical()) {
      if (allowReset)
        m_pib->reset();
      else
        NDN_THROW(LocatorMismatchError("Supplied TPM locator (" + tpmLocator.canonical() +
                                       ") does not match TPM locator in PIB (" + oldTpmLocator + ")"));
    }
  }
  // Note that key mismatch may still happen if the TPM locator is initially set to a
  // wrong one or if the PIB was shared by more than one TPMs before.  This is due to the
  // old PIB not having TPM info, the new PIB should not have this problem.

  // Create TPM
  auto tpmFactory = getTpmFactories().find(tpmLocator.scheme);
  BOOST_ASSERT(tpmFactory != getTpmFactories().end());
  m_tpm.reset(new Tpm(tpmLocator.canonical(), tpmFactory->second(tpmLocator.location)));

  // Link PIB with TPM
  m_pib->setTpmLocator(tpmLocator.canonical());
}

KeyChain::~KeyChain() = default;

// public: management

Identity
KeyChain::createIdentity(const Name& identityName, const KeyParams& params)
{
  NDN_LOG_DEBUG("Requesting creation of identity " << identityName);
  Identity id = m_pib->addIdentity(identityName);

  Key key;
  try {
    key = id.getDefaultKey();
  }
  catch (const Pib::Error&) {
    key = createKey(id, params);
  }

  try {
    key.getDefaultCertificate();
  }
  catch (const Pib::Error&) {
    NDN_LOG_DEBUG("No default certificate for " << key << ", requesting self-signing");
    selfSign(key);
  }

  return id;
}

void
KeyChain::deleteIdentity(const Identity& identity)
{
  if (!identity) {
    return;
  }

  Name identityName = identity.getName();
  NDN_LOG_DEBUG("Requesting deletion of identity " << identityName);

  for (const auto& key : identity.getKeys()) {
    m_tpm->deleteKey(key.getName());
  }

  m_pib->removeIdentity(identityName);
}

void
KeyChain::setDefaultIdentity(const Identity& identity)
{
  BOOST_ASSERT(identity);

  m_pib->setDefaultIdentity(identity.getName());
}

Key
KeyChain::createKey(const Identity& identity, const KeyParams& params)
{
  BOOST_ASSERT(identity);

  // create key in TPM
  Name keyName = m_tpm->createKey(identity.getName(), params);

  // set up key info in PIB
  Key key = identity.addKey(*m_tpm->getPublicKey(keyName), keyName);

  NDN_LOG_DEBUG("Requesting self-signing for newly created key " << key);
  selfSign(key);

  return key;
}

Name
KeyChain::createHmacKey(const Name& prefix, const HmacKeyParams& params)
{
  return m_tpm->createKey(prefix, params);
}

void
KeyChain::deleteKey(const Identity& identity, const Key& key)
{
  BOOST_ASSERT(identity);
  if (!key) {
    return;
  }

  Name keyName = key.getName();
  identity.removeKey(keyName);
  m_tpm->deleteKey(keyName);
}

void
KeyChain::setDefaultKey(const Identity& identity, const Key& key)
{
  BOOST_ASSERT(identity);
  BOOST_ASSERT(key);

  identity.setDefaultKey(key.getName());
}

void
KeyChain::addCertificate(const Key& key, const Certificate& certificate)
{
  BOOST_ASSERT(key);

  key.addCertificate(certificate);
}

void
KeyChain::deleteCertificate(const Key& key, const Name& certName)
{
  BOOST_ASSERT(key);

  key.removeCertificate(certName);
}

void
KeyChain::setDefaultCertificate(const Key& key, const Certificate& cert)
{
  BOOST_ASSERT(key);

  key.setDefaultCertificate(cert);
}

shared_ptr<SafeBag>
KeyChain::exportSafeBag(const Certificate& certificate, const char* pw, size_t pwLen)
{
  Name keyName = certificate.getKeyName();

  ConstBufferPtr encryptedKey;
  try {
    encryptedKey = m_tpm->exportPrivateKey(keyName, pw, pwLen);
  }
  catch (const Tpm::Error&) {
    NDN_THROW_NESTED(Error("Failed to export private key `" + keyName.toUri() + "`"));
  }

  return make_shared<SafeBag>(certificate, *encryptedKey);
}

void
KeyChain::importSafeBag(const SafeBag& safeBag, const char* pw, size_t pwLen)
{
  Certificate cert(safeBag.getCertificate());
  Name identity = cert.getIdentity();
  Name keyName = cert.getKeyName();

  // check if private key already exists
  if (m_tpm->hasKey(keyName)) {
    NDN_THROW(Error("Private key `" + keyName.toUri() + "` already exists"));
  }

  // check if public key already exists
  try {
    m_pib->getIdentity(identity).getKey(keyName);
    NDN_THROW(Error("Public key `" + keyName.toUri() + "` already exists"));
  }
  catch (const Pib::Error&) {
    // Either identity or key doesn't exist. OK to import.
  }

  try {
    m_tpm->importPrivateKey(keyName, safeBag.getEncryptedKey(), pw, pwLen);
  }
  catch (const Tpm::Error&) {
    NDN_THROW_NESTED(Error("Failed to import private key `" + keyName.toUri() + "`"));
  }

  // check the consistency of private key and certificate (sign/verify a random message)
  const auto r = random::generateWord64();
  const auto msg = make_span(reinterpret_cast<const uint8_t*>(&r), sizeof(r));
  ConstBufferPtr sigBits;
  try {
    sigBits = m_tpm->sign({msg}, keyName, DigestAlgorithm::SHA256);
  }
  catch (const std::runtime_error&) {
    m_tpm->deleteKey(keyName);
    NDN_THROW(Error("Invalid private key `" + keyName.toUri() + "`"));
  }
  if (!verifySignature({msg}, *sigBits, cert.getPublicKey())) {
    m_tpm->deleteKey(keyName);
    NDN_THROW(Error("Certificate `" + cert.getName().toUri() + "` "
                    "and private key `" + keyName.toUri() + "` do not match"));
  }

  Identity id = m_pib->addIdentity(identity);
  Key key = id.addKey(cert.getPublicKey(), keyName);
  key.addCertificate(cert);
}

void
KeyChain::importPrivateKey(const Name& keyName, shared_ptr<transform::PrivateKey> key)
{
  if (m_tpm->hasKey(keyName)) {
    NDN_THROW(Error("Private key `" + keyName.toUri() + "` already exists"));
  }

  try {
    m_tpm->importPrivateKey(keyName, std::move(key));
  }
  catch (const Tpm::Error&) {
    NDN_THROW_NESTED(Error("Failed to import private key `" + keyName.toUri() + "`"));
  }
}

// public: signing

void
KeyChain::sign(Data& data, const SigningInfo& params)
{
  auto [keyName, sigInfo] = prepareSignatureInfo(params);

  data.setSignatureInfo(sigInfo);
  EncodingBuffer encoder;
  data.wireEncode(encoder, true);

  auto sigValue = sign({encoder}, keyName, params.getDigestAlgorithm());
  data.wireEncode(encoder, *sigValue);
}

void
KeyChain::sign(Interest& interest, const SigningInfo& params)
{
  auto [keyName, sigInfo] = prepareSignatureInfo(params);

  if (params.getSignedInterestFormat() == SignedInterestFormat::V03) {
    interest.setSignatureInfo(sigInfo);

    // Extract function will throw if not all necessary elements are present in Interest
    auto sigValue = sign(interest.extractSignedRanges(), keyName, params.getDigestAlgorithm());
    interest.setSignatureValue(std::move(sigValue));
  }
  else {
    Name signedName = interest.getName();

    // We encode in Data format because this is the format used prior to Packet Specification v0.3
    const auto& sigInfoBlock = sigInfo.wireEncode(SignatureInfo::Type::Data);
    signedName.append(sigInfoBlock); // SignatureInfo

    Block sigValue(tlv::SignatureValue,
                   sign({signedName.wireEncode().value_bytes()}, keyName, params.getDigestAlgorithm()));
    sigValue.encode();
    signedName.append(sigValue); // SignatureValue

    interest.setName(signedName);
  }
}

Certificate
KeyChain::makeCertificate(const pib::Key& publicKey, const SigningInfo& params,
                          const MakeCertificateOptions& opts)
{
  return makeCertificate(publicKey.getName(), publicKey.getPublicKey(), params, opts);
}

Certificate
KeyChain::makeCertificate(const Certificate& certRequest, const SigningInfo& params,
                          const MakeCertificateOptions& opts)
{
  auto pkcs8 = certRequest.getPublicKey();
  try {
    transform::PublicKey pub;
    pub.loadPkcs8(pkcs8);
  }
  catch (const transform::PublicKey::Error&) {
    NDN_THROW_NESTED(std::invalid_argument("Certificate request contains invalid public key"));
  }

  return makeCertificate(extractKeyNameFromCertName(certRequest.getName()), pkcs8, params, opts);
}

// private: PIB/TPM locator helpers

static std::tuple<std::string/*scheme*/, std::string/*location*/>
parseLocatorUri(const std::string& uri)
{
  if (auto pos = uri.find(':'); pos != std::string::npos) {
    return {uri.substr(0, pos), uri.substr(pos + 1)};
  }
  else {
    return {uri, ""};
  }
}

KeyChain::Locator
KeyChain::parseAndCheckPibLocator(const std::string& pibLocator)
{
  auto [pibScheme, pibLocation] = parseLocatorUri(pibLocator);
  if (pibScheme.empty()) {
    pibScheme = getDefaultPibScheme();
  }

  auto pibFactory = getPibFactories().find(pibScheme);
  if (pibFactory == getPibFactories().end()) {
    NDN_THROW(Error("PIB scheme `" + pibScheme + "` is not supported"));
  }

  return {pibScheme, pibLocation};
}

KeyChain::Locator
KeyChain::parseAndCheckTpmLocator(const std::string& tpmLocator)
{
  auto [tpmScheme, tpmLocation] = parseLocatorUri(tpmLocator);
  if (tpmScheme.empty()) {
    tpmScheme = getDefaultTpmScheme();
  }

  auto tpmFactory = getTpmFactories().find(tpmScheme);
  if (tpmFactory == getTpmFactories().end()) {
    NDN_THROW(Error("TPM scheme `" + tpmScheme + "` is not supported"));
  }

  return {tpmScheme, tpmLocation};
}

const KeyChain::Locator&
KeyChain::getDefaultPibLocator()
{
  if (!s_defaultPibLocator.empty())
    return s_defaultPibLocator;

  std::string input;
  const char* pibEnv = std::getenv("NDN_CLIENT_PIB");
  if (pibEnv != nullptr) {
    input = pibEnv;
  }
  else {
    ConfigFile config;
    input = config.getParsedConfiguration().get<std::string>("pib", getDefaultPibScheme());
  }

  s_defaultPibLocator = parseAndCheckPibLocator(input);
  BOOST_ASSERT(!s_defaultPibLocator.empty());
  return s_defaultPibLocator;
}

const KeyChain::Locator&
KeyChain::getDefaultTpmLocator()
{
  if (!s_defaultTpmLocator.empty())
    return s_defaultTpmLocator;

  std::string input;
  const char* tpmEnv = std::getenv("NDN_CLIENT_TPM");
  if (tpmEnv != nullptr) {
    input = tpmEnv;
  }
  else {
    ConfigFile config;
    input = config.getParsedConfiguration().get<std::string>("tpm", getDefaultTpmScheme());
  }

  s_defaultTpmLocator = parseAndCheckTpmLocator(input);
  BOOST_ASSERT(!s_defaultTpmLocator.empty());
  return s_defaultTpmLocator;
}

#ifdef NDN_CXX_HAVE_TESTS
void
KeyChain::resetDefaultLocators()
{
  s_defaultPibLocator = {};
  s_defaultTpmLocator = {};
}
#endif

// private: signing

Certificate
KeyChain::makeCertificate(const Name& keyName, span<const uint8_t> publicKey,
                          SigningInfo params, const MakeCertificateOptions& opts)
{
  if (opts.freshnessPeriod <= 0_ms) {
    // We cannot rely on Certificate constructor to check this, because
    // it throws Certificate::Error, not std::invalid_argument
    NDN_THROW(std::invalid_argument("FreshnessPeriod is not positive"));
  }

  Name name(keyName);
  name.append(opts.issuerId);
  name.appendVersion(opts.version);

  Data data;
  data.setName(name);
  data.setContentType(tlv::ContentType_Key);
  data.setFreshnessPeriod(opts.freshnessPeriod);
  data.setContent(publicKey);

  auto sigInfo = params.getSignatureInfo();
  // Call ValidityPeriod::makeRelative here instead of in MakeCertificateOptions struct
  // because the caller may prepare MakeCertificateOptions first and call makeCertificate
  // at a later time.
  sigInfo.setValidityPeriod(opts.validity.value_or(ValidityPeriod::makeRelative(-1_s, 365_days)));
  params.setSignatureInfo(sigInfo);

  sign(data, params);
  // let Certificate constructor double-check correctness of this function
  return Certificate(std::move(data));
}

Certificate
KeyChain::selfSign(Key& key)
{
  MakeCertificateOptions opts;
  opts.issuerId = SELF;
  // Note time::system_clock::max() or other NotAfter date results in incorrect encoded value
  // because of overflow during conversion to boost::posix_time::ptime (bug #3915, bug #5176).
  opts.validity = ValidityPeriod::makeRelative(-1_s, 20 * 365_days);
  auto cert = makeCertificate(key, signingByKey(key), opts);

  key.addCertificate(cert);
  return cert;
}

std::tuple<Name, SignatureInfo>
KeyChain::prepareSignatureInfo(const SigningInfo& params)
{
  switch (params.getSignerType()) {
    case SigningInfo::SIGNER_TYPE_NULL: {
      pib::Identity identity;
      try {
        identity = m_pib->getDefaultIdentity();
      }
      catch (const Pib::Error&) { // no default identity, use sha256 for signing.
        return prepareSignatureInfoSha256(params);
      }
      return prepareSignatureInfoWithIdentity(params, identity);
    }
    case SigningInfo::SIGNER_TYPE_ID: {
      auto identity = params.getPibIdentity();
      if (!identity) {
        auto identityName = params.getSignerName();
        try {
          identity = m_pib->getIdentity(identityName);
        }
        catch (const Pib::Error&) {
          NDN_THROW_NESTED(InvalidSigningInfoError("Signing identity `" +
                                                   identityName.toUri() + "` does not exist"));
        }
      }
      if (!identity) {
        NDN_THROW(InvalidSigningInfoError("Cannot determine signing parameters"));
      }
      return prepareSignatureInfoWithIdentity(params, identity);
    }
    case SigningInfo::SIGNER_TYPE_KEY: {
      auto key = params.getPibKey();
      if (!key) {
        auto keyName = params.getSignerName();
        auto identityName = extractIdentityFromKeyName(keyName);
        try {
          key = m_pib->getIdentity(identityName).getKey(keyName);
        }
        catch (const Pib::Error&) {
          NDN_THROW_NESTED(InvalidSigningInfoError("Signing key `" +
                                                   keyName.toUri() + "` does not exist"));
        }
      }
      if (!key) {
        NDN_THROW(InvalidSigningInfoError("Cannot determine signing parameters"));
      }
      return prepareSignatureInfoWithKey(params, key);
    }
    case SigningInfo::SIGNER_TYPE_CERT: {
      auto certName = params.getSignerName();
      auto keyName = extractKeyNameFromCertName(certName);
      auto identityName = extractIdentityFromCertName(certName);
      pib::Key key;
      try {
        key = m_pib->getIdentity(identityName).getKey(keyName);
      }
      catch (const Pib::Error&) {
        NDN_THROW_NESTED(InvalidSigningInfoError("Signing certificate `" +
                                                 certName.toUri() + "` does not exist"));
      }
      return prepareSignatureInfoWithKey(params, key, certName);
    }
    case SigningInfo::SIGNER_TYPE_SHA256: {
      return prepareSignatureInfoSha256(params);
    }
    case SigningInfo::SIGNER_TYPE_HMAC: {
      return prepareSignatureInfoHmac(params, *m_tpm);
    }
  }
  NDN_THROW(InvalidSigningInfoError("Unrecognized signer type " +
                                    to_string(params.getSignerType())));
}

std::tuple<Name, SignatureInfo>
KeyChain::prepareSignatureInfoSha256(const SigningInfo& params)
{
  auto sigInfo = params.getSignatureInfo();
  sigInfo.setSignatureType(tlv::DigestSha256);

  NDN_LOG_TRACE("Prepared signature info: " << sigInfo);
  return {SigningInfo::getDigestSha256Identity(), sigInfo};
}

std::tuple<Name, SignatureInfo>
KeyChain::prepareSignatureInfoHmac(const SigningInfo& params, Tpm& tpm)
{
  const Name& keyName = params.getSignerName();
  if (!tpm.hasKey(keyName)) {
    tpm.importPrivateKey(keyName, params.getHmacKey());
  }

  auto sigInfo = params.getSignatureInfo();
  sigInfo.setSignatureType(getSignatureType(KeyType::HMAC, params.getDigestAlgorithm()));
  sigInfo.setKeyLocator(keyName);

  NDN_LOG_TRACE("Prepared signature info: " << sigInfo);
  return {keyName, sigInfo};
}

std::tuple<Name, SignatureInfo>
KeyChain::prepareSignatureInfoWithIdentity(const SigningInfo& params, const pib::Identity& identity)
{
  pib::Key key;
  try {
    key = identity.getDefaultKey();
  }
  catch (const Pib::Error&) {
    NDN_THROW_NESTED(InvalidSigningInfoError("Signing identity `" + identity.getName().toUri() +
                                              "` does not have a default key"));
  }
  return prepareSignatureInfoWithKey(params, key);
}

std::tuple<Name, SignatureInfo>
KeyChain::prepareSignatureInfoWithKey(const SigningInfo& params, const pib::Key& key,
                                      const std::optional<Name>& certName)
{
  auto sigInfo = params.getSignatureInfo();
  sigInfo.setSignatureType(getSignatureType(key.getKeyType(), params.getDigestAlgorithm()));
  if (!sigInfo.hasKeyLocator()) {
    if (certName) {
      sigInfo.setKeyLocator(certName);
    }
    else {
      Name klName = key.getName();
      try {
        klName = key.getDefaultCertificate().getName();
      }
      catch (const Pib::Error&) {
      }
      sigInfo.setKeyLocator(klName);
    }
  }

  NDN_LOG_TRACE("Prepared signature info: " << sigInfo);
  return {key.getName(), sigInfo};
}

ConstBufferPtr
KeyChain::sign(const InputBuffers& bufs, const Name& keyName, DigestAlgorithm digestAlgorithm) const
{
  using namespace transform;

  if (keyName == SigningInfo::getDigestSha256Identity()) {
    OBufferStream os;
    bufferSource(bufs) >> digestFilter(DigestAlgorithm::SHA256) >> streamSink(os);
    return os.buf();
  }

  auto signature = m_tpm->sign(bufs, keyName, digestAlgorithm);
  if (!signature) {
    NDN_THROW(InvalidSigningInfoError("TPM signing failed for key `" + keyName.toUri() + "` "
                                      "(e.g., PIB contains info about the key, but TPM is missing "
                                      "the corresponding private key)"));
  }

  return signature;
}

tlv::SignatureTypeValue
KeyChain::getSignatureType(KeyType keyType, DigestAlgorithm)
{
  switch (keyType) {
  case KeyType::RSA:
    return tlv::SignatureSha256WithRsa;
  case KeyType::EC:
    return tlv::SignatureSha256WithEcdsa;
  case KeyType::HMAC:
    return tlv::SignatureHmacWithSha256;
  default:
    NDN_THROW(Error("Unsupported key type " + boost::lexical_cast<std::string>(keyType)));
  }
}

} // namespace ndn::security
