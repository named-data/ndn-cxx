/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SEC_PUBLIC_INFO_HPP
#define NDN_SECURITY_SEC_PUBLIC_INFO_HPP

#include "../name.hpp"
#include "security-common.hpp"
#include "public-key.hpp"
#include "identity-certificate.hpp"


namespace ndn {

/**
 * @brief SecPublicInfo is a base class for the storage of public information.
 *
 * It specify interfaces related to public information, such as identity, public keys and certificates.
 */
class SecPublicInfo {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * @brief The virtual Destructor.
   */
  virtual 
  ~SecPublicInfo() {}

  /**
   * @brief Check if the specified identity already exists.
   *
   * @param identityName The identity name.
   * @return true if the identity exists, otherwise false.
   */
  virtual bool 
  doesIdentityExist(const Name& identityName) = 0;

  /**
   * @brief Add a new identity.
   *
   * if identity already exist, do not add it again.
   *
   * @param identityName The identity name to be added.
   */
  virtual void
  addIdentity(const Name& identityName) = 0;

  /**
   * @brief Revoke the identity.
   *
   * @return true if the identity was revoked, otherwise false.
   */
  virtual bool 
  revokeIdentity() = 0;

  /**
   * @brief Check if the specified key already exists.
   *
   * @param keyName The name of the key.
   * @return true if the key exists, otherwise false.
   */
  virtual bool 
  doesPublicKeyExist(const Name& keyName) = 0;

  /**
   * @brief Add a public key to the identity storage.
   *
   * @param keyName The name of the public key to be added.
   * @param keyType Type of the public key to be added.
   * @param publicKeyDer A blob of the public key DER to be added.
   */
  virtual void 
  addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKeyDer) = 0;

  /**
   * @brief Get the public key DER blob from the identity storage.
   *
   * @param keyName The name of the requested public key.
   * @return The DER Blob.
   * @throws SecPublicInfo::Error if public key does not exist.
   */
  virtual shared_ptr<PublicKey>
  getPublicKey(const Name& keyName) = 0;

  /**
   * @brief Check if the specified certificate already exists.
   *
   * @param certificateName The name of the certificate.
   * @return true if the certificate exists, otherwise false.
   */
  virtual bool
  doesCertificateExist(const Name& certificateName) = 0;

  /**
   * @brief Add a certificate to the identity storage.
   *
   * It will add the corresponding public key and identity if they do not exist.
   *
   * @param certificate The certificate to be added.
   */
  virtual void 
  addCertificate(const IdentityCertificate& certificate) = 0;

  /**
   * @brief Get a certificate from the identity storage.
   *
   * @param certificateName The name of the requested certificate.
   * @return The requested certificate.  
   * @throws SecPublicInfo::Error if the certificate does not exist.
   */
  virtual shared_ptr<IdentityCertificate> 
  getCertificate(const Name &certificateName) = 0;


  /*****************************************
   *            Default Getter             *
   *****************************************/

  /**
   * @brief Get the default identity. 
   *
   * @param return The name of default identity, 
   * @throws SecPublicInfo::Error if there is no default.
   */
  virtual Name 
  getDefaultIdentity() = 0;

  /**
   * @brief Get the default key name for the specified identity.
   *
   * @param identityName The identity name.
   * @return The default key name.
   * @throws SecPublicInfo::Error if there is no default.
   */
  virtual Name 
  getDefaultKeyNameForIdentity(const Name& identityName) = 0;

  /**
   * @brief Get the default certificate name for the specified key.
   *
   * @param keyName The key name.
   * @return The default certificate name.
   * @throws SecPublicInfo::Error if there is no default.
   */
  virtual Name 
  getDefaultCertificateNameForKey(const Name& keyName) = 0;

  /**
   * @brief Get all the identities in public info.
   *
   * @param nameList On return, the identity list.
   * @param isDefault If specified, only the default identity is returned.
   */
  virtual void
  getAllIdentities(std::vector<Name> &nameList, bool isDefault) = 0;

  /**
   * @brief Get all the key name in public info.
   *
   * @param nameList On return, the key name list.
   * @param isDefault If specified, only the default keys are returned.
   */
  virtual void
  getAllKeyNames(std::vector<Name> &nameList, bool isDefault) = 0;

  /**
   * @brief Get all the key name of a particular identity.
   *
   * @param identity The specified identity name.
   * @param nameList On return, the key name list.
   * @param isDefault If specified, only the default key is returned.
   */
  virtual void
  getAllKeyNamesOfIdentity(const Name& identity, std::vector<Name> &nameList, bool isDefault) = 0;

  /**
   * @brief Get all the certificate name in public info.
   *
   * @param nameList On return, the certificate name list.
   * @param isDefault If specified, only the default certificates are returned.
   */
  virtual void
  getAllCertificateNames(std::vector<Name> &nameList, bool isDefault) = 0;
    
  /**
   * @brief Get all the certificate name of a particular key.
   *
   * @param identity The specified key name.
   * @param nameList On return, the certificate name list.
   * @param isDefault If specified, only the default certificate is returned.
   */
  virtual void
  getAllCertificateNamesOfKey(const Name& keyName, std::vector<Name> &nameList, bool isDefault) = 0;

protected:

  /*****************************************
   *            Default Setter             *
   *****************************************/
  
  /**
   * @brief Set the default identity.
   *
   * @param identityName The default identity name.
   */
  virtual void 
  setDefaultIdentityInternal(const Name& identityName) = 0;
  
  /**
   * @brief Set the default key name for the corresponding identity.
   * 
   * @param keyName The key name.
   * @throws SecPublicInfo::Error if the key does not exist.
   */
  virtual void
  setDefaultKeyNameForIdentityInternal(const Name& keyName) = 0;

  /**
   * @brief Set the default certificate name for the corresponding key.
   *
   * @param certificateName The certificate name.
   * @throws SecPublicInfo::Error if the certificatedoes not exist.
   */
  virtual void 
  setDefaultCertificateNameForKeyInternal(const Name& certificateName) = 0; 

  /*****************************************
   *            Delete Methods             *
   *****************************************/

  /**
   * @brief Delete a certificate.
   *
   * @param certificateName The certificate name.
   */
  virtual void
  deleteCertificateInfo(const Name &certificateName) = 0;

  /**
   * @brief Delete a public key and related certificates.
   *
   * @param keyName The key name.
   */
  virtual void
  deletePublicKeyInfo(const Name &keyName) = 0;

  /**
   * @brief Delete an identity and related public keys and certificates.
   *
   * @param identity The identity name.
   */
  virtual void
  deleteIdentityInfo(const Name &identity) = 0;

public:
  
  /*****************************************
   *            Helper Methods             *
   *****************************************/

  /**
   * @brief Set the default identity.  
   *
   * @param identityName The default identity name.
   * @throws SecPublicInfo::Error if the identity does not exist.
   */
  inline void 
  setDefaultIdentity(const Name& identityName);

  /**
   * @brief Set the default key name for the corresponding identity.
   * 
   * @param keyName The key name.
   * @throws SecPublicInfo::Error if either the identity or key does not exist.
   */
  inline void 
  setDefaultKeyNameForIdentity(const Name& keyName);

  /**
   * @brief Set the default certificate name for the corresponding key.
   *
   * @param certificateName The certificate name.
   * @throws SecPublicInfo::Error if either the certificate or key does not exist.
   */
  inline void 
  setDefaultCertificateNameForKey(const Name& certificateName); 

  /**
   * @brief Generate a key name for the identity.
   *
   * @param identityName The identity name.
   * @param useKsk If true, generate a KSK name, otherwise a DSK name.
   * @return The generated key name.
   */
  inline Name 
  getNewKeyName(const Name& identityName, bool useKsk);

  /**
   * @brief Get the default certificate name for the specified identity.
   *
   * @param identityName The identity name.
   * @return The default certificate name.
   * @throws SecPublicInfo::Error if no certificate is found.
   */
  inline Name 
  getDefaultCertificateNameForIdentity(const Name& identityName);

  /**
   * @brief Get the default certificate name of the default identity
   *
   * @return The requested certificate name.
   * @throws SecPublicInfo::Error if no certificate is found.
   */
  inline Name
  getDefaultCertificateName();

  /**
   * @brief Add a certificate and set the certificate as the default one of its corresponding key.
   *
   * @param certificate The certificate to be added.
   * @throws SecPublicInfo::Error if the certificate cannot be added (though it is really rare)
   */
  inline void
  addCertificateAsKeyDefault(const IdentityCertificate& certificate);

  /**
   * @brief Add a certificate into the public key identity storage and set the certificate as the default one of its corresponding identity.
   *
   * @param certificate The certificate to be added.
   * @throws SecPublicInfo::Error if the certificate cannot be added (though it is really rare)
   */
  inline void
  addCertificateAsIdentityDefault(const IdentityCertificate& certificate);

  /**
   * @brief Add a certificate into the public key identity storage and set the certificate as the default one of the default identity.
   *
   * @param certificate The certificate to be added.
   * @throws SecPublicInfo::Error if the certificate cannot be added (though it is really rare)
   */
  inline void
  addCertificateAsSystemDefault(const IdentityCertificate& certificate);

  /**
   * @brief get cached default certificate of the default identity.
   *
   * @return The certificate which might be a NULL pointer.
   */
  inline shared_ptr<IdentityCertificate>
  defaultCertificate();
  
  /**
   * @brief try to get the default certificate of the default identity from the public info.
   */
  inline void
  refreshDefaultCertificate();

protected:
  shared_ptr<IdentityCertificate> m_defaultCertificate;
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

  oss << getNow();
  
  Name keyName = Name(identityName).append(oss.str());

  if (doesPublicKeyExist(keyName))
    throw Error("Key name already exists: " + keyName.toUri());

  return keyName;
}

Name
SecPublicInfo::getDefaultCertificateName()
{
  if(!static_cast<bool>(m_defaultCertificate))
    refreshDefaultCertificate();

  if(!static_cast<bool>(m_defaultCertificate))
    throw Error("No default certificate is set");

  return m_defaultCertificate->getName();
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

shared_ptr<IdentityCertificate>
SecPublicInfo::defaultCertificate()
{
  return m_defaultCertificate;
}

void
SecPublicInfo::refreshDefaultCertificate()
{
  try
    {
      Name certName = getDefaultCertificateNameForIdentity(getDefaultIdentity());
      m_defaultCertificate = getCertificate(certName);
    }
  catch(SecPublicInfo::Error& e)
    {
      m_defaultCertificate.reset();
    }

}

} // namespace ndn

#endif //NDN_SECURITY_SEC_PUBLIC_INFO_HPP
