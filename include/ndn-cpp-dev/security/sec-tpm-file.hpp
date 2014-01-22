/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Xingyu Ma <maxy12@cs.ucla.edu>
 *          Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_TPM_FILE_HPP
#define NDN_SEC_TPM_FILE_HPP

#include "../common.hpp"

#include "sec-tpm.hpp"

namespace ndn 
{

class SecTpmFile : public SecTpm
{
public:
  struct Error : public SecTpm::Error { Error(const std::string &what) : SecTpm::Error(what) {} };

  SecTpmFile(const std::string & dir = "");

  /**
   * @brief destructor
   */
  virtual
  ~SecTpmFile() {};

  /**
   * Generate a pair of asymmetric keys.
   * @param keyName The name of the key pair.
   * @param keyType The type of the key pair, e.g. KEY_TYPE_RSA.
   * @param keySize The size of the key pair.
   */
  virtual void
  generateKeyPairInTpm(const Name & keyName, KeyType keyType, int keySize);

  /**
   * Get the public key
   * @param keyName The name of public key.
   * @return The public key.
   */
  virtual ptr_lib::shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name & keyName);

  /**
   * Fetch the private key for keyName and sign the data, returning a signature block.
   * Throw Error if signing fails.
   * @param data Pointer to the input byte array.
   * @param dataLength The length of data.
   * @param keyName The name of the signing key.
   * @param digestAlgorithm the digest algorithm.
   * @return The signature block.
   */  
  virtual Block
  signInTpm(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm);

  /**
   * Decrypt data.
   * @param keyName The name of the decrypting key.
   * @param data The byte to be decrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric encryption is used.
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
   * Generate a symmetric key.
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

  std::string
  nameTransform(const std::string &keyName, const std::string &extension);

private:
  void 
  maintainMapping(std::string str1, std::string str2);
  
private:
  class Impl;
  std::auto_ptr<Impl> impl_;
};
}//ndn

#endif
