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

#ifndef NDN_CXX_SECURITY_KEY_CHAIN_HPP
#define NDN_CXX_SECURITY_KEY_CHAIN_HPP

#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/security/certificate.hpp"
#include "ndn-cxx/security/key-params.hpp"
#include "ndn-cxx/security/pib/pib.hpp"
#include "ndn-cxx/security/safe-bag.hpp"
#include "ndn-cxx/security/signing-info.hpp"
#include "ndn-cxx/security/tpm/tpm.hpp"

/**
 * @brief Contains the ndn-cxx security framework.
 */
namespace ndn::security {

/**
 * @brief Options to KeyChain::makeCertificate().
 */
struct MakeCertificateOptions
{
  /**
   * @brief %Certificate name IssuerId component.
   *
   * Default is "NA".
   */
  name::Component issuerId = Certificate::DEFAULT_ISSUER_ID;

  /**
   * @brief %Certificate name version component.
   *
   * Default is deriving from current timestamp using the logic of Name::appendVersion().
   */
  std::optional<uint64_t> version;

  /**
   * @brief %Certificate packet FreshnessPeriod.
   *
   * As required by the certificate format, this must be positive.
   * Setting this to zero or negative causes @c std::invalid_argument exception.
   *
   * Default is 1 hour.
   */
  time::milliseconds freshnessPeriod = 1_h;

  /**
   * @brief %Certificate ValidityPeriod.
   *
   * It isn't an error to specify a ValidityPeriod that does not include the current time
   * or has zero duration, but the certificate won't be valid.
   *
   * Default is a ValidityPeriod from now until 365 days later.
   */
  std::optional<ValidityPeriod> validity;
};

/**
 * @brief The main interface for signing key management.
 *
 * The KeyChain class provides an interface to manage entities related to packet signing,
 * such as Identity, Key, and Certificates.  It consists of two parts: a private key module
 * (TPM) and a public key information base (PIB).  Managing signing keys and their related
 * entities through the KeyChain interface guarantees the consistency between TPM and PIB.
 */
class KeyChain : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  /**
   * @brief Error indicating that the supplied TPM locator does not match the locator stored in PIB.
   */
  class LocatorMismatchError : public Error
  {
  public:
    using Error::Error;
  };

  /**
   * @brief Error indicating that the supplied SigningInfo is invalid.
   */
  class InvalidSigningInfoError : public Error
  {
  public:
    using Error::Error;
  };

  /**
   * @brief Constructor to create KeyChain with default PIB and TPM.
   *
   * The default PIB and TPM are platform-dependent and can be overridden system-wide
   * or on a per-user or even per-application basis.
   *
   * @sa man ndn-client.conf(5)
   */
  KeyChain();

  /**
   * @brief KeyChain constructor.
   *
   * @param pibLocator PIB locator, e.g., `pib-sqlite3:/example/dir`
   * @param tpmLocator TPM locator, e.g., `tpm-memory:`
   * @param allowReset if true, the PIB will be reset when the supplied @p tpmLocator
   *                   does not match the one in the PIB
   *
   * @sa man ndn-client.conf(5)
   */
  KeyChain(const std::string& pibLocator, const std::string& tpmLocator, bool allowReset = false);

  ~KeyChain();

  const Pib&
  getPib() const noexcept
  {
    return *m_pib;
  }

  const Tpm&
  getTpm() const noexcept
  {
    return *m_tpm;
  }

  static const KeyParams&
  getDefaultKeyParams();

public: // Identity management
  /**
   * @brief Create an identity @p identityName.
   *
   * This method will check if the identity exists in the PIB and whether the identity has
   * a default key and default certificate.  If the identity does not exist, this method
   * will create it.  If the identity's default key does not exist, this method will create
   * a key pair and set it as the identity's default key.  If the key's default certificate
   * is missing, this method will create a self-signed certificate for the key.
   *
   * If @p identityName did not exist and no default identity was selected before, the created
   * identity will be set as the default identity.
   *
   * @param identityName The name of the identity.
   * @param params The key parameters if a key needs to be created for the identity (default:
   *               EC key with random key id).
   * @return The created Identity instance.
   */
  Identity
  createIdentity(const Name& identityName, const KeyParams& params = getDefaultKeyParams());

  /**
   * @brief Delete @p identity from this KeyChain.
   *
   * Attempting to delete an invalid identity has no effect.
   */
  void
  deleteIdentity(const Identity& identity);

  /**
   * @brief Set @p identity as the default identity.
   *
   * @pre @p identity must be valid.
   */
  void
  setDefaultIdentity(const Identity& identity);

public: // Key management
  /**
   * @brief Create a new key for @p identity.
   *
   * If @p identity had no default key selected, the created key will be set as the default for
   * this identity.
   *
   * This method will also create a self-signed certificate for the created key.
   *
   * @param identity The identity that will own the created key; must be valid.
   * @param params Key creation parameters (default: EC key with random key id).
   * @return The created Key instance.
   */
  Key
  createKey(const Identity& identity, const KeyParams& params = getDefaultKeyParams());

   /**
   * @brief Create a new HMAC key.
   *
   * The newly created key will be inserted in the TPM. HMAC keys don't have any PIB entries.
   *
   * @param prefix Prefix used to construct the key name (default: `/localhost/identity/hmac`);
   *               the full key name will include additional components according to @p params.
   * @param params Key creation parameters.
   * @return A name that can be subsequently used to reference the created key.
   */
  Name
  createHmacKey(const Name& prefix = SigningInfo::getHmacIdentity(),
                const HmacKeyParams& params = HmacKeyParams());

  /**
   * @brief Delete @p key from @p identity.
   *
   * Attempting to delete an invalid key has no effect.
   *
   * @pre @p identity must be valid.
   * @throw std::invalid_argument @p key does not belong to @p identity.
   */
  void
  deleteKey(const Identity& identity, const Key& key);

  /**
   * @brief Set @p key as the default key of @p identity.
   *
   * @pre @p identity must be valid.
   * @pre @p key must be valid.
   * @throw std::invalid_argument @p key does not belong to @p identity.
   */
  void
  setDefaultKey(const Identity& identity, const Key& key);

public: // Certificate management
  /**
   * @brief Add a certificate @p cert for @p key.
   *
   * If @p key had no default certificate selected, the added certificate will be set as the
   * default certificate for this key.
   *
   * This method will overwrite a certificate with the same name (without considering the
   * implicit digest).
   *
   * @pre @p key must be valid.
   * @throw std::invalid_argument The certificate does not match the key.
   */
  void
  addCertificate(const Key& key, const Certificate& cert);

  /**
   * @brief Delete a certificate with name @p certName from @p key.
   *
   * If the certificate does not exist, this method has no effect.
   *
   * @pre @p key must be valid.
   * @throw std::invalid_argument The certificate name is invalid or does not match the key name.
   */
  void
  deleteCertificate(const Key& key, const Name& certName);

  /**
   * @brief Set @p cert as the default certificate of @p key.
   *
   * The certificate @p cert will be added to @p key, potentially overwriting an existing
   * certificate with the same name (without considering the implicit digest).
   *
   * @pre @p key must be valid.
   * @throw std::invalid_argument The certificate does not match the key.
   */
  void
  setDefaultCertificate(const Key& key, const Certificate& cert);

public: // signing
  /**
   * @brief Sign a Data packet according to the supplied signing information.
   *
   * This method uses the supplied signing information in @p params to sign @p data as follows:
   * - It selects a private key and its associated certificate to sign the packet.
   * - It generates a KeyLocator based upon the certificate name.
   * - Using the SignatureInfo in @p params as a base, it generates the final SignatureInfo block
   *   for @p data.
   * - It adds the generated SignatureInfo block to @p data.
   * - It generates a signature for @p data and adds it as the SignatureValue block of @p data.
   *
   * @param data The data to sign
   * @param params The signing parameters
   * @throw Error Signing failed
   * @throw InvalidSigningInfoError Invalid @p params was specified or the specified identity, key,
   *                                or certificate does not exist
   * @see SigningInfo
   * @see SignatureInfo
   */
  void
  sign(Data& data, const SigningInfo& params = SigningInfo());

  /**
   * @brief Sign an Interest according to the supplied signing information.
   *
   * This method uses the supplied signing information in @p params to sign @p interest as follows:
   * - It selects a private key and its associated certificate to sign the packet.
   * - It generates a KeyLocator based upon the certificate name.
   * - Using the SignatureInfo in @p params as a base, it generates the final SignatureInfo block
   *   for @p interest.
   * - It adds the generated SignatureInfo element to @p interest. If Packet Specification v0.3
   *   formatting is desired, this block will be appended to @p interest as a separate
   *   InterestSignatureInfo element. Otherwise, it will be appended to the end of the name of
   *   @p interest as a SignatureInfo block.
   * - It generates a signature for @p interest. If Packet Specification v0.3 formatting is
   *   desired, this block will be added to @p interest as a separate InterestSignatureValue
   *   element. Otherwise, it will be appended to the end of the name of @p interest as a
   *   SignatureValue block.
   *
   * @param interest The interest to sign
   * @param params The signing parameters
   * @throw Error Signing failed
   * @throw InvalidSigningInfoError Invalid @p params was specified or the specified identity, key,
   *                                or certificate does not exist
   * @see SigningInfo
   * @see SignatureInfo
   * @see https://docs.named-data.net/NDN-packet-spec/0.3/signed-interest.html
   */
  void
  sign(Interest& interest, const SigningInfo& params = SigningInfo());

  /**
   * @brief Create and sign a certificate packet.
   * @param publicKey Public key being certified. It does not need to exist in this KeyChain.
   * @param params Signing parameters. The referenced key must exist in this KeyChain.
   *               It may contain SignatureInfo for customizing KeyLocator and CustomTlv (including
   *               AdditionalDescription), but ValidityPeriod will be overwritten.
   * @param opts Optional arguments.
   * @return A certificate of @p publicKey signed by a key from this KeyChain found by @p params .
   * @throw std::invalid_argument @p opts.freshnessPeriod is not positive.
   * @throw Error Certificate signing failure.
   */
  Certificate
  makeCertificate(const pib::Key& publicKey, const SigningInfo& params = SigningInfo(),
                  const MakeCertificateOptions& opts = {});

  /**
   * @brief Create and sign a certificate packet.
   * @param certRequest Certificate request enclosing the public key being certified.
   *                    It does not need to exist in this KeyChain.
   * @param params Signing parameters. The referenced key must exist in this KeyChain.
   *               It may contain SignatureInfo for customizing KeyLocator and CustomTlv (including
   *               AdditionalDescription), but ValidityPeriod will be overwritten.
   * @param opts Optional arguments.
   * @return A certificate of the public key enclosed in @p certRequest signed by a key from this
   *         KeyChain found by @p params .
   * @throw std::invalid_argument @p opts.freshnessPeriod is not positive.
   * @throw std::invalid_argument @p certRequest contains invalid public key.
   * @throw Error Certificate signing failure.
   */
  Certificate
  makeCertificate(const Certificate& certRequest, const SigningInfo& params = SigningInfo(),
                  const MakeCertificateOptions& opts = {});

public: // export & import
  /**
   * @brief Export a certificate and its corresponding private key.
   *
   * @param certificate The certificate to export.
   * @param pw The password to secure the private key.
   * @param pwLen The length of password.
   * @return A SafeBag carrying the certificate and encrypted private key.
   * @throw Error the certificate or private key does not exist
   */
  shared_ptr<SafeBag>
  exportSafeBag(const Certificate& certificate, const char* pw, size_t pwLen);

  /**
   * @brief Import a certificate and its corresponding private key from a SafeBag.
   *
   * If the certificate and key are imported properly, the default setting will be updated as if
   * a new key and certificate is added into KeyChain.
   *
   * @param safeBag The encoded data to import.
   * @param pw The password to secure the private key.
   * @param pwLen The length of password.
   * @throw Error any of following conditions:
   *              - the safebag cannot be decoded or its content does not match;
   *              - private key cannot be imported;
   *              - a private/public key of the same name already exists;
   *              - a certificate of the same name already exists.
   */
  void
  importSafeBag(const SafeBag& safeBag, const char* pw, size_t pwLen);

  /**
   * @brief Import a private key into the TPM.
   */
  void
  importPrivateKey(const Name& keyName, shared_ptr<transform::PrivateKey> key);

public: // PIB & TPM backend registry
  /**
   * @brief Register a new PIB backend type.
   * @param scheme Unique identifier for the registered PIB backend type.
   */
  template<class PibBackendType>
  static void
  registerPibBackend(const std::string& scheme)
  {
    getPibFactories().emplace(scheme, [] (const std::string& location) {
      return shared_ptr<pib::PibImpl>(new PibBackendType(location));
    });
  }

  /**
   * @brief Register a new TPM backend type.
   * @param scheme Unique identifier for the registered TPM backend type.
   */
  template<class TpmBackendType>
  static void
  registerTpmBackend(const std::string& scheme)
  {
    getTpmFactories().emplace(scheme, [] (const std::string& location) {
      return unique_ptr<tpm::BackEnd>(new TpmBackendType(location));
    });
  }

private:
  class Locator;

  KeyChain(Locator pibLocator, Locator tpmLocator, bool allowReset);

  using PibFactories = std::map<std::string, std::function<shared_ptr<pib::PibImpl>(const std::string&)>>;
  using TpmFactories = std::map<std::string, std::function<unique_ptr<tpm::BackEnd>(const std::string&)>>;

  static PibFactories&
  getPibFactories();

  static TpmFactories&
  getTpmFactories();

  static Locator
  parseAndCheckPibLocator(const std::string& pibLocator);

  static Locator
  parseAndCheckTpmLocator(const std::string& tpmLocator);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static const Locator&
  getDefaultPibLocator();

  static const Locator&
  getDefaultTpmLocator();

#ifdef NDN_CXX_HAVE_TESTS
  static void
  resetDefaultLocators();
#endif

  /**
   * @brief Derive SignatureTypeValue according to key type and digest algorithm.
   */
  static tlv::SignatureTypeValue
  getSignatureType(KeyType keyType, DigestAlgorithm digestAlgorithm);

private: // signing
  Certificate
  makeCertificate(const Name& keyName, span<const uint8_t> publicKey, SigningInfo params,
                  const MakeCertificateOptions& opts);

  /**
   * @brief Generate a self-signed certificate for a public key.
   *
   * The self-signed certificate will also be added to the PIB.
   */
  Certificate
  selfSign(Key& key);

  /**
   * @brief Prepare a SignatureInfo TLV according to signing information and return the signing
   *        key name.
   *
   * @param params The signing parameters
   * @return The signing key name and prepared SignatureInfo
   * @throw InvalidSigningInfoError The requested signing method cannot be satisfied
   */
  std::tuple<Name, SignatureInfo>
  prepareSignatureInfo(const SigningInfo& params);

  static std::tuple<Name, SignatureInfo>
  prepareSignatureInfoSha256(const SigningInfo& params);

  static std::tuple<Name, SignatureInfo>
  prepareSignatureInfoHmac(const SigningInfo& params, Tpm& tpm);

  static std::tuple<Name, SignatureInfo>
  prepareSignatureInfoWithIdentity(const SigningInfo& params, const pib::Identity& identity);

  static std::tuple<Name, SignatureInfo>
  prepareSignatureInfoWithKey(const SigningInfo& params, const pib::Key& key,
                              const std::optional<Name>& certName = std::nullopt);

  /**
   * @brief Generate and return a raw signature for the byte ranges in @p bufs using
   *        the specified key and digest algorithm.
   */
  ConstBufferPtr
  sign(const InputBuffers& bufs, const Name& keyName, DigestAlgorithm digestAlgorithm) const;

private:
  unique_ptr<Pib> m_pib;
  unique_ptr<Tpm> m_tpm;

  static Locator s_defaultPibLocator;
  static Locator s_defaultTpmLocator;
};

} // namespace ndn::security

namespace ndn {
using security::KeyChain;
} // namespace ndn

/**
 * @brief Register Pib backend class in KeyChain
 *
 * This macro should be placed once in the implementation file of the
 * Pib backend class, inside the namespace where the type is declared.
 */
#define NDN_CXX_KEYCHAIN_REGISTER_PIB_BACKEND(PibType)     \
static class NdnCxxAuto ## PibType ## PibRegistrationClass    \
{                                                             \
public:                                                       \
  NdnCxxAuto ## PibType ## PibRegistrationClass()             \
  {                                                           \
    ::ndn::security::KeyChain::registerPibBackend<PibType>(PibType::getScheme()); \
  }                                                           \
} ndnCxxAuto ## PibType ## PibRegistrationVariable

/**
 * @brief Register Tpm backend class in KeyChain
 *
 * This macro should be placed once in the implementation file of the
 * Tpm backend class, inside the namespace where the type is declared.
 */
#define NDN_CXX_KEYCHAIN_REGISTER_TPM_BACKEND(TpmType)     \
static class NdnCxxAuto ## TpmType ## TpmRegistrationClass    \
{                                                             \
public:                                                       \
  NdnCxxAuto ## TpmType ## TpmRegistrationClass()             \
  {                                                           \
    ::ndn::security::KeyChain::registerTpmBackend<TpmType>(TpmType::getScheme()); \
  }                                                           \
} ndnCxxAuto ## TpmType ## TpmRegistrationVariable

#endif // NDN_CXX_SECURITY_KEY_CHAIN_HPP
