/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Xingyu Ma <maxy12@cs.ucla.edu>
 *          Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *          Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp-dev/security/sec-tpm-file.hpp>

#include <string>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <cryptopp/rsa.h>
#include <cryptopp/files.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/pssr.h>
#include <cryptopp/modes.h>

#include <sys/types.h>
#include <sys/stat.h>

using namespace CryptoPP;
using namespace ndn;
using namespace std;

namespace ndn
{

class SecTpmFile::Impl {
public:
  Impl(const string &dir)
  {
    if(dir.empty())
      m_keystorePath = boost::filesystem::path(getenv("HOME")) / ".ndnx" / "ndnsec-keys";
    else
      m_keystorePath = dir;
    
    boost::filesystem::create_directories (m_keystorePath);
  }

public:
  boost::filesystem::path m_keystorePath;
};

SecTpmFile::SecTpmFile(const string & dir)
  : impl_(new Impl(dir))
{}

void
SecTpmFile::generateKeyPairInTpm(const Name & keyName, KeyType keyType, int keySize)
{
  string keyURI = keyName.toUri();

  if(doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
    throw Error("public key exists");
  if(doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
    throw Error("private key exists");

  string keyFileName = nameTransform(keyURI, "");
  maintainMapping(keyURI, keyFileName);

  try{
    switch(keyType){
    case KEY_TYPE_RSA:
      {
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
  }catch(const CryptoPP::Exception& e){
    throw Error(e.what());
  }
}

ptr_lib::shared_ptr<PublicKey>
SecTpmFile::getPublicKeyFromTpm(const Name & keyName)
{
  string keyURI = keyName.toUri();

  if(!doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
    throw Error("public key doesn't exists");

  string publicKeyFileName = nameTransform(keyURI, ".pub");
  std::ostringstream os;
  try{
    FileSource(publicKeyFileName.c_str(), true, new Base64Decoder(new FileSink(os)));
  }catch(const CryptoPP::Exception& e){
    throw Error(e.what());
  }

  return ptr_lib::make_shared<PublicKey>(reinterpret_cast<const uint8_t*>(os.str().c_str()), os.str().size());
}

Block
SecTpmFile::signInTpm(const uint8_t *data, size_t dataLength, const Name& keyName, DigestAlgorithm digestAlgorithm)
{
  string keyURI = keyName.toUri();

  if(!doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
    throw Error("private key doesn't exists");
 
  try{
    AutoSeededRandomPool rng;
      
    //Read private key
    ByteQueue bytes;
    string privateKeyFileName = nameTransform(keyURI, ".pri");
    FileSource file(privateKeyFileName.c_str(), true, new Base64Decoder);
    file.TransferTo(bytes);
    bytes.MessageEnd();
    RSA::PrivateKey privateKey;
    privateKey.Load(bytes);
  
    //Sign message
    switch(digestAlgorithm){
    case DIGEST_ALGORITHM_SHA256:
      {
	RSASS<PKCS1v15, SHA256>::Signer signer(privateKey);
	
	OBufferStream os;
	StringSource(data, dataLength, true, new SignerFilter(rng, signer, new FileSink(os)));
	
	return Block(Tlv::SignatureValue, os.buf());
      }
    default:
      throw Error("Unsupported digest algorithm!");
    }
  }catch(const CryptoPP::Exception& e){
    throw Error(e.what());
  }
}


ConstBufferPtr
SecTpmFile::decryptInTpm(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric)
{
  string keyURI = keyName.toUri();
  if (!isSymmetric)
    {
      if(!doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE))
	throw Error("private key doesn't exist");

      try{
	AutoSeededRandomPool rng;
	
	//Read private key
	ByteQueue bytes;
	string privateKeyFileName = nameTransform(keyURI, ".pri");
	FileSource file(privateKeyFileName.c_str(), true, new Base64Decoder);
	file.TransferTo(bytes);
	bytes.MessageEnd();
	RSA::PrivateKey privateKey;
	privateKey.Load(bytes);
	RSAES_PKCS1v15_Decryptor decryptor(privateKey);
	
	OBufferStream os;
	StringSource(data, dataLength, true, new PK_DecryptorFilter(rng, decryptor, new FileSink(os)));
	
	return os.buf();
      }
      catch(const CryptoPP::Exception& e){
	throw Error(e.what());
      }
    }
  else
    {
      throw Error("Symmetric encryption is not implemented!");
      // if(!doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
      // 	throw Error("symmetric key doesn't exist");

      // try{
      // 	string keyBits;
      // 	string symKeyFileName = nameTransform(keyURI, ".key");
      // 	FileSource(symKeyFileName, true, new HexDecoder(new StringSink(keyBits)));
	
      // 	using CryptoPP::AES;
      // 	AutoSeededRandomPool rnd;
      // 	byte iv[AES::BLOCKSIZE];
      // 	rnd.GenerateBlock(iv, AES::BLOCKSIZE);

      // 	CFB_Mode<AES>::Decryption decryptor;
      // 	decryptor.SetKeyWithIV(reinterpret_cast<const uint8_t*>(keyBits.c_str()), keyBits.size(), iv);
	
      // 	OBufferStream os;
      // 	StringSource(data, dataLength, true, new StreamTransformationFilter(decryptor,new FileSink(os)));
      // 	return os.buf();

      // }catch(const CryptoPP::Exception& e){
      // 	throw Error(e.what());
      // }
    }
}

ConstBufferPtr
SecTpmFile::encryptInTpm(const Name& keyName, const uint8_t* data, size_t dataLength, bool isSymmetric)
{
  string keyURI = keyName.toUri();

  if (!isSymmetric)
    {
      if(!doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC))
	throw Error("public key doesn't exist");
      try
	{
	  AutoSeededRandomPool rng;

	  //Read private key
	  ByteQueue bytes;
	  string publicKeyFileName = nameTransform(keyURI, ".pub");
	  FileSource file(publicKeyFileName.c_str(), true, new Base64Decoder);
	  file.TransferTo(bytes);
	  bytes.MessageEnd();
	  RSA::PublicKey publicKey;
	  publicKey.Load(bytes);

	  OBufferStream os;
	  RSAES_PKCS1v15_Encryptor encryptor(publicKey);

	  StringSource(data, dataLength, true, new PK_EncryptorFilter(rng, encryptor, new FileSink(os)));
	  return os.buf();
	}
      catch(const CryptoPP::Exception& e){
	throw Error(e.what());
      }
    }
  else
    {
      throw Error("Symmetric encryption is not implemented!");
      // if(!doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
      // 	throw Error("symmetric key doesn't exist");

      // try{
      // 	string keyBits;
      // 	string symKeyFileName = nameTransform(keyURI, ".key");
      // 	FileSource(symKeyFileName, true, new HexDecoder(new StringSink(keyBits)));

      // 	using CryptoPP::AES;
      // 	AutoSeededRandomPool rnd;
      // 	byte iv[AES::BLOCKSIZE];
      // 	rnd.GenerateBlock(iv, AES::BLOCKSIZE);

      // 	CFB_Mode<AES>::Encryption encryptor;
      // 	encryptor.SetKeyWithIV(reinterpret_cast<const uint8_t*>(keyBits.c_str()), keyBits.size(), iv);

      // 	OBufferStream os;
      // 	StringSource(data, dataLength, true, new StreamTransformationFilter(encryptor, new FileSink(os)));
      // 	return os.buf();
      // }catch(const CryptoPP::Exception& e){
      // 	throw Error(e.what());
      // }
    }
}


void
SecTpmFile::generateSymmetricKeyInTpm(const Name & keyName, KeyType keyType, int keySize)
{
  string keyURI = keyName.toUri();

  if(doesKeyExistInTpm(keyName, KEY_CLASS_SYMMETRIC))
    throw Error("symmetric key exists");

  string keyFileName = nameTransform(keyURI, "");
  maintainMapping(keyURI, keyFileName);
  string symKeyFileName = keyFileName + ".key";

  try{
    switch(keyType){
    case KEY_TYPE_AES:
      {
	AutoSeededRandomPool rnd;
	SecByteBlock key(0x00, keySize);
	rnd.GenerateBlock(key, keySize );
	
	StringSource(key, key.size(), true, new HexEncoder(new FileSink(symKeyFileName.c_str())));
	
	chmod(symKeyFileName.c_str(), 0000400);
	return;
      }
    default:
      throw Error("Unsupported symmetric key type!");
    }
  }catch(const CryptoPP::Exception& e){
    throw Error(e.what());
  }
}

bool
SecTpmFile::doesKeyExistInTpm(const Name & keyName, KeyClass keyClass)
{
  string keyURI = keyName.toUri();
  if (keyClass == KEY_CLASS_PUBLIC)
    {
      string publicKeyName = SecTpmFile::nameTransform(keyURI, ".pub");
      fstream fin(publicKeyName.c_str(),ios::in);
      if (fin)
        return true;
      else
        return false;
    }
  if (keyClass == KEY_CLASS_PRIVATE)
    {
      string privateKeyName = SecTpmFile::nameTransform(keyURI, ".pri");
      fstream fin(privateKeyName.c_str(),ios::in);
      if (fin)
        return true;
      else
        return false;
    }
  if (keyClass == KEY_CLASS_SYMMETRIC)
    {
      string symmetricKeyName = SecTpmFile::nameTransform(keyURI, ".key");
      fstream fin(symmetricKeyName.c_str(),ios::in);
      if (fin)
        return true;
      else
        return false;
    }
  return false;
}

std::string SecTpmFile::nameTransform(const string &keyName, const string &extension)
{
  std::string digest;
  CryptoPP::SHA256 hash;
  CryptoPP::StringSource foo(keyName, true,
                             new CryptoPP::HashFilter(hash,
                                                      new CryptoPP::Base64Encoder (new CryptoPP::StringSink(digest))
                                                      )
                             );
  boost::algorithm::trim(digest);
  for (std::string::iterator ch = digest.begin(); ch != digest.end(); ch++)
    {
      if (*ch == '/')
        {
          *ch = '%';
        }
    }

  return (impl_->m_keystorePath / (digest + extension)).string();
}

void 
SecTpmFile::maintainMapping(string str1, string str2)
{
  std::ofstream outfile;
  string dirFile = (impl_->m_keystorePath / "mapping.txt").string();

  outfile.open(dirFile.c_str(), std::ios_base::app);
  outfile << str1 << ' ' << str2 << '\n';
  outfile.close();
}

} //ndn
