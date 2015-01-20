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
 * It specify interfaces related to public information, such as identity, public keys and
 * certificates.
 */
class SecPublicInfo : noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  explicit
  SecPublicInfo(const std::string& location);

  /**
   * @brief The virtual Destructor
   */
  virtual
  ~SecPublicInfo();

  /**
   * @brief Set the corresponding TPM information to @p tpmLocator
   *
   * If the provided @p tpmLocator is different from the existing one, the PIB will be reset,
   * otherwise nothing will be changed.
   *
   * For legacy issue, the TPM info may not exist (some old PIB content may not have this info),
   * this method will simply set the TPM info as provided without changing anything else. Thus an
   * ideal process of handling old PIB is to check if TPM info exists. If it does not exist,
   * then set it to the default value according to configuration.
   */
  virtual void
  setTpmLocator(const std::string& tpmLocator) = 0;

  /**
   * @brief Get TPM Locator
   *
   * @throws SecPublicInfo::Error if the TPM info does not exist
   */
  virtual std::string
  getTpmLocator() = 0;

  /**
   * @brief Get PIB Locator
   */
  std::string
  getPibLocator();

  /**
   * @brief Check if the specified identity already exists
   *
   * @param identityName The identity name
   * @return true if the identity exists, otherwise false
   */
  virtual bool
  doesIdentityExist(const Name& identityName) = 0;

  /**
   * @brief Add a new identity
   *
   * if identity already exist, do not add it again
   *
   * @param identityName The identity name to be added
   */
  virtual void
  addIdentity(const Name& identityName) = 0;

  /**
   * @brief Revoke the identity
   *
   * @return true if the identity was revoked, otherwise false
   */
  virtual bool
  revokeIdentity() = 0;

  /**
   * @brief Check if the specified key already exists
   *
   * @param keyName The name of the key
   * @return true if the key exists, otherwise false
   */
  virtual bool
  doesPublicKeyExist(const Name& keyName) = 0;

  /**
   * @brief Add a public key to the identity storage.
   *
   * @param keyName The name of the public key to be added
   * @param keyType Type of the public key to be added
   * @param publicKey Reference to the PublicKey object
   * @deprecated Use addKey instead
   */
  DEPRECATED(
  void
  addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKey));

  /**
   * @brief Add a public key to the identity storage.
   *
   * @param keyName The name of the public key to be added
   * @param publicKey Reference to the PublicKey object
   */
  virtual void
  addKey(const Name& keyName, const PublicKey& publicKey) = 0;

  /**
   * @brief Get shared pointer to PublicKey object from the identity storage
   *
   * @param keyName The name of the requested public key
   * @throws SecPublicInfo::Error if public key does not exist
   */
  virtual shared_ptr<PublicKey>
  getPublicKey(const Name& keyName) = 0;

  /**
   * @brief Get the type of the queried public key
   *
   * @note KeyType is also available from PublicKey instance.
   *       This method is more efficient if only KeyType is needed.
   *
   * @param keyName The name of the requested public key
   * @return the type of the key. If the queried key does not exist, KEY_TYPE_NULL will be returned
   */
  virtual KeyType
  getPublicKeyType(const Name& keyName) = 0;

  /**
   * @brief Check if the specified certificate already exists
   *
   * @param certificateName The name of the certificate
   */
  virtual bool
  doesCertificateExist(const Name& certificateName) = 0;

  /**
   * @brief Add a certificate to the identity storage.
   *
   * It will add the corresponding public key and identity if they do not exist
   *
   * @param certificate The certificate to be added
   */
  virtual void
  addCertificate(const IdentityCertificate& certificate) = 0;

  /**
   * @brief Get a shared pointer to identity certificate object from the identity storage
   *
   * @param certificateName The name of the requested certificate
   * @throws SecPublicInfo::Error if the certificate does not exist
   */
  virtual shared_ptr<IdentityCertificate>
  getCertificate(const Name& certificateName) = 0;


  /*****************************************
   *            Default Getter             *
   *****************************************/

  /**
   * @brief Get name of the default identity
   *
   * @throws SecPublicInfo::Error if there is no default.
   */
  virtual Name
  getDefaultIdentity() = 0;

  /**
   * @brief Get name of the default key name for the specified identity
   *
   * @param identityName The identity name
   * @throws SecPublicInfo::Error if there is no default
   */
  virtual Name
  getDefaultKeyNameForIdentity(const Name& identityName) = 0;

  /**
   * @brief Get name of the default certificate name for the specified key
   *
   * @param keyName The key name.
   * @throws SecPublicInfo::Error if there is no default.
   */
  virtual Name
  getDefaultCertificateNameForKey(const Name& keyName) = 0;

  /**
   * @brief Get all the identities from public info
   *
   * @param [out] nameList On return, the identity list
   * @param isDefault      If specified, only the default identity is returned
   */
  virtual void
  getAllIdentities(std::vector<Name>& nameList, bool isDefault) = 0;

  /**
   * @brief Get all the key names from public info
   *
   * @param [out] nameList On return, the key name list.
   * @param isDefault      If specified, only the default keys are returned
   */
  virtual void
  getAllKeyNames(std::vector<Name>& nameList, bool isDefault) = 0;

  /**
   * @brief Get all the key names of a particular identity
   *
   * @param identity       The specified identity name
   * @param [out] nameList On return, the key name list
   * @param isDefault      If specified, only the default key is returned
   */
  virtual void
  getAllKeyNamesOfIdentity(const Name& identity, std::vector<Name>& nameList, bool isDefault) = 0;

  /**
   * @brief Get all the certificate name in public info
   *
   * @param [out] nameList On return, the certificate name list
   * @param isDefault      If specified, only the default certificates are returned
   */
  virtual void
  getAllCertificateNames(std::vector<Name>& nameList, bool isDefault) = 0;

  /**
   * @brief Get all the certificate name of a particular key name
   *
   * @param keyName        The specified key name
   * @param [out] nameList On return, the certificate name list
   * @param isDefault      If specified, only the default certificate is returned
   */
  virtual void
  getAllCertificateNamesOfKey(const Name& keyName, std::vector<Name>& nameList, bool isDefault) = 0;

  /*****************************************
   *            Delete Methods             *
   *****************************************/

  /**
   * @brief Delete a certificate
   *
   * @param certificateName The certificate name
   */
  virtual void
  deleteCertificateInfo(const Name& certificateName) = 0;

  /**
   * @brief Delete a public key and related certificates
   *
   * @param keyName The key name
   */
  virtual void
  deletePublicKeyInfo(const Name& keyName) = 0;

  /**
   * @brief Delete an identity and related public keys and certificates
   *
   * @param identity The identity name
   */
  virtual void
  deleteIdentityInfo(const Name& identity) = 0;

protected:

  /*****************************************
   *            Default Setter             *
   *****************************************/

  /**
   * @brief Set the default identity
   *
   * @param identityName The default identity name
   */
  virtual void
  setDefaultIdentityInternal(const Name& identityName) = 0;

  /**
   * @brief Set the default key name for the corresponding identity
   *
   * @param keyName The key name
   * @throws SecPublicInfo::Error if the key does not exist
   */
  virtual void
  setDefaultKeyNameForIdentityInternal(const Name& keyName) = 0;

  /**
   * @brief Set the default certificate name for the corresponding key
   *
   * @param certificateName The certificate name
   * @throws SecPublicInfo::Error if the certificate does not exist
   */
  virtual void
  setDefaultCertificateNameForKeyInternal(const Name& certificateName) = 0;

  /**
   * @brief return the scheme of the PibLocator
   */
  virtual std::string
  getScheme() = 0;

public:

  /*****************************************
   *            Helper Methods             *
   *****************************************/

  /**
   * @brief Set the default identity
   *
   * @param identityName The default identity name
   * @throws SecPublicInfo::Error if the identity does not exist
   */
  void
  setDefaultIdentity(const Name& identityName);

  /**
   * @brief Set the default key name for the corresponding identity
   *
   * @param keyName The key name
   * @throws SecPublicInfo::Error if either the identity or key does not exist
   */
  void
  setDefaultKeyNameForIdentity(const Name& keyName);

  /**
   * @brief Set the default certificate name for the corresponding key
   *
   * @param certificateName The certificate name
   * @throws SecPublicInfo::Error if either the certificate or key does not exist
   */
  void
  setDefaultCertificateNameForKey(const Name& certificateName);

  /**
   * @brief Generate a key name for the identity
   *
   * @param identityName The identity name
   * @param useKsk If true, generate a KSK name, otherwise a DSK name
   * @return The generated key name
   */
  Name
  getNewKeyName(const Name& identityName, bool useKsk);

  /**
   * @brief Get the default certificate name for the specified identity
   *
   * @param identityName The identity name
   * @return The default certificate name
   * @throws SecPublicInfo::Error if no certificate is found
   */
  Name
  getDefaultCertificateNameForIdentity(const Name& identityName);

  /**
   * @brief Get the default certificate name of the default identity
   *
   * @return The requested certificate name
   * @throws SecPublicInfo::Error if no certificate is found
   */
  Name
  getDefaultCertificateName();

  /**
   * @brief Add a certificate and set the certificate as the default one of its corresponding key
   *
   * @param certificate The certificate to be added
   * @throws SecPublicInfo::Error if the certificate cannot be added (though it is really rare)
   */
  void
  addCertificateAsKeyDefault(const IdentityCertificate& certificate);

  /**
   * @brief Add a certificate into the public key identity storage and set the certificate as the
   *        default one of its corresponding identity
   *
   * @param certificate The certificate to be added
   * @throws SecPublicInfo::Error if the certificate cannot be added (though it is really rare)
   */
  void
  addCertificateAsIdentityDefault(const IdentityCertificate& certificate);

  /**
   * @brief Add a certificate into the public key identity storage and set the certificate as the
   *        default one of the default identity
   *
   * @param certificate The certificate to be added
   * @throws SecPublicInfo::Error if the certificate cannot be added (though it is really rare)
   */
  void
  addCertificateAsSystemDefault(const IdentityCertificate& certificate);

  /**
   * @brief Get cached default certificate of the default identity
   *
   * @return The certificate which might be empty shared_ptr<IdentityCertificate>()
   * @deprecated Use getDefaultCertificate instead
   */
  DEPRECATED(
  shared_ptr<IdentityCertificate>
  defaultCertificate());

  /**
   * @brief Get cached default certificate of the default identity
   *
   * @return The certificate which might be empty shared_ptr<IdentityCertificate>()
   */
  shared_ptr<IdentityCertificate>
  getDefaultCertificate();

  /**
   * @brief try to get the default certificate of the default identity from the public info
   */
  void
  refreshDefaultCertificate();

protected:
  shared_ptr<IdentityCertificate> m_defaultCertificate;
  std::string m_location;
};

} // namespace ndn

#endif // NDN_SECURITY_SEC_PUBLIC_INFO_HPP
