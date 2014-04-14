/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "sec-public-info-memory.hpp"
#include "identity-certificate.hpp"

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
    return;

  identityStore_.push_back(identityUri);
}

bool
SecPublicInfoMemory::revokeIdentity()
{
#if 1
  throw Error("SecPublicInfoMemory::revokeIdentity not implemented");
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

  addIdentity(identityName);

  keyStore_[keyName.toUri()] = make_shared<KeyRecord>(keyType, publicKey);
}

shared_ptr<PublicKey>
SecPublicInfoMemory::getPublicKey(const Name& keyName)
{
  KeyStore::iterator record = keyStore_.find(keyName.toUri());
  if (record == keyStore_.end())
    throw Error("SecPublicInfoMemory::getPublicKey  " + keyName.toUri());

  return make_shared<PublicKey> (record->second->getKey());
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
  const Name& identity = keyName.getPrefix(-1);

  addIdentity(identity);
  addPublicKey(keyName, KEY_TYPE_RSA, certificate.getPublicKeyInfo());
  certificateStore_[certificateName.toUri()] = make_shared<IdentityCertificate> (certificate);
}

shared_ptr<IdentityCertificate>
SecPublicInfoMemory::getCertificate(const Name& certificateName)
{
  CertificateStore::iterator record = certificateStore_.find(certificateName.toUri());
  if (record == certificateStore_.end())
    throw Error("SecPublicInfoMemory::getCertificate  " + certificateName.toUri());

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
SecPublicInfoMemory::getAllIdentities(std::vector<Name>& nameList, bool isDefault)
{
  throw Error("SecPublicInfoMemory::getAllIdentities not implemented");
}

void
SecPublicInfoMemory::getAllKeyNames(std::vector<Name>& nameList, bool isDefault)
{
  throw Error("SecPublicInfoMemory::getAllKeyNames not implemented");
}

void
SecPublicInfoMemory::getAllKeyNamesOfIdentity(const Name& identity, std::vector<Name>& nameList, bool isDefault)
{
  throw Error("SecPublicInfoMemory::getAllKeyNamesOfIdentity not implemented");
}

void
SecPublicInfoMemory::getAllCertificateNames(std::vector<Name>& nameList, bool isDefault)
{
  throw Error("SecPublicInfoMemory::getAllCertificateNames not implemented");
}

void
SecPublicInfoMemory::getAllCertificateNamesOfKey(const Name& keyName, std::vector<Name>& nameList, bool isDefault)
{
  throw Error("SecPublicInfoMemory::getAllCertificateNamesOfKey not implemented");
}

void
SecPublicInfoMemory::deleteCertificateInfo(const Name& certName)
{
  throw Error("SecPublicInfoMemory::deleteCertificateInfo not implemented");
}

void
SecPublicInfoMemory::deletePublicKeyInfo(const Name& keyName)
{
  throw Error("SecPublicInfoMemory::deletePublicKeyInfo not implemented");
}

void
SecPublicInfoMemory::deleteIdentityInfo(const Name& identityName)
{
  throw Error("SecPublicInfoMemory::deleteIdentityInfo not implemented");
}

} // namespace ndn
