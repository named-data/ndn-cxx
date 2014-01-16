/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/sec-tpm-memory.hpp>
#include <ndn-cpp/security/public-key.hpp>
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>

using namespace std;

namespace ndn {

/**
 * RsaPrivateKey is a simple class to hold an RSA private key.
 */
class SecTpmMemory::RsaPrivateKey {
public:
  RsaPrivateKey(const uint8_t *keyDer, size_t keyDerLength)
  {
    // Use a temporary pointer since d2i updates it.
    const uint8_t *derPointer = keyDer;
    privateKey_ = d2i_RSAPrivateKey(NULL, &derPointer, keyDerLength);
    if (!privateKey_)
      throw Error("RsaPrivateKey constructor: Error decoding private key DER");
  }
    
  ~RsaPrivateKey()
  {
    if (privateKey_)
      RSA_free(privateKey_);
  }
    
  rsa_st *
  getPrivateKey()
  {
    return privateKey_;
  }
    
private:
  rsa_st * privateKey_;
};

SecTpmMemory::~SecTpmMemory()
{
}

void
SecTpmMemory::setKeyPairForKeyName(const Name& keyName,
                                uint8_t *publicKeyDer, size_t publicKeyDerLength,
                                uint8_t *privateKeyDer, size_t privateKeyDerLength)
{
  publicKeyStore_[keyName.toUri()]  = ptr_lib::make_shared<PublicKey>(publicKeyDer, publicKeyDerLength);
  privateKeyStore_[keyName.toUri()] = ptr_lib::make_shared<RsaPrivateKey>(privateKeyDer, privateKeyDerLength);
}

void 
SecTpmMemory::generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw Error("MemoryPrivateKeyStorage::generateKeyPair not implemented");
#endif
}

ptr_lib::shared_ptr<PublicKey> 
SecTpmMemory::getPublicKeyFromTpm(const Name& keyName)
{
  PublicKeyStore::iterator publicKey = publicKeyStore_.find(keyName.toUri());
  if (publicKey == publicKeyStore_.end())
    throw Error(string("MemoryPrivateKeyStorage: Cannot find public key ") + keyName.toUri());
  return publicKey->second;
}

Block 
SecTpmMemory::signInTpm(const uint8_t *data, size_t dataLength,
                const Name& keyName,
                DigestAlgorithm digestAlgorithm)
{
  if (digestAlgorithm != DIGEST_ALGORITHM_SHA256)
    return ConstBufferPtr();

  // Find the private key and sign.
  PrivateKeyStore::iterator privateKey = privateKeyStore_.find(keyName.toUri());
  if (privateKey == privateKeyStore_.end())
    throw Error(string("MemoryPrivateKeyStorage: Cannot find private key ") + keyName.toUri());
  
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, dataLength);
  SHA256_Final(digest, &sha256);

  BufferPtr signatureBuffer = ptr_lib::make_shared<Buffer>();
  signatureBuffer->resize(RSA_size(privateKey->second->getPrivateKey()));
  
  unsigned int signatureBitsLength;  
  if (!RSA_sign(NID_sha256, digest, sizeof(digest),
                signatureBuffer->buf(),
                &signatureBitsLength,
                privateKey->second->getPrivateKey()))
    {
      throw Error("Error in RSA_sign");
    }

  return Block(Tlv::SignatureValue, signatureBuffer);
}

void
SecTpmMemory::signInTpm(Data &d,
                const Name& keyName,
                DigestAlgorithm digestAlgorithm)
{
  if (digestAlgorithm != DIGEST_ALGORITHM_SHA256)
    Error("MemoryPrivateKeyStorage::sign only SHA256 digest is supported");

  // Find the private key and sign.
  PrivateKeyStore::iterator privateKey = privateKeyStore_.find(keyName.toUri());
  if (privateKey == privateKeyStore_.end())
    throw Error(string("MemoryPrivateKeyStorage: Cannot find private key ") + keyName.toUri());
  
  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);

  SHA256_Update(&sha256, d.getName().    wireEncode().wire(), d.getName().    wireEncode().size());
  SHA256_Update(&sha256, d.getMetaInfo().wireEncode().wire(), d.getMetaInfo().wireEncode().size());
  SHA256_Update(&sha256, d.getContent().              wire(), d.getContent().              size());
  SHA256_Update(&sha256, d.getSignature().getInfo().  wire(), d.getSignature().getInfo().  size());
  
  SHA256_Final(digest, &sha256);

  BufferPtr signatureBuffer = ptr_lib::make_shared<Buffer>();
  signatureBuffer->resize(RSA_size(privateKey->second->getPrivateKey()));
  
  unsigned int signatureBitsLength;  
  if (!RSA_sign(NID_sha256, digest, sizeof(digest),
                signatureBuffer->buf(),
                &signatureBitsLength,
                privateKey->second->getPrivateKey()))
    {
      throw Error("Error in RSA_sign");
    }

  d.setSignatureValue(Block(Tlv::SignatureValue, signatureBuffer));
}

ConstBufferPtr
SecTpmMemory::decryptInTpm(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric)
{
#if 1
  throw Error("MemoryPrivateKeyStorage::decrypt not implemented");
#endif
}

ConstBufferPtr
SecTpmMemory::encryptInTpm(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric)
{
#if 1
  throw Error("MemoryPrivateKeyStorage::encrypt not implemented");
#endif
}

void 
SecTpmMemory::generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw Error("MemoryPrivateKeyStorage::generateKey not implemented");
#endif
}

bool
SecTpmMemory::doesKeyExistInTpm(const Name& keyName, KeyClass keyClass)
{
  if (keyClass == KEY_CLASS_PUBLIC)
    return publicKeyStore_.find(keyName.toUri()) != publicKeyStore_.end();
  else if (keyClass == KEY_CLASS_PRIVATE)
    return privateKeyStore_.find(keyName.toUri()) != privateKeyStore_.end();
  else
    // KEY_CLASS_SYMMETRIC not implemented yet.
    return false;
}

}
