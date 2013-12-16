/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../../c/util/crypto.h"
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include <ndn-cpp/security/security-exception.hpp>
#include <ndn-cpp/security/signature/sha256-with-rsa-handler.hpp>

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

bool
Sha256WithRsaHandler::verify(const Data& data, const PublicKey& publicKey)
{
  const Sha256WithRsaSignature *signature = dynamic_cast<const Sha256WithRsaSignature*>(data.getSignature());
  if (!signature)
    throw SecurityException("signature is not Sha256WithRsaSignature.");
  
  // Set the data packet's default wire encoding if it is not already there.
  if (signature->getDigestAlgorithm().size() != 0)
    // TODO: Allow a non-default digest algorithm.
    throw UnrecognizedDigestAlgorithmException("Cannot verify a data packet with a non-default digest algorithm.");
  if (!data.getDefaultWireEncoding())
    data.wireEncode();
  
  // Set signedPortionDigest to the digest of the signed portion of the wire encoding.
  uint8_t signedPortionDigest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(data.getDefaultWireEncoding().signedBuf(), data.getDefaultWireEncoding().signedSize(), signedPortionDigest);
  
  // Verify the signedPortionDigest.
  // Use a temporary pointer since d2i updates it.
  const uint8_t *derPointer = publicKey.getKeyDer().buf();
  RSA *rsaPublicKey = d2i_RSA_PUBKEY(NULL, &derPointer, publicKey.getKeyDer().size());
  if (!rsaPublicKey)
    throw UnrecognizedKeyFormatException("Error decoding public key in d2i_RSAPublicKey");
  int success = RSA_verify
    (NID_sha256, signedPortionDigest, sizeof(signedPortionDigest), (uint8_t *)signature->getSignature().buf(),
     signature->getSignature().size(), rsaPublicKey);
  // Free the public key before checking for success.
  RSA_free(rsaPublicKey);
  
  // RSA_verify returns 1 for a valid signature.
  return (success == 1);
}

}
