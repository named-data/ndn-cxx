/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "sec-tpm.hpp"

#include <cryptopp/rsa.h>
#include <cryptopp/files.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/pssr.h>
#include <cryptopp/modes.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/des.h>

using namespace std;

namespace ndn {

ConstBufferPtr
SecTpm::exportPrivateKeyPkcs8FromTpm(const Name& keyName, bool inTerminal, const string& passwordStr)
{
  uint8_t salt[8] = {0};
  uint8_t iv[8] = {0};
    
  try{
    using namespace CryptoPP;

    // check password
    string password;
    if(passwordStr.empty())
      if(!inTerminal)
        return shared_ptr<Buffer>();
      else
        {
          int count = 0;
          while(!getPassWord(password, keyName.toUri()))
            {
              cerr << "Password mismatch!" << endl;
              count++;
              if(count > 3)
                return shared_ptr<Buffer>();
            }
        }
    else
      password = passwordStr;

    // derive key
    if(!generateRandomBlock(salt, 8))
      return shared_ptr<Buffer>();
    
    if(!generateRandomBlock(iv, 8))
      return shared_ptr<Buffer>();
    
    uint32_t iterationCount = 2048;

    PKCS5_PBKDF2_HMAC<SHA1> keyGenerator;
    size_t derivedLen = 24; //For DES-EDE3-CBC-PAD
    byte derived[24] = {0};
    byte purpose = 0;

    keyGenerator.DeriveKey(derived, derivedLen, 
                           purpose, 
                           reinterpret_cast<const byte*>(password.c_str()), password.size(), 
                           salt, 8, 
                           iterationCount); 
    
    memset(const_cast<char*>(password.c_str()), 0, password.size());

    //encrypt
    CBC_Mode< DES_EDE3 >::Encryption e;
    e.SetKeyWithIV(derived, derivedLen, iv);
    
    string encrypted;
    OBufferStream encryptedOs;
    ConstBufferPtr pkcs1PrivateKey = exportPrivateKeyPkcs1FromTpm(keyName);
    StringSource stringSource(pkcs1PrivateKey->buf(), pkcs1PrivateKey->size(), true, 
			      new StreamTransformationFilter(e, new FileSink(encryptedOs)));

    //encode
    OID pbes2Id("1.2.840.113549.1.5.13");
    OID pbkdf2Id("1.2.840.113549.1.5.12");
    OID pbes2encsId("1.2.840.113549.3.7");

    OBufferStream pkcs8Os;
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

      DEREncodeOctetString(encryptedPrivateKeyInfo, encryptedOs.buf()->buf(), encryptedOs.buf()->size());
    }
    encryptedPrivateKeyInfo.MessageEnd();

    return pkcs8Os.buf();
  }catch(...){
    return shared_ptr<Buffer>();
  }
}

bool
SecTpm::importPrivateKeyPkcs8IntoTpm(const Name& keyName, const uint8_t* buf, size_t size, bool inTerminal, const string& passwordStr)
{
  try{
    using namespace CryptoPP;
    
    OID pbes2Id;
    OID pbkdf2Id;
    SecByteBlock saltBlock;
    uint32_t iterationCount;
    OID pbes2encsId;
    SecByteBlock ivBlock;
    SecByteBlock encryptedDataBlock;
    
    //decode some decoding processes are not necessary for now, because we assume only one encryption scheme.
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

  
    PKCS5_PBKDF2_HMAC<SHA1> keyGenerator;
    size_t derivedLen = 24; //For DES-EDE3-CBC-PAD
    byte derived[24] = {0};
    byte purpose = 0;

    string password;
    if(passwordStr.empty())
      if(inTerminal)
        {
          char* pw = getpass("Password for the private key: ");
          if (!pw)
            return false;
          password = pw;
          memset(pw, 0, strlen(pw));
        }
      else
        return false;
    else
      password = passwordStr;
      
    keyGenerator.DeriveKey(derived, derivedLen, 
                           purpose, 
                           reinterpret_cast<const byte*>(password.c_str()), password.size(), 
                           saltBlock.BytePtr(), saltBlock.size(), 
                           iterationCount);

    memset(const_cast<char*>(password.c_str()), 0, password.size());
        
    //decrypt
    CBC_Mode< DES_EDE3 >::Decryption d;
    d.SetKeyWithIV(derived, derivedLen, ivBlock.BytePtr());
    
    OBufferStream privateKeyOs;
    StringSource encryptedSource(encryptedDataBlock.BytePtr(), encryptedDataBlock.size(), true, 
                                 new StreamTransformationFilter(d,  new FileSink(privateKeyOs)));

    if(!importPrivateKeyPkcs1IntoTpm(keyName, privateKeyOs.buf()->buf(), privateKeyOs.buf()->size()))
      return false;
    
    //derive public key
    RSA::PrivateKey privateKey;
    privateKey.Load(StringStore(privateKeyOs.buf()->buf(), privateKeyOs.buf()->size()).Ref());

    RSAFunction publicKey(privateKey);

    OBufferStream publicKeyOs;
    FileSink publicKeySink(publicKeyOs);
    publicKey.DEREncode(publicKeySink);
    publicKeySink.MessageEnd();

    if(!importPublicKeyPkcs1IntoTpm(keyName, publicKeyOs.buf()->buf(), publicKeyOs.buf()->size()))
      return false;

    return true;
  }catch(std::runtime_error& e){
    cerr << e.what() << endl;
    return false;
  }
}


}//ndn
