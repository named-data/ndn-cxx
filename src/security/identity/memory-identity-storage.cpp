/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "memory-identity-storage.hpp"

#include <algorithm>
#include <ndn-cpp/security/certificate/identity-certificate.hpp>

using namespace std;

namespace ndn {

MemoryIdentityStorage::~MemoryIdentityStorage()
{
}

bool 
MemoryIdentityStorage::doesIdentityExist(const Name& identityName)
{
  string identityUri = identityName.toUri();
  return find(identityStore_.begin(), identityStore_.end(), identityUri) != identityStore_.end();
}

void
MemoryIdentityStorage::addIdentity(const Name& identityName)
{
  string identityUri = identityName.toUri();
  if (find(identityStore_.begin(), identityStore_.end(), identityUri) != identityStore_.end())
    throw Error("Identity already exists: " + identityUri);
  
  identityStore_.push_back(identityUri);
}

bool 
MemoryIdentityStorage::revokeIdentity()
{
#if 1
  throw runtime_error("MemoryIdentityStorage::revokeIdentity not implemented");
#endif
}

bool 
MemoryIdentityStorage::doesKeyExist(const Name& keyName)
{
  return keyStore_.find(keyName.toUri()) != keyStore_.end();
}

void 
MemoryIdentityStorage::addKey(const Name& keyName, KeyType keyType, const PublicKey& publicKey)
{
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

  if (!doesIdentityExist(identityName))
    addIdentity(identityName);

  if (doesKeyExist(keyName))
    throw Error("a key with the same name already exists!");
  
  keyStore_[keyName.toUri()] = ptr_lib::make_shared<KeyRecord>(keyType, publicKey);
}

ptr_lib::shared_ptr<PublicKey>
MemoryIdentityStorage::getKey(const Name& keyName)
{
  KeyStore::iterator record = keyStore_.find(keyName.toUri());
  if (record == keyStore_.end())
    // Not found.  Silently return null.
    return ptr_lib::shared_ptr<PublicKey>();
  
  return ptr_lib::make_shared<PublicKey> (record->second->getKey());
}

void 
MemoryIdentityStorage::activateKey(const Name& keyName)
{
#if 1
  throw runtime_error("MemoryIdentityStorage::activateKey not implemented");
#endif
}

void 
MemoryIdentityStorage::deactivateKey(const Name& keyName)
{
#if 1
  throw runtime_error("MemoryIdentityStorage::deactivateKey not implemented");
#endif
}

bool
MemoryIdentityStorage::doesCertificateExist(const Name& certificateName)
{
  return certificateStore_.find(certificateName.toUri()) != certificateStore_.end();
}

void 
MemoryIdentityStorage::addCertificate(const IdentityCertificate& certificate)
{
  const Name& certificateName = certificate.getName();
  const Name& keyName = certificate.getPublicKeyName();

  if (!doesKeyExist(keyName))
    throw Error("No corresponding Key record for certificate! " + keyName.toUri() + " " + certificateName.toUri());

  // Check if certificate has already existed!
  if (doesCertificateExist(certificateName))
    throw Error("Certificate has already been installed!");

  // Check if the public key of certificate is the same as the key record. 
  ptr_lib::shared_ptr<PublicKey> pubKey = getKey(keyName);
  if (!pubKey || (*pubKey) != certificate.getPublicKeyInfo())
    throw Error("Certificate does not match the public key!");
  
  // Insert the certificate.
  certificateStore_[certificateName.toUri()] = ptr_lib::make_shared<IdentityCertificate> (certificate);
}

ptr_lib::shared_ptr<IdentityCertificate> 
MemoryIdentityStorage::getCertificate(const Name& certificateName, bool allowAny)
{
  CertificateStore::iterator record = certificateStore_.find(certificateName.toUri());
  if (record == certificateStore_.end())
    // Not found.  Silently return null.
    return ptr_lib::shared_ptr<IdentityCertificate>();

  return record->second;
}

Name 
MemoryIdentityStorage::getDefaultIdentity()
{
  return Name(defaultIdentity_);
}

Name 
MemoryIdentityStorage::getDefaultKeyNameForIdentity(const Name& identityName)
{
  return defaultKeyName_;
}

Name 
MemoryIdentityStorage::getDefaultCertificateNameForKey(const Name& keyName)
{
  return defaultCert_;
}

void 
MemoryIdentityStorage::setDefaultIdentity(const Name& identityName)
{
  string identityUri = identityName.toUri();
  if (find(identityStore_.begin(), identityStore_.end(), identityUri) != identityStore_.end())
    defaultIdentity_ = identityUri;
  else
    // The identity doesn't exist, so clear the default.
    defaultIdentity_.clear();
}

void 
MemoryIdentityStorage::setDefaultKeyNameForIdentity(const Name& keyName, const Name& identityNameCheck)
{
  defaultKeyName_ = identityNameCheck;
}

void 
MemoryIdentityStorage::setDefaultCertificateNameForKey(const Name& keyName, const Name& certificateName)  
{
  defaultCert_ = certificateName;
}


std::vector<Name>
MemoryIdentityStorage::getAllIdentities(bool isDefault)
{
  throw runtime_error("MemoryIdentityStorage::getAllIdentities not implemented");
}

std::vector<Name>
MemoryIdentityStorage::getAllKeyNames(bool isDefault)
{
  throw runtime_error("MemoryIdentityStorage::getAllKeyNames not implemented");
}

std::vector<Name>
MemoryIdentityStorage::getAllKeyNamesOfIdentity(const Name& identity, bool isDefault)
{
  throw runtime_error("MemoryIdentityStorage::getAllKeyNamesOfIdentity not implemented");
}
    
std::vector<Name>
MemoryIdentityStorage::getAllCertificateNames(bool isDefault)
{
  throw runtime_error("MemoryIdentityStorage::getAllCertificateNames not implemented");
}

std::vector<Name>
MemoryIdentityStorage::getAllCertificateNamesOfKey(const Name& keyName, bool isDefault)
{
  throw runtime_error("MemoryIdentityStorage::getAllCertificateNamesOfKey not implemented");
}


}
