/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_TPM_MEMORY_HPP
#define NDN_SECURITY_SEC_TPM_MEMORY_HPP

#include "../common.hpp"
#include "sec-tpm.hpp"

struct rsa_st;

namespace ndn {

/**
 * MemoryPrivateKeyStorage extends PrivateKeyStorage to implement a simple in-memory private key store.  You should
 * initialize by calling setKeyPairForKeyName.
 */
class SecTpmMemory : public SecTpm {
public:
  struct Error : public SecTpm::Error { Error(const std::string &what) : SecTpm::Error(what) {} };

  virtual 
  ~SecTpmMemory();

  /******************************
   * From TrustedPlatformModule *
   ******************************/

  virtual void 
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize);

  virtual ptr_lib::shared_ptr<PublicKey> 
  getPublicKeyFromTpm(const Name& keyName);

  virtual void
  deleteKeyPairInTpm(const Name &keyName);

  virtual Block 
  signInTpm(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm);
  
  virtual ConstBufferPtr 
  decryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric);

  virtual ConstBufferPtr
  encryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric);

  virtual void 
  generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize);

  virtual bool
  doesKeyExistInTpm(const Name& keyName, KeyClass keyClass); 

  virtual bool
  generateRandomBlock(uint8_t* res, size_t size);

  /******************************
   *   SecTpmMemory specific    *
   ******************************/

  /**
   * @brief Set the public and private key for the keyName.
   *
   * @param keyName The key name.
   * @param publicKeyDer The public key DER byte array.
   * @param publicKeyDerLength The length of publicKeyDer.
   * @param privateKeyDer The private key DER byte array.
   * @param privateKeyDerLength The length of privateKeyDer.
   */
  void setKeyPairForKeyName(const Name& keyName,
                            uint8_t *publicKeyDer, size_t publicKeyDerLength,
                            uint8_t *privateKeyDer, size_t privateKeyDerLength);

protected:
  /******************************
   * From TrustedPlatformModule *
   ******************************/
  virtual ConstBufferPtr
  exportPrivateKeyPkcs1FromTpm(const Name& keyName);

  virtual bool
  importPrivateKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);
  
  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);
  
  
private:
  class RsaPrivateKey;

  typedef std::map<std::string, ptr_lib::shared_ptr<PublicKey> >     PublicKeyStore;
  typedef std::map<std::string, ptr_lib::shared_ptr<RsaPrivateKey> > PrivateKeyStore;
  
  PublicKeyStore  publicKeyStore_;  /**< The map key is the keyName.toUri() */
  PrivateKeyStore privateKeyStore_; /**< The map key is the keyName.toUri() */
};

} // namespace ndn

#endif //NDN_SECURITY_SEC_TPM_MEMORY_HPP
