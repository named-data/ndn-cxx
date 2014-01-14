/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <stdexcept>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/security/key-chain.hpp>
// #include <ndn-cpp/security/policy/self-verify-policy-manager.hpp>

// Hack: Hook directly into non-API functions.
#include "../src/c/encoding/binary-xml-decoder.h"
#include "../src/c/data.h"
#include "../src/c/encoding/binary-xml-data.h"
#include <ndn-cpp/c/util/crypto.h>

using namespace std;
using namespace ndn;

static double
getNowSeconds()
{
  struct timeval t;
  gettimeofday(&t, 0);
  return t.tv_sec + t.tv_usec / 1000000.0;
}

static bool
verifyRsaSignature
  (uint8_t* signedPortion, size_t signedPortionLength, uint8_t* signatureBits, size_t signatureBitsLength, 
   uint8_t* publicKeyDer, size_t publicKeyDerLength)
{
  // Set signedPortionDigest to the digest of the signed portion of the wire encoding.
  uint8_t signedPortionDigest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(signedPortion, signedPortionLength, signedPortionDigest);
  
  // Verify the signedPortionDigest.
  // Use a temporary pointer since d2i updates it.
  const uint8_t *derPointer = publicKeyDer;
  RSA *rsaPublicKey = d2i_RSA_PUBKEY(NULL, &derPointer, publicKeyDerLength);
  if (!rsaPublicKey) {
    // Don't expect this to happen.
    cout << "Error decoding public key in d2i_RSAPublicKey" << endl;
    return 0;
  }
  int success = RSA_verify
    (NID_sha256, signedPortionDigest, sizeof(signedPortionDigest), signatureBits, signatureBitsLength, rsaPublicKey);
  // Free the public key before checking for success.
  RSA_free(rsaPublicKey);
  
  // RSA_verify returns 1 for a valid signature.
  return (success == 1);
}

static uint8_t DEFAULT_PUBLIC_KEY_DER[] = {
0x30, 0x81, 0x9F, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81,
0x8D, 0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xE1, 0x7D, 0x30, 0xA7, 0xD8, 0x28, 0xAB, 0x1B, 0x84, 0x0B, 0x17,
0x54, 0x2D, 0xCA, 0xF6, 0x20, 0x7A, 0xFD, 0x22, 0x1E, 0x08, 0x6B, 0x2A, 0x60, 0xD1, 0x6C, 0xB7, 0xF5, 0x44, 0x48, 0xBA,
0x9F, 0x3F, 0x08, 0xBC, 0xD0, 0x99, 0xDB, 0x21, 0xDD, 0x16, 0x2A, 0x77, 0x9E, 0x61, 0xAA, 0x89, 0xEE, 0xE5, 0x54, 0xD3,
0xA4, 0x7D, 0xE2, 0x30, 0xBC, 0x7A, 0xC5, 0x90, 0xD5, 0x24, 0x06, 0x7C, 0x38, 0x98, 0xBB, 0xA6, 0xF5, 0xDC, 0x43, 0x60,
0xB8, 0x45, 0xED, 0xA4, 0x8C, 0xBD, 0x9C, 0xF1, 0x26, 0xA7, 0x23, 0x44, 0x5F, 0x0E, 0x19, 0x52, 0xD7, 0x32, 0x5A, 0x75,
0xFA, 0xF5, 0x56, 0x14, 0x4F, 0x9A, 0x98, 0xAF, 0x71, 0x86, 0xB0, 0x27, 0x86, 0x85, 0xB8, 0xE2, 0xC0, 0x8B, 0xEA, 0x87,
0x17, 0x1B, 0x4D, 0xEE, 0x58, 0x5C, 0x18, 0x28, 0x29, 0x5B, 0x53, 0x95, 0xEB, 0x4A, 0x17, 0x77, 0x9F, 0x02, 0x03, 0x01,
0x00, 0x01  
};

static uint8_t DEFAULT_PRIVATE_KEY_DER[] = {
0x30, 0x82, 0x02, 0x5d, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xe1, 0x7d, 0x30, 0xa7, 0xd8, 0x28, 0xab, 0x1b, 0x84,
0x0b, 0x17, 0x54, 0x2d, 0xca, 0xf6, 0x20, 0x7a, 0xfd, 0x22, 0x1e, 0x08, 0x6b, 0x2a, 0x60, 0xd1, 0x6c, 0xb7, 0xf5, 0x44,
0x48, 0xba, 0x9f, 0x3f, 0x08, 0xbc, 0xd0, 0x99, 0xdb, 0x21, 0xdd, 0x16, 0x2a, 0x77, 0x9e, 0x61, 0xaa, 0x89, 0xee, 0xe5,
0x54, 0xd3, 0xa4, 0x7d, 0xe2, 0x30, 0xbc, 0x7a, 0xc5, 0x90, 0xd5, 0x24, 0x06, 0x7c, 0x38, 0x98, 0xbb, 0xa6, 0xf5, 0xdc,
0x43, 0x60, 0xb8, 0x45, 0xed, 0xa4, 0x8c, 0xbd, 0x9c, 0xf1, 0x26, 0xa7, 0x23, 0x44, 0x5f, 0x0e, 0x19, 0x52, 0xd7, 0x32,
0x5a, 0x75, 0xfa, 0xf5, 0x56, 0x14, 0x4f, 0x9a, 0x98, 0xaf, 0x71, 0x86, 0xb0, 0x27, 0x86, 0x85, 0xb8, 0xe2, 0xc0, 0x8b,
0xea, 0x87, 0x17, 0x1b, 0x4d, 0xee, 0x58, 0x5c, 0x18, 0x28, 0x29, 0x5b, 0x53, 0x95, 0xeb, 0x4a, 0x17, 0x77, 0x9f, 0x02,
0x03, 0x01, 0x00, 0x01, 0x02, 0x81, 0x80, 0x1a, 0x4b, 0xfa, 0x4f, 0xa8, 0xc2, 0xdd, 0x69, 0xa1, 0x15, 0x96, 0x0b, 0xe8,
0x27, 0x42, 0x5a, 0xf9, 0x5c, 0xea, 0x0c, 0xac, 0x98, 0xaa, 0xe1, 0x8d, 0xaa, 0xeb, 0x2d, 0x3c, 0x60, 0x6a, 0xfb, 0x45,
0x63, 0xa4, 0x79, 0x83, 0x67, 0xed, 0xe4, 0x15, 0xc0, 0xb0, 0x20, 0x95, 0x6d, 0x49, 0x16, 0xc6, 0x42, 0x05, 0x48, 0xaa,
0xb1, 0xa5, 0x53, 0x65, 0xd2, 0x02, 0x99, 0x08, 0xd1, 0x84, 0xcc, 0xf0, 0xcd, 0xea, 0x61, 0xc9, 0x39, 0x02, 0x3f, 0x87,
0x4a, 0xe5, 0xc4, 0xd2, 0x07, 0x02, 0xe1, 0x9f, 0xa0, 0x06, 0xc2, 0xcc, 0x02, 0xe7, 0xaa, 0x6c, 0x99, 0x8a, 0xf8, 0x49,
0x00, 0xf1, 0xa2, 0x8c, 0x0c, 0x8a, 0xb9, 0x4f, 0x6d, 0x73, 0x3b, 0x2c, 0xb7, 0x9f, 0x8a, 0xa6, 0x7f, 0x9b, 0x9f, 0xb7,
0xa1, 0xcc, 0x74, 0x2e, 0x8f, 0xb8, 0xb0, 0x26, 0x89, 0xd2, 0xe5, 0x66, 0xe8, 0x8e, 0xa1, 0x02, 0x41, 0x00, 0xfc, 0xe7,
0x52, 0xbc, 0x4e, 0x95, 0xb6, 0x1a, 0xb4, 0x62, 0xcc, 0xd8, 0x06, 0xe1, 0xdc, 0x7a, 0xa2, 0xb6, 0x71, 0x01, 0xaa, 0x27,
0xfc, 0x99, 0xe5, 0xf2, 0x54, 0xbb, 0xb2, 0x85, 0xe1, 0x96, 0x54, 0x2d, 0xcb, 0xba, 0x86, 0xfa, 0x80, 0xdf, 0xcf, 0x39,
0xe6, 0x74, 0xcb, 0x22, 0xce, 0x70, 0xaa, 0x10, 0x00, 0x73, 0x1d, 0x45, 0x0a, 0x39, 0x51, 0x84, 0xf5, 0x15, 0x8f, 0x37,
0x76, 0x91, 0x02, 0x41, 0x00, 0xe4, 0x3f, 0xf0, 0xf4, 0xde, 0x79, 0x77, 0x48, 0x9b, 0x9c, 0x28, 0x45, 0x26, 0x57, 0x3c,
0x71, 0x40, 0x28, 0x6a, 0xa1, 0xfe, 0xc3, 0xe5, 0x37, 0xa1, 0x03, 0xf6, 0x2d, 0xbe, 0x80, 0x64, 0x72, 0x69, 0x2e, 0x9b,
0x4d, 0xe3, 0x2e, 0x1b, 0xfe, 0xe7, 0xf9, 0x77, 0x8c, 0x18, 0x53, 0x9f, 0xe2, 0xfe, 0x00, 0xbb, 0x49, 0x20, 0x47, 0xdf,
0x01, 0x61, 0x87, 0xd6, 0xe3, 0x44, 0xb5, 0x03, 0x2f, 0x02, 0x40, 0x54, 0xec, 0x7c, 0xbc, 0xdd, 0x0a, 0xaa, 0xde, 0xe6,
0xc9, 0xf2, 0x8d, 0x6c, 0x2a, 0x35, 0xf6, 0x3c, 0x63, 0x55, 0x29, 0x40, 0xf1, 0x32, 0x82, 0x9f, 0x53, 0xb3, 0x9e, 0x5f,
0xc1, 0x53, 0x52, 0x3e, 0xac, 0x2e, 0x28, 0x51, 0xa1, 0x16, 0xdb, 0x90, 0xe3, 0x99, 0x7e, 0x88, 0xa4, 0x04, 0x7c, 0x92,
0xae, 0xd2, 0xe7, 0xd4, 0xe1, 0x55, 0x20, 0x90, 0x3e, 0x3c, 0x6a, 0x63, 0xf0, 0x34, 0xf1, 0x02, 0x41, 0x00, 0x84, 0x5a,
0x17, 0x6c, 0xc6, 0x3c, 0x84, 0xd0, 0x93, 0x7a, 0xff, 0x56, 0xe9, 0x9e, 0x98, 0x2b, 0xcb, 0x5a, 0x24, 0x4a, 0xff, 0x21,
0xb4, 0x9e, 0x87, 0x3d, 0x76, 0xd8, 0x9b, 0xa8, 0x73, 0x96, 0x6c, 0x2b, 0x5c, 0x5e, 0xd3, 0xa6, 0xff, 0x10, 0xd6, 0x8e,
0xaf, 0xa5, 0x8a, 0xcd, 0xa2, 0xde, 0xcb, 0x0e, 0xbd, 0x8a, 0xef, 0xae, 0xfd, 0x3f, 0x1d, 0xc0, 0xd8, 0xf8, 0x3b, 0xf5,
0x02, 0x7d, 0x02, 0x41, 0x00, 0x8b, 0x26, 0xd3, 0x2c, 0x7d, 0x28, 0x38, 0x92, 0xf1, 0xbf, 0x15, 0x16, 0x39, 0x50, 0xc8,
0x6d, 0x32, 0xec, 0x28, 0xf2, 0x8b, 0xd8, 0x70, 0xc5, 0xed, 0xe1, 0x7b, 0xff, 0x2d, 0x66, 0x8c, 0x86, 0x77, 0x43, 0xeb,
0xb6, 0xf6, 0x50, 0x66, 0xb0, 0x40, 0x24, 0x6a, 0xaf, 0x98, 0x21, 0x45, 0x30, 0x01, 0x59, 0xd0, 0xc3, 0xfc, 0x7b, 0xae,
0x30, 0x18, 0xeb, 0x90, 0xfb, 0x17, 0xd3, 0xce, 0xb5
};

/**
 * Loop to encode a data packet nIterations times using C++.
 * @param nIterations The number of iterations.
 * @param useComplex If true, use a large name, large content and all fields.  If false, use a small name, small content
 * and only required fields.
 * @param useCrypto If true, sign the data packet.  If false, use a blank signature.
 * @param encoding Set this to the wire encoding.
 * @return The number of seconds for all iterations.
 */
static double
benchmarkEncodeDataSecondsCpp(int nIterations, bool useComplex, bool useCrypto, Block& encoding)
{
  Name name;
  Block content;
  if (useComplex) {
    // Use a large name and content.
    name = Name("/ndn/ucla.edu/apps/lwndn-test/numbers.txt/%FD%05%05%E8%0C%CE%1D/%00"); 
    
    ostringstream contentStream;
    int count = 1;
    contentStream << (count++);
    while (contentStream.str().length() < 1170)
      contentStream << " " << (count++);
    content = dataBlock(Tlv::Content, contentStream.str().c_str(), contentStream.str().length());
  }
  else {
    // Use a small name and content.
    name = Name("/test");
    content = dataBlock(Tlv::Content, "abc", 3);
  }
  std::cout << "Content size: " << content.value_size() << std::endl;
  
  // Initialize the KeyChain storage in case useCrypto is true.
  KeyChainImpl<SecPublicInfoMemory, SecTpmMemory> keyChain;

  Name keyName("/testname/dsk-123");

  // Initialize the storage.
  keyChain.addPublicKey(keyName, KEY_TYPE_RSA,
                        PublicKey(DEFAULT_PUBLIC_KEY_DER, sizeof(DEFAULT_PUBLIC_KEY_DER)));

  keyChain.setKeyPairForKeyName(keyName,
                                DEFAULT_PUBLIC_KEY_DER, sizeof(DEFAULT_PUBLIC_KEY_DER),
                                DEFAULT_PRIVATE_KEY_DER, sizeof(DEFAULT_PRIVATE_KEY_DER));

  keyChain.addCertificateAsKeyDefault(*keyChain.selfSign(keyName));
  Name certificateName = keyChain.getDefaultCertificateName();
  
  // Set up publisherPublicKeyDigest and signatureBits in case useCrypto is false.
  uint8_t signatureBitsArray[128];
  memset(signatureBitsArray, 0, sizeof(signatureBitsArray));
  Block signatureValue = dataBlock(Tlv::SignatureValue, signatureBitsArray, sizeof(signatureBitsArray));

  double start = getNowSeconds();
  for (int i = 0; i < nIterations; ++i) {
    Data data(name);
    data.setContent(content);
    if (useComplex) {
      data.setFreshnessPeriod(1000000);
    }

    if (useCrypto)
      // This sets the signature fields.
      keyChain.sign(data);
    else {
      // Imitate real sign method to set up the signature fields, but don't actually sign.
      SignatureSha256WithRsa signature;
      signature.setKeyLocator(certificateName);
      signature.setValue(signatureValue);
      data.setSignature(signature);
    }

    encoding = data.wireEncode();
  }
  double finish = getNowSeconds();
    
  return finish - start;
}

static void 
onVerified(const ptr_lib::shared_ptr<Data>& data)
{
  // Do nothing since we expect it to verify.
}

static void 
onVerifyFailed(const ptr_lib::shared_ptr<Data>& data)
{
  cout << "Signature verification: FAILED" << endl;
}

/**
 * Loop to decode a data packet nIterations times using C++.
 * @param nIterations The number of iterations.
 * @param useCrypto If true, verify the signature.  If false, don't verify.
 * @param encoding The wire encoding to decode.
 * @return The number of seconds for all iterations.
 */
static double 
benchmarkDecodeDataSecondsCpp(int nIterations, bool useCrypto, const ConstBufferPtr &encoding)
{
  // // Initialize the KeyChain storage in case useCrypto is true.
  // ptr_lib::shared_ptr<MemoryIdentityStorage> identityStorage(new MemoryIdentityStorage());
  // identityStorage->addKey(keyName, KEY_TYPE_RSA, Blob(DEFAULT_PUBLIC_KEY_DER, sizeof(DEFAULT_PUBLIC_KEY_DER)));
  
  // ptr_lib::shared_ptr<MemoryPrivateKeyStorage> privateKeyStorage(new MemoryPrivateKeyStorage());
  // KeyChain keyChain(identityStorage, privateKeyStorage);
  
  // Name keyName("/testname/DSK-123");

  size_t nameSize = 0;
  double start = getNowSeconds();
  for (int i = 0; i < nIterations; ++i) {
    Data data;
    data.wireDecode(encoding);
    
  //   if (useCrypto)
  //     keyChain.verifyData(data, onVerified, onVerifyFailed);
  }
  double finish = getNowSeconds();
 
  return finish - start;
}

/**
 * Loop to encode a data packet nIterations times using C.
 * @param nIterations The number of iterations.
 * @param useComplex If true, use a large name, large content and all fields.  If false, use a small name, small content
 * and only required fields.
 * @param useCrypto If true, sign the data packet.  If false, use a blank signature.
 * @param encoding Output buffer for the wire encoding.
 * @param maxEncodingLength The size of the encoding buffer.
 * @param encodingLength Return the number of output bytes in encoding.
 * @return The number of seconds for all iterations.
 */
static double 
benchmarkEncodeDataSecondsC
  (int nIterations, bool useComplex, bool useCrypto, uint8_t* encoding, size_t maxEncodingLength, size_t *encodingLength)
{
  struct ndn_Blob finalBlockId;
  ndn_Blob_initialize(&finalBlockId, (uint8_t*)"\x00", 1);

  struct ndn_NameComponent nameComponents[20];
  struct ndn_Name name;
  ndn_Name_initialize(&name, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
  Buffer contentBlob;
  struct ndn_Blob content;
  if (useComplex) {
    // Use a large name and content.
    ndn_Name_appendString(&name, (char*)"ndn");
    ndn_Name_appendString(&name, (char*)"ucla.edu");
    ndn_Name_appendString(&name, (char*)"apps");
    ndn_Name_appendString(&name, (char*)"lwndn-test");
    ndn_Name_appendString(&name, (char*)"numbers.txt");
    ndn_Name_appendString(&name, (char*)"\xFD\x05\x05\xE8\x0C\xCE\x1D");
    ndn_Name_appendBlob(&name, &finalBlockId);
    
    ostringstream contentStream;
    int count = 1;
    contentStream << (count++);
    while (contentStream.str().length() < 1170)
      contentStream << " " << (count++);
    contentBlob = Buffer((uint8_t*)contentStream.str().c_str(), contentStream.str().length());
  }
  else {
    // Use a small name and content.
    ndn_Name_appendString(&name, (char*)"test");
    contentBlob = Buffer((uint8_t*)"abc", 3);
  }
  ndn_Blob_initialize(&content, (uint8_t*)contentBlob.buf(), contentBlob.size());
  
  struct ndn_NameComponent certificateNameComponents[20];
  struct ndn_Name certificateName;
  ndn_Name_initialize(&certificateName, certificateNameComponents, sizeof(certificateNameComponents) / sizeof(certificateNameComponents[0]));
  ndn_Name_appendString(&certificateName, (char*)"testname");
  ndn_Name_appendString(&certificateName, (char*)"KEY");
  ndn_Name_appendString(&certificateName, (char*)"DSK-123");
  ndn_Name_appendString(&certificateName, (char*)"ID-CERT");
  ndn_Name_appendString(&certificateName, (char*)"0");
  
  // Set up publisherPublicKeyDigest and signatureBits in case useCrypto is false.
  uint8_t* publicKeyDer = DEFAULT_PUBLIC_KEY_DER;
  size_t publicKeyDerLength = sizeof(DEFAULT_PUBLIC_KEY_DER);
  uint8_t publisherPublicKeyDigestArray[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(publicKeyDer, publicKeyDerLength, publisherPublicKeyDigestArray);
  struct ndn_Blob publisherPublicKeyDigest;
  ndn_Blob_initialize(&publisherPublicKeyDigest, publisherPublicKeyDigestArray, sizeof(publisherPublicKeyDigestArray));
  uint8_t signatureBitsArray[128];
  memset(signatureBitsArray, 0, sizeof(signatureBitsArray));
  
  // Set up the private key now in case useCrypto is true.
  // Use a temporary pointer since d2i updates it.
  const uint8_t *privateKeyDerPointer = DEFAULT_PRIVATE_KEY_DER;
  RSA *privateKey = d2i_RSAPrivateKey(NULL, &privateKeyDerPointer, sizeof(DEFAULT_PRIVATE_KEY_DER));
  if (!privateKey) {
    // Don't expect this to happen.
    cout << "Error decoding private key DER" << endl;
    return 0;
  }
  
  double start = getNowSeconds();
  for (int i = 0; i < nIterations; ++i) {
    struct ndn_Data data;
    ndn_Data_initialize(&data, name.components, name.maxComponents, certificateName.components, certificateName.maxComponents);
    
    data.name = name;
    data.content = content;
    if (useComplex) {
      data.metaInfo.timestampMilliseconds = 1.3e+12;
      data.metaInfo.freshnessSeconds = 1000;
      ndn_NameComponent_initialize(&data.metaInfo.finalBlockID, finalBlockId.value, finalBlockId.length);
    }

    struct ndn_DynamicUInt8Array output;
    struct ndn_BinaryXmlEncoder encoder;
    size_t signedPortionBeginOffset, signedPortionEndOffset;
    ndn_Error error;

    data.signature.keyLocator.type = ndn_KeyLocatorType_KEYNAME;
    data.signature.keyLocator.keyName = certificateName;
    data.signature.keyLocator.keyNameType = (ndn_KeyNameType)-1;
    data.signature.publisherPublicKeyDigest.publisherPublicKeyDigest = publisherPublicKeyDigest;
    if (useCrypto) {
      // Encode once to get the signed portion.
      ndn_DynamicUInt8Array_initialize(&output, encoding, maxEncodingLength, 0);
      ndn_BinaryXmlEncoder_initialize(&encoder, &output);    
      if ((error = ndn_encodeBinaryXmlData(&data, &signedPortionBeginOffset, &signedPortionEndOffset, &encoder))) {
        cout << "Error in ndn_encodeBinaryXmlData: " << ndn_getErrorString(error) << endl;
        return 0;
      }
      
      // Imitate MemoryPrivateKeyStorage::sign.
      uint8_t digest[SHA256_DIGEST_LENGTH];
      ndn_digestSha256(encoding + signedPortionBeginOffset, signedPortionEndOffset - signedPortionBeginOffset, digest);
      unsigned int signatureBitsLength;
      if (!RSA_sign(NID_sha256, digest, sizeof(digest), signatureBitsArray, &signatureBitsLength, privateKey)) {
        // Don't expect this to happen.
        cout << "Error in RSA_sign" << endl;
        return 0;
      }    
      
      ndn_Blob_initialize(&data.signature.signature, signatureBitsArray, signatureBitsLength);
    }
    else
      // Set up the signature, but don't sign.
      ndn_Blob_initialize(&data.signature.signature, signatureBitsArray, sizeof(signatureBitsArray));

    // Assume the encoding buffer is big enough so we don't need to dynamically reallocate.
    ndn_DynamicUInt8Array_initialize(&output, encoding, maxEncodingLength, 0);
    ndn_BinaryXmlEncoder_initialize(&encoder, &output);    
    if ((error = ndn_encodeBinaryXmlData(&data, &signedPortionBeginOffset, &signedPortionEndOffset, &encoder))) {
      cout << "Error in ndn_encodeBinaryXmlData: " << ndn_getErrorString(error) << endl;
      return 0;
    }    
    *encodingLength = encoder.offset;
  }
  double finish = getNowSeconds();
  
  if (privateKey)
    RSA_free(privateKey);
  
  return finish - start;
}

/**
 * Loop to decode a data packet nIterations times using C.
 * @param nIterations The number of iterations.
 * @param useCrypto If true, verify the signature.  If false, don't verify.
 * @param encoding The buffer with wire encoding to decode.
 * @param encodingLength The number of bytes in the encoding.
 * @return The number of seconds for all iterations.
 */
static double 
benchmarkDecodeDataSecondsC(int nIterations, bool useCrypto, uint8_t* encoding, size_t encodingLength)
{
  double start = getNowSeconds();
  for (int i = 0; i < nIterations; ++i) {
    struct ndn_NameComponent nameComponents[100];
    struct ndn_NameComponent keyNameComponents[100];
    struct ndn_Data data;
    ndn_Data_initialize
      (&data, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
       keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));

    ndn_BinaryXmlDecoder decoder;
    ndn_BinaryXmlDecoder_initialize(&decoder, encoding, encodingLength);  
    size_t signedPortionBeginOffset, signedPortionEndOffset;
    ndn_Error error;
    if ((error = ndn_decodeBinaryXmlData(&data, &signedPortionBeginOffset, &signedPortionEndOffset, &decoder))) {
      cout << "Error in ndn_decodeBinaryXmlData: " << ndn_getErrorString(error) << endl;
      return 0;
    }
    
    if (useCrypto) {
      if (!verifyRsaSignature
          (encoding + signedPortionBeginOffset, signedPortionEndOffset - signedPortionBeginOffset,
           data.signature.signature.value, data.signature.signature.length,
           DEFAULT_PUBLIC_KEY_DER, sizeof(DEFAULT_PUBLIC_KEY_DER)))
        cout << "Signature verification: FAILED" << endl;
    }
  }
  double finish = getNowSeconds();
 
  return finish - start;
}

/**
 * Call benchmarkEncodeDataSecondsCpp and benchmarkDecodeDataSecondsCpp with appropriate nInterations.  Print the 
 * results to cout.
 * @param useComplex See benchmarkEncodeDataSecondsCpp.
 * @param useCrypto See benchmarkEncodeDataSecondsCpp and benchmarkDecodeDataSecondsCpp.
 */
static void
benchmarkEncodeDecodeDataCpp(bool useComplex, bool useCrypto)
{
  Block encoding;
  {
    int nIterations = useCrypto ? 20000 : 200000;
    double duration = benchmarkEncodeDataSecondsCpp(nIterations, useComplex, useCrypto, encoding);
    cout << "Encode " << (useComplex ? "complex" : "simple ") << " data C++: Crypto? " << (useCrypto ? "yes" : "no ") 
         << ", Duration sec, Hz: " << duration << ", " << (nIterations / duration) << endl;  
  }

  BufferPtr wire = ptr_lib::make_shared<Buffer>(encoding.wire(), encoding.size());
  {
    int nIterations = useCrypto ? 10000 : 1000000;
    double duration = benchmarkDecodeDataSecondsCpp(nIterations, useCrypto, wire);
    cout << "Decode " << (useComplex ? "complex" : "simple ") << " data C++: Crypto? " << (useCrypto ? "yes" : "no ") 
         << ", Duration sec, Hz: " << duration << ", " << (nIterations / duration) << endl;  
  }
}

/**
 * Call benchmarkEncodeDataSecondsC and benchmarkDecodeDataSecondsC with appropriate nInterations.  Print the 
 * results to cout.
 * @param useComplex See benchmarkEncodeDataSecondsC.
 * @param useCrypto See benchmarkEncodeDataSecondsC and benchmarkDecodeDataSecondsC.
 */
static void
benchmarkEncodeDecodeDataC(bool useComplex, bool useCrypto)
{
  uint8_t encoding[1500];
  size_t encodingLength;
  {
    int nIterations = useCrypto ? 20000 : 10000000;
    double duration = benchmarkEncodeDataSecondsC(nIterations, useComplex, useCrypto, encoding, sizeof(encoding), &encodingLength);
    cout << "Encode " << (useComplex ? "complex" : "simple ") << " data C:   Crypto? " << (useCrypto ? "yes" : "no ") 
         << ", Duration sec, Hz: " << duration << ", " << (nIterations / duration) << endl;  
  }
  {
    int nIterations = useCrypto ? 150000 : 15000000;
    double duration = benchmarkDecodeDataSecondsC(nIterations, useCrypto, encoding, encodingLength);
    cout << "Decode " << (useComplex ? "complex" : "simple ") << " data C:   Crypto? " << (useCrypto ? "yes" : "no ") 
         << ", Duration sec, Hz: " << duration << ", " << (nIterations / duration) << endl;  
  }
}

int 
main(int argc, char** argv)
{
  try {
    benchmarkEncodeDecodeDataCpp(false, false);
    benchmarkEncodeDecodeDataCpp(true, false);
    benchmarkEncodeDecodeDataCpp(false, true);
    benchmarkEncodeDecodeDataCpp(true, true);
    
    benchmarkEncodeDecodeDataC(false, false);
    benchmarkEncodeDecodeDataC(true, false);
    benchmarkEncodeDecodeDataC(false, true);
    benchmarkEncodeDecodeDataC(true, true);
  } catch (std::exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
