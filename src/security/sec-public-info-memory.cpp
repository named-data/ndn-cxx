/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "security/sec-public-info-memory.hpp"

#include <algorithm>
#include "security/identity-certificate.hpp"

using namespace std;

namespace ndn {

SecPublicInfoMemory::~SecPublicInfoMemory()
{
}

bool 
SecPublicInfoMemory::doesIdentityExist(const Name& identityName)
{
  string identityUri = identityName.toUri();
  return find(identityStore_.begin(), identityStore_.end(), identityUri) != identityStore_.end();
}

void
SecPublicInfoMemory::addIdentity(const Name& identityName)
{
  string identityUri = identityName.toUri();
  if (find(identityStore_.begin(), identityStore_.end(), identityUri) != identityStore_.end())
    throw Error("Identity already exists: " + identityUri);
  
  identityStore_.push_back(identityUri);
}

bool 
SecPublicInfoMemory::revokeIdentity()
{
#if 1
  throw runtime_error("SecPublicInfoMemory::revokeIdentity not implemented");
#endif
}

bool 
SecPublicInfoMemory::doesPublicKeyExist(const Name& keyName)
{
  return keyStore_.find(keyName.toUri()) != keyStore_.end();
}

void 
SecPublicInfoMemory::addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKey)
{
  Name identityName = keyName.getSubName(0, keyName.size() - 1);

  if (!doesIdentityExist(identityName))
    addIdentity(identityName);

  if (doesPublicKeyExist(keyName))
    throw Error("a key with the same name already exists!");
  
  keyStore_[keyName.toUri()] = ptr_lib::make_shared<KeyRecord>(keyType, publicKey);
}

ptr_lib::shared_ptr<PublicKey>
SecPublicInfoMemory::getPublicKey(const Name& keyName)
{
  KeyStore::iterator record = keyStore_.find(keyName.toUri());
  if (record == keyStore_.end())
    // Not found.  Silently return null.
    return ptr_lib::shared_ptr<PublicKey>();
  
  return ptr_lib::make_shared<PublicKey> (record->second->getKey());
}

void 
SecPublicInfoMemory::activatePublicKey(const Name& keyName)
{
#if 1
  throw runtime_error("SecPublicInfoMemory::activateKey not implemented");
#endif
}

void 
SecPublicInfoMemory::deactivatePublicKey(const Name& keyName)
{
#if 1
  throw runtime_error("SecPublicInfoMemory::deactivateKey not implemented");
#endif
}

bool
SecPublicInfoMemory::doesCertificateExist(const Name& certificateName)
{
  return certificateStore_.find(certificateName.toUri()) != certificateStore_.end();
}

void 
SecPublicInfoMemory::addCertificate(const IdentityCertificate& certificate)
{
  const Name& certificateName = certificate.getName();
  const Name& keyName = certificate.getPublicKeyName();

  if (!doesPublicKeyExist(keyName))
    throw Error("No corresponding Key record for certificate! " + keyName.toUri() + " " + certificateName.toUri());

  // Check if certificate has already existed!
  if (doesCertificateExist(certificateName))
    throw Error("Certificate has already been installed!");

  // Check if the public key of certificate is the same as the key record. 
  ptr_lib::shared_ptr<PublicKey> pubKey = getPublicKey(keyName);
  if (!pubKey || (*pubKey) != certificate.getPublicKeyInfo())
    throw Error("Certificate does not match the public key!");
  
  // Insert the certificate.
  certificateStore_[certificateName.toUri()] = ptr_lib::make_shared<IdentityCertificate> (certificate);
}

ptr_lib::shared_ptr<IdentityCertificate> 
SecPublicInfoMemory::getCertificate(const Name& certificateName)
{
  CertificateStore::iterator record = certificateStore_.find(certificateName.toUri());
  if (record == certificateStore_.end())
    // Not found.  Silently return null.
    return ptr_lib::shared_ptr<IdentityCertificate>();

  return record->second;
}

Name 
SecPublicInfoMemory::getDefaultIdentity()
{
  return Name(defaultIdentity_);
}

void 
SecPublicInfoMemory::setDefaultIdentityInternal(const Name& identityName)
{
  string identityUri = identityName.toUri();
  if (find(identityStore_.begin(), identityStore_.end(), identityUri) != identityStore_.end())
    defaultIdentity_ = identityUri;
  else
    // The identity doesn't exist, so clear the default.
    defaultIdentity_.clear();
}

Name 
SecPublicInfoMemory::getDefaultKeyNameForIdentity(const Name& identityName)
{
  return defaultKeyName_;
}

void 
SecPublicInfoMemory::setDefaultKeyNameForIdentityInternal(const Name& keyName)
{
  defaultKeyName_ = keyName;
}

Name 
SecPublicInfoMemory::getDefaultCertificateNameForKey(const Name& keyName)
{
  return defaultCert_;
}

void 
SecPublicInfoMemory::setDefaultCertificateNameForKeyInternal(const Name& certificateName)  
{
  defaultCert_ = certificateName;
}


void
SecPublicInfoMemory::getAllIdentities(std::vector<Name> &nameList, bool isDefault)
{
  throw runtime_error("SecPublicInfoMemory::getAllIdentities not implemented");
}

void
SecPublicInfoMemory::getAllKeyNames(std::vector<Name> &nameList, bool isDefault)
{
  throw runtime_error("SecPublicInfoMemory::getAllKeyNames not implemented");
}

void
SecPublicInfoMemory::getAllKeyNamesOfIdentity(const Name& identity, std::vector<Name> &nameList, bool isDefault)
{
  throw runtime_error("SecPublicInfoMemory::getAllKeyNamesOfIdentity not implemented");
}
    
void
SecPublicInfoMemory::getAllCertificateNames(std::vector<Name> &nameList, bool isDefault)
{
  throw runtime_error("SecPublicInfoMemory::getAllCertificateNames not implemented");
}

void
SecPublicInfoMemory::getAllCertificateNamesOfKey(const Name& keyName, std::vector<Name> &nameList, bool isDefault)
{
  throw runtime_error("SecPublicInfoMemory::getAllCertificateNamesOfKey not implemented");
}

void
SecPublicInfoMemory::deleteCertificateInfo(const Name &certName)
{
  throw runtime_error("SecPublicInfoMemory::deleteCertificateInfo not implemented");
}

void
SecPublicInfoMemory::deletePublicKeyInfo(const Name &keyName)
{
  throw runtime_error("SecPublicInfoMemory::deletePublicKeyInfo not implemented");
}

void
SecPublicInfoMemory::deleteIdentityInfo(const Name &identityName)
{
  throw runtime_error("SecPublicInfoMemory::deleteIdentityInfo not implemented");
}



}
