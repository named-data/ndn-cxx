/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_PRIVATE_KEY_STORAGE_HPP
#define NDN_PRIVATE_KEY_STORAGE_HPP

#include <string>
#include "../security-common.hpp"
#include "../../name.hpp"
#include "../../data.hpp"

namespace ndn {

class PublicKey;

class PrivateKeyStorage {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * The virtual destructor.
   */    
  virtual 
  ~PrivateKeyStorage() {}

  /**
   * Generate a pair of asymmetric keys.
   * @param keyName The name of the key pair.
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   */
  virtual void 
  generateKeyPair(const Name& keyName, KeyType keyType = KEY_TYPE_RSA, int keySize = 2048) = 0;

  /**
   * Get the public key
   * @param keyName The name of public key.
   * @return The public key.
   */
  virtual ptr_lib::shared_ptr<PublicKey> 
  getPublicKey(const Name& keyName) = 0;
  
  /**
   * Fetch the private key for keyName and sign the data, returning a signature Blob.
   * @param data Pointer to the input byte array.
   * @param dataLength The length of data.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @return The signature, or a null pointer if signing fails.
   */  
  virtual Block
  sign(const uint8_t *data, size_t dataLength,
       const Signature &signature,
       const Name& keyName, DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256) = 0;

  virtual Block
  sign(const Data &data,
       const Signature &signature,
       const Name& keyName, DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256) = 0;
  
  /**
   * Decrypt data.
   * @param keyName The name of the decrypting key.
   * @param data The byte to be decrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric encryption is used.
   * @return The decrypted data.
   */
  virtual ConstBufferPtr 
  decrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric = false) = 0;

  /**
   * Encrypt data.
   * @param keyName The name of the encrypting key.
   * @param data The byte to be encrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric encryption is used.
   * @return The encrypted data.
   */
  virtual ConstBufferPtr
  encrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric = false) = 0;

  /**
   * @brief Generate a symmetric key.
   * @param keyName The name of the key.
   * @param keyType The type of the key, e.g. KEY_TYPE_AES.
   * @param keySize The size of the key.
   */
  virtual void 
  generateKey(const Name& keyName, KeyType keyType = KEY_TYPE_AES, int keySize = 256) = 0;

  /**
   * Check if a particular key exists.
   * @param keyName The name of the key.
   * @param keyClass The class of the key, e.g. KEY_CLASS_PUBLIC, KEY_CLASS_PRIVATE, or KEY_CLASS_SYMMETRIC.
   * @return True if the key exists, otherwise false.
   */
  virtual bool
  doesKeyExist(const Name& keyName, KeyClass keyClass) = 0;  
};

}

#endif
