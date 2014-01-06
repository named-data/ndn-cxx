/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_KEY_CHAIN_HPP
#define NDN_KEY_CHAIN_HPP

#include "../data.hpp"
#include "../face.hpp"
#include "identity/identity-manager.hpp"
#include "encryption/encryption-manager.hpp"
#include "policy/validation-request.hpp"

namespace ndn {

class PolicyManager;
  
/**
 * KeyChain is the main class of the security library.
 *
 * The KeyChain class provides a set of interfaces to the security library such as identity management, policy configuration 
 * and packet signing and verification.
 */
class KeyChain {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  KeyChain(const ptr_lib::shared_ptr<IdentityManager>   &identityManager   = DefaultIdentityManager,
           const ptr_lib::shared_ptr<PolicyManager>     &policyManager     = DefaultPolicyManager,
           const ptr_lib::shared_ptr<EncryptionManager> &encryptionManager = DefaultEncryptionManager);

  /**
   * @brief Set the Face which will be used to fetch required certificates.
   * @param face A pointer to the Face object.
   *
   * Setting face is necessary for keychain operation that involve fetching data.
   */
  void
  setFace(const ptr_lib::shared_ptr<Face> &face) { face_ = face; }
  
  /*****************************************
   *          Identity Management          *
   *****************************************/

  inline IdentityManager&
  identities()
  {
    if (!identityManager_)
      throw Error("IdentityManager is not assigned to the KeyChain");

    return *identityManager_;
  }

  /*****************************************
   *           Policy Management           *
   *****************************************/

  inline PolicyManager&
  policies()
  {
    if (!policyManager_)
      throw Error("PolicyManager is not assigned to the KeyChain");

    return *policyManager_;
  }

  /*****************************************
   *         Encryption Management         *
   *****************************************/

  inline EncryptionManager&
  encryption()
  {
    if (!encryptionManager_)
      throw Error("EncryptionManager is not assigned to the KeyChain");

    return *encryptionManager_;
  }

  /*****************************************
   *              Sign/Verify              *
   *****************************************/

  inline void 
  sign(Data& data);
  
  /**
   * Wire encode the Data object, sign it and set its signature.
   * Note: the caller must make sure the timestamp is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param certificateName The certificate name of the key to use for signing.  If omitted, infer the signing identity from the data packet name.
   */
  inline void 
  sign(Data& data, const Name& certificateName);
  
  /**
   * Sign the byte array using a certificate name and return a Signature object.
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param certificateName The certificate name used to get the signing key and which will be put into KeyLocator.
   * @return The Signature.
   */
  inline Signature
  sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName);

  /**
   * Wire encode the Data object, sign it and set its signature.
   * Note: the caller must make sure the timestamp is correct, for example with 
   * data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0).
   * @param data The Data object to be signed.  This updates its signature and key locator field and wireEncoding.
   * @param identityName The identity name for the key to use for signing.  If omitted, infer the signing identity from the data packet name.
   */
  void 
  signByIdentity(Data& data, const Name& identityName = Name());

  /**
   * Sign the byte array using an identity name and return a Signature object.
   * @param buffer The byte array to be signed.
   * @param bufferLength the length of buffer.
   * @param identityName The identity name.
   * @return The Signature.
   */
  Signature
  signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName);

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
  // todo

public:
  static const ptr_lib::shared_ptr<IdentityManager>   DefaultIdentityManager;
  static const ptr_lib::shared_ptr<PolicyManager>     DefaultPolicyManager;
  static const ptr_lib::shared_ptr<EncryptionManager> DefaultEncryptionManager;
    
private:
  void
  onCertificateData
    (const ptr_lib::shared_ptr<const Interest> &interest, const ptr_lib::shared_ptr<Data> &data, ptr_lib::shared_ptr<ValidationRequest> nextStep);
  
  void
  onCertificateInterestTimeout
    (const ptr_lib::shared_ptr<const Interest> &interest, int retry, const OnVerifyFailed& onVerifyFailed, 
     const ptr_lib::shared_ptr<Data> &data, ptr_lib::shared_ptr<ValidationRequest> nextStep);

private:
  ptr_lib::shared_ptr<IdentityManager>   identityManager_;
  ptr_lib::shared_ptr<PolicyManager>     policyManager_;
  ptr_lib::shared_ptr<EncryptionManager> encryptionManager_;

  ptr_lib::shared_ptr<Face>        face_;
  
  const int maxSteps_;
};

void 
KeyChain::sign(Data& data)
{
  identities().sign(data);
}

void 
KeyChain::sign(Data& data, const Name& certificateName)
{
  identities().signByCertificate(data, certificateName);
}

Signature
KeyChain::sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
{
  return identities().signByCertificate(buffer, bufferLength, certificateName);
}

}

#endif
