/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
  setTpmLocator(const std::string& tpmLocator) final;

  virtual std::string
  getTpmLocator() const final;

public: // Identity management

  virtual bool
  hasIdentity(const Name& identity) const final;

  virtual void
  addIdentity(const Name& identity) final;

  virtual void
  removeIdentity(const Name& identity) final;

  virtual std::set<Name>
  getIdentities() const final;

  virtual void
  setDefaultIdentity(const Name& identityName) final;

  virtual Name
  getDefaultIdentity() const final;

public: // Key management

  virtual bool
  hasKey(const Name& identity, const name::Component& keyId) const final;

  virtual void
  addKey(const Name& identity, const name::Component& keyId, const v1::PublicKey& publicKey) final;

  virtual void
  removeKey(const Name& identity, const name::Component& keyId) final;

  virtual v1::PublicKey
  getKeyBits(const Name& identity, const name::Component& keyId) const final;

  virtual std::set<name::Component>
  getKeysOfIdentity(const Name& identity) const final;

  virtual void
  setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId) final;

  virtual name::Component
  getDefaultKeyOfIdentity(const Name& identity) const final;

public: // Certificate Management

  virtual bool
  hasCertificate(const Name& certName) const final;

  virtual void
  addCertificate(const v1::IdentityCertificate& certificate) final;

  virtual void
  removeCertificate(const Name& certName) final;

  virtual v1::IdentityCertificate
  getCertificate(const Name& certName) const final;

  virtual std::set<Name>
  getCertificatesOfKey(const Name& identity, const name::Component& keyId) const final;

  virtual void
  setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId,
                             const Name& certName) final;

  virtual v1::IdentityCertificate
  getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const final;

private:
  sqlite3* m_database;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITTY_PIB_SQLITE3_HPP
