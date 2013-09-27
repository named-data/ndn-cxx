/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_CHAIN_HPP
#define NDN_KEY_CHAIN_HPP

#include "../data.hpp"
#include "../face.hpp"
#include "identity/identity-manager.hpp"
#include "encryption/encryption-manager.hpp"

namespace ndn {

class PolicyManager;
  
/**
 * An OnVerified function object is used to pass a callback to verifyData to report a successful verification.
 */
typedef func_lib::function<void(const ptr_lib::shared_ptr<Data>& data)> OnVerified;

/**
 * An OnVerifyFailed function object is used to pass a callback to verifyData to report a failed verification.
 */
typedef func_lib::function<void(const ptr_lib::shared_ptr<Data>& data)> OnVerifyFailed;

/**
 * Keychain is the main class of the security library.
 *
 * The Keychain class provides a set of interfaces to the security library such as identity management, policy configuration 
 * and packet signing and verification.
 */
class KeyChain {
public:
  KeyChain
    (const ptr_lib::shared_ptr<IdentityManager>& identityManager, const ptr_lib::shared_ptr<PolicyManager>& policyManager);

  /*****************************************
   *          Identity Management          *
   *****************************************/

#if 0
  /**
   * Create an identity by creating a pair of Key-Signing-Key (KSK) for this identity and a self-signed certificate of the KSK.
   * @param identityName The name of the identity.
   * @return The key name of the auto-generated KSK of the identity.
   */
  Name
  createIdentity(const Name& identityName)
  {
    return identityManager_->createIdentity(identityName);
  }
#endif

  /**
   * Get the default identity.
   * @return The default identity name.
   */
  Name
  getDefaultIdentity()
  {
    return identityManager_->getDefaultIdentity();
  }
  
#if 0
  /**
   * Generate a pair of RSA keys for the specified identity
   * @param identity the name of the identity
   * @param ksk create a KSK or not, true for KSK, false for DSK 
   * @param keySize the size of the key
   * @return the generated key name 
   */
  Name
  generateRSAKeyPair (const Name& identity, bool ksk = false, int keySize = 2048);

  /**
   * Set a key as the default key of an identity
   * @param keyName the name of the key
   * @param identity the name of the identity, if not specified the identity name can be inferred from the keyName
   */
  void
  setDefaultKeyForIdentity (const Name& keyName, const Name& identity = Name());

  /**
   * Generate a pair of RSA keys for the specified identity and set it as default key of the identity
   * @param identity the name of the identity
   * @param ksk create a KSK or not, true for KSK, false for DSK 
   * @param keySize the size of the key
   * @return the generated key name
   */
  Name
  generateRSAKeyPairAsDefault (const Name& identity, bool ksk = false, int keySize = 2048);

  /**
   * Create a public key signing request
   * @param keyName the name of the key
   * @returns signing request blob
   */
  Ptr<Blob> 
  createSigningRequest(const Name& keyName);

  /**
   * Install a certificate into identity
   * @param certificate the certificate in terms of Data packet
   */
  void 
  installCertificate(Ptr<Certificate> certificate);

  /**
   * Set a certificate as the default certificate name of the corresponding key
   * @param certificateName the name of the certificate
   */
  void
  setDefaultCertificateForKey(const Name& certificateName);

  /**
   * Get certificate
   * @param certificateName name of the certificate
   * @returns certificate that is valid 
   */
  Ptr<Certificate> 
  getCertificate(const Name& certificateName);

  /**
   * Get certificate even if it is not valid
   * @param certificateName name of the certificate
   * @returns certificate that is valid 
   */
  Ptr<Certificate>
  getAnyCertificate(const Name& certName);

  /**
   * Revoke a key
   * @param keyName the name of the key that will be revoked
   */
  void 
  revokeKey(const Name & keyName);

  /**
   * Revoke a certificate
   * @param certificateName the name of the certificate that will be revoked
   */
  void 
  revokeCertificate(const Name & certificateName);
#endif

  /*****************************************
   *           Policy Management           *
   *****************************************/

  const ptr_lib::shared_ptr<PolicyManager>&
  getPolicyManager() { return policyManager_; }
  
  /*****************************************
   *              Sign/Verify              *
   *****************************************/

  /**
   * Wire encode the Data object, sign it and set its signature.
   * Note: the caller must make sure the timestamp is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param certificateName The certificate name of the key to use for signing.  If omitted, infer the signing identity from the data packet name.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  sign(Data& data, const Name& certificateName, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());
  
  /**
   * Wire encode the Data object, sign it and set its signature.
   * Note: the caller must make sure the timestamp is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param identityName The identity name for the key to use for signing.  If omitted, infer the signing identity from the data packet name.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  signByIdentity(Data& data, const Name& identityName = Name(), WireFormat& wireFormat = *WireFormat::getDefaultWireFormat());

  /**
   * Check the signature on the Data object and call either onVerify or onVerifyFailed. 
   * We use callback functions because verify may fetch information to check the signature.
   * @param data The Data object with the signature to check. It is an error if data does not have a wireEncoding. 
   * To set the wireEncoding, you can call data.wireDecode.
   * @param onVerified If the signature is verified, this calls onVerified(data).
   * @param onVerifyFailed If the signature check fails, this calls onVerifyFailed(data).
   */
  void
  verifyData
    (const ptr_lib::shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed, int stepCount = 0);

  /*****************************************
   *           Encrypt/Decrypt             *
   *****************************************/

  /**
   * Generate a symmetric key.
   * @param keyName The name of the generated key.
   * @param keyType The type of the key, e.g. KEY_TYPE_AES
   */
  void 
  generateSymmetricKey(const Name& keyName, KeyType keyType)
  {
    encryptionManager_->createSymmetricKey(keyName, keyType);
  }

  /**
   * Encrypt a byte array.
   * @param keyName The name of the encrypting key.
   * @param data The byte array that will be encrypted.
   * @param dataLength The length of data.
   * @param useSymmetric If true then symmetric encryption is used, otherwise asymmetric encryption is used.
   * @param encryptMode the encryption mode
   * @return the encrypted data as an immutable Blob.
   */
  Blob
  encrypt(const Name &keyName, const uint8_t* data, size_t dataLength, bool useSymmetric = true, 
          EncryptMode encryptMode = ENCRYPT_MODE_DEFAULT)
  {
    return encryptionManager_->encrypt(keyName, data, dataLength, useSymmetric, encryptMode);
  }

  /**
   * Decrypt a byte array.
   * @param keyName The name of the decrypting key.
   * @param data The byte array that will be decrypted.
   * @param dataLength The length of data.
   * @param useSymmetric If true then symmetric encryption is used, otherwise asymmetric encryption is used.
   * @param encryptMode the encryption mode
   * @return the decrypted data as an immutable Blob.
   */
  Blob
  decrypt(const Name &keyName, const uint8_t* data, size_t dataLength, bool useSymmetric = true, 
          EncryptMode encryptMode = ENCRYPT_MODE_DEFAULT)
  {
     return encryptionManager_->decrypt(keyName, data, dataLength, useSymmetric, encryptMode);
  }
  
  /**
   * Set the Face which will be used to fetch required certificates.
   * @param face A pointer to the Face object.
   */
  void
  setFace(Face* face) { face_ = face; }

private:
  ptr_lib::shared_ptr<IdentityManager> identityManager_;
  ptr_lib::shared_ptr<PolicyManager> policyManager_;
  ptr_lib::shared_ptr<EncryptionManager> encryptionManager_;
  Face* face_;
  const int maxSteps_;
};

}

#endif
