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

#include "pib-sqlite3.hpp"
#include "pib.hpp"
#include "../security-common.hpp"
#include "../../util/sqlite3-statement.hpp"

#include <sqlite3.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace ndn {
namespace security {
namespace pib {

using util::Sqlite3Statement;

static const std::string INITIALIZATION = R"SQL(
CREATE TABLE IF NOT EXISTS
  tpmInfo(
    tpm_locator           BLOB
  );

CREATE TABLE IF NOT EXISTS
  identities(
    id                    INTEGER PRIMARY KEY,
    identity              BLOB NOT NULL,
    is_default            INTEGER DEFAULT 0
  );

CREATE UNIQUE INDEX IF NOT EXISTS
  identityIndex ON identities(identity);

CREATE TRIGGER IF NOT EXISTS
  identity_default_before_insert_trigger
  BEFORE INSERT ON identities
  FOR EACH ROW
  WHEN NEW.is_default=1
  BEGIN
    UPDATE identities SET is_default=0;
  END;

CREATE TRIGGER IF NOT EXISTS
  identity_default_after_insert_trigger
  AFTER INSERT ON identities
  FOR EACH ROW
  WHEN NOT EXISTS
    (SELECT id
       FROM identities
       WHERE is_default=1)
  BEGIN
    UPDATE identities
      SET is_default=1
      WHERE identity=NEW.identity;
  END;

CREATE TRIGGER IF NOT EXISTS
  identity_default_update_trigger
  BEFORE UPDATE ON identities
  FOR EACH ROW
  WHEN NEW.is_default=1 AND OLD.is_default=0
  BEGIN
    UPDATE identities SET is_default=0;
  END;


CREATE TABLE IF NOT EXISTS
  keys(
    id                    INTEGER PRIMARY KEY,
    identity_id           INTEGER NOT NULL,
    key_name              BLOB NOT NULL,
    key_bits              BLOB NOT NULL,
    is_default            INTEGER DEFAULT 0,
    FOREIGN KEY(identity_id)
      REFERENCES identities(id)
      ON DELETE CASCADE
      ON UPDATE CASCADE
  );

CREATE UNIQUE INDEX IF NOT EXISTS
  keyIndex ON keys(key_name);

CREATE TRIGGER IF NOT EXISTS
  key_default_before_insert_trigger
  BEFORE INSERT ON keys
  FOR EACH ROW
  WHEN NEW.is_default=1
  BEGIN
    UPDATE keys
      SET is_default=0
      WHERE identity_id=NEW.identity_id;
  END;

CREATE TRIGGER IF NOT EXISTS
  key_default_after_insert_trigger
  AFTER INSERT ON keys
  FOR EACH ROW
  WHEN NOT EXISTS
    (SELECT id
       FROM keys
       WHERE is_default=1
         AND identity_id=NEW.identity_id)
  BEGIN
    UPDATE keys
      SET is_default=1
      WHERE key_name=NEW.key_name;
  END;

CREATE TRIGGER IF NOT EXISTS
  key_default_update_trigger
  BEFORE UPDATE ON keys
  FOR EACH ROW
  WHEN NEW.is_default=1 AND OLD.is_default=0
  BEGIN
    UPDATE keys
      SET is_default=0
      WHERE identity_id=NEW.identity_id;
  END;


CREATE TABLE IF NOT EXISTS
  certificates(
    id                    INTEGER PRIMARY KEY,
    key_id                INTEGER NOT NULL,
    certificate_name      BLOB NOT NULL,
    certificate_data      BLOB NOT NULL,
    is_default            INTEGER DEFAULT 0,
    FOREIGN KEY(key_id)
      REFERENCES keys(id)
      ON DELETE CASCADE
      ON UPDATE CASCADE
  );

CREATE UNIQUE INDEX IF NOT EXISTS
  certIndex ON certificates(certificate_name);

CREATE TRIGGER IF NOT EXISTS
  cert_default_before_insert_trigger
  BEFORE INSERT ON certificates
  FOR EACH ROW
  WHEN NEW.is_default=1
  BEGIN
    UPDATE certificates
      SET is_default=0
      WHERE key_id=NEW.key_id;
  END;

CREATE TRIGGER IF NOT EXISTS
  cert_default_after_insert_trigger
  AFTER INSERT ON certificates
  FOR EACH ROW
  WHEN NOT EXISTS
    (SELECT id
       FROM certificates
       WHERE is_default=1
         AND key_id=NEW.key_id)
  BEGIN
    UPDATE certificates
      SET is_default=1
      WHERE certificate_name=NEW.certificate_name;
  END;

CREATE TRIGGER IF NOT EXISTS
  cert_default_update_trigger
  BEFORE UPDATE ON certificates
  FOR EACH ROW
  WHEN NEW.is_default=1 AND OLD.is_default=0
  BEGIN
    UPDATE certificates
      SET is_default=0
      WHERE key_id=NEW.key_id;
  END;
)SQL";

PibSqlite3::PibSqlite3(const std::string& location)
{
  // Determine the path of PIB DB
  boost::filesystem::path dbDir;
  if (!location.empty()) {
    dbDir = boost::filesystem::path(location);
  }
#ifdef NDN_CXX_HAVE_TESTS
  else if (getenv("TEST_HOME") != nullptr) {
    dbDir = boost::filesystem::path(getenv("TEST_HOME")) / ".ndn";
  }
#endif // NDN_CXX_HAVE_TESTS
  else if (getenv("HOME") != nullptr) {
    dbDir = boost::filesystem::path(getenv("HOME")) / ".ndn";
  }
  else {
    dbDir = boost::filesystem::current_path() / ".ndn";
  }
  boost::filesystem::create_directories(dbDir);

  // Open PIB
  int result = sqlite3_open_v2((dbDir / "pib.db").c_str(), &m_database,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
#ifdef NDN_CXX_DISABLE_SQLITE3_FS_LOCKING
                               "unix-dotfile"
#else
                               nullptr
#endif
                               );

  if (result != SQLITE_OK) {
    BOOST_THROW_EXCEPTION(PibImpl::Error("PIB database cannot be opened/created in " + location));
  }

  // enable foreign key
  sqlite3_exec(m_database, "PRAGMA foreign_keys=ON", nullptr, nullptr, nullptr);

  // initialize PIB tables
  char* errorMessage = nullptr;
  result = sqlite3_exec(m_database, INITIALIZATION.c_str(), nullptr, nullptr, &errorMessage);
  if (result != SQLITE_OK && errorMessage != nullptr) {
    sqlite3_free(errorMessage);
    BOOST_THROW_EXCEPTION(PibImpl::Error("PIB DB cannot be initialized"));
  }
}

PibSqlite3::~PibSqlite3()
{
  sqlite3_close(m_database);
}

const std::string&
PibSqlite3::getScheme()
{
  static std::string scheme = "pib-sqlite3";
  return scheme;
}

void
PibSqlite3::setTpmLocator(const std::string& tpmLocator)
{
  Sqlite3Statement statement(m_database, "UPDATE tpmInfo SET tpm_locator=?");
  statement.bind(1, tpmLocator, SQLITE_TRANSIENT);
  statement.step();

  if (sqlite3_changes(m_database) == 0) {
    // no row is updated, tpm_locator does not exist, insert it directly
    Sqlite3Statement insertStatement(m_database, "INSERT INTO tpmInfo (tpm_locator) values (?)");
    insertStatement.bind(1, tpmLocator, SQLITE_TRANSIENT);
    insertStatement.step();
  }
}

std::string
PibSqlite3::getTpmLocator() const
{
  Sqlite3Statement statement(m_database, "SELECT tpm_locator FROM tpmInfo");
  int res = statement.step();
  if (res == SQLITE_ROW)
    return statement.getString(0);
  else
    return "";
}

bool
PibSqlite3::hasIdentity(const Name& identity) const
{
  Sqlite3Statement statement(m_database, "SELECT id FROM identities WHERE identity=?");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
  return (statement.step() == SQLITE_ROW);
}

void
PibSqlite3::addIdentity(const Name& identity)
{
  if (!hasIdentity(identity)) {
    Sqlite3Statement statement(m_database, "INSERT INTO identities (identity) values (?)");
    statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
    statement.step();
  }

  if (!hasDefaultIdentity()) {
    setDefaultIdentity(identity);
  }
}

void
PibSqlite3::removeIdentity(const Name& identity)
{
  Sqlite3Statement statement(m_database, "DELETE FROM identities WHERE identity=?");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

void
PibSqlite3::clearIdentities()
{
  Sqlite3Statement statement(m_database, "DELETE FROM identities");
  statement.step();
}

std::set<Name>
PibSqlite3::getIdentities() const
{
  std::set<Name> identities;
  Sqlite3Statement statement(m_database, "SELECT identity FROM identities");

  while (statement.step() == SQLITE_ROW)
    identities.insert(Name(statement.getBlock(0)));

  return identities;
}

void
PibSqlite3::setDefaultIdentity(const Name& identityName)
{
  Sqlite3Statement statement(m_database, "UPDATE identities SET is_default=1 WHERE identity=?");
  statement.bind(1, identityName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

Name
PibSqlite3::getDefaultIdentity() const
{
  Sqlite3Statement statement(m_database, "SELECT identity FROM identities WHERE is_default=1");

  if (statement.step() == SQLITE_ROW)
    return Name(statement.getBlock(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("No default identity"));
}

bool
PibSqlite3::hasDefaultIdentity() const
{
  Sqlite3Statement statement(m_database, "SELECT identity FROM identities WHERE is_default=1");
  return (statement.step() == SQLITE_ROW);
}

bool
PibSqlite3::hasKey(const Name& keyName) const
{
  Sqlite3Statement statement(m_database, "SELECT id FROM keys WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  return (statement.step() == SQLITE_ROW);
}

void
PibSqlite3::addKey(const Name& identity, const Name& keyName,
                   const uint8_t* key, size_t keyLen)
{
  // ensure identity exists
  addIdentity(identity);

  if (!hasKey(keyName)) {
    Sqlite3Statement statement(m_database,
                               "INSERT INTO keys (identity_id, key_name, key_bits) "
                               "VALUES ((SELECT id FROM identities WHERE identity=?), ?, ?)");
    statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
    statement.bind(2, keyName.wireEncode(), SQLITE_TRANSIENT);
    statement.bind(3, key, keyLen, SQLITE_STATIC);
    statement.step();
  }
  else {
    Sqlite3Statement statement(m_database,
                               "UPDATE keys SET key_bits=? WHERE key_name=?");
    statement.bind(1, key, keyLen, SQLITE_STATIC);
    statement.bind(2, keyName.wireEncode(), SQLITE_TRANSIENT);
    statement.step();
  }

  if (!hasDefaultKeyOfIdentity(identity)) {
    setDefaultKeyOfIdentity(identity, keyName);
  }
}

void
PibSqlite3::removeKey(const Name& keyName)
{
  Sqlite3Statement statement(m_database, "DELETE FROM keys WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

Buffer
PibSqlite3::getKeyBits(const Name& keyName) const
{
  Sqlite3Statement statement(m_database, "SELECT key_bits FROM keys WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW)
    return Buffer(statement.getBlob(0), statement.getSize(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("Key `" + keyName.toUri() + "` does not exist"));
}

std::set<Name>
PibSqlite3::getKeysOfIdentity(const Name& identity) const
{
  std::set<Name> keyNames;

  Sqlite3Statement statement(m_database,
                             "SELECT key_name "
                             "FROM keys JOIN identities ON keys.identity_id=identities.id "
                             "WHERE identities.identity=?");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);

  while (statement.step() == SQLITE_ROW) {
    keyNames.insert(Name(statement.getBlock(0)));
  }

  return keyNames;
}

void
PibSqlite3::setDefaultKeyOfIdentity(const Name& identity, const Name& keyName)
{
  if (!hasKey(keyName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Key `" + keyName.toUri() + "` does not exist"));
  }

  Sqlite3Statement statement(m_database, "UPDATE keys SET is_default=1 WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

Name
PibSqlite3::getDefaultKeyOfIdentity(const Name& identity) const
{
  if (!hasIdentity(identity)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Identity `" + identity.toUri() + "` does not exist"));
  }

  Sqlite3Statement statement(m_database,
                             "SELECT key_name "
                             "FROM keys JOIN identities ON keys.identity_id=identities.id "
                             "WHERE identities.identity=? AND keys.is_default=1");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW) {
    return Name(statement.getBlock(0));
  }
  else
    BOOST_THROW_EXCEPTION(Pib::Error("No default key for identity `" + identity.toUri() + "`"));
}

bool
PibSqlite3::hasDefaultKeyOfIdentity(const Name& identity) const
{
  Sqlite3Statement statement(m_database,
                             "SELECT key_name "
                             "FROM keys JOIN identities ON keys.identity_id=identities.id "
                             "WHERE identities.identity=? AND keys.is_default=1");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);

  return (statement.step() == SQLITE_ROW);
}

bool
PibSqlite3::hasCertificate(const Name& certName) const
{
  Sqlite3Statement statement(m_database, "SELECT id FROM certificates WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);
  return (statement.step() == SQLITE_ROW);
}

void
PibSqlite3::addCertificate(const v2::Certificate& certificate)
{
  // ensure key exists
  const Block& content = certificate.getContent();
  addKey(certificate.getIdentity(), certificate.getKeyName(), content.value(), content.value_size());

  if (!hasCertificate(certificate.getName())) {
    Sqlite3Statement statement(m_database,
                               "INSERT INTO certificates "
                               "(key_id, certificate_name, certificate_data) "
                               "VALUES ((SELECT id FROM keys WHERE key_name=?), ?, ?)");
    statement.bind(1, certificate.getKeyName().wireEncode(), SQLITE_TRANSIENT);
    statement.bind(2, certificate.getName().wireEncode(), SQLITE_TRANSIENT);
    statement.bind(3, certificate.wireEncode(), SQLITE_STATIC);
    statement.step();
  }
  else {
    Sqlite3Statement statement(m_database,
                               "UPDATE certificates SET certificate_data=? WHERE certificate_name=?");
    statement.bind(1, certificate.wireEncode(), SQLITE_STATIC);
    statement.bind(2, certificate.getName().wireEncode(), SQLITE_TRANSIENT);
    statement.step();
  }

  if (!hasDefaultCertificateOfKey(certificate.getKeyName())) {
    setDefaultCertificateOfKey(certificate.getKeyName(), certificate.getName());
  }
}

void
PibSqlite3::removeCertificate(const Name& certName)
{
  Sqlite3Statement statement(m_database, "DELETE FROM certificates WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

v2::Certificate
PibSqlite3::getCertificate(const Name& certName) const
{
  Sqlite3Statement statement(m_database,
                             "SELECT certificate_data FROM certificates WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW)
    return v2::Certificate(statement.getBlock(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate `" + certName.toUri() + "` does not exit"));
}

std::set<Name>
PibSqlite3::getCertificatesOfKey(const Name& keyName) const
{
  std::set<Name> certNames;

  Sqlite3Statement statement(m_database,
                             "SELECT certificate_name "
                             "FROM certificates JOIN keys ON certificates.key_id=keys.id "
                             "WHERE keys.key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  while (statement.step() == SQLITE_ROW)
    certNames.insert(Name(statement.getBlock(0)));

  return certNames;
}

void
PibSqlite3::setDefaultCertificateOfKey(const Name& keyName, const Name& certName)
{
  if (!hasCertificate(certName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate `" + certName.toUri() + "` does not exist"));
  }

  Sqlite3Statement statement(m_database,
                             "UPDATE certificates SET is_default=1 WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

v2::Certificate
PibSqlite3::getDefaultCertificateOfKey(const Name& keyName) const
{
  Sqlite3Statement statement(m_database,
                             "SELECT certificate_data "
                             "FROM certificates JOIN keys ON certificates.key_id=keys.id "
                             "WHERE certificates.is_default=1 AND keys.key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW)
    return v2::Certificate(statement.getBlock(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("No default certificate for key `" + keyName.toUri() + "`"));
}

bool
PibSqlite3::hasDefaultCertificateOfKey(const Name& keyName) const
{
  Sqlite3Statement statement(m_database,
                             "SELECT certificate_data "
                             "FROM certificates JOIN keys ON certificates.key_id=keys.id "
                             "WHERE certificates.is_default=1 AND keys.key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  return (statement.step() == SQLITE_ROW);
}

} // namespace pib
} // namespace security
} // namespace ndn
