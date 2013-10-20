/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// Only compile if ndn-cpp-config.h defines NDN_CPP_HAVE_SQLITE3.
#include <ndn-cpp/ndn-cpp-config.h>
#ifdef NDN_CPP_HAVE_SQLITE3

#if 1
#include <stdexcept>
#endif
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <math.h>
#include <ndn-cpp/security/identity/basic-identity-storage.hpp>
#include "../../util/logging.hpp"
#include <ndn-cpp/security/security-exception.hpp>
#include "ndn-cpp/data.hpp"
#include <ndn-cpp/security/certificate/identity-certificate.hpp>
#include "../../c/util/time.h"
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#if 0
#include "ndn.cxx/regex/regex.h"
#include <boost/filesystem.hpp>
#endif

INIT_LOGGER("BasicIdentityStorage");

using namespace std;
using namespace ndn::ptr_lib;
#if 0
namespace fs = boost::filesystem;
#endif

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
      valid_flag        INTEGER DEFAULT 0,                           \n \
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

BasicIdentityStorage::BasicIdentityStorage()
{
#if 0
  fs::path identityDir = fs::path(getenv("HOME")) / ".ndn-identity";
  fs::create_directories(identityDir);
  
  int res = sqlite3_open((identityDir / "identity.db").c_str(), &database_);

  if (res != SQLITE_OK)
    {
      throw SecurityException("identity DB cannot be opened/created");
    }
#endif
  
  //Check if Key table exists;
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT name FROM sqlite_master WHERE type='table' And name='Identity'", -1, &statement, 0);
  int res = sqlite3_step(statement);

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

BasicIdentityStorage::~BasicIdentityStorage()
{
}

bool 
BasicIdentityStorage::doesIdentityExist(const Name& identityName)
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
BasicIdentityStorage::addIdentity(const Name& identityName)
{
  if (doesIdentityExist(identityName))
    throw SecurityException("Identity already exists");

  sqlite3_stmt *statement;

  sqlite3_prepare_v2(database_, "INSERT INTO Identity (identity_name) values (?)", -1, &statement, 0);
      
  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  
  int res = sqlite3_step(statement);
  
  sqlite3_finalize(statement);
}

bool 
BasicIdentityStorage::revokeIdentity()
{
  //TODO:
  return false;
}

Name 
BasicIdentityStorage::getNewKeyName(const Name& identityName, bool useKsk)
{
  MillisecondsSince1970 ti = ::ndn_getNowMilliseconds();
  // Get the number of seconds.
  ostringstream oss;
  oss << floor(ti / 1000.0);

  string keyIdStr;
  
  if (useKsk)
    keyIdStr = ("KSK-" + oss.str());
  else
    keyIdStr = ("DSK-" + oss.str());


  Name keyName = Name(identityName).append(keyIdStr);

  if (doesKeyExist(keyName))
    throw SecurityException("Key name already exists");

  return keyName;
}

bool 
BasicIdentityStorage::doesKeyExist(const Name& keyName)
{
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

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

Name 
BasicIdentityStorage::getKeyNameForCertificate(const Name& certificateName)
{
  int i = certificateName.size() - 1;

  for (; i >= 0; --i) {
    if (certificateName.get(i).toEscapedString() == string("ID-CERT"))
      break; 
  }
  
  return certificateName.getSubName(0, i);
}

void
BasicIdentityStorage::addKey(const Name& keyName, KeyType keyType, const Blob& publicKeyDer)
{
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);


  if (!doesIdentityExist(identityName))
    addIdentity(identityName);

  if (doesKeyExist(keyName))
    throw SecurityException("a key with the same name already exists!");

  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "INSERT INTO Key (identity_name, key_identifier, key_type, public_key) values (?, ?, ?, ?)", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);
  sqlite3_bind_int(statement, 3, (int)keyType);
  sqlite3_bind_blob(statement, 4, publicKeyDer.buf(), publicKeyDer.size(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  sqlite3_finalize(statement);
}

Blob
BasicIdentityStorage::getKey(const Name& keyName)
{
  if (!doesKeyExist(keyName)) {
    _LOG_DEBUG("keyName does not exist");
    return Blob();
  }

  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);
  
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "SELECT public_key FROM Key WHERE identity_name=? AND key_identifier=?", -1, &statement, 0);

  sqlite3_bind_text(statement, 1, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  Blob result;
  if (res == SQLITE_ROW)
    result = Blob(static_cast<const uint8_t*>(sqlite3_column_blob(statement, 0)), sqlite3_column_bytes(statement, 0));

  sqlite3_finalize(statement);

  return result;
}

void 
BasicIdentityStorage::activateKey(const Name& keyName)
{
  updateKeyStatus(keyName, true);
}

void 
BasicIdentityStorage::deactivateKey(const Name& keyName)
{
  updateKeyStatus(keyName, false);
}

void 
BasicIdentityStorage::updateKeyStatus(const Name& keyName, bool isActive)
{
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);
  
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, "UPDATE Key SET active=? WHERE identity_name=? AND key_identifier=?", -1, &statement, 0);

  sqlite3_bind_int(statement, 1, (isActive ? 1 : 0));
  sqlite3_bind_text(statement, 2, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, keyId, SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  sqlite3_finalize(statement);
}

bool
BasicIdentityStorage::doesCertificateExist(const Name& certificateName)
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
BasicIdentityStorage::addAnyCertificate(const IdentityCertificate& certificate)
{
  const Name& certificateName = certificate.getName();
  Name keyName = getKeyNameForCertificate(certificateName);

  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, 
                      "INSERT INTO Certificate (cert_name, cert_issuer, identity_name, key_identifier, not_before, not_after, certificate_data)\
                       values (?, ?, ?, ?, datetime(?, 'unixepoch'), datetime(?, 'unixepoch'), ?)",
                      -1, &statement, 0);

  
  _LOG_DEBUG("certName: " << certificateName.toUri().c_str());
  sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);

  const Sha256WithRsaSignature* signature = dynamic_cast<const Sha256WithRsaSignature*>(certificate.getSignature());
  const Name& signerName = signature->getKeyLocator().getKeyName();
  sqlite3_bind_text(statement, 2, signerName.toUri(), SQLITE_TRANSIENT);

  sqlite3_bind_text(statement, 3, identityName.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 4, keyId, SQLITE_TRANSIENT);

  // Convert from milliseconds to seconds since 1/1/1970.
  sqlite3_bind_int64(statement, 5, (sqlite3_int64)floor(certificate.getNotBefore() / 1000.0));
  sqlite3_bind_int64(statement, 6, (sqlite3_int64)floor(certificate.getNotAfter() / 1000.0));

  if (!certificate.getDefaultWireEncoding())
    certificate.wireEncode();
  sqlite3_bind_blob(statement, 7, certificate.getDefaultWireEncoding().buf(), certificate.getDefaultWireEncoding().size(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  sqlite3_finalize(statement);
}

void 
BasicIdentityStorage::addCertificate(const IdentityCertificate& certificate)
{
  _LOG_DEBUG("1");
  const Name& certificateName = certificate.getName();
  Name keyName = getKeyNameForCertificate(certificateName);

  _LOG_DEBUG("2");
  if (!doesKeyExist(keyName))
    {
      _LOG_DEBUG("here wrong");
      throw SecurityException("No corresponding Key record for the certificate!");
    }

  // Check if certificate has already existed!
  if (doesCertificateExist(certificateName))
    throw SecurityException("Certificate has already been installed!");

  _LOG_DEBUG("3");
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identity = keyName.getSubName(0, keyName.size() - 1);
  
  // Check if the public key of certificate is the same as the key record
 
  Blob keyBlob = getKey(keyName);
  
  if (!keyBlob || (*keyBlob) != *(certificate.getPublicKeyInfo().getKeyDer()))
    throw SecurityException("Certificate does not match the public key!");

  _LOG_DEBUG("4");
  // Insert the certificate
  sqlite3_stmt *statement;
  sqlite3_prepare_v2(database_, 
                      "INSERT INTO Certificate (cert_name, cert_issuer, identity_name, key_identifier, not_before, not_after, certificate_data)\
                       values (?, ?, ?, ?, datetime(?, 'unixepoch'), datetime(?, 'unixepoch'), ?)",
                      -1, &statement, 0);

  _LOG_DEBUG("certName: " << certificateName.toUri().c_str());
  sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);

  const Sha256WithRsaSignature* signature = dynamic_cast<const Sha256WithRsaSignature*>(certificate.getSignature());
  const Name & signerName = signature->getKeyLocator().getKeyName();
  sqlite3_bind_text(statement, 2, signerName.toUri(), SQLITE_TRANSIENT);

  sqlite3_bind_text(statement, 3, identity.toUri(), SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 4, keyId, SQLITE_TRANSIENT);

  // Convert from milliseconds to seconds since 1/1/1970.
  sqlite3_bind_int64(statement, 5, (sqlite3_int64)floor(certificate.getNotBefore() / 1000.0));
  sqlite3_bind_int64(statement, 6, (sqlite3_int64)floor(certificate.getNotAfter() / 1000.0));

  if (!certificate.getDefaultWireEncoding())
    certificate.wireEncode();
  sqlite3_bind_blob(statement, 7, certificate.getDefaultWireEncoding().buf(), certificate.getDefaultWireEncoding().size(), SQLITE_TRANSIENT);

  int res = sqlite3_step(statement);

  sqlite3_finalize(statement);
}

shared_ptr<Data> 
BasicIdentityStorage::getCertificate(const Name &certificateName, bool allowAny)
{
  if (doesCertificateExist(certificateName)) {
    sqlite3_stmt *statement;
    if (!allowAny) {
      sqlite3_prepare_v2(database_, 
                          "SELECT certificate_data FROM Certificate \
                           WHERE cert_name=? AND not_before<datetime(?, 'unixepoch') AND not_after>datetime(?, 'unixepoch') and valid_flag=1",
                          -1, &statement, 0);
          
      sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);
      sqlite3_bind_int64(statement, 2, (sqlite3_int64)floor(ndn_getNowMilliseconds() / 1000.0));
      sqlite3_bind_int64(statement, 3, (sqlite3_int64)floor(ndn_getNowMilliseconds() / 1000.0));
    }
    else {
      sqlite3_prepare_v2(database_, 
                          "SELECT certificate_data FROM Certificate WHERE cert_name=?", -1, &statement, 0);

      sqlite3_bind_text(statement, 1, certificateName.toUri(), SQLITE_TRANSIENT);
    }
      
    int res = sqlite3_step(statement);
      
    shared_ptr<Data> data;

    if (res == SQLITE_ROW)
      data->wireDecode((const uint8_t*)sqlite3_column_blob(statement, 0), sqlite3_column_bytes(statement, 0));            
    sqlite3_finalize(statement);
      
    return data;
  }
  else {
    _LOG_DEBUG("Certificate does not exist!");
    return shared_ptr<Data>();
  }
}

Name 
BasicIdentityStorage::getDefaultIdentity()
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

Name 
BasicIdentityStorage::getDefaultKeyNameForIdentity(const Name& identityName)
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

Name 
BasicIdentityStorage::getDefaultCertificateNameForKey(const Name& keyName)
{
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

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
BasicIdentityStorage::setDefaultIdentity(const Name& identityName)
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

void 
BasicIdentityStorage::setDefaultKeyNameForIdentity(const Name& keyName, const Name& identityNameCheck)
{
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

  if (identityNameCheck.size() > 0 && !identityNameCheck.equals(identityName))
    throw SecurityException("Specified identity name does not match the key name");

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

void 
BasicIdentityStorage::setDefaultCertificateNameForKey(const Name& keyName, const Name& certificateName)
{
  string keyId = keyName.get(keyName.size() - 1).toEscapedString();
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

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
        
}

#endif // NDN_CPP_HAVE_SQLITE3
