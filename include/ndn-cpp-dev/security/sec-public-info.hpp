/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_PUBLIC_INFO_HPP
#define NDN_SEC_PUBLIC_INFO_HPP

#include "../name.hpp"
#include "security-common.hpp"
#include "public-key.hpp"
#include "identity-certificate.hpp"


namespace ndn {

/**
 * SecPublicInfo is a base class for the storage of identity, public keys and certificates. 
 * Private keys are stored in SecTpm.
 * This is an abstract base class.  A subclass must implement the methods.
 */
class SecPublicInfo {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * The virtual Destructor.
   */
  virtual 
  ~SecPublicInfo() {}

  /**
   * Check if the specified identity already exists.
   * @param identityName The identity name.
   * @return true if the identity exists, otherwise false.
   */
  virtual bool 
  doesIdentityExist(const Name& identityName) = 0;

  /**
   * Add a new identity. An exception will be thrown if the identity already exists.
   * @param identityName The identity name to be added.
   */
  virtual void
  addIdentity(const Name& identityName) = 0;

  /**
   * Revoke the identity.
   * @return true if the identity was revoked, false if not.
   */
  virtual bool 
  revokeIdentity() = 0;

  /**
   * Check if the specified key already exists.
   * @param keyName The name of the key.
   * @return true if the key exists, otherwise false.
   */
  virtual bool 
  doesPublicKeyExist(const Name& keyName) = 0;

  /**
   * Add a public key to the identity storage.
   * @param keyName The name of the public key to be added.
   * @param keyType Type of the public key to be added.
   * @param publicKeyDer A blob of the public key DER to be added.
   */
  virtual void 
  addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKeyDer) = 0;

  /**
   * Get the public key DER blob from the identity storage.
   * @param keyName The name of the requested public key.
   * @return The DER Blob.  If not found, return a Blob with a null pointer.
   */
  virtual ptr_lib::shared_ptr<PublicKey>
  getPublicKey(const Name& keyName) = 0;

  /**
   * Activate a key.  If a key is marked as inactive, its private part will not be used in packet signing.
   * @param keyName name of the key
   */
  virtual void 
  activatePublicKey(const Name& keyName) = 0;

  /**
   * Deactivate a key. If a key is marked as inactive, its private part will not be used in packet signing.
   * @param keyName name of the key
   */
  virtual void 
  deactivatePublicKey(const Name& keyName) = 0;

  /**
   * Check if the specified certificate already exists.
   * @param certificateName The name of the certificate.
   * @return true if the certificate exists, otherwise false.
   */
  virtual bool
  doesCertificateExist(const Name& certificateName) = 0;

  /**
   * Add a certificate to the identity storage.
   * @param certificate The certificate to be added.  This makes a copy of the certificate.
   */
  virtual void 
  addCertificate(const IdentityCertificate& certificate) = 0;

  /**
   * Get a certificate from the identity storage.
   * @param certificateName The name of the requested certificate.
   * @param allowAny If false, only a valid certificate will be returned, otherwise validity is disregarded.
   * @return The requested certificate.  If not found, return a shared_ptr with a null pointer.
   */
  virtual ptr_lib::shared_ptr<IdentityCertificate> 
  getCertificate(const Name &certificateName) = 0;


  /*****************************************
   *            Default Getter             *
   *****************************************/

  /**
   * Get the default identity. 
   * @param return The name of default identity, or an empty name if there is no default.
   */
  virtual Name 
  getDefaultIdentity() = 0;

  /**
   * Get the default key name for the specified identity.
   * @param identityName The identity name.
   * @return The default key name.
   */
  virtual Name 
  getDefaultKeyNameForIdentity(const Name& identityName) = 0;

  /**
   * Get the default certificate name for the specified key.
   * @param keyName The key name.
   * @return The default certificate name.
   */
  virtual Name 
  getDefaultCertificateNameForKey(const Name& keyName) = 0;

  virtual std::vector<Name>
  getAllIdentities(bool isDefault) = 0;

  virtual std::vector<Name>
  getAllKeyNames(bool isDefault) = 0;

  virtual std::vector<Name>
  getAllKeyNamesOfIdentity(const Name& identity, bool isDefault) = 0;
    
  virtual std::vector<Name>
  getAllCertificateNames(bool isDefault) = 0;
    
  virtual std::vector<Name>
  getAllCertificateNamesOfKey(const Name& keyName, bool isDefault) = 0;

protected:

  /*****************************************
   *            Default Setter             *
   *****************************************/
  
  /**
   * Set the default identity.  If the identityName does not exist, then clear the default identity
   * so that getDefaultIdentity() returns an empty name.
   * @param identityName The default identity name.
   */
  virtual void 
  setDefaultIdentityInternal(const Name& identityName) = 0;
  
  /**
   * Set the default key name for the corresponding identity.
   * @param keyName The key name.
   */
  virtual void
  setDefaultKeyNameForIdentityInternal(const Name& keyName) = 0;

  /**
   * Set the default certificate name for the corresponding key.
   * @param certificateName The certificate name.
   */
  virtual void 
  setDefaultCertificateNameForKeyInternal(const Name& certificateName) = 0; 

public:
  
  /*****************************************
   *            Helper Methods             *
   *****************************************/

  /**
   * Set the default identity.  If the identityName does not exist, then clear the default identity
   * so that getDefaultIdentity() returns an empty name.
   * @param identityName The default identity name.
   */
  inline void 
  setDefaultIdentity(const Name& identityName);

  /**
   * Set the default key name for the corresponding identity.
   * @param keyName The key name.
   */
  inline void 
  setDefaultKeyNameForIdentity(const Name& keyName);

  /**
   * Set the default certificate name for the corresponding key.
   * @param certificateName The certificate name.
   */
  inline void 
  setDefaultCertificateNameForKey(const Name& certificateName); 

  /**
   * Generate a name for a new key belonging to the identity.
   * @param identityName The identity name.
   * @param useKsk If true, generate a KSK name, otherwise a DSK name.
   * @return The generated key name.
   */
  inline Name 
  getNewKeyName(const Name& identityName, bool useKsk);

    /**
   * Get the default certificate name for the specified identity.
   * @param identityName The identity name.
   * @return The default certificate name.
   */
  inline Name 
  getDefaultCertificateNameForIdentity(const Name& identityName);

  /**
   * Get the default certificate name of the default identity, which will be used when signing is based on identity and 
   * the identity is not specified.
   * @return The requested certificate name.
   */
  inline Name
  getDefaultCertificateName();

  /**
   * Add a certificate and set the certificate as the default of its corresponding key.
   * @param certificate The certificate to be added.  This makes a copy of the certificate.
   */
  inline void
  addCertificateAsKeyDefault(const IdentityCertificate& certificate);

  /**
   * Add a certificate into the public key identity storage and set the certificate as the default for its corresponding identity.
   * @param certificate The certificate to be added.  This makes a copy of the certificate.
   */
  inline void
  addCertificateAsIdentityDefault(const IdentityCertificate& certificate);

  inline void
  addCertificateAsSystemDefault(const IdentityCertificate& certificate);

  inline ptr_lib::shared_ptr<IdentityCertificate>
  defaultCertificate();
  
  inline void
  refreshDefaultCertificate();

protected:
  ptr_lib::shared_ptr<IdentityCertificate> defaultCertificate_;

};

void
SecPublicInfo::setDefaultIdentity(const Name& identityName)
{
  setDefaultIdentityInternal(identityName);
  refreshDefaultCertificate();
}

void
SecPublicInfo::setDefaultKeyNameForIdentity(const Name& keyName)
{
  setDefaultKeyNameForIdentityInternal(keyName);
  refreshDefaultCertificate();
}

void 
SecPublicInfo::setDefaultCertificateNameForKey(const Name& certificateName)
{
  setDefaultCertificateNameForKeyInternal(certificateName);
  refreshDefaultCertificate();
}

Name 
SecPublicInfo::getDefaultCertificateNameForIdentity(const Name& identityName)
{
  return getDefaultCertificateNameForKey(getDefaultKeyNameForIdentity(identityName));
}

Name
SecPublicInfo::getNewKeyName (const Name& identityName, bool useKsk)
{
  std::ostringstream oss;

  if (useKsk)
    oss << "ksk-";
  else
    oss << "dsk-";

  oss << static_cast<int>(getNow()/1000);  

  Name keyName = Name(identityName).append(oss.str());

  if (doesPublicKeyExist(keyName))
    throw Error("Key name already exists");

  return keyName;
}

Name
SecPublicInfo::getDefaultCertificateName()
{
  if(!static_cast<bool>(defaultCertificate_))
    refreshDefaultCertificate();

  if(!static_cast<bool>(defaultCertificate_))
    return Name();

  return defaultCertificate_->getName();
}

void
SecPublicInfo::addCertificateAsKeyDefault(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  setDefaultCertificateNameForKeyInternal(certificate.getName());
  refreshDefaultCertificate();
}

void
SecPublicInfo::addCertificateAsIdentityDefault(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  Name certName = certificate.getName();
  setDefaultKeyNameForIdentityInternal(IdentityCertificate::certificateNameToPublicKeyName(certName));
  setDefaultCertificateNameForKeyInternal(certName);
  refreshDefaultCertificate();
}

void
SecPublicInfo::addCertificateAsSystemDefault(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  Name certName = certificate.getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certName);
  setDefaultIdentityInternal(keyName.getPrefix(-1));
  setDefaultKeyNameForIdentityInternal(keyName);
  setDefaultCertificateNameForKeyInternal(certName);
  refreshDefaultCertificate();
}

ptr_lib::shared_ptr<IdentityCertificate>
SecPublicInfo::defaultCertificate()
{
  return defaultCertificate_;
}

void
SecPublicInfo::refreshDefaultCertificate()
{
  Name certName = getDefaultCertificateNameForIdentity(getDefaultIdentity());
  if(certName.empty())
    defaultCertificate_.reset();
  else
    defaultCertificate_ = getCertificate(certName);
}


}

#endif
