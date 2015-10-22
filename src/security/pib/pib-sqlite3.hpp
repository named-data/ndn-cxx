/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_SECURITTY_PIB_PIB_SQLITE3_HPP
#define NDN_SECURITTY_PIB_PIB_SQLITE3_HPP

#include "pib-impl.hpp"

struct sqlite3;

namespace ndn {
namespace security {
namespace pib {

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
   * @brief Create sqlite3-based PIB backed
   *
   * This method will create a SQLite3 database file under the directory @p location.
   * If the directory does not exist, it will be created automatically.
   * It assumes that the directory does not contain a PIB database of an older version,
   * It is user's responsibility to update the older version database or remove the database.
   *
   * @param location The directory where the database file is located. By default, it points to the
   *                 $HOME/.ndn directory.
   * @throw PibImpl::Error when initialization fails.
   */
  explicit
  PibSqlite3(const std::string& location = "");

  /**
   * @brief Destruct and cleanup internal state
   */
  ~PibSqlite3();

  static const std::string&
  getScheme();

public: // TpmLocator management
  void
  setTpmLocator(const std::string& tpmLocator) final;

  std::string
  getTpmLocator() const final;

public: // Identity management
  bool
  hasIdentity(const Name& identity) const final;

  void
  addIdentity(const Name& identity) final;

  void
  removeIdentity(const Name& identity) final;

  void
  clearIdentities() final;

  std::set<Name>
  getIdentities() const final;

  void
  setDefaultIdentity(const Name& identityName) final;

  Name
  getDefaultIdentity() const final;

public: // Key management
  bool
  hasKey(const Name& keyName) const final;

  void
  addKey(const Name& identity, const Name& keyName,
         const uint8_t* key, size_t keyLen) final;

  void
  removeKey(const Name& keyName) final;

  Buffer
  getKeyBits(const Name& keyName) const final;

  std::set<Name>
  getKeysOfIdentity(const Name& identity) const final;

  void
  setDefaultKeyOfIdentity(const Name& identity, const Name& keyName) final;

  Name
  getDefaultKeyOfIdentity(const Name& identity) const final;

public: // Certificate Management
  bool
  hasCertificate(const Name& certName) const final;

  void
  addCertificate(const v2::Certificate& certificate) final;

  void
  removeCertificate(const Name& certName) final;

  v2::Certificate
  getCertificate(const Name& certName) const final;

  std::set<Name>
  getCertificatesOfKey(const Name& keyName) const final;

  void
  setDefaultCertificateOfKey(const Name& keyName, const Name& certName) final;

  v2::Certificate
  getDefaultCertificateOfKey(const Name& keyName) const final;

private:
  bool
  hasDefaultIdentity() const;

  bool
  hasDefaultKeyOfIdentity(const Name& identity) const;

  bool
  hasDefaultCertificateOfKey(const Name& keyName) const;

private:
  sqlite3* m_database;
};

} // namespace pib
} // namespace security
} // namespace ndn

#endif // NDN_SECURITTY_PIB_PIB_SQLITE3_HPP
