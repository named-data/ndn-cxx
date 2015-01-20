/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
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

  explicit
  SecTpmFile(const std::string& dir = "");

  virtual
  ~SecTpmFile();

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
  getInTerminal() const
  {
    return m_inTerminal;
  }

  virtual bool
  isLocked()
  {
    return false;
  }

  virtual bool
  unlockTpm(const char* password, size_t passwordLength, bool usePassword)
  {
    return !isLocked();
  }

  virtual void
  generateKeyPairInTpm(const Name& keyName, const KeyParams& params);

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
  generateSymmetricKeyInTpm(const Name& keyName, const KeyParams& params);

  virtual bool
  doesKeyExistInTpm(const Name& keyName, KeyClass keyClass);

  virtual bool
  generateRandomBlock(uint8_t* res, size_t size);

  virtual void
  addAppToAcl(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl)
  {
  }

protected:
  ////////////////////////////////
  // From TrustedPlatformModule //
  ////////////////////////////////
  virtual std::string
  getScheme();

  virtual ConstBufferPtr
  exportPrivateKeyPkcs8FromTpm(const Name& keyName);

  virtual bool
  importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);

  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size);

public:
  static const std::string SCHEME;

private:
  class Impl;
  unique_ptr<Impl> m_impl;
  bool m_inTerminal;
};

} // namespace ndn

#endif  // NDN_SECURITY_SEC_TPM_FILE_HPP
