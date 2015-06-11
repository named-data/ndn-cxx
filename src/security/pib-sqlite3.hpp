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

#ifndef NDN_SECURITTY_PIB_SQLITE3_HPP
#define NDN_SECURITTY_PIB_SQLITE3_HPP

#include "pib-impl.hpp"

struct sqlite3;

namespace ndn {
namespace security {

/**
 * @brief Pib backend implementation based on SQLite3 database
 *
 * All the contents in Pib are stored in a SQLite3 database file.
 * This backend provides more persistent storage than PibMemory.
 */
class PibSqlite3 : public PibImpl
{
public:
  /**
   * @brief Constructor of PibSqlite3
   *
   * This method will create a SQLite3 database file under the directory @p dir.
   * If the directory does not exist, it will be created automatically.
   * It assumes that the directory does not contain a PIB database of an older version,
   * It is user's responsibility to update the older version database or remove the database.
   *
   * @param dir The directory where the database file is located. By default, it points to the
   *        $HOME/.ndn directory.
   * @throws PibImpl::Error when initialization fails.
   */
  explicit
  PibSqlite3(const std::string& dir = "");

  /**
   * @brief Destruct and cleanup internal state
   */
  ~PibSqlite3();

public: // TpmLocator management

  /**
   * @brief Set the corresponding TPM information to @p tpmLocator.
   *
   * If the provided @p tpmLocator is different from the existing one, the
   * content in PIB will be cleaned up, otherwise nothing will be changed.
   *
   * @param tmpLocator The name for the new tmpLocator
   */
  virtual void
  setTpmLocator(const std::string& tpmLocator) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get TPM Locator
   */
  virtual std::string
  getTpmLocator() const NDN_CXX_DECL_FINAL;

public: // Identity management

  /**
   * @brief Check the existence of an identity.
   *
   * @param identity The name of the identity.
   * @return true if the identity exists, otherwise false.
   */
  virtual bool
  hasIdentity(const Name& identity) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Add an identity.
   *
   * If the identity already exists, do nothing.
   * If no default identity has been set, set the added one as default identity.
   *
   * @param identity The name of the identity to add.
   */
  virtual void
  addIdentity(const Name& identity) NDN_CXX_DECL_FINAL;

  /**
   * @brief Remove an identity
   *
   * If the identity does not exist, do nothing.
   * Remove related keys and certificates as well.
   *
   * @param identity The name of the identity to remove.
   */
  virtual void
  removeIdentity(const Name& identity) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get the name of all the identities
   */
  virtual std::set<Name>
  getIdentities() const NDN_CXX_DECL_FINAL;

  /**
   * @brief Set an identity with name @p identityName as the default identity.
   *
   * Since adding an identity only requires the identity name, create the
   * identity if it does not exist.
   *
   * @param identityName The name for the default identity.
   */
  virtual void
  setDefaultIdentity(const Name& identityName) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get the default identity.
   *
   * @return The name for the default identity.
   * @throws Pib::Error if no default identity.
   */
  virtual Name
  getDefaultIdentity() const NDN_CXX_DECL_FINAL;

public: // Key management

  /**
   * @brief Check the existence of a key.
   *
   * @param identity The name of the belonged identity.
   * @param keyId The key id component.
   * @return true if the key exists, otherwise false. Return false if the identity does not exist
   */
  virtual bool
  hasKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Add a key.
   *
   * If the key already exists, do nothing.
   * If the identity does not exist, add the identity as well.
   * If no default key of the identity has been set, set the added one as default
   * key of the identity.
   *
   * @param identity The name of the belonged identity.
   * @param keyId The key id component.
   * @param publicKey The public key bits.
   */
  virtual void
  addKey(const Name& identity, const name::Component& keyId, const PublicKey& publicKey) NDN_CXX_DECL_FINAL;

  /**
   * @brief Remove a key.
   *
   * If the key does not exist, do nothing.
   * Remove related certificates as well.
   *
   * @param identity The name of the belonged identity.
   * @param keyId The key id component.
   */
  virtual void
  removeKey(const Name& identity, const name::Component& keyId) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get the key bits of a key.
   *
   * @param identity The name of the belonged identity.
   * @param keyId The key id component.
   * @return key bits
   * @throws Pib::Error if the key does not exist.
   */
  virtual PublicKey
  getKeyBits(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Get all the key ids of an identity with name @p identity
   *
   * The returned key ids can be used to create a KeyContainer.
   * With key id, identity name, backend implementation, one can create a Key frontend instance.
   *
   * @return the key id name component set. If the identity does not exist, return an empty set.
   */
  virtual std::set<name::Component>
  getKeysOfIdentity(const Name& identity) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Set an key with id @p keyId as the default key of an identity with name @p identity.
   *
   * @param identity The name of the belonged identity.
   * @param keyId The key id component.
   * @throws Pib::Error if the key does not exist.
   */
  virtual void
  setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get the id of the default key of an identity with name @p identity.
   *
   * @param identity The name of the belonged identity.
   * @throws Pib::Error if no default key or the identity does not exist.
   */
  virtual name::Component
  getDefaultKeyOfIdentity(const Name& identity) const NDN_CXX_DECL_FINAL;

public: // Certificate Management

  /**
   * @brief Check the existence of a certificate with name @p certName.
   *
   * @param certName The name of the certificate.
   * @return true if the certificate exists, otherwise false.
   */
  virtual bool
  hasCertificate(const Name& certName) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Add a certificate.
   *
   * If the certificate already exists, do nothing.
   * If the key or identity do not exist, add them as well.
   * If no default certificate of the key has been set, set the added one as
   * default certificate of the key.
   *
   * @param certificate The certificate to add.
   */
  virtual void
  addCertificate(const IdentityCertificate& certificate) NDN_CXX_DECL_FINAL;

  /**
   * @brief Remove a certificate with name @p certName.
   *
   * If the certificate does not exist, do nothing.
   *
   * @param certName The name of the certificate.
   */
  virtual void
  removeCertificate(const Name& certName) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get a certificate with name @p certName.
   *
   * @param certName The name of the certificate.
   * @return the certificate.
   * @throws Pib::Error if the certificate does not exist.
   */
  virtual IdentityCertificate
  getCertificate(const Name& certName) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Get a list of certificate names of a key with id @p keyId of @p identity.
   *
   * The returned certificate names can be used to create a CertificateContainer.
   * With certificate name and backend implementation, one can obtain the certificate directly.
   *
   * @param identity The name of the belonging identity.
   * @param keyId The key id.
   * @return The certificate name set. If the key does not exist, return an empty set.
   */
  virtual std::set<Name>
  getCertificatesOfKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

  /**
   * @brief Set a cert with name @p certName as the default of a key with id @keyId of @identity.
   *
   * @param identity The name of the belonging identity.
   * @param keyId The key id.
   * @param certName The name of the certificate.
   * @throws Pib::Error if the certificate with name @p certName does not exist.
   */
  virtual void
  setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId,
                             const Name& certName) NDN_CXX_DECL_FINAL;

  /**
   * @brief Get the default certificate of a key with id @keyId of @identity.
   *
   * @param identity The name of the belonging identity.
   * @param keyId The key id.
   * @return a pointer to the certificate, null if no default certificate for the key.
   * @throws Pib::Error if the default certificate does not exist.
   */
  virtual IdentityCertificate
  getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

private:
  sqlite3* m_database;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITTY_PIB_SQLITE3_HPP
