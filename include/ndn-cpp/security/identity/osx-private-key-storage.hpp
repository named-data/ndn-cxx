/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_OSX_PRIVATEKEY_STORAGE_H
#define NDN_OSX_PRIVATEKEY_STORAGE_H

// Only compile if ndn-cpp-config.h defines NDN_CPP_HAVE_OSX_SECURITY 1.
#include <ndn-cpp/ndn-cpp-config.h>
#if NDN_CPP_HAVE_OSX_SECURITY

#include "../../common.hpp"
#include "private-key-storage.hpp"

namespace ndn
{
  
class OSXPrivateKeyStorage : public PrivateKeyStorage {
public:
  /**
   * constructor of OSXPrivateKeyStorage
   * @param keychainName the name of keychain
   */
  OSXPrivateKeyStorage(const std::string & keychainName = "");

  /**
   * destructor of OSXPrivateKeyStore
   */    
  virtual 
  ~OSXPrivateKeyStorage();


  // From PrivateKeyStorage
  virtual void 
  generateKeyPair(const Name& keyName, KeyType keyType = KEY_TYPE_RSA, int keySize = 2048);

  virtual ptr_lib::shared_ptr<PublicKey> 
  getPublicKey(const Name& keyName);
  
  virtual Block
  sign(const uint8_t *data, size_t dataLength,
       const Name& keyName, DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256);

  virtual void
  sign(Data &data,
       const Name& keyName, DigestAlgorithm digestAlgorithm = DIGEST_ALGORITHM_SHA256);
  
  /**
   * Decrypt data.
   * @param keyName The name of the decrypting key.
   * @param data The byte to be decrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric decryption is used.
   * @return The decrypted data.
   */
  virtual ConstBufferPtr 
  decrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric = false);

  /**
   * Encrypt data.
   * @param keyName The name of the encrypting key.
   * @param data The byte to be encrypted.
   * @param dataLength the length of data.
   * @param isSymmetric If true symmetric encryption is used, otherwise asymmetric decryption is used.
   * @return The encrypted data.
   */
  virtual ConstBufferPtr
  encrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric = false);

  /**
   * Generate a symmetric key.
   * @param keyName The name of the key.
   * @param keyType The type of the key, e.g. KEY_TYPE_AES.
   * @param keySize The size of the key.
   */
  virtual void 
  generateKey(const Name& keyName, KeyType keyType = KEY_TYPE_AES, int keySize = 256);

  /**
   * Check if a particular key exists.
   * @param keyName The name of the key.
   * @param keyClass The class of the key, e.g. KEY_CLASS_PUBLIC, KEY_CLASS_PRIVATE, or KEY_CLASS_SYMMETRIC.
   * @return True if the key exists, otherwise false.
   */
  virtual bool
  doesKeyExist(const Name& keyName, KeyClass keyClass);  


  ////////////////////////////////////////////////////////////////////////////////////
  // OSX-specifics
  ////////////////////////////////////////////////////////////////////////////////////
  
  /**
   * configure ACL of a particular key
   * @param keyName the name of key
   * @param keyClass the class of key, e.g. Private Key
   * @param acl the new acl of the key
   * @param appPath the absolute path to the application
   * @returns true if setting succeeds
   */
  bool 
  setACL(const Name & keyName, KeyClass keyClass, int acl, const std::string & appPath);

  // /**
  //  * verify data (test only)
  //  * @param keyName the name of key
  //  * @param pData the data to be verified
  //  * @param pSig the signature associated with the data
  //  * @param digestAlgo digest algorithm
  //  * @return true if signature can be verified, otherwise false
  //  */
  // bool 
  // verifyData(const Name & keyName, const Blob & pData, const Blob & pSig, DigestAlgorithm digestAlgo = DIGEST_ALGORITHM_SHA256);

 private:
  class Impl;
  std::auto_ptr<Impl> impl_;
};
  
}

#endif // NDN_CPP_HAVE_OSX_SECURITY

#endif
