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
 */

#ifndef NDN_TESTS_SECURITY_DUMMY_KEYCHAIN_HPP
#define NDN_TESTS_SECURITY_DUMMY_KEYCHAIN_HPP

#include "security/key-chain.hpp"

namespace ndn {
namespace security {

class DummyPublicInfo : public SecPublicInfo
{
public:
  explicit
  DummyPublicInfo(const std::string& locator);

  virtual bool
  doesIdentityExist(const Name& identityName);

  virtual void
  addIdentity(const Name& identityName);

  virtual bool
  revokeIdentity();

  virtual bool
  doesPublicKeyExist(const Name& keyName);

  virtual void
  addKey(const Name& keyName, const PublicKey& publicKey);

  virtual shared_ptr<PublicKey>
  getPublicKey(const Name& keyName);

  virtual KeyType
  getPublicKeyType(const Name& keyName);

  virtual bool
  doesCertificateExist(const Name& certificateName);

  virtual void
  addCertificate(const IdentityCertificate& certificate);

  virtual shared_ptr<IdentityCertificate>
  getCertificate(const Name& certificateName);

  virtual Name
  getDefaultIdentity();

  virtual Name
  getDefaultKeyNameForIdentity(const Name& identityName);

  virtual Name
  getDefaultCertificateNameForKey(const Name& keyName);

  virtual void
  getAllIdentities(std::vector<Name>& nameList, bool isDefault);

  virtual void
  getAllKeyNames(std::vector<Name>& nameList, bool isDefault);

  virtual void
  getAllKeyNamesOfIdentity(const Name& identity, std::vector<Name>& nameList, bool isDefault);

  virtual void
  getAllCertificateNames(std::vector<Name>& nameList, bool isDefault);

  virtual void
  getAllCertificateNamesOfKey(const Name& keyName, std::vector<Name>& nameList, bool isDefault);

  virtual void
  deleteCertificateInfo(const Name& certificateName);

  virtual void
  deletePublicKeyInfo(const Name& keyName);

  virtual void
  deleteIdentityInfo(const Name& identity);

  virtual void
  setTpmLocator(const std::string& tpmLocator);

  virtual std::string
  getTpmLocator();

protected:
  virtual void
  setDefaultIdentityInternal(const Name& identityName);

  virtual void
  setDefaultKeyNameForIdentityInternal(const Name& keyName);

  virtual void
  setDefaultCertificateNameForKeyInternal(const Name& certificateName);

  virtual std::string
  getScheme();

public:
  static const std::string SCHEME;

private:
  std::string m_tpmLocator;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

class DummyTpm : public SecTpm
{
public:
  explicit
  DummyTpm(const std::string& locator);

  virtual void
  setTpmPassword(const uint8_t* password, size_t passwordLength);

  virtual void
  resetTpmPassword();

  virtual void
  setInTerminal(bool inTerminal);

  virtual bool
  getInTerminal() const;

  virtual bool
  isLocked();

  virtual bool
  unlockTpm(const char* password, size_t passwordLength, bool usePassword);

  virtual void
  generateKeyPairInTpm(const Name& keyName, const KeyParams& params);

  virtual void
  deleteKeyPairInTpm(const Name& keyName);

  virtual shared_ptr<PublicKey>
  getPublicKeyFromTpm(const Name& keyName);

  virtual Block
  signInTpm(const uint8_t* data, size_t dataLength, const Name& keyName,
            DigestAlgorithm digestAlgorithm);

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
  addAppToAcl(const Name& keyName, KeyClass keyClass, const std::string& appPath, AclType acl);

  virtual std::string
  getScheme();

protected:
  virtual ConstBufferPtr
  exportPrivateKeyPkcs8FromTpm(const Name& keyName);

  virtual bool
  importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buffer, size_t bufferSize);

  virtual bool
  importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buffer, size_t bufferSize);

public:
  static const std::string SCHEME;
};

} // namespace security
} // namespace ndn

#endif // NDN_TESTS_SECURITY_DUMMY_KEYCHAIN_HPP
