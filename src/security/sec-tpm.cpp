/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "sec-tpm.hpp"

#include "../encoding/oid.hpp"
#include "../encoding/buffer-stream.hpp"
#include "cryptopp.hpp"
#include <unistd.h>

namespace ndn {

using std::string;

SecTpm::SecTpm(const string& location)
  : m_location(location)
{
}

SecTpm::~SecTpm()
{
}

std::string
SecTpm::getTpmLocator()
{
  return this->getScheme() + ":" + m_location;
}

ConstBufferPtr
SecTpm::exportPrivateKeyPkcs5FromTpm(const Name& keyName, const string& passwordStr)
{
  using namespace CryptoPP;

  uint8_t salt[8] = {0};
  uint8_t iv[8] = {0};

  // derive key
  if (!generateRandomBlock(salt, 8) || !generateRandomBlock(iv, 8))
    BOOST_THROW_EXCEPTION(Error("Cannot generate salt or iv"));

  uint32_t iterationCount = 2048;

  PKCS5_PBKDF2_HMAC<SHA1> keyGenerator;
  size_t derivedLen = 24; //For DES-EDE3-CBC-PAD
  byte derived[24] = {0};
  byte purpose = 0;

  try
    {
      keyGenerator.DeriveKey(derived, derivedLen, purpose,
                             reinterpret_cast<const byte*>(passwordStr.c_str()), passwordStr.size(),
                             salt, 8, iterationCount);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_THROW_EXCEPTION(Error("Cannot derived the encryption key"));
    }

  //encrypt
  CBC_Mode< DES_EDE3 >::Encryption e;
  e.SetKeyWithIV(derived, derivedLen, iv);

  ConstBufferPtr pkcs8PrivateKey = exportPrivateKeyPkcs8FromTpm(keyName);

  if (!static_cast<bool>(pkcs8PrivateKey))
    BOOST_THROW_EXCEPTION(Error("Cannot export the private key, #1"));

  OBufferStream encryptedOs;
  try
    {
      StringSource stringSource(pkcs8PrivateKey->buf(), pkcs8PrivateKey->size(), true,
                                new StreamTransformationFilter(e, new FileSink(encryptedOs)));
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_THROW_EXCEPTION(Error("Cannot export the private key, #2"));
    }

  //encode
  OID pbes2Id("1.2.840.113549.1.5.13");
  OID pbkdf2Id("1.2.840.113549.1.5.12");
  OID pbes2encsId("1.2.840.113549.3.7");

  OBufferStream pkcs8Os;
  try
    {
      FileSink sink(pkcs8Os);

      // EncryptedPrivateKeyInfo ::= SEQUENCE {
      //   encryptionAlgorithm  EncryptionAlgorithmIdentifier,
      //   encryptedData        OCTET STRING }
      DERSequenceEncoder encryptedPrivateKeyInfo(sink);
      {
        // EncryptionAlgorithmIdentifier ::= SEQUENCE {
        //   algorithm      OBJECT IDENTIFIER {{PBES2-id}},
        //   parameters     SEQUENCE {{PBES2-params}} }
        DERSequenceEncoder encryptionAlgorithm(encryptedPrivateKeyInfo);
        {
          pbes2Id.encode(encryptionAlgorithm);
          // PBES2-params ::= SEQUENCE {
          //   keyDerivationFunc AlgorithmIdentifier {{PBES2-KDFs}},
          //   encryptionScheme AlgorithmIdentifier {{PBES2-Encs}} }
          DERSequenceEncoder pbes2Params(encryptionAlgorithm);
          {
            // AlgorithmIdentifier ::= SEQUENCE {
            //   algorithm      OBJECT IDENTIFIER {{PBKDF2-id}},
            //   parameters     SEQUENCE {{PBKDF2-params}} }
            DERSequenceEncoder pbes2KDFs(pbes2Params);
            {
              pbkdf2Id.encode(pbes2KDFs);
              // AlgorithmIdentifier ::= SEQUENCE {
              //   salt           OCTET STRING,
              //   iterationCount INTEGER (1..MAX),
              //   keyLength      INTEGER (1..MAX) OPTIONAL,
              //   prf AlgorithmIdentifier {{PBKDF2-PRFs}} DEFAULT algid-hmacWithSHA1 }
              DERSequenceEncoder pbkdf2Params(pbes2KDFs);
              {
                DEREncodeOctetString(pbkdf2Params, salt, 8);
                DEREncodeUnsigned<uint32_t>(pbkdf2Params, iterationCount, INTEGER);
              }
              pbkdf2Params.MessageEnd();
            }
            pbes2KDFs.MessageEnd();

            // AlgorithmIdentifier ::= SEQUENCE {
            //   algorithm   OBJECT IDENTIFIER {{DES-EDE3-CBC-PAD}},
            //   parameters  OCTET STRING} {{iv}} }
            DERSequenceEncoder pbes2Encs(pbes2Params);
            {
              pbes2encsId.encode(pbes2Encs);
              DEREncodeOctetString(pbes2Encs, iv, 8);
            }
            pbes2Encs.MessageEnd();
          }
          pbes2Params.MessageEnd();
        }
        encryptionAlgorithm.MessageEnd();

        DEREncodeOctetString(encryptedPrivateKeyInfo,
                             encryptedOs.buf()->buf(), encryptedOs.buf()->size());
      }
      encryptedPrivateKeyInfo.MessageEnd();

      return pkcs8Os.buf();
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_THROW_EXCEPTION(Error("Cannot export the private key, #3"));
    }
}

bool
SecTpm::importPrivateKeyPkcs5IntoTpm(const Name& keyName,
                                     const uint8_t* buf, size_t size,
                                     const string& passwordStr)
{
  using namespace CryptoPP;

  OID pbes2Id;
  OID pbkdf2Id;
  SecByteBlock saltBlock;
  uint32_t iterationCount;
  OID pbes2encsId;
  SecByteBlock ivBlock;
  SecByteBlock encryptedDataBlock;

  try
    {
      // decode some decoding processes are not necessary for now,
      // because we assume only one encryption scheme.
      StringSource source(buf, size, true);

      // EncryptedPrivateKeyInfo ::= SEQUENCE {
      //   encryptionAlgorithm  EncryptionAlgorithmIdentifier,
      //   encryptedData        OCTET STRING }
      BERSequenceDecoder encryptedPrivateKeyInfo(source);
      {
        // EncryptionAlgorithmIdentifier ::= SEQUENCE {
        //   algorithm      OBJECT IDENTIFIER {{PBES2-id}},
        //   parameters     SEQUENCE {{PBES2-params}} }
        BERSequenceDecoder encryptionAlgorithm(encryptedPrivateKeyInfo);
        {
          pbes2Id.decode(encryptionAlgorithm);
          // PBES2-params ::= SEQUENCE {
          //   keyDerivationFunc AlgorithmIdentifier {{PBES2-KDFs}},
          //   encryptionScheme AlgorithmIdentifier {{PBES2-Encs}} }
          BERSequenceDecoder pbes2Params(encryptionAlgorithm);
          {
            // AlgorithmIdentifier ::= SEQUENCE {
            //   algorithm      OBJECT IDENTIFIER {{PBKDF2-id}},
            //   parameters     SEQUENCE {{PBKDF2-params}} }
            BERSequenceDecoder pbes2KDFs(pbes2Params);
            {
              pbkdf2Id.decode(pbes2KDFs);
              // AlgorithmIdentifier ::= SEQUENCE {
              //   salt           OCTET STRING,
              //   iterationCount INTEGER (1..MAX),
              //   keyLength      INTEGER (1..MAX) OPTIONAL,
              //   prf AlgorithmIdentifier {{PBKDF2-PRFs}} DEFAULT algid-hmacWithSHA1 }
              BERSequenceDecoder pbkdf2Params(pbes2KDFs);
              {
                BERDecodeOctetString(pbkdf2Params, saltBlock);
                BERDecodeUnsigned<uint32_t>(pbkdf2Params, iterationCount, INTEGER);
              }
              pbkdf2Params.MessageEnd();
            }
            pbes2KDFs.MessageEnd();

            // AlgorithmIdentifier ::= SEQUENCE {
            //   algorithm   OBJECT IDENTIFIER {{DES-EDE3-CBC-PAD}},
            //   parameters  OCTET STRING} {{iv}} }
            BERSequenceDecoder pbes2Encs(pbes2Params);
            {
              pbes2encsId.decode(pbes2Encs);
              BERDecodeOctetString(pbes2Encs, ivBlock);
            }
            pbes2Encs.MessageEnd();
          }
          pbes2Params.MessageEnd();
        }
        encryptionAlgorithm.MessageEnd();

        BERDecodeOctetString(encryptedPrivateKeyInfo, encryptedDataBlock);
      }
      encryptedPrivateKeyInfo.MessageEnd();
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }

  PKCS5_PBKDF2_HMAC<SHA1> keyGenerator;
  size_t derivedLen = 24; //For DES-EDE3-CBC-PAD
  byte derived[24] = {0};
  byte purpose = 0;

  try
    {
      keyGenerator.DeriveKey(derived, derivedLen,
                             purpose,
                             reinterpret_cast<const byte*>(passwordStr.c_str()), passwordStr.size(),
                             saltBlock.BytePtr(), saltBlock.size(),
                             iterationCount);
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }

  //decrypt
  CBC_Mode< DES_EDE3 >::Decryption d;
  d.SetKeyWithIV(derived, derivedLen, ivBlock.BytePtr());

  OBufferStream privateKeyOs;
  try
    {
      StringSource encryptedSource(encryptedDataBlock.BytePtr(), encryptedDataBlock.size(), true,
                                   new StreamTransformationFilter(d,  new FileSink(privateKeyOs)));
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }

  if (!importPrivateKeyPkcs8IntoTpm(keyName,
                                    privateKeyOs.buf()->buf(), privateKeyOs.buf()->size()))
    return false;

  //determine key type
  StringSource privateKeySource(privateKeyOs.buf()->buf(), privateKeyOs.buf()->size(), true);

  KeyType publicKeyType = KEY_TYPE_NULL;
  SecByteBlock rawKeyBits;
  // PrivateKeyInfo ::= SEQUENCE {
  //   INTEGER,
  //   SEQUENCE,
  //   OCTECT STRING}
  BERSequenceDecoder privateKeyInfo(privateKeySource);
  {
    uint32_t versionNum;
    BERDecodeUnsigned<uint32_t>(privateKeyInfo, versionNum, INTEGER);
    BERSequenceDecoder sequenceDecoder(privateKeyInfo);
    {
      OID keyTypeOID;
      keyTypeOID.decode(sequenceDecoder);
      if (keyTypeOID == oid::RSA)
        publicKeyType = KEY_TYPE_RSA;
      else if (keyTypeOID == oid::ECDSA)
        publicKeyType = KEY_TYPE_ECDSA;
      else
        return false; // Unsupported key type;
    }
  }


  //derive public key
  OBufferStream publicKeyOs;

  try {
    switch (publicKeyType) {
    case KEY_TYPE_RSA:
      {
        RSA::PrivateKey privateKey;
        privateKey.Load(StringStore(privateKeyOs.buf()->buf(), privateKeyOs.buf()->size()).Ref());
        RSAFunction publicKey(privateKey);

        FileSink publicKeySink(publicKeyOs);
        publicKey.DEREncode(publicKeySink);
        publicKeySink.MessageEnd();
        break;
      }
    case KEY_TYPE_ECDSA:
      {
        ECDSA<ECP, SHA256>::PrivateKey privateKey;
        privateKey.Load(StringStore(privateKeyOs.buf()->buf(), privateKeyOs.buf()->size()).Ref());

        ECDSA<ECP, SHA256>::PublicKey publicKey;
        privateKey.MakePublicKey(publicKey);
        publicKey.AccessGroupParameters().SetEncodeAsOID(true);

        FileSink publicKeySink(publicKeyOs);
        publicKey.DEREncode(publicKeySink);
        publicKeySink.MessageEnd();
        break;
      }
    default:
      return false;
    }
  }
  catch (CryptoPP::Exception& e) {
      return false;
  }

  if (!importPublicKeyPkcs1IntoTpm(keyName, publicKeyOs.buf()->buf(), publicKeyOs.buf()->size()))
    return false;

  return true;
}

bool
SecTpm::getImpExpPassWord(std::string& password, const std::string& prompt)
{
  bool isInitialized = false;

#ifdef NDN_CXX_HAVE_GETPASS
  char* pw0 = 0;

  pw0 = getpass(prompt.c_str());
  if (0 == pw0)
    return false;
  std::string password1 = pw0;
  memset(pw0, 0, strlen(pw0));

  pw0 = getpass("Confirm:");
  if (0 == pw0)
    {
      std::fill(password1.begin(), password1.end(), 0);
      return false;
    }

  if (0 == password1.compare(pw0))
    {
      isInitialized = true;
      password.swap(password1);
    }

  std::fill(password1.begin(), password1.end(), 0);
  memset(pw0, 0, strlen(pw0));

  if (password.empty())
    return false;

#endif // NDN_CXX_HAVE_GETPASS

  return isInitialized;
}


} // namespace ndn
