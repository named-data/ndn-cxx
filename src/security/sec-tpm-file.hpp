/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Xingyu Ma <maxy12@cs.ucla.edu>
 *          Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_TPM_FILE_HPP
#define NDN_SECURITY_SEC_TPM_FILE_HPP

#include "../common.hpp"

#include "sec-tpm.hpp"

namespace ndn {

class SecTpmFile : public SecTpm
{
public:
  struct Error : public SecTpm::Error { Error(const std::string &what) : SecTpm::Error(what) {} };

  SecTpmFile(const std::string & dir = "");

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

  virtual void
  deleteKeyPairInTpm(const Name &keyName);

  virtual shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name & keyName);

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
  class Impl;
  shared_ptr<Impl> m_impl;
};

} // namespace ndn

#endif  //NDN_SECURITY_SEC_TPM_FILE_HPP
