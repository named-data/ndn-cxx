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

#ifndef NDN_SECURITY_V2_KEY_CHAIN_HPP
#define NDN_SECURITY_V2_KEY_CHAIN_HPP

#include "../security-common.hpp"
#include "certificate.hpp"
#include "../key-params.hpp"
#include "../pib/pib.hpp"
#include "../safe-bag.hpp"
#include "../signing-info.hpp"
#include "../tpm/tpm.hpp"
#include "../../interest.hpp"

namespace ndn {
namespace security {
namespace v2 {

/**
 * @brief The interface of signing key management.
 *
 * The KeyChain class provides an interface to manage entities related to packet signing,
 * such as Identity, Key, and Certificates.  It consists of two parts: a private key module
 * (TPM) and a public key information base (PIB).  Managing signing key and its related
 * entities through KeyChain interface guarantees the consistency between TPM and PIB.
 */
class KeyChain : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
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
   * Default PIB and TPM are platform-dependent and can be overriden system-wide or
   * individually for the user.
   *
   * @sa manpage ndn-client.conf
   *
   * @todo Add detailed description about config file behavior here
   */
  KeyChain();

  /**
   * @brief KeyChain constructor
   *
   * @sa manpage ndn-client.conf
   *
   * @param pibLocator PIB locator, e.g., `pib-sqlite3:/example/dir`
   * @param tpmLocator TPM locator, e.g., `tpm-memory:`
   * @param allowReset if true, the PIB will be reset when the supplied @p tpmLocator
   *                   does not match the one in the PIB
   */
  KeyChain(const std::string& pibLocator, const std::string& tpmLocator, bool allowReset = false);

  ~KeyChain();

  const Pib&
  getPib() const
  {
    return *m_pib;
  }

  const Tpm&
  getTpm() const
  {
    return *m_tpm;
  }

public: // Identity management
  /**
   * @brief Create an identity @p identityName.
   *
   * This method will check if the identity exists in PIB and whether the identity has a
   * default key and default certificate.  If the identity does not exist, this method will
   * create the identity in PIB.  If the identity's default key does not exist, this method
   * will create a key pair and set it as the identity's default key.  If the key's default
   * certificate is missing, this method will create a self-signed certificate for the key.
   *
   * If @p identityName did not exist and no default identity was selected before, the created
   * identity will be set as the default identity
   *
   * @param identityName The name of the identity.
   * @param params The key parameters if a key needs to be created for the identity (default:
   *               EC key with random key id)
   * @return The created Identity instance.
   */
  Identity
  createIdentity(const Name& identityName, const KeyParams& params = getDefaultKeyParams());

  /**
   * @brief delete @p identity.
   *
   * @pre @p identity must be valid.
   * @post @p identity becomes invalid.
   */
  void
  deleteIdentity(const Identity& identity);

  /**
   * @brief Set @p identity as the default identity.
   * @pre @p identity must be valid.
   */
  void
  setDefaultIdentity(const Identity& identity);

public: // Key management
  /**
   * @brief Create a key for @p identity according to @p params.
   *
   * @param identity reference to a valid Identity object
   * @param params The key parameters if a key needs to be created for the identity (default:
   *               EC key with random key id)
   *
   * If @p identity had no default key selected, the created key will be set as the default for
   * this identity.
   *
   * This method will also create a self-signed certificate for the created key.
   * @pre @p identity must be valid.
   */
  Key
  createKey(const Identity& identity, const KeyParams& params = getDefaultKeyParams());

  /**
   * @brief Delete a key @p key of @p identity.
   *
   * @pre @p identity must be valid.
   * @pre @p key must be valid.
   * @post @p key becomes invalid.
   * @throw std::invalid_argument @p key does not belong to @p identity
   */
  void
  deleteKey(const Identity& identity, const Key& key);

  /**
   * @brief Set @p key as the default key of @p identity.
   *
   * @pre @p identity must be valid.
   * @pre @p key must be valid.
   * @throw std::invalid_argument @p key does not belong to @p identity
   */
  void
  setDefaultKey(const Identity& identity, const Key& key);

public: // Certificate management
  /**
   * @brief Add a certificate @p certificate for @p key
   *
   * If @p key had no default certificate selected, the added certificate will be set as the
   * default certificate for this key.
   *
   * @note This method overwrites certificate with the same name, without considering the
   *       implicit digest.
   *
   * @pre @p key must be valid.
   * @throw std::invalid_argument @p key does not match @p certificate
   */
  void
  addCertificate(const Key& key, const Certificate& certificate);

  /**
   * @brief delete a certificate with name @p certificateName of @p key.
   *
   * If the certificate @p certificateName does not exist, this method has no effect.
   *
   * @pre @p key must be valid.
   * @throw std::invalid_argument @p certificateName does not follow certificate naming convention.
   */
  void
  deleteCertificate(const Key& key, const Name& certificateName);

  /**
   * @brief Set @p cert as the default certificate of @p key.
   *
   * The certificate @p cert will be added to the @p key, potentially overriding existing
   * certificate if it has the same name (without considering implicit digest).
   *
   * @pre @p key must be valid.
   * @throw std::invalid_argument @p key does not match @p certificate
   */
  void
  setDefaultCertificate(const Key& key, const Certificate& certificate);

public: // signing
  /**
   * @brief Sign data according to the supplied signing information.
   *
   * This method uses the supplied signing information @p params to create the SignatureInfo block:
   * - it selects a private key and its certificate to sign the packet
   * - sets the KeyLocator field with the certificate name, and
   * - adds other requested information to the SignatureInfo block.
   *
   * After that, the method assigns the created SignatureInfo to the data packets, generate a
   * signature and sets as part of the SignatureValue block.
   *
   * @note The exception throwing semantics has changed from v1::KeyChain.
   *       If the requested identity/key/certificate does not exist, it will **not** be created
   *       and exception will be thrown.
   *
   * @param data The data to sign
   * @param params The signing parameters.
   * @throw Error signing fails
   * @throw InvalidSigningInfoError invalid @p params is specified or specified identity, key,
   *                                or certificate does not exist
   * @see SigningInfo
   */
  void
  sign(Data& data, const SigningInfo& params = getDefaultSigningInfo());

  /**
   * @brief Sign interest according to the supplied signing information
   *
   * This method uses the supplied signing information @p params to create the SignatureInfo block:
   * - it selects a private key and its certificate to sign the packet
   * - sets the KeyLocator field with the certificate name, and
   * - adds other requested information to the SignatureInfo block.
   *
   * After that, the method appends the created SignatureInfo to the interest name, generate a
   * signature and appends it as part of the SignatureValue block to the interest name.
   *
   * @note The exception throwing semantics has changed from v1::KeyChain.  If the requested
   *       identity/key/certificate does not exist, it will **not** be created and exception
   *       will be thrown.
   *
   * @param interest The interest to sign
   * @param params The signing parameters.
   * @throw Error signing fails
   * @throw InvalidSigningInfoError invalid @p params is specified or specified identity, key,
   *                                or certificate does not exist
   * @see SigningInfo
   * @see docs/specs/signed-interest.rst
   */
  void
  sign(Interest& interest, const SigningInfo& params = getDefaultSigningInfo());

  /**
   * @brief Sign buffer according to the supplied signing information @p params
   *
   * If @p params refers to an identity, the method selects the default key of the identity.
   * If @p params refers to a key or certificate, the method select the corresponding key.
   *
   * @param buffer The buffer to sign
   * @param bufferLength The buffer size
   * @param params The signing parameters.
   * @return a SignatureValue TLV block
   * @throw Error signing fails
   * @see SigningInfo
   */
  Block
  sign(const uint8_t* buffer, size_t bufferLength, const SigningInfo& params = getDefaultSigningInfo());

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
   * @brief Import a pair of certificate and its corresponding private key encapsulated in a SafeBag.
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

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Derive SignatureTypeValue according to key type and digest algorithm.
   */
  static tlv::SignatureTypeValue
  getSignatureType(KeyType keyType, DigestAlgorithm digestAlgorithm);

public: // PIB & TPM backend registry
  /**
   * @brief Register a new PIB backend
   * @param scheme Name for the registered PIB backend scheme
   *
   * @note This interface is implementation detail and may change without notice.
   */
  template<class PibBackendType>
  static void
  registerPibBackend(const std::string& scheme);

  /**
   * @brief Register a new TPM backend
   * @param scheme Name for the registered TPM backend scheme
   *
   * @note This interface is implementation detail and may change without notice.
   */
  template<class TpmBackendType>
  static void
  registerTpmBackend(const std::string& scheme);

private:
  typedef std::map<std::string, function<unique_ptr<pib::PibImpl>(const std::string& location)>> PibFactories;
  typedef std::map<std::string, function<unique_ptr<tpm::BackEnd>(const std::string& location)>> TpmFactories;

  static PibFactories&
  getPibFactories();

  static TpmFactories&
  getTpmFactories();

  static std::tuple<std::string/*type*/, std::string/*location*/>
  parseAndCheckPibLocator(const std::string& pibLocator);

  static std::tuple<std::string/*type*/, std::string/*location*/>
  parseAndCheckTpmLocator(const std::string& tpmLocator);

  static const std::string&
  getDefaultPibScheme();

  static const std::string&
  getDefaultTpmScheme();

  /**
    * @brief Create a PIB according to @p pibLocator
    */
  static unique_ptr<Pib>
  createPib(const std::string& pibLocator);

  /**
   * @brief Create a TPM according to @p tpmLocator
   */
  static unique_ptr<Tpm>
  createTpm(const std::string& tpmLocator);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static const std::string&
  getDefaultPibLocator();

  static const std::string&
  getDefaultTpmLocator();

private: // signing
  /**
   * @brief Generate a self-signed certificate for a public key.
   *
   * The self-signed certificate will also be added into PIB
   *
   * @param keyName The name of the public key
   * @return The generated certificate
   */
  Certificate
  selfSign(Key& key);

  /**
   * @brief Prepare a SignatureInfo TLV according to signing information and return the signing
   *        key name
   *
   * @param sigInfo The SignatureInfo to prepare.
   * @param params The signing parameters.
   * @return The signing key name and prepared SignatureInfo.
   * @throw InvalidSigningInfoError when the requested signing method cannot be satisfied.
   */
  std::tuple<Name, SignatureInfo>
  prepareSignatureInfo(const SigningInfo& params);

  /**
   * @brief Generate a SignatureValue block for a buffer @p buf with size @p size using
   *        a key with name @p keyName and digest algorithm @p digestAlgorithm.
   */
  Block
  sign(const uint8_t* buf, size_t size, const Name& keyName, DigestAlgorithm digestAlgorithm) const;

public:
  static const SigningInfo&
  getDefaultSigningInfo();

  static const KeyParams&
  getDefaultKeyParams();

private:
  std::unique_ptr<Pib> m_pib;
  std::unique_ptr<Tpm> m_tpm;

  static std::string s_defaultPibLocator;
  static std::string s_defaultTpmLocator;
};

template<class PibType>
inline void
KeyChain::registerPibBackend(const std::string& scheme)
{
  getPibFactories().emplace(scheme, [] (const std::string& locator) {
      return unique_ptr<pib::PibImpl>(new PibType(locator));
    });
}

template<class TpmType>
inline void
KeyChain::registerTpmBackend(const std::string& scheme)
{
  getTpmFactories().emplace(scheme, [] (const std::string& locator) {
      return unique_ptr<tpm::BackEnd>(new TpmType(locator));
    });
}

/**
 * @brief Register Pib backend class in KeyChain
 *
 * This macro should be placed once in the implementation file of the
 * Pib backend class within the namespace where the type is declared.
 *
 * @note This interface is implementation detail and may change without notice.
 */
#define NDN_CXX_V2_KEYCHAIN_REGISTER_PIB_BACKEND(PibType)     \
static class NdnCxxAuto ## PibType ## PibRegistrationClass    \
{                                                             \
public:                                                       \
  NdnCxxAuto ## PibType ## PibRegistrationClass()             \
  {                                                           \
    ::ndn::security::v2::KeyChain::registerPibBackend<PibType>(PibType::getScheme()); \
  }                                                           \
} ndnCxxAuto ## PibType ## PibRegistrationVariable

/**
 * @brief Register Tpm backend class in KeyChain
 *
 * This macro should be placed once in the implementation file of the
 * Tpm backend class within the namespace where the type is declared.
 *
 * @note This interface is implementation detail and may change without notice.
 */
#define NDN_CXX_V2_KEYCHAIN_REGISTER_TPM_BACKEND(TpmType)     \
static class NdnCxxAuto ## TpmType ## TpmRegistrationClass    \
{                                                             \
public:                                                       \
  NdnCxxAuto ## TpmType ## TpmRegistrationClass()             \
  {                                                           \
    ::ndn::security::v2::KeyChain::registerTpmBackend<TpmType>(TpmType::getScheme()); \
  }                                                           \
} ndnCxxAuto ## TpmType ## TpmRegistrationVariable

} // namespace v2

using v2::KeyChain;

} // namespace security

using security::v2::KeyChain;

} // namespace ndn

#endif // NDN_SECURITY_V2_KEY_CHAIN_HPP
