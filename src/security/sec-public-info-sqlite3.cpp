/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#include "common.hpp"

#include "sec-public-info-sqlite3.hpp"
#include "identity-certificate.hpp"
#include "signature-sha256-with-rsa.hpp"
#include "../data.hpp"

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>

using namespace std;

namespace ndn {

static const string INIT_ID_TABLE = "\
CREATE TABLE IF NOT EXISTS                                           \n \
  Identity(                                                          \n \
      identity_name     BLOB NOT NULL,                               \n \
      default_identity  INTEGER DEFAULT 0,                           \n \
                                                                     \
      PRIMARY KEY (identity_name)                                    \n \
  );                                                                 \n \
                                                                     \
CREATE INDEX identity_index ON Identity(identity_name);              \n \
";

static const string INIT_KEY_TABLE = "\
CREATE TABLE IF NOT EXISTS                                           \n \
  Key(                                                               \n \
      identity_name     BLOB NOT NULL,                               \n \
      key_identifier    BLOB NOT NULL,                               \n \
      key_type          INTEGER,                                     \n \
      public_key        BLOB,                                        \n \
      default_key       INTEGER DEFAULT 0,                           \n \
      active            INTEGER DEFAULT 0,                           \n \
                                                                     \
      PRIMARY KEY (identity_name, key_identifier)                    \n \
  );                                                                 \n \
                                                                     \
CREATE INDEX key_index ON Key(identity_name);                        \n \
";

static const string INIT_CERT_TABLE = "\
CREATE TABLE IF NOT EXISTS                                           \n \
  Certificate(                                                       \n \
      cert_name         BLOB NOT NULL,                               \n \
      cert_issuer       BLOB NOT NULL,                               \n \
      identity_name     BLOB NOT NULL,                               \n \
      key_identifier    BLOB NOT NULL,                               \n \
      not_before        TIMESTAMP,                                   \n \
      not_after         TIMESTAMP,                                   \n \
      certificate_data  BLOB NOT NULL,                               \n \
      valid_flag        INTEGER DEFAULT 1,                           \n \
      default_cert      INTEGER DEFAULT 0,                           \n \
                                                                     \
      PRIMARY KEY (cert_name)                                        \n \
  );                                                                 \n \
                                                                     \
CREATE INDEX cert_index ON Certificate(cert_name);           \n \
CREATE INDEX subject ON Certificate(identity_name);          \n \
";

/**
 * A utility function to call the normal sqlite3_bind_text where the value and length are
 * value.c_str() and value.size().
 */
static int sqlite3_bind_text(sqlite3_stmt* statement,
                             int index,
                             const string& value,
                             void(*destructor)(void*))
{
  return sqlite3_bind_text(statement, index, value.c_str(), value.size(), destructor);
}

SecPublicInfoSqlite3::SecPublicInfoSqlite3()
{
  boost::filesystem::path identityDir = boost::filesystem::path(getenv("HOME")) / ".ndn";
  boost::filesystem::create_directories (identityDir);

  /// @todo Add define for windows/unix in wscript. The following may completely fail on windows
  int res = sqlite3_open_v2((identityDir / "ndnsec-public-info.db").c_str(), &m_database,
                            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
#ifdef NDN_CXX_DISABLE_SQLITE3_FS_LOCKING
                                                       "unix-dotfile"
#else
                            0
#endif
                            );
  if (res != SQLITE_OK)
    throw Error("identity DB cannot be opened/created");

  //Check if Key table exists;
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT name FROM sqlite_master WHERE type='table' And name='Identity'",
                     -1, &statement, 0);
  res = sqlite3_step(statement);

  bool idTableExists = false;
  if (res == SQLITE_ROW)
    idTableExists = true;

  sqlite3_finalize(statement);

  if (!idTableExists) {
    char* errorMessage = 0;
    res = sqlite3_exec(m_database, INIT_ID_TABLE.c_str(), NULL, NULL, &errorMessage);

    if (res != SQLITE_OK && errorMessage != 0) {
      sqlite3_free(errorMessage);
    }
  }

  //Check if Key table exists;
  sqlite3_prepare_v2(m_database,
                     "SELECT name FROM sqlite_master WHERE type='table' And name='Key'",
                     -1, &statement, 0);
  res = sqlite3_step(statement);

  bool keyTableExists = false;
  if (res == SQLITE_ROW)
    keyTableExists = true;

  sqlite3_finalize(statement);

  if (!keyTableExists) {
    char* errorMessage = 0;
    res = sqlite3_exec(m_database, INIT_KEY_TABLE.c_str(), NULL, NULL, &errorMessage);

    if (res != SQLITE_OK && errorMessage != 0) {
      sqlite3_free(errorMessage);
    }
  }

  //Check if Certificate table exists;
  sqlite3_prepare_v2(m_database,
                     "SELECT name FROM sqlite_master WHERE type='table' And name='Certificate'",
                     -1, &statement, 0);
  res = sqlite3_step(statement);

  bool idCertificateTableExists = false;
  if (res == SQLITE_ROW)
    idCertificateTableExists = true;

  sqlite3_finalize(statement);

  if (!idCertificateTableExists) {
    char* errorMessage = 0;
    res = sqlite3_exec(m_database, INIT_CERT_TABLE.c_str(), NULL, NULL, &errorMessage);

    if (res != SQLITE_OK && errorMessage != 0) {
      sqlite3_free(errorMessage);
    }
  }
}

SecPublicInfoSqlite3::~SecPublicInfoSqlite3()
{
}

bool
SecPublicInfoSqlite3::doesIdentityExist(const Name& identityName)
{
  bool result = false;

  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT count(*) FROM Identity WHERE identity_name=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  int res = sqlite3_step(statement);

  if (res == SQLITE_ROW) {
    int countAll = sqlite3_column_int(statement, 0);
    if (countAll > 0)
      result = true;
  }

  sqlite3_finalize(statement);

  return result;
}

void
SecPublicInfoSqlite3::addIdentity(const Name& identityName)
{
  if (doesIdentityExist(identityName))
    return;

  sqlite3_stmt* statement;

  sqlite3_prepare_v2(m_database,
                     "INSERT OR REPLACE INTO Identity (identity_name) values (?)",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

bool
SecPublicInfoSqlite3::revokeIdentity()
{
  //TODO:
  return false;
}

bool
SecPublicInfoSqlite3::doesPublicKeyExist(const Name& keyName)
{
  if (keyName.empty())
    throw Error("Incorrect key name " + keyName.toUri());

  string keyId = keyName.get(-1).toUri();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT count(*) FROM Key WHERE identity_name=? AND key_identifier=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  bool keyIdExist = false;
  if (res == SQLITE_ROW) {
    int countAll = sqlite3_column_int(statement, 0);
    if (countAll > 0)
      keyIdExist = true;
  }

  sqlite3_finalize(statement);

  return keyIdExist;
}

void
SecPublicInfoSqlite3::addPublicKey(const Name& keyName,
                                   KeyType keyType,
                                   const PublicKey& publicKeyDer)
{
  if (keyName.empty())
    return;

  if (doesPublicKeyExist(keyName))
    return;

  string keyId = keyName.get(-1).toUri();
  Name identityName = keyName.getPrefix(-1);

  addIdentity(identityName);

  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "INSERT OR REPLACE INTO Key \
                      (identity_name, key_identifier, key_type, public_key) \
                      values (?, ?, ?, ?)",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);
  sqlite3_bind_int(statement, 3, (int)keyType);
  sqlite3_bind_blob(statement, 4,
                    publicKeyDer.get().buf(),
                    publicKeyDer.get().size(),
                    SQLITE_STATIC);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

shared_ptr<PublicKey>
SecPublicInfoSqlite3::getPublicKey(const Name& keyName)
{
  if (keyName.empty())
    {
      throw Error("SecPublicInfoSqlite3::getPublicKey  Empty keyName");
    }

  string keyId = keyName.get(-1).toUri();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT public_key FROM Key WHERE identity_name=? AND key_identifier=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  shared_ptr<PublicKey> result;
  if (res == SQLITE_ROW)
    {
      result =
        make_shared<PublicKey>(static_cast<const uint8_t*>(sqlite3_column_blob(statement, 0)),
                               sqlite3_column_bytes(statement, 0));
      sqlite3_finalize(statement);
      return result;
    }
  else
    {
      sqlite3_finalize(statement);
      throw Error("SecPublicInfoSqlite3::getPublicKey  public key does not exist");
    }
}

bool
SecPublicInfoSqlite3::doesCertificateExist(const Name& certificateName)
{
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT count(*) FROM Certificate WHERE cert_name=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  bool certExist = false;
  if (res == SQLITE_ROW) {
    int countAll = sqlite3_column_int(statement, 0);
    if (countAll > 0)
      certExist = true;
  }

  sqlite3_finalize(statement);

  return certExist;
}

// void
// SecPublicInfoSqlite3::addAnyCertificate(const IdentityCertificate& certificate)
// {
//   std::string certificateName = certificate.getName().toUri();
//   Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());

//   if (keyName.empty())
//     return;

//   std::string keyId = keyName.get(-1).toUri();
//   std::string identityName = keyName.getPrefix(-1).toUri();

//   sqlite3_stmt* statement;
//   sqlite3_prepare_v2(m_database,
//                       "INSERT OR REPLACE INTO Certificate (cert_name, cert_issuer, identity_name, key_identifier, not_before, not_after, certificate_data) "
//                        "VALUES (?, ?, ?, ?, datetime(?, 'unixepoch'), datetime(?, 'unixepoch'), ?)",
//                       -1, &statement, 0);

//   sqlite3_bind_text(statement, 1, certificateName, SQLITE_STATIC);

//   try
//     {
//       SignatureSha256WithRsa signature(certificate.getSignature());
//       std::string signerName = signature.getKeyLocator().getName().toUri();

//       sqlite3_bind_text(statement, 2, signerName, SQLITE_STATIC);
//     }
//   catch (KeyLocator::Error& e)
//     {
//       return;
//     }
//   catch (SignatureSha256WithRsa::Error& e)
//     {
//       return;
//     }

//   sqlite3_bind_text(statement, 3, identityName, SQLITE_STATIC);
//   sqlite3_bind_text(statement, 4, keyId, SQLITE_STATIC);

//   // Convert from time::milliseconds to time::seconds since 1/1/1970.
// sqlite3_bind_int64(statement, 5, static_cast<sqlite3_int64>(
//                                    time::toUnixTimestamp(certificate.getNotBefore()).count()));
// sqlite3_bind_int64(statement, 6, static_cast<sqlite3_int64>(
//                                    time::toUnixTimestamp(certificate.getNotAfter()).count()));

//   sqlite3_bind_blob(statement, 7, certificate.wireEncode().wire(), certificate.wireEncode().size(), SQLITE_STATIC);

//   sqlite3_step(statement);

//   sqlite3_finalize(statement);
// }

void
SecPublicInfoSqlite3::addCertificate(const IdentityCertificate& certificate)
{
  const Name& certificateName = certificate.getName();
  // KeyName is from IdentityCertificate name, so should be qualified.
  Name keyName =
    IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());

  //HACK!!! Assume the key type is RSA, we should check more.
  addPublicKey(keyName, KEY_TYPE_RSA, certificate.getPublicKeyInfo());

  if (doesCertificateExist(certificateName))
    return;

  string keyId = keyName.get(-1).toUri();
  Name identity = keyName.getPrefix(-1);

  // Insert the certificate
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "INSERT OR REPLACE INTO Certificate \
                      (cert_name, cert_issuer, identity_name, key_identifier, \
                       not_before, not_after, certificate_data) \
                      values (?, ?, ?, ?, datetime(?, 'unixepoch'), datetime(?, 'unixepoch'), ?)",
                      -1, &statement, 0);

  sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);

  try
    {
      // this will throw an exception if the signature is not the standard one
      // or there is no key locator present
      SignatureSha256WithRsa signature(certificate.getSignature());
      std::string signerName = signature.getKeyLocator().getName().toUri();

      sqlite3_bind_text(statement, 2, signerName, SQLITE_STATIC);
    }
  catch (std::runtime_error& e)
    {
      return;
    }

  sqlite3_bind_text(statement, 3, identity.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 4, keyId, SQLITE_STATIC);

  sqlite3_bind_int64(statement, 5,
                     static_cast<sqlite3_int64>(
                       time::toUnixTimestamp(certificate.getNotBefore()).count()));
  sqlite3_bind_int64(statement, 6,
                     static_cast<sqlite3_int64>(
                       time::toUnixTimestamp(certificate.getNotAfter()).count()));

  sqlite3_bind_blob(statement, 7,
                    certificate.wireEncode().wire(),
                    certificate.wireEncode().size(),
                    SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

shared_ptr<IdentityCertificate>
SecPublicInfoSqlite3::getCertificate(const Name& certificateName)
{
  sqlite3_stmt* statement;

  sqlite3_prepare_v2(m_database,
                     "SELECT certificate_data FROM Certificate WHERE cert_name=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  if (res == SQLITE_ROW)
    {
      shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();
      certificate->wireDecode(Block(static_cast<const uint8_t*>(sqlite3_column_blob(statement, 0)),
                                    sqlite3_column_bytes(statement, 0)));
      sqlite3_finalize(statement);
      return certificate;
    }
  else
    {
      sqlite3_finalize(statement);
      throw Error("SecPublicInfoSqlite3::getCertificate  certificate does not exist");
    }
}


Name
SecPublicInfoSqlite3::getDefaultIdentity()
{
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT identity_name FROM Identity WHERE default_identity=1",
                     -1, &statement, 0);

  int res = sqlite3_step(statement);

  if (res == SQLITE_ROW)
    {
      Name identity(string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)),
                           sqlite3_column_bytes(statement, 0)));
      sqlite3_finalize(statement);
      return identity;
    }
  else
    {
      sqlite3_finalize(statement);
      throw Error("SecPublicInfoSqlite3::getDefaultIdentity  no default identity");
    }
}

void
SecPublicInfoSqlite3::setDefaultIdentityInternal(const Name& identityName)
{
  addIdentity(identityName);

  sqlite3_stmt* statement;

  //Reset previous default identity
  sqlite3_prepare_v2(m_database,
                     "UPDATE Identity SET default_identity=0 WHERE default_identity=1",
                     -1, &statement, 0);

  while (sqlite3_step(statement) == SQLITE_ROW)
    {
    }

  sqlite3_finalize(statement);

  //Set current default identity
  sqlite3_prepare_v2(m_database,
                     "UPDATE Identity SET default_identity=1 WHERE identity_name=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

Name
SecPublicInfoSqlite3::getDefaultKeyNameForIdentity(const Name& identityName)
{
  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT key_identifier FROM Key WHERE identity_name=? AND default_key=1",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  if (res == SQLITE_ROW)
    {
      Name keyName = identityName;
      keyName.append(string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)),
                            sqlite3_column_bytes(statement, 0)));
      sqlite3_finalize(statement);
      return keyName;
    }
  else
    {
      sqlite3_finalize(statement);
      throw Error("SecPublicInfoSqlite3::getDefaultKeyNameForIdentity key not found");
    }
}

void
SecPublicInfoSqlite3::setDefaultKeyNameForIdentityInternal(const Name& keyName)
{
  if (!doesPublicKeyExist(keyName))
    throw Error("Key does not exist:" + keyName.toUri());

  string keyId = keyName.get(-1).toUri();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt* statement;

  //Reset previous default Key
  sqlite3_prepare_v2(m_database,
                     "UPDATE Key SET default_key=0 WHERE default_key=1 and identity_name=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);

  while (sqlite3_step(statement) == SQLITE_ROW)
    {
    }

  sqlite3_finalize(statement);

  //Set current default Key
  sqlite3_prepare_v2(m_database,
                     "UPDATE Key SET default_key=1 WHERE identity_name=? AND key_identifier=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

Name
SecPublicInfoSqlite3::getDefaultCertificateNameForKey(const Name& keyName)
{
  if (keyName.empty())
    throw Error("SecPublicInfoSqlite3::getDefaultCertificateNameForKey wrong key");

  string keyId = keyName.get(-1).toUri();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt* statement;
  sqlite3_prepare_v2(m_database,
                     "SELECT cert_name FROM Certificate \
                      WHERE identity_name=? AND key_identifier=? AND default_cert=1",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  if (res == SQLITE_ROW)
    {
      Name certName(string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)),
                           sqlite3_column_bytes(statement, 0)));
      sqlite3_finalize(statement);
      return certName;
    }
  else
    {
      sqlite3_finalize(statement);
      throw Error("certificate not found");
    }
}

void
SecPublicInfoSqlite3::setDefaultCertificateNameForKeyInternal(const Name& certificateName)
{
  if (!doesCertificateExist(certificateName))
    throw Error("certificate does not exist:" + certificateName.toUri());

  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);
  string keyId = keyName.get(-1).toUri();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt* statement;

  //Reset previous default Key
  sqlite3_prepare_v2(m_database,
                     "UPDATE Certificate SET default_cert=0 \
                      WHERE default_cert=1 AND identity_name=? AND key_identifier=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  while (sqlite3_step(statement) == SQLITE_ROW)
    {
    }

  sqlite3_finalize(statement);

  //Set current default Key
  sqlite3_prepare_v2(m_database,
                     "UPDATE Certificate SET default_cert=1 \
                      WHERE identity_name=? AND key_identifier=? AND cert_name=?",
                     -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, certificateName.toUri(), SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

void
SecPublicInfoSqlite3::getAllIdentities(vector<Name>& nameList, bool isDefault)
{
  sqlite3_stmt* stmt;
  if (isDefault)
    sqlite3_prepare_v2(m_database,
                       "SELECT identity_name FROM Identity WHERE default_identity=1",
                       -1, &stmt, 0);
  else
    sqlite3_prepare_v2(m_database,
                       "SELECT identity_name FROM Identity WHERE default_identity=0",
                       -1, &stmt, 0);

  while (sqlite3_step(stmt) == SQLITE_ROW)
    nameList.push_back(Name(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)),
                                   sqlite3_column_bytes(stmt, 0))));

  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::getAllKeyNames(vector<Name>& nameList, bool isDefault)
{
  sqlite3_stmt* stmt;

  if (isDefault)
    sqlite3_prepare_v2(m_database,
                       "SELECT identity_name, key_identifier FROM Key WHERE default_key=1",
                       -1, &stmt, 0);
  else
    sqlite3_prepare_v2(m_database,
                       "SELECT identity_name, key_identifier FROM Key WHERE default_key=0",
                       -1, &stmt, 0);

  while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      Name keyName(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)),
                          sqlite3_column_bytes(stmt, 0)));
      keyName.append(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)),
                            sqlite3_column_bytes(stmt, 1)));
      nameList.push_back(keyName);
    }
  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::getAllKeyNamesOfIdentity(const Name& identity,
                                               vector<Name>& nameList,
                                               bool isDefault)
{
  sqlite3_stmt* stmt;

  if (isDefault)
    sqlite3_prepare_v2(m_database,
                       "SELECT key_identifier FROM Key WHERE default_key=1 and identity_name=?",
                       -1, &stmt, 0);
  else
    sqlite3_prepare_v2(m_database,
                       "SELECT key_identifier FROM Key WHERE default_key=0 and identity_name=?",
                       -1, &stmt, 0);

  sqlite3_bind_text(stmt, 1,
                    identity.toUri().c_str(),
                    identity.toUri().size(),
                    SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      Name keyName(identity);
      keyName.append(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)),
                            sqlite3_column_bytes(stmt, 0)));
      nameList.push_back(keyName);
    }
  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::getAllCertificateNames(vector<Name>& nameList, bool isDefault)
{
  sqlite3_stmt* stmt;

  if (isDefault)
    sqlite3_prepare_v2(m_database,
                       "SELECT cert_name FROM Certificate WHERE default_cert=1",
                       -1, &stmt, 0);
  else
    sqlite3_prepare_v2(m_database,
                       "SELECT cert_name FROM Certificate WHERE default_cert=0",
                       -1, &stmt, 0);

  while (sqlite3_step(stmt) == SQLITE_ROW)
    nameList.push_back(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)),
                              sqlite3_column_bytes(stmt, 0)));

  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::getAllCertificateNamesOfKey(const Name& keyName,
                                                  vector<Name>& nameList,
                                                  bool isDefault)
{
  if (keyName.empty())
    return;

  sqlite3_stmt* stmt;
  if (isDefault)
    sqlite3_prepare_v2(m_database,
                       "SELECT cert_name FROM Certificate \
                        WHERE default_cert=1 and identity_name=? and key_identifier=?",
                       -1, &stmt, 0);
  else
    sqlite3_prepare_v2(m_database,
                       "SELECT cert_name FROM Certificate \
                        WHERE default_cert=0 and identity_name=? and key_identifier=?",
                       -1, &stmt, 0);

  Name identity = keyName.getPrefix(-1);
  sqlite3_bind_text(stmt, 1, identity.toUri().c_str(), identity.toUri().size (), SQLITE_TRANSIENT);

  std::string baseKeyName = keyName.get(-1).toUri();
  sqlite3_bind_text(stmt, 2, baseKeyName.c_str(), baseKeyName.size(), SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW)
    nameList.push_back(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)),
                              sqlite3_column_bytes(stmt, 0)));

  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::deleteCertificateInfo(const Name& certName)
{
  if (certName.empty())
    return;

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(m_database, "DELETE FROM Certificate WHERE cert_name=?", -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, certName.toUri().c_str(), certName.toUri().size (), SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::deletePublicKeyInfo(const Name& keyName)
{
  if (keyName.empty())
    return;

  string identity = keyName.getPrefix(-1).toUri();
  string keyId = keyName.get(-1).toUri();

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(m_database,
                     "DELETE FROM Certificate WHERE identity_name=? and key_identifier=?",
                     -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, identity.c_str(), identity.size (), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, keyId.c_str(), keyId.size(), SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  sqlite3_prepare_v2(m_database,
                     "DELETE FROM Key WHERE identity_name=? and key_identifier=?",
                     -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, identity.c_str(), identity.size (), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, keyId.c_str(), keyId.size(), SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void
SecPublicInfoSqlite3::deleteIdentityInfo(const Name& identityName)
{
  string identity = identityName.toUri();

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(m_database, "DELETE FROM Certificate WHERE identity_name=?", -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, identity.c_str(), identity.size (), SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  sqlite3_prepare_v2(m_database, "DELETE FROM Key WHERE identity_name=?", -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, identity.c_str(), identity.size (), SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  sqlite3_prepare_v2(m_database, "DELETE FROM Identity WHERE identity_name=?", -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, identity.c_str(), identity.size (), SQLITE_TRANSIENT);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

} // namespace ndn
