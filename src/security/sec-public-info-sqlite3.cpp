/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// Only compile if ndn-cpp-config.h defines NDN_CPP_HAVE_SQLITE3.
#include <ndn-cpp/ndn-cpp-config.h>
#ifdef NDN_CPP_HAVE_SQLITE3

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "../util/logging.hpp"
#include "../c/util/time.h"
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/security/identity-certificate.hpp>
#include <ndn-cpp/security/sec-public-info-sqlite3.hpp>
#include <ndn-cpp/security/signature-sha256-with-rsa.hpp>


INIT_LOGGER("BasicKeyMetaInfo");

using namespace std;

namespace ndn
{

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
 * A utility function to call the normal sqlite3_bind_text where the value and length are value.c_str() and value.size().
 */
static int sqlite3_bind_text(sqlite3_stmt* statement, int index, const string& value, void(*destructor)(void*))
{
  return sqlite3_bind_text(statement, index, value.c_str(), value.size(), destructor);
}

SecPublicInfoSqlite3::SecPublicInfoSqlite3()
{
  // Note: We don't use <filesystem> support because it is not "header-only" and require linking to libraries.
  // TODO: Handle non-unix file system paths which don't use '/'.
  const char* home = getenv("HOME");
  if (!home || *home == '\0')
    // Don't expect this to happen;
    home = ".";
  string homeDir(home);
  if (homeDir[homeDir.size() - 1] == '/')
    // Strip the ending '/'.
    homeDir.erase(homeDir.size() - 1);
  
  string identityDir = homeDir + '/' + ".ndnx";
  // TODO: Handle non-unix file systems which don't have "mkdir -p".
  ::system(("mkdir -p " + identityDir).c_str());
  
  int res = sqlite3_open((identityDir + '/' + "ndnsec-identity.db").c_str(), &database_);

  if (res != SQLITE_OK)
    throw Error("identity DB cannot be opened/created");
  
  //Check if Key table exists;
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT name FROM sqlite_master WHERE type='table' And name='Identity'", -1, &statement, 0);
  res = sqlite3_step(statement);

  bool idTableExists = false;
  if (res == SQLITE_ROW)
    idTableExists = true;

  sqlite3_finalize(statement);

  if (!idTableExists) {
    char *errorMessage = 0;
    res = sqlite3_exec(database_, INIT_ID_TABLE.c_str(), NULL, NULL, &errorMessage);
      
    if (res != SQLITE_OK && errorMessage != 0) {
      _LOG_TRACE("Init \"error\" in Identity: " << errorMessage);
      sqlite3_free(errorMessage);
    }
  }

  //Check if Key table exists;
  sqlite3_prepare_v2(database_, "SELECT name FROM sqlite_master WHERE type='table' And name='Key'", -1, &statement, 0);
  res = sqlite3_step(statement);

  bool keyTableExists = false;
  if (res == SQLITE_ROW)
    keyTableExists = true;

  sqlite3_finalize(statement);

  if (!keyTableExists) {
    char *errorMessage = 0;
    res = sqlite3_exec(database_, INIT_KEY_TABLE.c_str(), NULL, NULL, &errorMessage);
      
    if (res != SQLITE_OK && errorMessage != 0) {
      _LOG_TRACE("Init \"error\" in KEY: " << errorMessage);
      sqlite3_free(errorMessage);
    }
  }

  //Check if Certificate table exists;
  sqlite3_prepare_v2(database_, "SELECT name FROM sqlite_master WHERE type='table' And name='Certificate'", -1, &statement, 0);
  res = sqlite3_step(statement);

  bool idCertificateTableExists = false;
  if (res == SQLITE_ROW)
    idCertificateTableExists = true;
  
  sqlite3_finalize(statement);

  if (!idCertificateTableExists) {
    char *errorMessage = 0;
    res = sqlite3_exec(database_, INIT_CERT_TABLE.c_str(), NULL, NULL, &errorMessage);
      
    if (res != SQLITE_OK && errorMessage != 0) {
      _LOG_TRACE("Init \"error\" in ID-CERT: " << errorMessage);
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
  
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT count(*) FROM Identity WHERE identity_name=?", -1, &statement, 0);

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
    throw Error("Identity already exists");

  sqlite3_stmt *statement;

  sqlite3_prepare_v2(database_, "INSERT INTO Identity (identity_name) values (?)", -1, &statement, 0);
      
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
  if(keyName.empty())
    throw Error("Incorrect key name " + keyName.toUri());

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT count(*) FROM Key WHERE identity_name=? AND key_identifier=?", -1, &statement, 0);

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
SecPublicInfoSqlite3::addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKeyDer)
{
  if(keyName.empty())
    throw Error("Incorrect key name " + keyName.toUri());

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);


  if (!doesIdentityExist(identityName))
    addIdentity(identityName);

  if (doesPublicKeyExist(keyName))
    throw Error("a key with the same name already exists!");

  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "INSERT INTO Key (identity_name, key_identifier, key_type, public_key) values (?, ?, ?, ?)", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);
  sqlite3_bind_int(statement, 3, (int)keyType);
  sqlite3_bind_blob(statement, 4, publicKeyDer.get().buf(), publicKeyDer.get().size(), SQLITE_STATIC);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

ptr_lib::shared_ptr<PublicKey>
SecPublicInfoSqlite3::getPublicKey(const Name& keyName)
{
  if (!doesPublicKeyExist(keyName)) {
    _LOG_DEBUG("keyName does not exist");
    return ptr_lib::shared_ptr<PublicKey>();
  }

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);
  
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT public_key FROM Key WHERE identity_name=? AND key_identifier=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  ptr_lib::shared_ptr<PublicKey> result;
  if (res == SQLITE_ROW)
    result = ptr_lib::make_shared<PublicKey>(static_cast<const uint8_t*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0));

  sqlite3_finalize(statement);

  return result;
}

void 
SecPublicInfoSqlite3::updateKeyStatus(const Name& keyName, bool isActive)
{
  if(keyName.empty())
    throw Error("Incorrect key name " + keyName.toUri());

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);
  
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "UPDATE Key SET active=? WHERE identity_name=? AND key_identifier=?", -1, &statement, 0);

  sqlite3_bind_int(statement, 1, (isActive ? 1 : 0));
  sqlite3_bind_text(statement, 2, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, keyId, SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

bool
SecPublicInfoSqlite3::doesCertificateExist(const Name& certificateName)
{
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT count(*) FROM Certificate WHERE cert_name=?", -1, &statement, 0);

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

void
SecPublicInfoSqlite3::addAnyCertificate(const IdentityCertificate& certificate)
{
  std::string certificateName = certificate.getName().toUri();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());

  if(keyName.empty())
    throw Error("Incorrect key name " + keyName.toUri());

  std::string keyId = keyName.get(-1).toEscapedString();
  std::string identityName = keyName.getPrefix(-1).toUri();

  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, 
                      "INSERT INTO Certificate (cert_name, cert_issuer, identity_name, key_identifier, not_before, not_after, certificate_data)\
                       values (?, ?, ?, ?, datetime(?, 'unixepoch'), datetime(?, 'unixepoch'), ?)",
                      -1, &statement, 0);

  
  _LOG_DEBUG("certName: " << certificateName);
  sqlite3_bind_text(statement, 1, certificateName, SQLITE_STATIC);

  // this will throw an exception if the signature is not the standard one or there is no key locator present
  SignatureSha256WithRsa signature(certificate.getSignature());
  std::string signerName = signature.getKeyLocator().getName().toUri();

  sqlite3_bind_text(statement, 2, signerName, SQLITE_STATIC);

  sqlite3_bind_text(statement, 3, identityName, SQLITE_STATIC);
  sqlite3_bind_text(statement, 4, keyId, SQLITE_STATIC);

  // Convert from milliseconds to seconds since 1/1/1970.
  sqlite3_bind_int64(statement, 5, static_cast<sqlite3_int64>(certificate.getNotBefore() / 1000));
  sqlite3_bind_int64(statement, 6, static_cast<sqlite3_int64>(certificate.getNotAfter() / 1000));

  sqlite3_bind_blob(statement, 7, certificate.wireEncode().wire(), certificate.wireEncode().size(), SQLITE_STATIC);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

void 
SecPublicInfoSqlite3::addCertificate(const IdentityCertificate& certificate)
{
  const Name& certificateName = certificate.getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());

  if (!doesPublicKeyExist(keyName))
    throw Error("No corresponding Key record for certificate!" + keyName.toUri() + " " + certificateName.toUri());

  // Check if certificate has already existed!
  if (doesCertificateExist(certificateName))
    throw Error("Certificate has already been installed!");

  string keyId = keyName.get(-1).toEscapedString();
  Name identity = keyName.getPrefix(-1);
  
  // Check if the public key of certificate is the same as the key record
 
  ptr_lib::shared_ptr<PublicKey> pubKey = getPublicKey(keyName);
  
  if (!pubKey || (*pubKey) != certificate.getPublicKeyInfo())
    throw Error("Certificate does not match the public key!");

  // Insert the certificate
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, 
                      "INSERT INTO Certificate (cert_name, cert_issuer, identity_name, key_identifier, not_before, not_after, certificate_data)\
                       values (?, ?, ?, ?, datetime(?, 'unixepoch'), datetime(?, 'unixepoch'), ?)",
                      -1, &statement, 0);

  _LOG_DEBUG("certName: " << certificateName.toUri());
  sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);

  // this will throw an exception if the signature is not the standard one or there is no key locator present
  SignatureSha256WithRsa signature(certificate.getSignature());
  std::string signerName = signature.getKeyLocator().getName().toUri();

  sqlite3_bind_text(statement, 2, signerName, SQLITE_STATIC);

  sqlite3_bind_text(statement, 3, identity.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 4, keyId, SQLITE_STATIC);

  // Convert from milliseconds to seconds since 1/1/1970.
  sqlite3_bind_int64(statement, 5, static_cast<sqlite3_int64>(certificate.getNotBefore() / 1000));
  sqlite3_bind_int64(statement, 6, static_cast<sqlite3_int64>(certificate.getNotAfter() / 1000));

  sqlite3_bind_blob(statement, 7, certificate.wireEncode().wire(), certificate.wireEncode().size(), SQLITE_TRANSIENT);

  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

ptr_lib::shared_ptr<IdentityCertificate> 
SecPublicInfoSqlite3::getCertificate(const Name &certificateName)
{
  if (doesCertificateExist(certificateName)) {
    sqlite3_stmt *statement;

    sqlite3_prepare_v2(database_, 
                       "SELECT certificate_data FROM Certificate \
                        WHERE cert_name=? AND not_before<datetime('now') AND not_after>datetime('now') and valid_flag=1",
                       -1, &statement, 0);
          
    sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);
      
    int res = sqlite3_step(statement);
      
    ptr_lib::shared_ptr<IdentityCertificate> certificate = ptr_lib::make_shared<IdentityCertificate>();
    if (res == SQLITE_ROW)
      {
        certificate->wireDecode(Block((const uint8_t*)sqlite3_column_blob(statement, 0), sqlite3_column_bytes(statement, 0)));
      }
    sqlite3_finalize(statement);
      
    return certificate;
  }
  else {
    _LOG_DEBUG("Certificate does not exist!");
    return ptr_lib::shared_ptr<IdentityCertificate>();
  }
}

Name 
SecPublicInfoSqlite3::getDefaultIdentity()
{
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT identity_name FROM Identity WHERE default_identity=1", -1, &statement, 0);

  int res = sqlite3_step(statement);
      
  Name identity;

  if (res == SQLITE_ROW)
    identity = Name(string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)), sqlite3_column_bytes(statement, 0)));
 
  sqlite3_finalize(statement);
      
  return identity;
}

void 
SecPublicInfoSqlite3::setDefaultIdentityInternal(const Name& identityName)
{
  sqlite3_stmt *statement;

  //Reset previous default identity
  sqlite3_prepare_v2(database_, "UPDATE Identity SET default_identity=0 WHERE default_identity=1", -1, &statement, 0);

  while (sqlite3_step(statement) == SQLITE_ROW)
    {}
  
  sqlite3_finalize(statement);

  //Set current default identity
  sqlite3_prepare_v2(database_, "UPDATE Identity SET default_identity=1 WHERE identity_name=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  
  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

Name 
SecPublicInfoSqlite3::getDefaultKeyNameForIdentity(const Name& identityName)
{
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT key_identifier FROM Key WHERE identity_name=? AND default_key=1", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);
      
  Name keyName;

  if (res == SQLITE_ROW)
    keyName = Name(identityName).append(string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)), sqlite3_column_bytes(statement, 0)));
 
  sqlite3_finalize(statement);
      
  return keyName;
}

void 
SecPublicInfoSqlite3::setDefaultKeyNameForIdentityInternal(const Name& keyName)
{
  if(keyName.empty())
    throw Error("Incorrect key name " + keyName.toUri());

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt *statement;

  //Reset previous default Key
  sqlite3_prepare_v2(database_, "UPDATE Key SET default_key=0 WHERE default_key=1 and identity_name=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);

  while (sqlite3_step(statement) == SQLITE_ROW)
    {}
  
  sqlite3_finalize(statement);

  //Set current default Key
  sqlite3_prepare_v2(database_, "UPDATE Key SET default_key=1 WHERE identity_name=? AND key_identifier=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);
  
  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

Name 
SecPublicInfoSqlite3::getDefaultCertificateNameForKey(const Name& keyName)
{
  if(keyName.empty())
    return Name();

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT cert_name FROM Certificate WHERE identity_name=? AND key_identifier=? AND default_cert=1", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  Name certName;

  if (res == SQLITE_ROW)
    certName = Name(string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)), sqlite3_column_bytes(statement, 0)));
 
  sqlite3_finalize(statement);
      
  return certName;
}

void 
SecPublicInfoSqlite3::setDefaultCertificateNameForKeyInternal(const Name& certificateName)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificateName);
  if(keyName.empty())
    throw Error("Incorrect key name for certificate " + certificateName.toUri());

  string keyId = keyName.get(-1).toEscapedString();
  Name identityName = keyName.getPrefix(-1);

  sqlite3_stmt *statement;

  //Reset previous default Key
  sqlite3_prepare_v2(database_, "UPDATE Certificate SET default_cert=0 WHERE default_cert=1 AND identity_name=? AND key_identifier=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  while (sqlite3_step(statement) == SQLITE_ROW)
    {}
  
  sqlite3_finalize(statement);

  //Set current default Key
  sqlite3_prepare_v2(database_, "UPDATE Certificate SET default_cert=1 WHERE identity_name=? AND key_identifier=? AND cert_name=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, certificateName.toUri(), SQLITE_TRANSIENT);
  
  sqlite3_step(statement);

  sqlite3_finalize(statement);
}

vector<Name>
SecPublicInfoSqlite3::getAllIdentities(bool isDefault)
{
  sqlite3_stmt *stmt;
  if(isDefault)
    sqlite3_prepare_v2 (database_, "SELECT identity_name FROM Identity WHERE default_identity=1", -1, &stmt, 0);
  else
    sqlite3_prepare_v2 (database_, "SELECT identity_name FROM Identity WHERE default_identity=0", -1, &stmt, 0);

  vector<Name> nameList;
  while(sqlite3_step (stmt) == SQLITE_ROW)
    nameList.push_back(Name(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)), sqlite3_column_bytes (stmt, 0))));
        
  sqlite3_finalize (stmt);        
  return nameList;
}

vector<Name>
SecPublicInfoSqlite3::getAllKeyNames(bool isDefault)
{
  sqlite3_stmt *stmt;
  if(isDefault)
    sqlite3_prepare_v2 (database_, "SELECT identity_name, key_identifier FROM Key WHERE default_key=1", -1, &stmt, 0);
  else
    sqlite3_prepare_v2 (database_, "SELECT identity_name, key_identifier FROM Key WHERE default_key=0", -1, &stmt, 0);

  vector<Name> nameList;
  while(sqlite3_step (stmt) == SQLITE_ROW)
    {
      Name keyName(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)), sqlite3_column_bytes (stmt, 0)));
      keyName.append(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)), sqlite3_column_bytes (stmt, 1)));
      nameList.push_back(keyName);
    } 
  sqlite3_finalize (stmt);        
  return nameList;
}

vector<Name>
SecPublicInfoSqlite3::getAllKeyNamesOfIdentity(const Name& identity, bool isDefault)
{
  sqlite3_stmt *stmt;
  if(isDefault)
    sqlite3_prepare_v2 (database_, "SELECT key_identifier FROM Key WHERE default_key=1 and identity_name=?", -1, &stmt, 0);
  else
    sqlite3_prepare_v2 (database_, "SELECT key_identifier FROM Key WHERE default_key=0 and identity_name=?", -1, &stmt, 0);
    
  sqlite3_bind_text(stmt, 1, identity.toUri().c_str(),  identity.toUri().size (),  SQLITE_TRANSIENT);

  vector<Name> nameList;
  while(sqlite3_step (stmt) == SQLITE_ROW)
    {
      Name keyName(identity);
      keyName.append(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)), sqlite3_column_bytes (stmt, 0)));
      nameList.push_back(keyName);
    } 
  sqlite3_finalize (stmt);        
  return nameList;
}
    
vector<Name>
SecPublicInfoSqlite3::getAllCertificateNames(bool isDefault)
{
  sqlite3_stmt *stmt;
  if(isDefault)
    sqlite3_prepare_v2 (database_, "SELECT cert_name FROM Certificate WHERE default_cert=1", -1, &stmt, 0);
  else
    sqlite3_prepare_v2 (database_, "SELECT cert_name FROM Certificate WHERE default_cert=0", -1, &stmt, 0);

  vector<Name> nameList;
  while(sqlite3_step (stmt) == SQLITE_ROW)
    nameList.push_back(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)), sqlite3_column_bytes (stmt, 0)));

  sqlite3_finalize (stmt);        
  return nameList;
}

vector<Name>
SecPublicInfoSqlite3::getAllCertificateNamesOfKey(const Name& keyName, bool isDefault)
{
  if(keyName.empty())
    return vector<Name>();

  sqlite3_stmt *stmt;
  if(isDefault)
    sqlite3_prepare_v2 (database_, "SELECT cert_name FROM Certificate WHERE default_cert=1 and identity_name=? and key_identifier=?", -1, &stmt, 0);
  else
    sqlite3_prepare_v2 (database_, "SELECT cert_name FROM Certificate WHERE default_cert=0 and identity_name=? and key_identifier=?", -1, &stmt, 0);

  Name identity = keyName.getPrefix(-1);
  sqlite3_bind_text(stmt, 1, identity.toUri().c_str(),  identity.toUri().size (),  SQLITE_TRANSIENT);
  std::string baseKeyName = keyName.get(-1).toEscapedString();
  sqlite3_bind_text(stmt, 2, baseKeyName.c_str(), baseKeyName.size(), SQLITE_TRANSIENT);

  vector<Name> nameList;
  while(sqlite3_step (stmt) == SQLITE_ROW)
    nameList.push_back(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)), sqlite3_column_bytes (stmt, 0)));

  sqlite3_finalize (stmt);        
  return nameList;
}

} // namespace ndn

#endif // NDN_CPP_HAVE_SQLITE3
