/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_SECURITY_SEC_TPM_MEMORY_HPP
#define NDN_SECURITY_SEC_TPM_MEMORY_HPP

#include "../common.hpp"
#include "sec-tpm.hpp"

#include <map>

struct rsa_st;

namespace ndn {

/**
 * @brief SecTpmMemory implements a simple in-memory TPM.
 *
 * You should initialize by calling setKeyPairForKeyName.
 */
class SecTpmMemory : public SecTpm
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

  virtual
  ~SecTpmMemory();

  /******************************
   * From TrustedPlatformModule *
   ******************************/

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
  generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize);

  virtual shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name& keyName);

  virtual void
  deleteKeyPairInTpm(const Name& keyName);

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
  addAppToAcl(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl)
  {
  }

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
                            const uint8_t* publicKeyDer, size_t publicKeyDerLength,
                            const uint8_t* privateKeyDer, size_t privateKeyDerLength);

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

  typedef std::map<std::string, shared_ptr<PublicKey> >     PublicKeyStore;
  typedef std::map<std::string, shared_ptr<RsaPrivateKey> > PrivateKeyStore;

  PublicKeyStore  m_publicKeyStore;  /**< The map key is the keyName.toUri() */
  PrivateKeyStore m_privateKeyStore; /**< The map key is the keyName.toUri() */

  bool m_inTerminal;
};

} // namespace ndn

#endif //NDN_SECURITY_SEC_TPM_MEMORY_HPP
