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

#include "pib-sqlite3.hpp"

#include "common.hpp"
#include "pib.hpp"
#include "util/sqlite3-statement.hpp"

#include <sqlite3.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace ndn {
namespace security {

using std::string;
using util::Sqlite3Statement;

static const string INITIALIZATION =
  "CREATE TABLE IF NOT EXISTS                    \n"
  "  tpmInfo(                                    \n"
  "    tpm_locator           BLOB                \n"
  "  );                                          \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  tpm_update_trigger                          \n"
  "  BEFORE UPDATE ON tpmInfo                    \n"
  "  WHEN NEW.tpm_locator!=OLD.tpm_locator       \n"
  "  BEGIN                                       \n"
  "    DELETE FROM certificates;                 \n"
  "    DELETE FROM keys;                         \n"
  "    DELETE FROM identities;                   \n"
  "  END;                                        \n"
  "                                              \n"
  "                                              \n"
  "CREATE TABLE IF NOT EXISTS                    \n"
  "  identities(                                 \n"
  "    id                    INTEGER PRIMARY KEY,\n"
  "    identity              BLOB NOT NULL,      \n"
  "    is_default            INTEGER DEFAULT 0   \n"
  "  );                                          \n"
  "                                              \n"
  "CREATE UNIQUE INDEX IF NOT EXISTS             \n"
  "  identityIndex ON identities(identity);      \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  identity_default_before_insert_trigger      \n"
  "  BEFORE INSERT ON identities                 \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NEW.is_default=1                       \n"
  "  BEGIN                                       \n"
  "    UPDATE identities SET is_default=0;       \n"
  "  END;                                        \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  identity_default_after_insert_trigger       \n"
  "  AFTER INSERT ON identities                  \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NOT EXISTS                             \n"
  "    (SELECT id                                \n"
  "       FROM identities                        \n"
  "       WHERE is_default=1)                    \n"
  "  BEGIN                                       \n"
  "    UPDATE identities                         \n"
  "      SET is_default=1                        \n"
  "      WHERE identity=NEW.identity;            \n"
  "  END;                                        \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  identity_default_update_trigger             \n"
  "  BEFORE UPDATE ON identities                 \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NEW.is_default=1 AND OLD.is_default=0  \n"
  "  BEGIN                                       \n"
  "    UPDATE identities SET is_default=0;       \n"
  "  END;                                        \n"
  "                                              \n"
  "                                              \n"
  "CREATE TABLE IF NOT EXISTS                    \n"
  "  keys(                                       \n"
  "    id                    INTEGER PRIMARY KEY,\n"
  "    identity_id           INTEGER NOT NULL,   \n"
  "    key_name              BLOB NOT NULL,      \n"
  "    key_type              INTEGER NOT NULL,   \n"
  "    key_bits              BLOB NOT NULL,      \n"
  "    is_default            INTEGER DEFAULT 0,  \n"
  "    FOREIGN KEY(identity_id)                  \n"
  "      REFERENCES identities(id)               \n"
  "      ON DELETE CASCADE                       \n"
  "      ON UPDATE CASCADE                       \n"
  "  );                                          \n"
  "                                              \n"
  "CREATE UNIQUE INDEX IF NOT EXISTS             \n"
  "  keyIndex ON keys(key_name);                 \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  key_default_before_insert_trigger           \n"
  "  BEFORE INSERT ON keys                       \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NEW.is_default=1                       \n"
  "  BEGIN                                       \n"
  "    UPDATE keys                               \n"
  "      SET is_default=0                        \n"
  "      WHERE identity_id=NEW.identity_id;      \n"
  "  END;                                        \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  key_default_after_insert_trigger            \n"
  "  AFTER INSERT ON keys                        \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NOT EXISTS                             \n"
  "    (SELECT id                                \n"
  "       FROM keys                              \n"
  "       WHERE is_default=1                     \n"
  "         AND identity_id=NEW.identity_id)     \n"
  "  BEGIN                                       \n"
  "    UPDATE keys                               \n"
  "      SET is_default=1                        \n"
  "      WHERE key_name=NEW.key_name;            \n"
  "  END;                                        \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  key_default_update_trigger                  \n"
  "  BEFORE UPDATE ON keys                       \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NEW.is_default=1 AND OLD.is_default=0  \n"
  "  BEGIN                                       \n"
  "    UPDATE keys                               \n"
  "      SET is_default=0                        \n"
  "      WHERE identity_id=NEW.identity_id;      \n"
  "  END;                                        \n"
  "                                              \n"
  "                                              \n"
  "CREATE TABLE IF NOT EXISTS                    \n"
  "  certificates(                               \n"
  "    id                    INTEGER PRIMARY KEY,\n"
  "    key_id                INTEGER NOT NULL,   \n"
  "    certificate_name      BLOB NOT NULL,      \n"
  "    certificate_data      BLOB NOT NULL,      \n"
  "    is_default            INTEGER DEFAULT 0,  \n"
  "    FOREIGN KEY(key_id)                       \n"
  "      REFERENCES keys(id)                     \n"
  "      ON DELETE CASCADE                       \n"
  "      ON UPDATE CASCADE                       \n"
  "  );                                          \n"
  "                                              \n"
  "CREATE UNIQUE INDEX IF NOT EXISTS             \n"
  "  certIndex ON certificates(certificate_name);\n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  cert_default_before_insert_trigger          \n"
  "  BEFORE INSERT ON certificates               \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NEW.is_default=1                       \n"
  "  BEGIN                                       \n"
  "    UPDATE certificates                       \n"
  "      SET is_default=0                        \n"
  "      WHERE key_id=NEW.key_id;                \n"
  "  END;                                        \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  cert_default_after_insert_trigger           \n"
  "  AFTER INSERT ON certificates                \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NOT EXISTS                             \n"
  "    (SELECT id                                \n"
  "       FROM certificates                      \n"
  "       WHERE is_default=1                     \n"
  "         AND key_id=NEW.key_id)               \n"
  "  BEGIN                                       \n"
  "    UPDATE certificates                       \n"
  "      SET is_default=1                        \n"
  "      WHERE certificate_name=NEW.certificate_name;\n"
  "  END;                                        \n"
  "                                              \n"
  "CREATE TRIGGER IF NOT EXISTS                  \n"
  "  cert_default_update_trigger                 \n"
  "  BEFORE UPDATE ON certificates               \n"
  "  FOR EACH ROW                                \n"
  "  WHEN NEW.is_default=1 AND OLD.is_default=0  \n"
  "  BEGIN                                       \n"
  "    UPDATE certificates                       \n"
  "      SET is_default=0                        \n"
  "      WHERE key_id=NEW.key_id;                \n"
  "  END;                                        \n";

static Name
getKeyName(const Name& identity, const name::Component& keyId)
{
  Name keyName = identity;
  keyName.append(keyId);
  return keyName;
}

PibSqlite3::PibSqlite3(const string& dir)
{
  // Determine the path of PIB DB
  boost::filesystem::path actualDir;
  if (dir == "") {
    if (getenv("HOME") == nullptr)
      BOOST_THROW_EXCEPTION(PibImpl::Error("Environment variable HOME is not set"));

    actualDir = boost::filesystem::path(getenv("HOME")) / ".ndn";
    boost::filesystem::create_directories(actualDir);
  }
  else {
    actualDir = boost::filesystem::path(dir);
    boost::filesystem::create_directories(actualDir);
  }
  // Open PIB
  int result = sqlite3_open_v2((actualDir / "pib.db").c_str(), &m_database,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
#ifdef NDN_CXX_DISABLE_SQLITE3_FS_LOCKING
                               "unix-dotfile"
#else
                               nullptr
#endif
                               );

  if (result != SQLITE_OK)
    BOOST_THROW_EXCEPTION(PibImpl::Error("PIB DB cannot be opened/created: " + dir));


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

void
PibSqlite3::setTpmLocator(const std::string& tpmLocator)
{
  Sqlite3Statement statement(m_database, "UPDATE tpmInfo SET tpm_locator=?");
  statement.bind(1, tpmLocator, SQLITE_TRANSIENT);
  statement.step();

  // no row is updated, tpm_locator does not exist, insert it directly
  if (0 == sqlite3_changes(m_database)) {
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

  string tpmLocator;
  if (res == SQLITE_ROW)
    return statement.getString(0);
  else
    BOOST_THROW_EXCEPTION(Pib::Error("TPM info does not exist"));
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
  Sqlite3Statement statement(m_database, "INSERT INTO identities (identity) values (?)");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

void
PibSqlite3::removeIdentity(const Name& identity)
{
  Sqlite3Statement statement(m_database, "DELETE FROM identities WHERE identity=?");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
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
PibSqlite3::hasKey(const Name& identity, const name::Component& keyId) const
{
  Name keyName = getKeyName(identity, keyId);

  Sqlite3Statement statement(m_database, "SELECT id FROM keys WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  return (statement.step() == SQLITE_ROW);
}

void
PibSqlite3::addKey(const Name& identity, const name::Component& keyId, const PublicKey& publicKey)
{
  if (hasKey(identity, keyId)) {
    return;
  }

  // ensure identity exists
  addIdentity(identity);

  // add key
  Name keyName = getKeyName(identity, keyId);

  Sqlite3Statement statement(m_database,
                             "INSERT INTO keys (identity_id, key_name, key_type, key_bits) "
                             "VALUES ((SELECT id FROM identities WHERE identity=?), ?, ?, ?)");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);
  statement.bind(2, keyName.wireEncode(), SQLITE_TRANSIENT);
  statement.bind(3, publicKey.getKeyType());
  statement.bind(4, publicKey.get().buf(), publicKey.get().size(), SQLITE_STATIC);
  statement.step();
}

void
PibSqlite3::removeKey(const Name& identity, const name::Component& keyId)
{
  Name keyName = getKeyName(identity, keyId);

  Sqlite3Statement statement(m_database, "DELETE FROM keys WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

PublicKey
PibSqlite3::getKeyBits(const Name& identity, const name::Component& keyId) const
{
  Name keyName = getKeyName(identity, keyId);

  Sqlite3Statement statement(m_database, "SELECT key_bits FROM keys WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW)
    return PublicKey(statement.getBlob(0), statement.getSize(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("Key does not exist"));
}

std::set<name::Component>
PibSqlite3::getKeysOfIdentity(const Name& identity) const
{
  std::set<name::Component> keyNames;

  Sqlite3Statement statement(m_database,
                             "SELECT key_name "
                             "FROM keys JOIN identities ON keys.identity_id=identities.id "
                             "WHERE identities.identity=?");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);

  while (statement.step() == SQLITE_ROW) {
    Name keyName(statement.getBlock(0));
    keyNames.insert(keyName.get(-1));
  }

  return keyNames;
}

void
PibSqlite3::setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId)
{
  Name keyName = getKeyName(identity, keyId);

  if (!hasKey(identity, keyId)) {
    BOOST_THROW_EXCEPTION(Pib::Error("No such key"));
  }

  Sqlite3Statement statement(m_database, "UPDATE keys SET is_default=1 WHERE key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

name::Component
PibSqlite3::getDefaultKeyOfIdentity(const Name& identity) const
{
  if (!hasIdentity(identity)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Identity does not exist"));
  }

  Sqlite3Statement statement(m_database,
                             "SELECT key_name "
                             "FROM keys JOIN identities ON keys.identity_id=identities.id "
                             "WHERE identities.identity=? AND keys.is_default=1");
  statement.bind(1, identity.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW) {
    Name keyName(statement.getBlock(0));
    return keyName.get(-1);
  }
  else
    BOOST_THROW_EXCEPTION(Pib::Error("No default key"));
}

bool
PibSqlite3::hasCertificate(const Name& certName) const
{
  Sqlite3Statement statement(m_database, "SELECT id FROM certificates WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);
  return (statement.step() == SQLITE_ROW);
}

void
PibSqlite3::addCertificate(const IdentityCertificate& certificate)
{
  const Name& certName = certificate.getName();
  const Name& keyName = certificate.getPublicKeyName();

  name::Component keyId = keyName.get(-1);
  Name identityName = keyName.getPrefix(-1);

  // ensure key exists
  addKey(identityName, keyId, certificate.getPublicKeyInfo());

  Sqlite3Statement statement(m_database,
                             "INSERT INTO certificates "
                             "(key_id, certificate_name, certificate_data) "
                             "VALUES ((SELECT id FROM keys WHERE key_name=?), ?, ?)");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);
  statement.bind(2, certName.wireEncode(), SQLITE_TRANSIENT);
  statement.bind(3, certificate.wireEncode(), SQLITE_STATIC);
  statement.step();
}

void
PibSqlite3::removeCertificate(const Name& certName)
{
  Sqlite3Statement statement(m_database, "DELETE FROM certificates WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

IdentityCertificate
PibSqlite3::getCertificate(const Name& certName) const
{
  Sqlite3Statement statement(m_database,
                             "SELECT certificate_data FROM certificates WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW)
    return IdentityCertificate(statement.getBlock(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate does not exit"));
}

std::set<Name>
PibSqlite3::getCertificatesOfKey(const Name& identity, const name::Component& keyId) const
{
  std::set<Name> certNames;

  Name keyName = getKeyName(identity, keyId);

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
PibSqlite3::setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId,
                                       const Name& certName)
{
  if (!hasCertificate(certName)) {
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate does not exist"));
  }

  Sqlite3Statement statement(m_database,
                             "UPDATE certificates SET is_default=1 WHERE certificate_name=?");
  statement.bind(1, certName.wireEncode(), SQLITE_TRANSIENT);
  statement.step();
}

IdentityCertificate
PibSqlite3::getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const
{
  Name keyName = getKeyName(identity, keyId);

  Sqlite3Statement statement(m_database,
                             "SELECT certificate_data "
                             "FROM certificates JOIN keys ON certificates.key_id=keys.id "
                             "WHERE certificates.is_default=1 AND keys.key_name=?");
  statement.bind(1, keyName.wireEncode(), SQLITE_TRANSIENT);

  if (statement.step() == SQLITE_ROW)
    return IdentityCertificate(statement.getBlock(0));
  else
    BOOST_THROW_EXCEPTION(Pib::Error("Certificate does not exit"));
}

} // namespace security
} // namespace ndn
