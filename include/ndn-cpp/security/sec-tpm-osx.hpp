/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_TPM_OSX_HPP
#define NDN_SEC_TPM_OSX_HPP

// Only compile if ndn-cpp-config.h defines NDN_CPP_HAVE_OSX_SECURITY 1.
#include <ndn-cpp/ndn-cpp-config.h>
#if NDN_CPP_HAVE_OSX_SECURITY

#include "../common.hpp"
#include "sec-tpm.hpp"

namespace ndn
{
  
class SecTpmOsx : public SecTpm {
public:
  struct Error : public SecTpm::Error { Error(const std::string &what) : SecTpm::Error(what) {} };

  /**
   * constructor of OSXKeyChainTpm
   * @param keychainName the name of keychain
   */
  SecTpmOsx(const std::string & keychainName = "");

  /**
   * destructor of OSXKeyChainTpm
   */    
  virtual 
  ~SecTpmOsx();


  // From TrustedPlatformModule
  virtual void 
  generateKeyPairInTpm(const Name& keyName, KeyType keyType = KEY_TYPE_RSA, int keySize = 2048);

  virtual ptr_lib::shared_ptr<PublicKey> 
  getPublicKeyFromTpm(const Name& keyName);
  
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
