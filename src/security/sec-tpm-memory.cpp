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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
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
class SecTpmMemory::RsaPrivateKey
{
public:
  RsaPrivateKey(const uint8_t* keyDer, size_t keyDerLength)
  {
    // Use a temporary pointer since d2i updates it.
    const uint8_t* derPointer = keyDer;
    m_privateKey = d2i_RSAPrivateKey(NULL, &derPointer, keyDerLength);
    if (!m_privateKey)
      throw Error("RsaPrivateKey constructor: Error decoding private key DER");
  }

  ~RsaPrivateKey()
  {
    if (m_privateKey)
      RSA_free(m_privateKey);
  }

  rsa_st*
  getPrivateKey()
  {
    return m_privateKey;
  }

private:
  rsa_st* m_privateKey;
};

SecTpmMemory::~SecTpmMemory()
{
}

void
SecTpmMemory::setKeyPairForKeyName(const Name& keyName,
                                   const uint8_t* publicKeyDer, size_t publicKeyDerLength,
                                   const uint8_t* privateKeyDer, size_t privateKeyDerLength)
{
  m_publicKeyStore[keyName.toUri()]  = make_shared<PublicKey>(publicKeyDer, publicKeyDerLength);
  m_privateKeyStore[keyName.toUri()] = make_shared<RsaPrivateKey>(privateKeyDer,
                                                                  privateKeyDerLength);
}

void
SecTpmMemory::generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize)
{
  throw Error("SecTpmMemory::generateKeyPair not implemented");
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
  PublicKeyStore::iterator publicKey = m_publicKeyStore.find(keyName.toUri());
  if (publicKey == m_publicKeyStore.end())
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
  PrivateKeyStore::iterator privateKey = m_privateKeyStore.find(keyName.toUri());
  if (privateKey == m_privateKeyStore.end())
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
SecTpmMemory::decryptInTpm(const uint8_t* data, size_t dataLength,
                           const Name& keyName, bool isSymmetric)
{
  throw Error("MemoryPrivateKeyStorage::decrypt not implemented");
}

ConstBufferPtr
SecTpmMemory::encryptInTpm(const uint8_t* data, size_t dataLength,
                           const Name& keyName, bool isSymmetric)
{
  throw Error("MemoryPrivateKeyStorage::encrypt not implemented");
}

void
SecTpmMemory::generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize)
{
  throw Error("MemoryPrivateKeyStorage::generateKey not implemented");
}

bool
SecTpmMemory::doesKeyExistInTpm(const Name& keyName, KeyClass keyClass)
{
  if (keyClass == KEY_CLASS_PUBLIC)
    return m_publicKeyStore.find(keyName.toUri()) != m_publicKeyStore.end();
  else if (keyClass == KEY_CLASS_PRIVATE)
    return m_privateKeyStore.find(keyName.toUri()) != m_privateKeyStore.end();
  else
    // KEY_CLASS_SYMMETRIC not implemented yet.
    return false;
}

bool
SecTpmMemory::generateRandomBlock(uint8_t* res, size_t size)
{
  try
    {
      CryptoPP::AutoSeededRandomPool rng;
      rng.GenerateBlock(res, size);
      return true;
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }
}

} // namespace ndn
