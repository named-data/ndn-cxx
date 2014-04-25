/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Xingyu Ma <http://www.linkedin.com/pub/xingyu-ma/1a/384/5a8>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_SECURITY_SEC_TPM_FILE_HPP
#define NDN_SECURITY_SEC_TPM_FILE_HPP

#include "../common.hpp"

#include "sec-tpm.hpp"

namespace ndn {

class SecTpmFile : public SecTpm
{
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

  SecTpmFile(const std::string& dir = "");

  virtual
  ~SecTpmFile()
  {
  }

  virtual void
  setTpmPassword(const uint8_t* password, size_t passwordLength)
  {
  }

  virtual void
  resetTpmPassword()
  {
  }

  virtual void
  setInTerminal(bool inTerminal)
  {
    m_inTerminal = inTerminal;
  }

  virtual bool
  getInTerminal()
  {
    return m_inTerminal;
  }

  virtual bool
  locked()
  {
    return false;
  }

  virtual bool
  unlockTpm(const char* password, size_t passwordLength, bool usePassword)
  {
    return !locked();
  }

  virtual void
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize);

  virtual void
  deleteKeyPairInTpm(const Name& keyName);

  virtual shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name&  keyName);

  virtual Block
  signInTpm(const uint8_t* data, size_t dataLength,
            const Name& keyName, DigestAlgorithm digestAlgorithm);

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
  addAppToACL(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl)
  {
  }

protected:
  /******************************
   * From TrustedPlatformModule *
   ******************************/
  virtual ConstBufferPtr
  exportPrivateKeyPkcs8FromTpm(const Name& keyName);

  virtual bool
  importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);

  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);

private:
  class Impl;
  shared_ptr<Impl> m_impl;
  bool m_inTerminal;
};

} // namespace ndn

#endif  //NDN_SECURITY_SEC_TPM_FILE_HPP
