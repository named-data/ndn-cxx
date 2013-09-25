/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../../c/util/crypto.h"
#include "../security-exception.hpp"
#include "memory-private-key-storage.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

MemoryPrivateKeyStorage::~MemoryPrivateKeyStorage()
{
}

void MemoryPrivateKeyStorage::setKeyPairForKeyName
  (const Name& keyName, uint8_t *publicKeyDer, size_t publicKeyDerLength, uint8_t *privateKeyDer, 
   size_t privateKeyDerLength)
{
  publicKeyStore_[keyName.toUri()] = PublicKey::fromDer(Blob(publicKeyDer, publicKeyDerLength));
  privateKeyStore_[keyName.toUri()] = shared_ptr<RsaPrivateKey>(new RsaPrivateKey(privateKeyDer, privateKeyDerLength));
}

void 
MemoryPrivateKeyStorage::generateKeyPair(const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw std::runtime_error("MemoryPrivateKeyStorage::generateKeyPair not implemented");
#endif
}

shared_ptr<PublicKey> 
MemoryPrivateKeyStorage::getPublicKey(const Name& keyName)
{
  map<string, shared_ptr<PublicKey> >::iterator publicKey = publicKeyStore_.find(keyName.toUri());
  if (publicKey == publicKeyStore_.end())
    throw SecurityException(string("MemoryPrivateKeyStorage: Cannot find public key ") + keyName.toUri());
  return publicKey->second;
}

Blob 
MemoryPrivateKeyStorage::sign(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  if (digestAlgorithm != DIGEST_ALGORITHM_SHA256)
    return Blob();

  uint8_t digest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(data, dataLength, digest);
  // TODO: use RSA_size to get the proper size of the signature buffer.
  uint8_t signatureBits[1000];
  unsigned int signatureBitsLength;
  
  // Find the private key and sign.
  map<string, shared_ptr<RsaPrivateKey> >::iterator privateKey = privateKeyStore_.find(keyName.toUri());
  if (privateKey == privateKeyStore_.end())
    throw SecurityException(string("MemoryPrivateKeyStorage: Cannot find private key ") + keyName.toUri());
  if (!RSA_sign(NID_sha256, digest, sizeof(digest), signatureBits, &signatureBitsLength, privateKey->second->getPrivateKey()))
    throw SecurityException("Error in RSA_sign");
  
  return Blob(signatureBits, (size_t)signatureBitsLength);
}

Blob 
MemoryPrivateKeyStorage::decrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric)
{
#if 1
  throw std::runtime_error("MemoryPrivateKeyStorage::decrypt not implemented");
#endif
}

Blob
MemoryPrivateKeyStorage::encrypt(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric)
{
#if 1
  throw std::runtime_error("MemoryPrivateKeyStorage::encrypt not implemented");
#endif
}

void 
MemoryPrivateKeyStorage::generateKey(const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw std::runtime_error("MemoryPrivateKeyStorage::generateKey not implemented");
#endif
}

bool
MemoryPrivateKeyStorage::doesKeyExist(const Name& keyName, KeyClass keyClass)
{
  if (keyClass == KEY_CLASS_PUBLIC)
    return publicKeyStore_.find(keyName.toUri()) != publicKeyStore_.end();
  else if (keyClass == KEY_CLASS_PRIVATE)
    return privateKeyStore_.find(keyName.toUri()) != privateKeyStore_.end();
  else
    // KEY_CLASS_SYMMETRIC not implemented yet.
    return false;
}

MemoryPrivateKeyStorage::RsaPrivateKey::RsaPrivateKey(uint8_t *keyDer, size_t keyDerLength)
{
  // Use a temporary pointer since d2i updates it.
  const uint8_t *derPointer = keyDer;
  privateKey_ = d2i_RSAPrivateKey(NULL, &derPointer, keyDerLength);
  if (!privateKey_)
    throw SecurityException("RsaPrivateKey constructor: Error decoding private key DER");
}

MemoryPrivateKeyStorage::RsaPrivateKey::~RsaPrivateKey()
{
  if (privateKey_)
    RSA_free(privateKey_);
}

}
