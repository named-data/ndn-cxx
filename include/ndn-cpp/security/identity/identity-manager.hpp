/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_IDENTITY_MANAGER_HPP
#define NDN_IDENTITY_MANAGER_HPP

#include "identity-storage.hpp"
#include "private-key-storage.hpp"
#include "../certificate/public-key.hpp"

#include "../../data.hpp"
#include "../certificate/identity-certificate.hpp"

namespace ndn {

/**
 * An IdentityManager is the interface of operations related to identity, keys, and certificates.
 */
class IdentityManager {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  IdentityManager(const ptr_lib::shared_ptr<IdentityStorage>   &identityStorage,
                  const ptr_lib::shared_ptr<PrivateKeyStorage> &privateKeyStorage);

  inline IdentityStorage&
  info();

  inline const IdentityStorage&
  info() const;

  inline PrivateKeyStorage&
  tpm();

  inline const PrivateKeyStorage&
  tpm() const;
  
  /**
   * Create an identity by creating a pair of Key-Signing-Key (KSK) for this identity and a self-signed certificate of the KSK.
   * @param identityName The name of the identity.
   * @return The key name of the auto-generated KSK of the identity.
   */
  Name
  createIdentity(const Name& identityName);
    
  /**
   * Get the default identity.
   * @return The default identity name.
   */
  Name
  getDefaultIdentity()
  {
    return info().getDefaultIdentity();
  }

  /**
   * Generate a pair of RSA keys for the specified identity.
   * @param identityName The name of the identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keySize The size of the key.
   * @return The generated key name.
   */
  Name
  generateRSAKeyPair(const Name& identityName, bool isKsk = false, int keySize = 2048);

  /**
   * Set a key as the default key of an identity.
   * @param keyName The name of the key.
   * @param identityName the name of the identity. If not specified, the identity name is inferred from the keyName.
   */
  void
  setDefaultKeyForIdentity(const Name& keyName, const Name& identityName = Name())
  {
    info().setDefaultKeyNameForIdentity(keyName, identityName);
    defaultCertificate_.reset();
  }

  /**
   * Get the default key for an identity.
   * @param identityName the name of the identity. If omitted, the identity name is inferred from the keyName.
   * @return The default key name.
   */
  Name
  getDefaultKeyNameForIdentity(const Name& identityName = Name())
  {
    return info().getDefaultKeyNameForIdentity(identityName);
  }
  
  /**
   * Generate a pair of RSA keys for the specified identity and set it as default key for the identity.
   * @param identityName The name of the identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keySize The size of the key.
   * @return The generated key name.
   */
  Name
  generateRSAKeyPairAsDefault(const Name& identityName, bool isKsk = false, int keySize = 2048);

  /**
   * Get the public key with the specified name.
   * @param keyName The name of the key.
   * @return The public key.
   */
  ptr_lib::shared_ptr<PublicKey>
  getPublicKey(const Name& keyName)
  {
    return info().getKey(keyName);
  }

  /**
   * Create an identity certificate for a public key managed by this IdentityManager.
   * @param certificatePrefix The name of public key to be signed.
   * @param signerCertificateName The name of signing certificate.
   * @param notBefore The notBefore value in the validity field of the generated certificate.
   * @param notAfter The notAfter vallue in validity field of the generated certificate.
   * @return The name of generated identity certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  createIdentityCertificate
    (const Name& certificatePrefix,
     const Name& signerCertificateName,
     const MillisecondsSince1970& notBefore, 
     const MillisecondsSince1970& notAfter);

  /**
   * Create an identity certificate for a public key supplied by the caller.
   * @param certificatePrefix The name of public key to be signed.
   * @param publickey The public key to be signed.
   * @param signerCertificateName The name of signing certificate.
   * @param notBefore The notBefore value in the validity field of the generated certificate.
   * @param notAfter The notAfter vallue in validity field of the generated certificate.
   * @return The generated identity certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  createIdentityCertificate
    (const Name& certificatePrefix,
     const PublicKey& publickey,
     const Name& signerCertificateName, 
     const MillisecondsSince1970& notBefore,
     const MillisecondsSince1970& notAfter); 
    
  /**
   * Add a certificate into the public key identity storage.
   * @param certificate The certificate to to added.  This makes a copy of the certificate.
   */
  void
  addCertificate(const IdentityCertificate& certificate)
  {
    info().addCertificate(certificate);
  }

  /**
   * Set the certificate as the default for its corresponding key.
   * @param certificateName The certificate.
   */
  void
  setDefaultCertificateForKey(const IdentityCertificate& certificate);

  /**
   * Add a certificate into the public key identity storage and set the certificate as the default for its corresponding identity.
   * @param certificate The certificate to be added.  This makes a copy of the certificate.
   */
  void
  addCertificateAsIdentityDefault(const IdentityCertificate& certificate);

  /**
   * Add a certificate into the public key identity storage and set the certificate as the default of its corresponding key.
   * @param certificate The certificate to be added.  This makes a copy of the certificate.
   */
  void
  addCertificateAsDefault(const IdentityCertificate& certificate);

  /**
   * Get a certificate with the specified name.
   * @param certificateName The name of the requested certificate.
   * @return the requested certificate which is valid.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  getCertificate(const Name& certificateName)
  {
    return info().getCertificate(certificateName, false);
  }
    
  /**
   * Get a certificate even if the certificate is not valid anymore.
   * @param certificateName The name of the requested certificate.
   * @return the requested certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  getAnyCertificate(const Name& certificateName)
  {
    return info().getCertificate(certificateName, true);
  }
    
  /**
   * Get the default certificate name for the specified identity, which will be used when signing is performed based on identity.
   * @param identityName The name of the specified identity.
   * @return The requested certificate name.
   */
  Name
  getDefaultCertificateNameForIdentity(const Name& identityName)
  {
    return info().getDefaultCertificateNameForIdentity(identityName);
  }
    
  /**
   * Get the default certificate name of the default identity, which will be used when signing is based on identity and 
   * the identity is not specified.
   * @return The requested certificate name.
   */
  Name
  getDefaultCertificateName()
  {
    return info().getDefaultCertificateNameForIdentity(getDefaultIdentity());
  }

  void
  sign(Data &data);
  
  /**
   * Sign the byte array data based on the certificate name.
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param certificateName The signing certificate name.
   * @return The generated signature.
   */
  Signature
  signByCertificate(const uint8_t* buffer, size_t bufferLength, const Name& certificateName);

  /**
   * Sign data packet based on the certificate name.
   * Note: the caller must make sure the timestamp in data is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to sign and update its signature.
   * @param certificateName The Name identifying the certificate which identifies the signing key.
   * @param wireFormat The WireFormat for calling encodeData, or WireFormat::getDefaultWireFormat() if omitted.
   */
  void 
  signByCertificate(Data& data, const Name& certificateName);

  void
  signByCertificate(Data& data, const IdentityCertificate& certificate);

  /**
   * Generate a self-signed certificate for a public key.
   * @param keyName The name of the public key.
   * @return The generated certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  selfSign(const Name& keyName);

  /**
   * @brief Self-sign the supplied identity certificate
   */
  void
  selfSign (IdentityCertificate& cert);

private:
  /**
   * Generate a key pair for the specified identity.
   * @param identityName The name of the specified identity.
   * @param isKsk true for generating a Key-Signing-Key (KSK), false for a Data-Signing-Key (KSK).
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   * @return The name of the generated key.
   */
  Name
  generateKeyPair(const Name& identityName, bool isKsk = false, KeyType keyType = KEY_TYPE_RSA, int keySize = 2048);

  static Name
  getKeyNameFromCertificatePrefix(const Name& certificatePrefix);

private:
  ptr_lib::shared_ptr<IdentityStorage>   identityStorage_;
  ptr_lib::shared_ptr<PrivateKeyStorage> privateKeyStorage_;

  ptr_lib::shared_ptr<IdentityCertificate> defaultCertificate_;
};

inline IdentityStorage&
IdentityManager::info()
{
  if (!identityStorage_)
    throw Error("IdentityStorage is not assigned to IdentityManager");

  return *identityStorage_;
}

inline const IdentityStorage&
IdentityManager::info() const
{
  if (!identityStorage_)
    throw Error("IdentityStorage is not assigned to IdentityManager");
  
  return *identityStorage_;
}

inline PrivateKeyStorage&
IdentityManager::tpm()
{
  if (!identityStorage_)
    throw Error("PrivateKeyStorage is not assigned to IdentityManager");
  
  return *privateKeyStorage_;
}

inline const PrivateKeyStorage&
IdentityManager::tpm() const
{
  if (!identityStorage_)
    throw Error("PrivateKeyStorage is not assigned to IdentityManager");
  return *privateKeyStorage_;
}

}

#endif
