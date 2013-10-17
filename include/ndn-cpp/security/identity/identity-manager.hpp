/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_IDENTITY_MANAGER_HPP
#define NDN_IDENTITY_MANAGER_HPP

#include "../certificate/identity-certificate.hpp"
#include "identity-storage.hpp"
#include "../certificate/public-key.hpp"
#include "private-key-storage.hpp"

namespace ndn {

/**
 * An IdentityManager is the interface of operations related to identity, keys, and certificates.
 */
class IdentityManager {
public:
  IdentityManager(const ptr_lib::shared_ptr<IdentityStorage>& identityStorage, const ptr_lib::shared_ptr<PrivateKeyStorage>& privateKeyStorage)
  : identityStorage_(identityStorage), privateKeyStorage_(privateKeyStorage)
  {
  }
  
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
    return identityStorage_->getDefaultIdentity();
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
    identityStorage_->setDefaultKeyNameForIdentity(keyName, identityName);
  }

  /**
   * Get the default key for an identity.
   * @param identityName the name of the identity. If omitted, the identity name is inferred from the keyName.
   * @return The default key name.
   */
  Name
  getDefaultKeyNameForIdentity(const Name& identityName = Name())
  {
    return identityStorage_->getDefaultKeyNameForIdentity(identityName);
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
    return PublicKey::fromDer(identityStorage_->getKey(keyName));
  }

  /**
   * Create an identity certificate for a public key managed by this IdentityManager.
   * @param keyName The name of public key to be signed.
   * @param signerCertificateName The name of signing certificate.
   * @param notBefore The notBefore value in the validity field of the generated certificate.
   * @param notAfter The notAfter vallue in validity field of the generated certificate.
   * @return The name of generated identity certificate.
   */
  Name
  createIdentityCertificate(const Name& keyName, const Name& signerCertificateName, const MillisecondsSince1970& notBefore, const MillisecondsSince1970& notAfter);

  /**
   * Create an identity certificate for a public key supplied by the caller.
   * @param keyName The name of public key to be signed.
   * @param publickey The public key to be signed.
   * @param signerCertificateName The name of signing certificate.
   * @param notBefore The notBefore value in the validity field of the generated certificate.
   * @param notAfter The notAfter vallue in validity field of the generated certificate.
   * @return The generated identity certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  createIdentityCertificate
    (const Name& keyName, const PublicKey& publickey, const Name& signerCertificateName, const MillisecondsSince1970& notBefore, const MillisecondsSince1970& notAfter); 
    
  /**
   * Add a certificate into the public key identity storage.
   * @param certificate The certificate to to added.  This makes a copy of the certificate.
   */
  void
  addCertificate(const IdentityCertificate& certificate)
  {
    identityStorage_->addCertificate(certificate);
  }

  /**
   * Set the certificate as the default for its corresponding key.
   * @param certificateName The name of the certificate.
   */
  void
  setDefaultCertificateForKey(const Name& certificateName);

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
    return ptr_lib::make_shared<IdentityCertificate>(*identityStorage_->getCertificate(certificateName, false));
  }
    
  /**
   * Get a certificate even if the certificate is not valid anymore.
   * @param certificateName The name of the requested certificate.
   * @return the requested certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  getAnyCertificate(const Name& certificateName)
  {
    return ptr_lib::make_shared<IdentityCertificate>(*identityStorage_->getCertificate(certificateName, true));
  }
    
  /**
   * Get the default certificate name for the specified identity, which will be used when signing is performed based on identity.
   * @param identityName The name of the specified identity.
   * @return The requested certificate name.
   */
  Name
  getDefaultCertificateNameForIdentity(const Name& identityName)
  {
    return identityStorage_->getDefaultCertificateNameForIdentity(identityName);
  }
    
  /**
   * Get the default certificate name of the default identity, which will be used when signing is based on identity and 
   * the identity is not specified.
   * @return The requested certificate name.
   */
  Name
  getDefaultCertificateName()
  {
    return identityStorage_->getDefaultCertificateNameForIdentity(getDefaultIdentity());
  }
        
  /**
   * Sign the byte array data based on the certificate name.
   * @param data The data to be signed.
   * @param dataLength the length of data.
   * @param certificateName The signing certificate name.
   * @return The generated signature.
   */
  ptr_lib::shared_ptr<Signature>
  signByCertificate(const uint8_t* data, size_t dataLength, const Name& certificateName);
    
  /**
   * Sign data packet based on the certificate name.
   * Note: the caller must make sure the timestamp in data is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to sign and update its signature.
   * @param certificateName The Name identifying the certificate which identifies the signing key.
   * @param wireFormat The WireFormat for calling encodeData, or WireFormat::getDefaultWireFormat() if omitted.
   */
  void 
  signByCertificate(Data& data, const Name& certificateName, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());
  
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

  /**
   * Generate a self-signed certificate for a public key.
   * @param keyName The name of the public key.
   * @return The generated certificate.
   */
  ptr_lib::shared_ptr<IdentityCertificate>
  selfSign(const Name& keyName);
  
  ptr_lib::shared_ptr<IdentityStorage> identityStorage_;
  ptr_lib::shared_ptr<PrivateKeyStorage> privateKeyStorage_;
};

}

#endif
