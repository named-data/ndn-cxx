/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_TPM_MEMORY_HPP
#define NDN_SEC_TPM_MEMORY_HPP

#include <map>
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

  /**
   * The virtual destructor
   */    
  virtual 
  ~SecTpmMemory();

  /**
   * Set the public and private key for the keyName.
   * @param keyName The key name.
   * @param publicKeyDer The public key DER byte array.
   * @param publicKeyDerLength The length of publicKeyDer.
   * @param privateKeyDer The private key DER byte array.
   * @param privateKeyDerLength The length of privateKeyDer.
   */
  void setKeyPairForKeyName(const Name& keyName,
                            uint8_t *publicKeyDer, size_t publicKeyDerLength,
                            uint8_t *privateKeyDer, size_t privateKeyDerLength);
  
  /**
   * Generate a pair of asymmetric keys.
   * @param keyName The name of the key pair.
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   */
  virtual void 
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize);

  /**
   * Get the public key
   * @param keyName The name of public key.
   * @return The public key.
   */
  virtual ptr_lib::shared_ptr<PublicKey> 
  getPublicKeyFromTpm(const Name& keyName);
  
  /**
   * Fetch the private key for keyName and sign the data, returning a signature Blob.
   * @param data Pointer to the input byte array.
   * @param dataLength The length of data.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @return The signature, or a null pointer if signing fails.
   */  
  virtual Block 
  signInTpm(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm);

  virtual void 
  signInTpm(Data &data, const Name& keyName, DigestAlgorithm digestAlgorithm);
  
  /**
   * Decrypt data.
   * @param keyName The name of the decrypting key.
   * @param data The byte to be decrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric decryption is used.
   * @return The decrypted data.
   */
  virtual ConstBufferPtr 
  decryptInTpm(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric);

  /**
   * Encrypt data.
   * @param keyName The name of the encrypting key.
   * @param data The byte to be encrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric decryption is used.
   * @return The encrypted data.
   */
  virtual ConstBufferPtr
  encryptInTpm(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric);

  /**
   * @brief Generate a symmetric key.
   * @param keyName The name of the key.
   * @param keyType The type of the key, e.g. KEY_TYPE_AES.
   * @param keySize The size of the key.
   */
  virtual void 
  generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize);

  /**
   * Check if a particular key exists.
   * @param keyName The name of the key.
   * @param keyClass The class of the key, e.g. KEY_CLASS_PUBLIC, KEY_CLASS_PRIVATE, or KEY_CLASS_SYMMETRIC.
   * @return True if the key exists, otherwise false.
   */
  virtual bool
  doesKeyExistInTpm(const Name& keyName, KeyClass keyClass);  
  
private:
  class RsaPrivateKey;

  typedef std::map<std::string, ptr_lib::shared_ptr<PublicKey> >     PublicKeyStore;
  typedef std::map<std::string, ptr_lib::shared_ptr<RsaPrivateKey> > PrivateKeyStore;
  
  PublicKeyStore  publicKeyStore_;  /**< The map key is the keyName.toUri() */
  PrivateKeyStore privateKeyStore_; /**< The map key is the keyName.toUri() */
};

}

#endif
