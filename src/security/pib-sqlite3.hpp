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

  virtual void
  setTpmLocator(const std::string& tpmLocator) NDN_CXX_DECL_FINAL;

  virtual std::string
  getTpmLocator() const NDN_CXX_DECL_FINAL;

public: // Identity management

  virtual bool
  hasIdentity(const Name& identity) const NDN_CXX_DECL_FINAL;

  virtual void
  addIdentity(const Name& identity) NDN_CXX_DECL_FINAL;

  virtual void
  removeIdentity(const Name& identity) NDN_CXX_DECL_FINAL;

  virtual std::set<Name>
  getIdentities() const NDN_CXX_DECL_FINAL;

  virtual void
  setDefaultIdentity(const Name& identityName) NDN_CXX_DECL_FINAL;

  virtual Name
  getDefaultIdentity() const NDN_CXX_DECL_FINAL;

public: // Key management

  virtual bool
  hasKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

  virtual void
  addKey(const Name& identity, const name::Component& keyId, const PublicKey& publicKey) NDN_CXX_DECL_FINAL;

  virtual void
  removeKey(const Name& identity, const name::Component& keyId) NDN_CXX_DECL_FINAL;

  virtual PublicKey
  getKeyBits(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

  virtual std::set<name::Component>
  getKeysOfIdentity(const Name& identity) const NDN_CXX_DECL_FINAL;

  virtual void
  setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId) NDN_CXX_DECL_FINAL;

  virtual name::Component
  getDefaultKeyOfIdentity(const Name& identity) const NDN_CXX_DECL_FINAL;

public: // Certificate Management

  virtual bool
  hasCertificate(const Name& certName) const NDN_CXX_DECL_FINAL;

  virtual void
  addCertificate(const IdentityCertificate& certificate) NDN_CXX_DECL_FINAL;

  virtual void
  removeCertificate(const Name& certName) NDN_CXX_DECL_FINAL;

  virtual IdentityCertificate
  getCertificate(const Name& certName) const NDN_CXX_DECL_FINAL;

  virtual std::set<Name>
  getCertificatesOfKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

  virtual void
  setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId,
                             const Name& certName) NDN_CXX_DECL_FINAL;

  virtual IdentityCertificate
  getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_FINAL;

private:
  sqlite3* m_database;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITTY_PIB_SQLITE3_HPP
