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
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_SECURITY_SEC_PUBLIC_INFO_SQLITE3_HPP
#define NDN_SECURITY_SEC_PUBLIC_INFO_SQLITE3_HPP

#include "../common.hpp"
#include "sec-public-info.hpp"

struct sqlite3;

namespace ndn {

class SecPublicInfoSqlite3 : public SecPublicInfo
{
public:
  class Error : public SecPublicInfo::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : SecPublicInfo::Error(what)
    {
    }
  };

  explicit
  SecPublicInfoSqlite3(const std::string& dir = "");

  virtual
  ~SecPublicInfoSqlite3();

  /**********************
   * from SecPublicInfo *
   **********************/

  virtual void
  setTpmLocator(const std::string& tpmLocator);

  virtual std::string
  getTpmLocator();

  virtual std::string
  getPibLocator();

  virtual bool
  doesIdentityExist(const Name& identityName);

  virtual void
  addIdentity(const Name& identityName);

  virtual bool
  revokeIdentity();

  virtual bool
  doesPublicKeyExist(const Name& keyName);

  virtual void
  addKey(const Name& keyName, const PublicKey& publicKeyDer);

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

private:
  bool
  initializeTable(const std::string& tableName, const std::string& initCommand);

  void
  deleteTable(const std::string& tableName);

  void
  setTpmLocatorInternal(const std::string& tpmLocator, bool needReset);

  void
  setDefaultIdentityInternal(const Name& identityName);

  void
  setDefaultKeyNameForIdentityInternal(const Name& keyName);

  void
  setDefaultCertificateNameForKeyInternal(const Name& certificateName);

  std::string
  getScheme();

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  bool
  doesTableExist(const std::string& tableName);

public:
  static const std::string SCHEME;

private:
  sqlite3* m_database;
};

} // namespace ndn

#endif // NDN_SECURITY_SEC_PUBLIC_INFO_SQLITE3_HPP
