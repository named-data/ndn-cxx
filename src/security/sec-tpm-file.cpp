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
 * @author Xingyu Ma <http://www.linkedin.com/pub/xingyu-ma/1a/384/5a8>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "common.hpp"

#include "sec-tpm-file.hpp"
#include "../encoding/buffer-stream.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "cryptopp.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include <algorithm>

using namespace std;

namespace ndn {

class SecTpmFile::Impl
{
public:
  Impl(const string& dir)
  {
    if (dir.empty())
      m_keystorePath = boost::filesystem::path(getenv("HOME")) / ".ndn" / "ndnsec-tpm-file";
    else
      m_keystorePath = dir;

    boost::filesystem::create_directories (m_keystorePath);
  }

  boost::filesystem::path
  nameTransform(const string& keyName, const string& extension)
  {
    using namespace CryptoPP;
    string digest;
    SHA256 hash;
    StringSource src(keyName,
                     true,
                     new HashFilter(hash,
                                    new Base64Encoder(new CryptoPP::StringSink(digest))));

    boost::algorithm::trim(digest);
    std::replace(digest.begin(), digest.end(), '/', '%');

    return m_keystorePath / (digest + extension);
  }

  string
  maintainMapping(const string& keyName)
  {
    string keyFileName = nameTransform(keyName, "").string();

    ofstream outfile;
    string dirFile = (m_keystorePath / "mapping.txt").string();

    outfile.open(dirFile.c_str(), std::ios_base::app);
    outfile << keyName << ' ' << keyFileName << '\n';
    outfile.close();

    return keyFileName;
  }

public:
  boost::filesystem::path m_keystorePath;
};


SecTpmFile::SecTpmFile(const string& dir)
  : m_impl(new Impl(dir))
  , m_inTerminal(false)
{}

void
SecTpmFile::generateKeyPairInTpm(const Name& keyName, KeyType keyType, int keySize)
{
  string keyURI = keyName.toUri();

  if (doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
    throw Error("public key exists");
  if (doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
    throw Error("private key exists");

  string keyFileName = m_impl->maintainMapping(keyURI);

  try
    {
      switch (keyType)
        {
        case KEY_TYPE_RSA:
          {
            using namespace CryptoPP;
            AutoSeededRandomPool rng;

            InvertibleRSAFunction privateKey;
            privateKey.Initialize(rng, keySize);

            string privateKeyFileName = keyFileName + ".pri";
            Base64Encoder privateKeySink(new FileSink(privateKeyFileName.c_str()));
            privateKey.DEREncode(privateKeySink);
            privateKeySink.MessageEnd();

            RSAFunction publicKey(privateKey);
            string publicKeyFileName = keyFileName + ".pub";
            Base64Encoder publicKeySink(new FileSink(publicKeyFileName.c_str()));
            publicKey.DEREncode(publicKeySink);
            publicKeySink.MessageEnd();

            /*set file permission*/
            chmod(privateKeyFileName.c_str(), 0000400);
            chmod(publicKeyFileName.c_str(), 0000444);
            return;
          }
        default:
          throw Error("Unsupported key type!");
        }
    }
  catch (CryptoPP::Exception& e)
    {
      throw Error(e.what());
    }
}

void
SecTpmFile::deleteKeyPairInTpm(const Name& keyName)
{
  boost::filesystem::path publicKeyPath(m_impl->nameTransform(keyName.toUri(), ".pub"));
  boost::filesystem::path privateKeyPath(m_impl->nameTransform(keyName.toUri(), ".pri"));

  if (boost::filesystem::exists(publicKeyPath))
    boost::filesystem::remove(publicKeyPath);

  if (boost::filesystem::exists(privateKeyPath))
    boost::filesystem::remove(privateKeyPath);
}

shared_ptr<PublicKey>
SecTpmFile::getPublicKeyFromTpm(const Name&  keyName)
{
  string keyURI = keyName.toUri();

  if (!doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
    throw Error("Public Key does not exist");

  ostringstream os;
  try
    {
      using namespace CryptoPP;
      FileSource(m_impl->nameTransform(keyURI, ".pub").string().c_str(),
                 true,
                 new Base64Decoder(new FileSink(os)));
    }
  catch (CryptoPP::Exception& e)
    {
      throw Error(e.what());
    }

  return make_shared<PublicKey>(reinterpret_cast<const uint8_t*>(os.str().c_str()),
                                os.str().size());
}

ConstBufferPtr
SecTpmFile::exportPrivateKeyPkcs8FromTpm(const Name& keyName)
{
  OBufferStream privateKeyOs;
  CryptoPP::FileSource(m_impl->nameTransform(keyName.toUri(), ".pri").string().c_str(), true,
                       new CryptoPP::Base64Decoder(new CryptoPP::FileSink(privateKeyOs)));

  return privateKeyOs.buf();
}

bool
SecTpmFile::importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
{
  try
    {
      using namespace CryptoPP;

      string keyFileName = m_impl->maintainMapping(keyName.toUri());
      keyFileName.append(".pri");
      StringSource(buf, size,
                   true,
                   new Base64Encoder(new FileSink(keyFileName.c_str())));
      return true;
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }
}

bool
SecTpmFile::importPublicKeyPkcs1IntoTpm(const Name& keyName, const uint8_t* buf, size_t size)
{
  try
    {
      using namespace CryptoPP;

      string keyFileName = m_impl->maintainMapping(keyName.toUri());
      keyFileName.append(".pub");
      StringSource(buf, size,
                   true,
                   new Base64Encoder(new FileSink(keyFileName.c_str())));
      return true;
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }
}

Block
SecTpmFile::signInTpm(const uint8_t* data, size_t dataLength,
                      const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  string keyURI = keyName.toUri();

  if (!doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
    throw Error("private key doesn't exists");

  try
    {
      using namespace CryptoPP;
      AutoSeededRandomPool rng;

      //Read private key
      ByteQueue bytes;
      FileSource file(m_impl->nameTransform(keyURI, ".pri").string().c_str(),
                      true, new Base64Decoder);
      file.TransferTo(bytes);
      bytes.MessageEnd();
      RSA::PrivateKey privateKey;
      privateKey.Load(bytes);

      //Sign message
      switch (digestAlgorithm)
        {
        case DIGEST_ALGORITHM_SHA256:
          {
            RSASS<PKCS1v15, SHA256>::Signer signer(privateKey);

            OBufferStream os;
            StringSource(data, dataLength,
                         true,
                         new SignerFilter(rng, signer, new FileSink(os)));

            return Block(Tlv::SignatureValue, os.buf());
          }
        default:
          throw Error("Unsupported digest algorithm!");
        }
    }
  catch (CryptoPP::Exception& e)
    {
      throw Error(e.what());
    }
}


ConstBufferPtr
SecTpmFile::decryptInTpm(const uint8_t* data, size_t dataLength,
                         const Name& keyName, bool isSymmetric)
{
  throw Error("SecTpmFile::decryptInTpm is not supported!");
  // string keyURI = keyName.toUri();
  // if (!isSymmetric)
  //   {
  //     if (!doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
  //       throw Error("private key doesn't exist");

  //     try{
  //       using namespace CryptoPP;
  //       AutoSeededRandomPool rng;

  //       //Read private key
  //       ByteQueue bytes;
  //       FileSource file(m_impl->nameTransform(keyURI, ".pri").string().c_str(), true, new Base64Decoder);
  //       file.TransferTo(bytes);
  //       bytes.MessageEnd();
  //       RSA::PrivateKey privateKey;
  //       privateKey.Load(bytes);
  //       RSAES_PKCS1v15_Decryptor decryptor(privateKey);

  //       OBufferStream os;
  //       StringSource(data, dataLength, true, new PK_DecryptorFilter(rng, decryptor, new FileSink(os)));

  //       return os.buf();
  //     }
  //     catch (CryptoPP::Exception& e){
  //       throw Error(e.what());
  //     }
  //   }
  // else
  //   {
  //     throw Error("Symmetric encryption is not implemented!");
  //     // if (!doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
  //     // 	throw Error("symmetric key doesn't exist");

  //     // try{
  //     // 	string keyBits;
  //     // 	string symKeyFileName = m_impl->nameTransform(keyURI, ".key");
  //     // 	FileSource(symKeyFileName, true, new HexDecoder(new StringSink(keyBits)));

  //     // 	using CryptoPP::AES;
  //     // 	AutoSeededRandomPool rnd;
  //     // 	byte iv[AES::BLOCKSIZE];
  //     // 	rnd.GenerateBlock(iv, AES::BLOCKSIZE);

  //     // 	CFB_Mode<AES>::Decryption decryptor;
  //     // 	decryptor.SetKeyWithIV(reinterpret_cast<const uint8_t*>(keyBits.c_str()), keyBits.size(), iv);

  //     // 	OBufferStream os;
  //     // 	StringSource(data, dataLength, true, new StreamTransformationFilter(decryptor,new FileSink(os)));
  //     // 	return os.buf();

  //     // }catch (CryptoPP::Exception& e){
  //     // 	throw Error(e.what());
  //     // }
  //   }
}

ConstBufferPtr
SecTpmFile::encryptInTpm(const uint8_t* data, size_t dataLength,
                         const Name& keyName, bool isSymmetric)
{
  throw Error("SecTpmFile::encryptInTpm is not supported!");
  // string keyURI = keyName.toUri();

  // if (!isSymmetric)
  //   {
  //     if (!doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
  //       throw Error("public key doesn't exist");
  //     try
  //       {
  //         using namespace CryptoPP;
  //         AutoSeededRandomPool rng;

  //         //Read private key
  //         ByteQueue bytes;
  //         FileSource file(m_impl->nameTransform(keyURI, ".pub").string().c_str(), true, new Base64Decoder);
  //         file.TransferTo(bytes);
  //         bytes.MessageEnd();
  //         RSA::PublicKey publicKey;
  //         publicKey.Load(bytes);

  //         OBufferStream os;
  //         RSAES_PKCS1v15_Encryptor encryptor(publicKey);

  //         StringSource(data, dataLength, true, new PK_EncryptorFilter(rng, encryptor, new FileSink(os)));
  //         return os.buf();
  //       }
  //     catch (CryptoPP::Exception& e){
  //       throw Error(e.what());
  //     }
  //   }
  // else
  //   {
  //     throw Error("Symmetric encryption is not implemented!");
  //     // if (!doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
  //     // 	throw Error("symmetric key doesn't exist");

  //     // try{
  //     // 	string keyBits;
  //     // 	string symKeyFileName = m_impl->nameTransform(keyURI, ".key");
  //     // 	FileSource(symKeyFileName, true, new HexDecoder(new StringSink(keyBits)));

  //     // 	using CryptoPP::AES;
  //     // 	AutoSeededRandomPool rnd;
  //     // 	byte iv[AES::BLOCKSIZE];
  //     // 	rnd.GenerateBlock(iv, AES::BLOCKSIZE);

  //     // 	CFB_Mode<AES>::Encryption encryptor;
  //     // 	encryptor.SetKeyWithIV(reinterpret_cast<const uint8_t*>(keyBits.c_str()), keyBits.size(), iv);

  //     // 	OBufferStream os;
  //     // 	StringSource(data, dataLength, true, new StreamTransformationFilter(encryptor, new FileSink(os)));
  //     // 	return os.buf();
  //     // }catch (CryptoPP::Exception& e){
  //     // 	throw Error(e.what());
  //     // }
  //   }
}


void
SecTpmFile::generateSymmetricKeyInTpm(const Name& keyName, KeyType keyType, int keySize)
{
  throw Error("SecTpmFile::generateSymmetricKeyInTpm is not supported!");
  // string keyURI = keyName.toUri();

  // if (doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
  //   throw Error("symmetric key exists");

  // string keyFileName = m_impl->maintainMapping(keyURI);
  // string symKeyFileName = keyFileName + ".key";

  // try{
  //   switch (keyType){
  //   case KEY_TYPE_AES:
  //     {
  //       using namespace CryptoPP;
  //       AutoSeededRandomPool rng;

  //       SecByteBlock key(0x00, keySize);
  //       rng.GenerateBlock(key, keySize);

  //       StringSource(key, key.size(), true, new HexEncoder(new FileSink(symKeyFileName.c_str())));

  //       chmod(symKeyFileName.c_str(), 0000400);
  //       return;
  //     }
  //   default:
  //     throw Error("Unsupported symmetric key type!");
  //   }
  // }catch (CryptoPP::Exception& e){
  //   throw Error(e.what());
  // }
}

bool
SecTpmFile::doesKeyExistInTpm(const Name& keyName, KeyClass keyClass)
{
  string keyURI = keyName.toUri();
  if (keyClass == KEY_CLASS_PUBLIC)
    {
      if (boost::filesystem::exists(m_impl->nameTransform(keyURI, ".pub")))
        return true;
      else
        return false;
    }
  if (keyClass == KEY_CLASS_PRIVATE)
    {
      if (boost::filesystem::exists(m_impl->nameTransform(keyURI, ".pri")))
        return true;
      else
        return false;
    }
  if (keyClass == KEY_CLASS_SYMMETRIC)
    {
      if (boost::filesystem::exists(m_impl->nameTransform(keyURI, ".key")))
        return true;
      else
        return false;
    }
  return false;
}

bool
SecTpmFile::generateRandomBlock(uint8_t* res, size_t size)
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
