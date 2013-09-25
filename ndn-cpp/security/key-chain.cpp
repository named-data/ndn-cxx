/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../c/util/crypto.h"
#include "../c/encoding/binary-xml-data.h"
#include "../encoding/binary-xml-encoder.hpp"
#include "../sha256-with-rsa-signature.hpp"
#include "../util/logging.hpp"
#include "security-exception.hpp"
#include "key-chain.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

#if 1
static uint8_t DEFAULT_PUBLIC_KEY_DER[] = {
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
#endif

KeyChain::KeyChain(const shared_ptr<IdentityManager>& identityManager)
: identityManager_(identityManager), face_(0), maxSteps_(100)
{  
}

static bool 
verifySignature(const Data& data /*, const Publickey& publickey */)
{
#if 0
  using namespace CryptoPP;

  Blob unsignedData(data.getSignedBlob()->signed_buf(), data.getSignedBlob()->signed_size());
  bool result = false;
    
  // Temporarily hardwire.  It should be assigned by Signature.getAlgorithm().
  DigestAlgorithm digestAlg = DIGEST_SHA256;
  // Temporarily hardwire.  It should be assigned by Publickey.getKeyType().
  KeyType keyType = KEY_TYPE_RSA; 
  if (keyType == KEY_TYPE_RSA) {
    RSA::PublicKey pubKey;
    ByteQueue queue;

    queue.Put((const byte*)publickey.getKeyBlob ().buf (), publickey.getKeyBlob ().size ());
    pubKey.Load(queue);

    if (DIGEST_SHA256 == digestAlg) {
      Ptr<const signature::Sha256WithRsa> sigPtr = boost::dynamic_pointer_cast<const signature::Sha256WithRsa> (data.getSignature());
      const Blob & sigBits = sigPtr->getSignatureBits();

      RSASS<PKCS1v15, SHA256>::Verifier verifier (pubKey);
      result = verifier.VerifyMessage((const byte*) unsignedData.buf(), unsignedData.size(), (const byte*)sigBits.buf(), sigBits.size());            
      _LOG_DEBUG("Signature verified? " << data.getName() << " " << boolalpha << result);      
    }
  }
   
 return result;
#else
  const Sha256WithRsaSignature *signature = dynamic_cast<const Sha256WithRsaSignature*>(data.getSignature());
  if (!signature)
    throw SecurityException("signature is not Sha256WithRsaSignature.");
  
  if (signature->getDigestAlgorithm().size() != 0)
    // TODO: Allow a non-default digest algorithm.
    throw UnrecognizedDigestAlgorithmException("Cannot verify a data packet with a non-default digest algorithm.");
  if (!data.getWireEncoding())
    // Don't expect this to happen
    throw SecurityException("The Data wireEncoding is null.");
  uint8_t signedPortionDigest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(data.getWireEncoding().signedBuf(), data.getWireEncoding().signedSize(), signedPortionDigest);
  
  // Verify the signedPortionDigest.
  // Use a temporary pointer since d2i updates it.
  const uint8_t *derPointer = DEFAULT_PUBLIC_KEY_DER;
  RSA *publicKey = d2i_RSA_PUBKEY(NULL, &derPointer, sizeof(DEFAULT_PUBLIC_KEY_DER));
  if (!publicKey)
    throw UnrecognizedKeyFormatException("Error decoding public key in d2i_RSAPublicKey");
  int success = RSA_verify
    (NID_sha256, signedPortionDigest, sizeof(signedPortionDigest), (uint8_t *)signature->getSignature().buf(), 
     signature->getSignature().size(), publicKey);
  // Free the public key before checking for success.
  RSA_free(publicKey);
  
  return (success == 1);
#endif
}

void 
KeyChain::signData(Data& data, const Name& certificateNameIn, WireFormat& wireFormat)
{
  Name inferredCertificateName;
  const Name* certificateName;
  
  if (certificateNameIn.getComponentCount() == 0) {
#if 0
    inferredCertificateName = identityManager_->getDefaultCertificateNameForIdentity(policyManager_->inferSigningIdentity(data.getName ()));
#else
    inferredCertificateName = Name();
#endif
    if (inferredCertificateName.getComponentCount() == 0)
      throw SecurityException("No qualified certificate name can be inferred");
    
    certificateName = &inferredCertificateName;
  }
  else
    certificateName = &certificateNameIn;
        
#if 0
  if (!policyManager_->checkSigningPolicy (data.getName (), certificateName))
    throw SecurityException("Signing Cert name does not comply with signing policy");
#endif
  
  identityManager_->signByCertificate(data, *certificateName, wireFormat);  
}

void
KeyChain::verifyData
  (const shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed, int stepCount)
{
  _LOG_TRACE("Enter Verify");

#if 0
  if (m_policyManager->requireVerify(*dataPtr))
    stepVerify(dataPtr, true, m_maxStep, onVerified, onVerifyFailed);
  else if(m_policyManager->skipVerify(*dataPtr))
#else
  if (verifySignature(*data))
#endif
    onVerified(data);
  else
    onVerifyFailed();
}

}
