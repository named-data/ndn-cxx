/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "memory-identity-storage.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

MemoryIdentityStorage::~MemoryIdentityStorage()
{
}

bool 
MemoryIdentityStorage::doesIdentityExist(const Name& identityName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::doesIdentityExist not implemented");
#endif
}

void
MemoryIdentityStorage::addIdentity(const Name& identityName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::addIdentity not implemented");
#endif
}

bool 
MemoryIdentityStorage::revokeIdentity()
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::revokeIdentity not implemented");
#endif
}

Name 
MemoryIdentityStorage::getNewKeyName(const Name& identityName, bool useKsk)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getNewKeyName not implemented");
#endif
}

bool 
MemoryIdentityStorage::doesKeyExist(const Name& keyName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::doesKeyExist not implemented");
#endif
}

Name 
MemoryIdentityStorage::getKeyNameForCertificate(const Name& certificateName)
{
  int i = certificateName.getComponentCount() - 1;

  for (; i >= 0; --i) {
    if(certificateName.getComponent(i).toEscapedString() == string("ID-CERT"))
      break; 
  }
    
  return certificateName.getSubName(0, i);
}

void 
MemoryIdentityStorage::addKey(const Name& keyName, KeyType keyType, Blob& publicKeyDer)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::addKey not implemented");
#endif
}

Blob
MemoryIdentityStorage::getKey(const Name& keyName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getKey not implemented");
#endif
}

void 
MemoryIdentityStorage::activateKey(const Name& keyName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::activateKey not implemented");
#endif
}

void 
MemoryIdentityStorage::deactivateKey(const Name& keyName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::deactivateKey not implemented");
#endif
}

bool
MemoryIdentityStorage::doesCertificateExist(const Name& certificateName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::doesCertificateExist not implemented");
#endif
}

void 
MemoryIdentityStorage::addCertificate(const Certificate& certificate)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::addCertificate not implemented");
#endif
}

ptr_lib::shared_ptr<Data> 
MemoryIdentityStorage::getCertificate(const Name &certificateName, bool allowAny)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getCertificate not implemented");
#endif
}

Name 
MemoryIdentityStorage::getDefaultIdentity()
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getDefaultIdentity not implemented");
#endif
}

Name 
MemoryIdentityStorage::getDefaultKeyNameForIdentity(const Name& identityName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getDefaultKeyNameForIdentity not implemented");
#endif
}

Name 
MemoryIdentityStorage::getDefaultCertificateNameForKey(const Name& keyName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::getDefaultCertificateNameForKey not implemented");
#endif
}

void 
MemoryIdentityStorage::setDefaultIdentity(const Name& identityName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::setDefaultIdentity not implemented");
#endif
}

void 
MemoryIdentityStorage::setDefaultKeyNameForIdentity(const Name& keyName, const Name& identityName)
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::setDefaultKeyNameForIdentity not implemented");
#endif
}

void 
MemoryIdentityStorage::setDefaultCertificateNameForKey(const Name& keyName, const Name& certificateName)  
{
#if 1
  throw std::runtime_error("MemoryIdentityStorage::setDefaultCertificateNameForKey not implemented");
#endif
}

}
