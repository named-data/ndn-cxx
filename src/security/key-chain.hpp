/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_SECURITY_KEY_CHAIN_HPP
#define NDN_SECURITY_KEY_CHAIN_HPP

#include "sec-public-info.hpp"
#include "sec-tpm.hpp"
#include "secured-bag.hpp"
#include "signature-sha256-with-rsa.hpp"
#include "signature-sha256.hpp"

#include "../interest.hpp"
#include "../util/crypto.hpp"


namespace ndn {

template<class TypePib, class TypeTpm>
class KeyChainTraits
{
public:
  typedef TypePib Pib;
  typedef TypeTpm Tpm;
};

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

  KeyChain();

  template<class KeyChainTraits>
  KeyChain(KeyChainTraits traits);

  KeyChain(const std::string& pibName,
           const std::string& tpmName);

  virtual
  ~KeyChain()
  {
    if (m_pib != 0)
      delete m_pib;

    if (m_tpm != 0)
      delete m_tpm;
  }

  /**
   * @brief Create an identity by creating a pair of Key-Signing-Key (KSK) for this identity and a
   *        self-signed certificate of the KSK.
   *
   * @param identityName The name of the identity.
   * @return The name of the default certificate of the identity.
   */
  inline Name
  createIdentity(const Name& identityName);

  /**
   * @brief Generate a pair of RSA keys for the specified identity.
   *
   * @param identityName The name of the identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keySize The size of the key.
   * @return The generated key name.
   */
  inline Name
  generateRsaKeyPair(const Name& identityName, bool isKsk = false, int keySize = 2048);

  /**
   * @brief Generate a pair of RSA keys for the specified identity and set it as default key for
   *        the identity.
   *
   * @param identityName The name of the identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keySize The size of the key.
   * @return The generated key name.
   */
  inline Name
  generateRsaKeyPairAsDefault(const Name& identityName, bool isKsk = false, int keySize = 2048);

  /**
   * @brief prepare an unsigned identity certificate
   *
   * @param keyName Key name, e.g., `/<identity_name>/ksk-123456`.
   * @param signingIdentity The signing identity.
   * @param notBefore Refer to IdentityCertificate.
   * @param notAfter Refer to IdentityCertificate.
   * @param subjectDescription Refer to IdentityCertificate.
   * @return IdentityCertificate.
   */
  shared_ptr<IdentityCertificate>
  prepareUnsignedIdentityCertificate(const Name& keyName,
    const Name& signingIdentity,
    const time::system_clock::TimePoint& notBefore,
    const time::system_clock::TimePoint& notAfter,
    const std::vector<CertificateSubjectDescription>& subjectDescription);

  /**
   * @brief Sign packet with default identity
   *
   * On return, signatureInfo and signatureValue in the packet are set.
   * If default identity does not exist,
   * a temporary identity will be created and set as default.
   *
   * @param packet The packet to be signed
   */
  template<typename T>
  void
  sign(T& packet);

  /**
   * @brief Sign packet with a particular certificate.
   *
   * @param packet The packet to be signed.
   * @param certificateName The certificate name of the key to use for signing.
   * @throws SecPublicInfo::Error if certificate does not exist.
   */
  template<typename T>
  void
  sign(T& packet, const Name& certificateName);

  /**
   * @brief Sign the byte array using a particular certificate.
   *
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param certificateName The certificate name of the signing key.
   * @return The Signature.
   * @throws SecPublicInfo::Error if certificate does not exist.
   */
  Signature
  sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName);

  /**
   * @brief Sign packet using the default certificate of a particular identity.
   *
   * If there is no default certificate of that identity, this method will create a self-signed
   * certificate.
   *
   * @param packet The packet to be signed.
   * @param identityName The signing identity name.
   */
  template<typename T>
  void
  signByIdentity(T& packet, const Name& identityName);

  /**
   * @brief Sign the byte array using the default certificate of a particular identity.
   *
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param identityName The identity name.
   * @return The Signature.
   */
  inline Signature
  signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName);

  /**
   * @brief Set Sha256 weak signature for @param data
   */
  inline void
  signWithSha256(Data& data);

  /**
   * @brief Generate a self-signed certificate for a public key.
   *
   * @param keyName The name of the public key
   * @return The generated certificate, shared_ptr<IdentityCertificate>() if selfSign fails
   */
  shared_ptr<IdentityCertificate>
  selfSign(const Name& keyName);

  /**
   * @brief Self-sign the supplied identity certificate.
   *
   * @param cert The supplied cert.
   * @throws SecTpm::Error if the private key does not exist.
   */
  void
  selfSign(IdentityCertificate& cert);

  /**
   * @brief delete a certificate.
   *
   * If the certificate to be deleted is current default system default,
   * the method will not delete the certificate and return immediately.
   *
   * @param certificateName The certificate to be deleted.
   */
  inline void
  deleteCertificate(const Name& certificateName);

  /**
   * @brief delete a key.
   *
   * If the key to be deleted is current default system default,
   * the method will not delete the key and return immediately.
   *
   * @param keyName The key to be deleted.
   */
  inline void
  deleteKey(const Name& keyName);

  /**
   * @brief delete an identity.
   *
   * If the identity to be deleted is current default system default,
   * the method will not delete the identity and return immediately.
   *
   * @param identity The identity to be deleted.
   */
  inline void
  deleteIdentity(const Name& identity);

  /**
   * @brief export an identity.
   *
   * @param identity The identity to export.
   * @param passwordStr The password to secure the private key.
   * @return The encoded export data.
   * @throws SecPublicInfo::Error if anything goes wrong in exporting.
   */
  shared_ptr<SecuredBag>
  exportIdentity(const Name& identity, const std::string& passwordStr);

  /**
   * @brief import an identity.
   *
   * @param securedBag The encoded import data.
   * @param passwordStr The password to secure the private key.
   */
  void
  importIdentity(const SecuredBag& securedBag, const std::string& passwordStr);

  SecPublicInfo&
  getPib()
  {
    return *m_pib;
  }

  const SecPublicInfo&
  getPib() const
  {
    return *m_pib;
  }

  SecTpm&
  getTpm()
  {
    return *m_tpm;
  }

  const SecTpm&
  getTpm() const
  {
    return *m_tpm;
  }

  /*******************************
   *  Wrapper of SecPublicInfo   *
   *******************************/
  bool
  doesIdentityExist(const Name& identityName) const
  {
    return m_pib->doesIdentityExist(identityName);
  }

  void
  addIdentity(const Name& identityName)
  {
    return m_pib->addIdentity(identityName);
  }

  bool
  doesPublicKeyExist(const Name& keyName) const
  {
    return m_pib->doesPublicKeyExist(keyName);
  }

  void
  addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKeyDer)
  {
    return m_pib->addPublicKey(keyName, keyType, publicKeyDer);
  }

  shared_ptr<PublicKey>
  getPublicKey(const Name& keyName) const
  {
    return m_pib->getPublicKey(keyName);
  }

  bool
  doesCertificateExist(const Name& certificateName) const
  {
    return m_pib->doesCertificateExist(certificateName);
  }

  void
  addCertificate(const IdentityCertificate& certificate)
  {
    return m_pib->addCertificate(certificate);
  }

  shared_ptr<IdentityCertificate>
  getCertificate(const Name& certificateName) const
  {
    return m_pib->getCertificate(certificateName);
  }

  Name
  getDefaultIdentity() const
  {
    return m_pib->getDefaultIdentity();
  }

  Name
  getDefaultKeyNameForIdentity(const Name& identityName) const
  {
    return m_pib->getDefaultKeyNameForIdentity(identityName);
  }

  Name
  getDefaultCertificateNameForKey(const Name& keyName) const
  {
    return m_pib->getDefaultCertificateNameForKey(keyName);
  }

  void
  getAllIdentities(std::vector<Name>& nameList, bool isDefault) const
  {
    return m_pib->getAllIdentities(nameList, isDefault);
  }

  void
  getAllKeyNames(std::vector<Name>& nameList, bool isDefault) const
  {
    return m_pib->getAllKeyNames(nameList, isDefault);
  }

  void
  getAllKeyNamesOfIdentity(const Name& identity, std::vector<Name>& nameList, bool isDefault) const
  {
    return m_pib->getAllKeyNamesOfIdentity(identity, nameList, isDefault);
  }

  void
  getAllCertificateNames(std::vector<Name>& nameList, bool isDefault) const
  {
    return m_pib->getAllCertificateNames(nameList, isDefault);
  }

  void
  getAllCertificateNamesOfKey(const Name& keyName,
                              std::vector<Name>& nameList,
                              bool isDefault) const
  {
    return m_pib->getAllCertificateNamesOfKey(keyName, nameList, isDefault);
  }

  void
  deleteCertificateInfo(const Name& certificateName)
  {
    return m_pib->deleteCertificateInfo(certificateName);
  }

  void
  deletePublicKeyInfo(const Name& keyName)
  {
    return m_pib->deletePublicKeyInfo(keyName);
  }

  void
  deleteIdentityInfo(const Name& identity)
  {
    return m_pib->deleteIdentityInfo(identity);
  }

  void
  setDefaultIdentity(const Name& identityName)
  {
    return m_pib->setDefaultIdentity(identityName);
  }

  void
  setDefaultKeyNameForIdentity(const Name& keyName)
  {
    return m_pib->setDefaultKeyNameForIdentity(keyName);
  }

  void
  setDefaultCertificateNameForKey(const Name& certificateName)
  {
    return m_pib->setDefaultCertificateNameForKey(certificateName);
  }

  Name
  getNewKeyName(const Name& identityName, bool useKsk)
  {
    return m_pib->getNewKeyName(identityName, useKsk);
  }

  Name
  getDefaultCertificateNameForIdentity(const Name& identityName) const
  {
    return m_pib->getDefaultCertificateNameForIdentity(identityName);
  }

  Name
  getDefaultCertificateName() const
  {
    return m_pib->getDefaultCertificateName();
  }

  void
  addCertificateAsKeyDefault(const IdentityCertificate& certificate)
  {
    return m_pib->addCertificateAsKeyDefault(certificate);
  }

  void
  addCertificateAsIdentityDefault(const IdentityCertificate& certificate)
  {
    return m_pib->addCertificateAsIdentityDefault(certificate);
  }

  void
  addCertificateAsSystemDefault(const IdentityCertificate& certificate)
  {
    return m_pib->addCertificateAsSystemDefault(certificate);
  }

  shared_ptr<IdentityCertificate>
  getDefaultCertificate() const
  {
    return m_pib->defaultCertificate();
  }

  void
  refreshDefaultCertificate()
  {
    return m_pib->refreshDefaultCertificate();
  }

  /*******************************
   *  Wrapper of SecTpm          *
   *******************************/

  void
  setTpmPassword(const uint8_t* password, size_t passwordLength)
  {
    return m_tpm->setTpmPassword(password, passwordLength);
  }

  void
  resetTpmPassword()
  {
    return m_tpm->resetTpmPassword();
  }

  void
  setInTerminal(bool inTerminal)
  {
    return m_tpm->setInTerminal(inTerminal);
  }

  bool
  getInTerminal() const
  {
    return m_tpm->getInTerminal();
  }

  bool
  isLocked() const
  {
    return m_tpm->isLocked();
  }

  bool
  unlockTpm(const char* password, size_t passwordLength, bool usePassword)
  {
    return m_tpm->unlockTpm(password, passwordLength, usePassword);
  }

  void
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize)
  {
    return m_tpm->generateKeyPairInTpm(keyName, keyType, keySize);
  }

  void
  deleteKeyPairInTpm(const Name& keyName)
  {
    return m_tpm->deleteKeyPairInTpm(keyName);
  }

  shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name& keyName) const
  {
    return m_tpm->getPublicKeyFromTpm(keyName);
  }

  Block
  signInTpm(const uint8_t* data, size_t dataLength,
            const Name& keyName,
            DigestAlgorithm digestAlgorithm)
  {
    return m_tpm->signInTpm(data, dataLength, keyName, digestAlgorithm);
  }

  ConstBufferPtr
  decryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric)
  {
    return m_tpm->decryptInTpm(data, dataLength, keyName, isSymmetric);
  }

  ConstBufferPtr
  encryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric)
  {
    return m_tpm->encryptInTpm(data, dataLength, keyName, isSymmetric);
  }

  void
  generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize)
  {
    return m_tpm->generateSymmetricKeyInTpm(keyName, keyType, keySize);
  }

  bool
  doesKeyExistInTpm(const Name& keyName, KeyClass keyClass) const
  {
    return m_tpm->doesKeyExistInTpm(keyName, keyClass);
  }

  bool
  generateRandomBlock(uint8_t* res, size_t size) const
  {
    return m_tpm->generateRandomBlock(res, size);
  }

  void
  addAppToAcl(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl)
  {
    return m_tpm->addAppToAcl(keyName, keyClass, appPath, acl);
  }

  ConstBufferPtr
  exportPrivateKeyPkcs5FromTpm(const Name& keyName, const std::string& password)
  {
    return m_tpm->exportPrivateKeyPkcs5FromTpm(keyName, password);
  }

  bool
  importPrivateKeyPkcs5IntoTpm(const Name& keyName,
                               const uint8_t* buf, size_t size,
                               const std::string& password)
  {
    return m_tpm->importPrivateKeyPkcs5IntoTpm(keyName, buf, size, password);
  }

private:
  /**
   * @brief Set default certificate if it is not initialized
   */
  void
  setDefaultCertificateInternal();

  /**
   * @brief Sign a packet using a pariticular certificate.
   *
   * @param packet The packet to be signed.
   * @param certificate The signing certificate.
   */
  template<typename T>
  void
  sign(T& packet, const IdentityCertificate& certificate);

  /**
   * @brief Generate a key pair for the specified identity.
   *
   * @param identityName The name of the specified identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   * @return The name of the generated key.
   */
  inline Name
  generateKeyPair(const Name& identityName, bool isKsk = false,
                  KeyType keyType = KEY_TYPE_RSA, int keySize = 2048);

  /**
   * @brief Sign the data using a particular key.
   *
   * @param data Reference to the data packet.
   * @param signature Signature to be added.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @throws Tpm::Error
   */
  inline void
  signPacketWrapper(Data& data, const SignatureSha256WithRsa& signature,
                    const Name& keyName, DigestAlgorithm digestAlgorithm);

  /**
   * @brief Sign the interest using a particular key.
   *
   * @param interest Reference to the interest packet.
   * @param signature Signature to be added.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @throws Tpm::Error
   */
  inline void
  signPacketWrapper(Interest& interest, const SignatureSha256WithRsa& signature,
                    const Name& keyName, DigestAlgorithm digestAlgorithm);


private:
  SecPublicInfo* m_pib;
  SecTpm* m_tpm;
};

template<class T>
inline
KeyChain::KeyChain(T)
  : m_pib(new typename T::Pib)
  , m_tpm(new typename T::Tpm)
{
}

inline Name
KeyChain::createIdentity(const Name& identityName)
{
  m_pib->addIdentity(identityName);

  Name keyName;
  try
    {
      keyName = m_pib->getDefaultKeyNameForIdentity(identityName);
    }
  catch (SecPublicInfo::Error& e)
    {
      keyName = generateRsaKeyPairAsDefault(identityName, true);
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

inline Name
KeyChain::generateRsaKeyPair(const Name& identityName, bool isKsk, int keySize)
{
  return generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);
}

inline Name
KeyChain::generateRsaKeyPairAsDefault(const Name& identityName, bool isKsk, int keySize)
{
  Name keyName = generateKeyPair(identityName, isKsk, KEY_TYPE_RSA, keySize);

  m_pib->setDefaultKeyNameForIdentity(keyName);

  return keyName;
}

template<typename T>
void
KeyChain::sign(T& packet)
{
  if (!static_cast<bool>(m_pib->defaultCertificate()))
    setDefaultCertificateInternal();

  sign(packet, *m_pib->defaultCertificate());
}

template<typename T>
void
KeyChain::sign(T& packet, const Name& certificateName)
{
  if (!m_pib->doesCertificateExist(certificateName))
    throw SecPublicInfo::Error("Requested certificate [" +
                               certificateName.toUri() + "] doesn't exist");

  SignatureSha256WithRsa signature;
  // implicit conversion should take care
  signature.setKeyLocator(certificateName.getPrefix(-1));

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  signPacketWrapper(packet, signature,
                    IdentityCertificate::certificateNameToPublicKeyName(certificateName),
                    DIGEST_ALGORITHM_SHA256);
}

template<typename T>
void
KeyChain::signByIdentity(T& packet, const Name& identityName)
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

  // We either get or create the signing certificate, sign packet! (no exception unless fatal
  // error in TPM)
  sign(packet, signingCertificateName);
}

inline Signature
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
  return sign(buffer, bufferLength, signingCertificateName);
}

inline void
KeyChain::signWithSha256(Data& data)
{
  SignatureSha256 sig;
  data.setSignature(sig);

  Block sigValue(Tlv::SignatureValue,
                 crypto::sha256(data.wireEncode().value(),
                                data.wireEncode().value_size() -
                                data.getSignature().getValue().size()));
  data.setSignatureValue(sigValue);
}

inline void
KeyChain::deleteCertificate(const Name& certificateName)
{
  try
    {
      if (m_pib->getDefaultCertificateName() == certificateName)
        return;
    }
  catch (SecPublicInfo::Error& e)
    {
      // Not a real error, just try to delete the certificate
    }

  m_pib->deleteCertificateInfo(certificateName);
}

inline void
KeyChain::deleteKey(const Name& keyName)
{
  try
    {
      if (m_pib->getDefaultKeyNameForIdentity(m_pib->getDefaultIdentity()) == keyName)
        return;
    }
  catch (SecPublicInfo::Error& e)
    {
      // Not a real error, just try to delete the key
    }

  m_pib->deletePublicKeyInfo(keyName);
  m_tpm->deleteKeyPairInTpm(keyName);
}

inline void
KeyChain::deleteIdentity(const Name& identity)
{
  try
    {
      if (m_pib->getDefaultIdentity() == identity)
        return;
    }
  catch (SecPublicInfo::Error& e)
    {
      // Not a real error, just try to delete the identity
    }

  std::vector<Name> nameList;
  m_pib->getAllKeyNamesOfIdentity(identity, nameList, true);
  m_pib->getAllKeyNamesOfIdentity(identity, nameList, false);

  m_pib->deleteIdentityInfo(identity);

  std::vector<Name>::const_iterator it = nameList.begin();
  for(; it != nameList.end(); it++)
    m_tpm->deleteKeyPairInTpm(*it);
}

template<typename T>
void
KeyChain::sign(T& packet, const IdentityCertificate& certificate)
{
  SignatureSha256WithRsa signature;
  signature.setKeyLocator(certificate.getName().getPrefix(-1));

  // For temporary usage, we support RSA + SHA256 only, but will support more.
  signPacketWrapper(packet, signature, certificate.getPublicKeyName(), DIGEST_ALGORITHM_SHA256);
}

inline Name
KeyChain::generateKeyPair(const Name& identityName, bool isKsk, KeyType keyType, int keySize)
{
  Name keyName = m_pib->getNewKeyName(identityName, isKsk);

  m_tpm->generateKeyPairInTpm(keyName.toUri(), keyType, keySize);

  shared_ptr<PublicKey> pubKey = m_tpm->getPublicKeyFromTpm(keyName.toUri());
  m_pib->addPublicKey(keyName, keyType, *pubKey);

  return keyName;
}

inline void
KeyChain::signPacketWrapper(Data& data, const SignatureSha256WithRsa& signature,
                            const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  data.setSignature(signature);
  data.setSignatureValue(m_tpm->signInTpm(data.wireEncode().value(),
                                          data.wireEncode().value_size() -
                                          data.getSignature().getValue().size(),
                                          keyName, digestAlgorithm));
}

inline void
KeyChain::signPacketWrapper(Interest& interest, const SignatureSha256WithRsa& signature,
                            const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  Name signedName = interest.getName();
  signedName.append(signature.getInfo());

  Block sigValue = m_tpm->signInTpm(signedName.wireEncode().value(),
                                    signedName.wireEncode().value_size(),
                                    keyName,
                                    digestAlgorithm);
  sigValue.encode();
  signedName.append(sigValue);
  interest.setName(signedName);
}

}

#endif // NDN_SECURITY_KEY_CHAIN_HPP
