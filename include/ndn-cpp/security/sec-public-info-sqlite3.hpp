/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_PUBLIC_INFO_SQLITE3_HPP
#define NDN_SEC_PUBLIC_INFO_SQLITE3_HPP

// Only compile if ndn-cpp-config.h defines NDN_CPP_HAVE_SQLITE3.
#include <ndn-cpp/ndn-cpp-config.h>
#ifdef NDN_CPP_HAVE_SQLITE3

#include <sqlite3.h>
#include "../common.hpp"
#include "sec-public-info.hpp"

namespace ndn
{
  
/**
 * BasicIdentityStorage extends IdentityStorage to implement a basic storage of identity, public keys and certificates
 * using SQLite.
 */
class SecPublicInfoSqlite3 : public SecPublicInfo {
public:
  struct Error : public SecPublicInfo::Error { Error(const std::string &what) : SecPublicInfo::Error(what) {} };

  SecPublicInfoSqlite3();
  
  /**
   * The virtual Destructor.
   */
  virtual 
  ~SecPublicInfoSqlite3();

  // from SecPublicInfo
  /**
   * Check if the specified identity already exists.
   * @param identityName The identity name.
   * @return true if the identity exists, otherwise false.
   */
  virtual bool 
  doesIdentityExist(const Name& identityName);

  /**
   * Add a new identity. An exception will be thrown if the identity already exists.
   * @param identityName The identity name to be added.
   */
  virtual void
  addIdentity(const Name& identityName);

  /**
   * Revoke the identity.
   * @return true if the identity was revoked, false if not.
   */
  virtual bool 
  revokeIdentity();

  /**
   * Check if the specified key already exists.
   * @param keyName The name of the key.
   * @return true if the key exists, otherwise false.
   */
  virtual bool 
  doesPublicKeyExist(const Name& keyName);

  /**
   * Add a public key to the identity storage.
   * @param keyName The name of the public key to be added.
   * @param keyType Type of the public key to be added.
   * @param publicKeyDer A blob of the public key DER to be added.
   */
  virtual void 
  addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKeyDer);

  /**
   * Get the public key DER blob from the identity storage.
   * @param keyName The name of the requested public key.
   * @return The DER Blob.  If not found, return a Blob with a null pointer.
   */
  virtual ptr_lib::shared_ptr<PublicKey>
  getPublicKey(const Name& keyName);

  /**
   * Activate a key.  If a key is marked as inactive, its private part will not be used in packet signing.
   * @param keyName name of the key
   */
  virtual inline void 
  activatePublicKey(const Name& keyName);

  /**
   * Deactivate a key. If a key is marked as inactive, its private part will not be used in packet signing.
   * @param keyName name of the key
   */
  virtual inline void 
  deactivatePublicKey(const Name& keyName);

  /**
   * Check if the specified certificate already exists.
   * @param certificateName The name of the certificate.
   * @return true if the certificate exists, otherwise false.
   */
  virtual bool
  doesCertificateExist(const Name& certificateName);

  /**
   * Add a certificate in to the identity storage without checking if the identity and key exists.
   * @param certificate The certificate to be added.
   */
  virtual void
  addAnyCertificate (const IdentityCertificate& certificate);

  /**
   * Add a certificate to the identity storage.
   * @param certificate The certificate to be added.  This makes a copy of the certificate.
   */
  virtual void 
  addCertificate(const IdentityCertificate& certificate);

  /**
   * Get a certificate from the identity storage.
   * @param certificateName The name of the requested certificate.
   * @param allowAny If false, only a valid certificate will be returned, otherwise validity is disregarded.
   * @return The requested certificate.  If not found, return a shared_ptr with a null pointer.
   */
  virtual ptr_lib::shared_ptr<IdentityCertificate> 
  getCertificate(const Name &certificateName);


  /*****************************************
   *            Default Getter             *
   *****************************************/

  /**
   * Get the default identity. 
   * @param return The name of default identity, or an empty name if there is no default.
   */
  virtual Name 
  getDefaultIdentity();

  /**
   * Get the default key name for the specified identity.
   * @param identityName The identity name.
   * @return The default key name.
   */
  virtual Name 
  getDefaultKeyNameForIdentity(const Name& identityName);

  /**
   * Get the default certificate name for the specified key.
   * @param keyName The key name.
   * @return The default certificate name.
   */
  virtual Name 
  getDefaultCertificateNameForKey(const Name& keyName);

  virtual std::vector<Name>
  getAllIdentities(bool isDefault);

  virtual std::vector<Name>
  getAllKeyNames(bool isDefault);

  virtual std::vector<Name>
  getAllKeyNamesOfIdentity(const Name& identity, bool isDefault);
    
  virtual std::vector<Name>
  getAllCertificateNames(bool isDefault);

  virtual std::vector<Name>
  getAllCertificateNamesOfKey(const Name& keyName, bool isDefault);
  
protected:
  /**
   * Set the default identity.  If the identityName does not exist, then clear the default identity
   * so that getDefaultIdentity() returns an empty name.
   * @param identityName The default identity name.
   */
  virtual void 
  setDefaultIdentityInternal(const Name& identityName);

  /**
   * Set the default key name for the specified identity.
   * @param keyName The key name.
   * @param identityNameCheck (optional) The identity name to check the keyName.
   */
  virtual void
  setDefaultKeyNameForIdentityInternal(const Name& keyName);

  /**
   * Set the default key name for the specified identity.
   * @param keyName The key name.
   * @param certificateName The certificate name.
   */
  virtual void 
  setDefaultCertificateNameForKeyInternal(const Name& certificateName);  
  
private:
  void
  updateKeyStatus(const Name& keyName, bool isActive);

  sqlite3 *database_;
};

void
SecPublicInfoSqlite3::activatePublicKey(const Name& keyName)
{
  updateKeyStatus(keyName, true);
}

void
SecPublicInfoSqlite3::deactivatePublicKey(const Name& keyName)
{
  updateKeyStatus(keyName, false);
}

}

#endif // NDN_CPP_HAVE_SQLITE3

#endif
