/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <openssl/ssl.h>
#include "c/encoding/binary-xml-data.h"
#include "encoding/binary-xml-encoder.hpp"
#include "key-chain.hpp"

using namespace std;

namespace ndn {

static unsigned char DEFAULT_PUBLIC_KEY[] = {
0x30, 0x81, 0x9F, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81,
0x8D, 0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xE1, 0x7D, 0x30, 0xA7, 0xD8, 0x28, 0xAB, 0x1B, 0x84, 0x0B, 0x17,
0x54, 0x2D, 0xCA, 0xF6, 0x20, 0x7A, 0xFD, 0x22, 0x1E, 0x08, 0x6B, 0x2A, 0x60, 0xD1, 0x6C, 0xB7, 0xF5, 0x44, 0x48, 0xBA,
0x9F, 0x3F, 0x08, 0xBC, 0xD0, 0x99, 0xDB, 0x21, 0xDD, 0x16, 0x2A, 0x77, 0x9E, 0x61, 0xAA, 0x89, 0xEE, 0xE5, 0x54, 0xD3,
0xA4, 0x7D, 0xE2, 0x30, 0xBC, 0x7A, 0xC5, 0x90, 0xD5, 0x24, 0x06, 0x7C, 0x38, 0x98, 0xBB, 0xA6, 0xF5, 0xDC, 0x43, 0x60,
0xB8, 0x45, 0xED, 0xA4, 0x8C, 0xBD, 0x9C, 0xF1, 0x26, 0xA7, 0x23, 0x44, 0x5F, 0x0E, 0x19, 0x52, 0xD7, 0x32, 0x5A, 0x75,
0xFA, 0xF5, 0x56, 0x14, 0x4F, 0x9A, 0x98, 0xAF, 0x71, 0x86, 0xB0, 0x27, 0x86, 0x85, 0xB8, 0xE2, 0xC0, 0x8B, 0xEA, 0x87,
0x17, 0x1B, 0x4D, 0xEE, 0x58, 0x5C, 0x18, 0x28, 0x29, 0x5B, 0x53, 0x95, 0xEB, 0x4A, 0x17, 0x77, 0x9F, 0x02, 0x03, 0x01,
0x00, 01  
};

/**
 * Set digest to the sha-256 digest of data
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @param digest Set this to the resulting digest.
 */
static void setSha256(unsigned char *data, unsigned int dataLength, vector<unsigned char> &digest)
{
  unsigned char digestBuffer[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, dataLength);
  SHA256_Final(digestBuffer, &sha256);
  setVector(digest, digestBuffer, sizeof(digestBuffer));
}

/**
 * Encode the fields of the Data object and set digest to the sha-256 digest.
 * @param data The Data object with the fields to digest.
 * @param digest Set this to the resulting digest.
 */
static void setDataFieldsSha256(const Data &data, vector<unsigned char> &digest)
{
  // Imitate BinaryXmlWireFormat::encodeData.
  struct ndn_NameComponent nameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_init(&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]));
  data.get(dataStruct);

  BinaryXmlEncoder encoder;
  unsigned int signedFieldsBeginOffset, signedFieldsEndOffset;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlData(&dataStruct, &signedFieldsBeginOffset, &signedFieldsEndOffset, &encoder)))
    throw std::runtime_error(ndn_getErrorString(error));
  
  setSha256(encoder.output.array + signedFieldsBeginOffset, signedFieldsEndOffset - signedFieldsBeginOffset, digest);
}

void KeyChain::defaultSign(Data &data)
{
  setSha256(DEFAULT_PUBLIC_KEY, sizeof(DEFAULT_PUBLIC_KEY), data.getSignedInfo().getPublisherPublicKeyDigest().getPublisherPublicKeyDigest());
  // TODO: Get the real current timestamp.
  data.getSignedInfo().setTimestampMilliseconds(256);
  data.getSignedInfo().getKeyLocator().setType(ndn_KeyLocatorType_KEY);
  data.getSignedInfo().getKeyLocator().setKeyOrCertificate(DEFAULT_PUBLIC_KEY, sizeof(DEFAULT_PUBLIC_KEY));
  
  data.getSignature().getSignature().clear();
  vector<unsigned char> dataFieldsDigest;
  setDataFieldsSha256(data, dataFieldsDigest);
  // TODO: use RSA_size to get the proper size of the signature buffer.
  unsigned char signature[1000];
  unsigned int signatureLength;
  RSA *privateKey;
  if (!RSA_sign(NID_sha256, &dataFieldsDigest[0], dataFieldsDigest.size(), signature, &signatureLength, privateKey))
    throw std::runtime_error("Errir in RSA_sign");
  
  data.getSignature().setSignature(signature, signatureLength);
}


}
