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

  SecPublicInfoSqlite3();

  virtual
  ~SecPublicInfoSqlite3();

  /**********************
   * from SecPublicInfo *
   **********************/
  virtual bool
  doesIdentityExist(const Name& identityName);

  virtual void
  addIdentity(const Name& identityName);

  virtual bool
  revokeIdentity();

  virtual bool
  doesPublicKeyExist(const Name& keyName);

  virtual void
  addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKeyDer);

  virtual shared_ptr<PublicKey>
  getPublicKey(const Name& keyName);

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

protected:
  virtual void
  setDefaultIdentityInternal(const Name& identityName);

  virtual void
  setDefaultKeyNameForIdentityInternal(const Name& keyName);

  virtual void
  setDefaultCertificateNameForKeyInternal(const Name& certificateName);

  virtual void
  deleteCertificateInfo(const Name& certificateName);

  virtual void
  deletePublicKeyInfo(const Name& keyName);

  virtual void
  deleteIdentityInfo(const Name& identity);

private:
  sqlite3 * m_database;
};

} // namespace ndn

#endif //NDN_SECURITY_SEC_PUBLIC_INFO_SQLITE3_HPP
