/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_TPM_OSX_HPP
#define NDN_SECURITY_SEC_TPM_OSX_HPP

#include "../common.hpp"
#include "sec-tpm.hpp"

namespace ndn {

class SecTpmOsx : public SecTpm {
public:
  class Error : public SecTpm::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : SecTpm::Error(what)
    {
    }
  };

  SecTpmOsx();

  virtual
  ~SecTpmOsx();


  /******************************
   * From TrustedPlatformModule *
   ******************************/

  virtual void
  setTpmPassword(const uint8_t* password, size_t passwordLength);

  virtual void
  resetTpmPassword();

  virtual void
  setInTerminal(bool inTerminal);

  virtual bool
  getInTerminal();

  virtual bool
  locked();

  virtual bool
  unlockTpm(const char* password, size_t passwordLength, bool usePassword);

  virtual void
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize)
  {
    generateKeyPairInTpmInternal(keyName, keyType, keySize, false);
  }

  virtual void
  deleteKeyPairInTpm(const Name& keyName)
  {
    deleteKeyPairInTpmInternal(keyName, false);
  }

  virtual shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name& keyName);

  virtual Block
  signInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm)
  {
    return signInTpmInternal(data, dataLength, keyName, digestAlgorithm, false);
  }

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

  virtual void
  addAppToACL(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl);

protected:
  /******************************
   * From TrustedPlatformModule *
   ******************************/
  virtual ConstBufferPtr
  exportPrivateKeyPkcs1FromTpm(const Name& keyName)
  {
    return exportPrivateKeyPkcs1FromTpmInternal(keyName, false);
  }

  virtual bool
  importPrivateKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
  {
    return importPrivateKeyPkcs1IntoTpmInternal(keyName, buf, size, false);
  }

  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);

  /******************************
   *       OSX-specifics        *
   ******************************/
  void
  generateKeyPairInTpmInternal(const Name& keyName, KeyType keyType, int keySize, bool retry);

  void
  deleteKeyPairInTpmInternal(const Name& keyName, bool retry);

  ConstBufferPtr
  exportPrivateKeyPkcs1FromTpmInternal(const Name& keyName, bool retry);

  bool
  importPrivateKeyPkcs1IntoTpmInternal(const Name& keyName, const uint8_t* buf, size_t size, bool retry);

  Block
  signInTpmInternal(const uint8_t* data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm, bool retry);

private:
  class Impl;
  shared_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // NDN_SECURITY_SEC_TPM_OSX_HPP
