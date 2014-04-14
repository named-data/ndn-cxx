/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "sec-tpm-memory.hpp"
#include "public-key.hpp"

#include "openssl.hpp"
#include "cryptopp.hpp"

using namespace std;

namespace ndn {

/**
 * RsaPrivateKey is a simple class to hold an RSA private key.
 */
class SecTpmMemory::RsaPrivateKey {
public:
  RsaPrivateKey(const uint8_t* keyDer, size_t keyDerLength)
  {
    // Use a temporary pointer since d2i updates it.
    const uint8_t* derPointer = keyDer;
    privateKey_ = d2i_RSAPrivateKey(NULL, &derPointer, keyDerLength);
    if (!privateKey_)
      throw Error("RsaPrivateKey constructor: Error decoding private key DER");
  }

  ~RsaPrivateKey()
  {
    if (privateKey_)
      RSA_free(privateKey_);
  }

  rsa_st*
  getPrivateKey()
  {
    return privateKey_;
  }

private:
  rsa_st*  privateKey_;
};

SecTpmMemory::~SecTpmMemory()
{
}

void
SecTpmMemory::setKeyPairForKeyName(const Name& keyName,
                                   uint8_t* publicKeyDer, size_t publicKeyDerLength,
                                   uint8_t* privateKeyDer, size_t privateKeyDerLength)
{
  publicKeyStore_[keyName.toUri()]  = make_shared<PublicKey>(publicKeyDer, publicKeyDerLength);
  privateKeyStore_[keyName.toUri()] = make_shared<RsaPrivateKey>(privateKeyDer, privateKeyDerLength);
}

void
SecTpmMemory::generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize)
{
#if 1
  throw Error("SecTpmMemory::generateKeyPair not implemented");
#endif
}

void
SecTpmMemory::deleteKeyPairInTpm(const Name& keyName)
{
  throw Error("SecTpmMemory::deleteKeyPairInTpm not implemented");
}

ConstBufferPtr
SecTpmMemory::exportPrivateKeyPkcs1FromTpm(const Name& keyName)
{
  return shared_ptr<Buffer>();
}

bool
SecTpmMemory::importPrivateKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
{
  return false;
}

bool
SecTpmMemory::importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
{
  return false;
}

shared_ptr<PublicKey>
SecTpmMemory::getPublicKeyFromTpm(const Name& keyName)
{
  PublicKeyStore::iterator publicKey = publicKeyStore_.find(keyName.toUri());
  if (publicKey == publicKeyStore_.end())
    throw Error(string("MemoryPrivateKeyStorage: Cannot find public key ") + keyName.toUri());
  return publicKey->second;
}

Block
SecTpmMemory::signInTpm(const uint8_t* data, size_t dataLength,
                        const Name& keyName,
                        DigestAlgorithm digestAlgorithm)
{
  if (digestAlgorithm != DIGEST_ALGORITHM_SHA256)
    throw Error("Unsupported digest algorithm.");

  // Find the private key and sign.
  PrivateKeyStore::iterator privateKey = privateKeyStore_.find(keyName.toUri());
  if (privateKey == privateKeyStore_.end())
    throw Error(string("MemoryPrivateKeyStorage: Cannot find private key ") + keyName.toUri());

  uint8_t digest[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, dataLength);
  SHA256_Final(digest,& sha256);

  BufferPtr signatureBuffer = make_shared<Buffer>();
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

ConstBufferPtr
SecTpmMemory::decryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric)
{
#if 1
  throw Error("MemoryPrivateKeyStorage::decrypt not implemented");
#endif
}

ConstBufferPtr
SecTpmMemory::encryptInTpm(const uint8_t* data, size_t dataLength, const Name& keyName, bool isSymmetric)
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

bool
SecTpmMemory::generateRandomBlock(uint8_t* res, size_t size)
{
  try {
    CryptoPP::AutoSeededRandomPool rng;
    rng.GenerateBlock(res, size);
    return true;
  }
  catch (const CryptoPP::Exception& e) {
    return false;
  }
}

} // namespace ndn
