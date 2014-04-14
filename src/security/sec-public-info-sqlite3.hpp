/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_PUBLIC_INFO_SQLITE3_HPP
#define NDN_SECURITY_SEC_PUBLIC_INFO_SQLITE3_HPP

#include "../common.hpp"
#include "sec-public-info.hpp"

struct sqlite3;

namespace ndn {

class SecPublicInfoSqlite3 : public SecPublicInfo {
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

  virtual ptr_lib::shared_ptr<PublicKey>
  getPublicKey(const Name& keyName);

  virtual bool
  doesCertificateExist(const Name& certificateName);

  virtual void
  addCertificate(const IdentityCertificate& certificate);

  virtual ptr_lib::shared_ptr<IdentityCertificate>
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
