/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../c/util/crypto.h"
#include "../c/encoding/binary-xml-data.h"
#include "../encoding/binary-xml-encoder.hpp"
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include "../util/logging.hpp"
#include <ndn-cpp/security/security-exception.hpp>
#include <ndn-cpp/security/policy/policy-manager.hpp>
#include "policy/validation-request.hpp"
#include <ndn-cpp/security/key-chain.hpp>

using namespace std;
using namespace ndn::ptr_lib;
using namespace ndn::func_lib;
#if NDN_CPP_HAVE_STD_FUNCTION
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

namespace ndn {

/**
 * Verify the signature on the data packet using the given public key.  If there is no data.getDefaultWireEncoding(),
 * this calls data.wireEncode() to set it.
 * @param data The data packet with the signed portion and the signature to verify.  The data packet must have a
 * Sha256WithRsaSignature.
 * @param publicKey The public key used to verify the signature.
 * @return true if the signature verifies, false if not.
 * @throw SecurityException if data does not have a Sha256WithRsaSignature.
 */
static bool
verifySha256WithRsaSignature(const Data& data, const PublicKey& publicKey)
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
  
KeyChain::KeyChain(const shared_ptr<IdentityManager>& identityManager, const shared_ptr<PolicyManager>& policyManager)
: identityManager_(identityManager), policyManager_(policyManager), face_(0), maxSteps_(100)
{  
}

void 
KeyChain::sign(Data& data, const Name& certificateName, WireFormat& wireFormat)
{
  identityManager_->signByCertificate(data, certificateName, wireFormat);
}

shared_ptr<Signature> 
KeyChain::sign(const uint8_t* buffer, size_t bufferLength, const Name& certificateName)
{
  return identityManager_->signByCertificate(buffer, bufferLength, certificateName);
}

void 
KeyChain::signByIdentity(Data& data, const Name& identityName, WireFormat& wireFormat)
{
  Name signingCertificateName;
  
  if (identityName.getComponentCount() == 0) {
    Name inferredIdentity = policyManager_->inferSigningIdentity(data.getName());
    if (inferredIdentity.getComponentCount() == 0)
      signingCertificateName = identityManager_->getDefaultCertificateName();
    else
      signingCertificateName = identityManager_->getDefaultCertificateNameForIdentity(inferredIdentity);    
  }
  else
    signingCertificateName = identityManager_->getDefaultCertificateNameForIdentity(identityName);

  if (signingCertificateName.getComponentCount() == 0)
    throw SecurityException("No qualified certificate name found!");

  if (!policyManager_->checkSigningPolicy(data.getName(), signingCertificateName))
    throw SecurityException("Signing Cert name does not comply with signing policy");

  identityManager_->signByCertificate(data, signingCertificateName, wireFormat);  
}

shared_ptr<Signature> 
KeyChain::signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName)
{
  Name signingCertificateName = identityManager_->getDefaultCertificateNameForIdentity(identityName);
    
  if (signingCertificateName.size() == 0)
    throw SecurityException("No qualified certificate name found!");

  return identityManager_->signByCertificate(buffer, bufferLength, signingCertificateName);
}

void
KeyChain::verifyData
  (const shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed, int stepCount)
{
  _LOG_TRACE("Enter Verify");

  if (policyManager_->requireVerify(*data)) {
    shared_ptr<ValidationRequest> nextStep = policyManager_->checkVerificationPolicy
      (data, stepCount, onVerified, onVerifyFailed);
    if (nextStep)
      face_->expressInterest
        (*nextStep->interest_, 
         bind(&KeyChain::onCertificateData, this, _1, _2, nextStep), 
         bind(&KeyChain::onCertificateInterestTimeout, this, _1, nextStep->retry_, onVerifyFailed, data, nextStep));
  }
  else if (policyManager_->skipVerifyAndTrust(*data))
    onVerified(data);
  else
    onVerifyFailed(data);
}

void
KeyChain::onCertificateData(const shared_ptr<const Interest> &interest, const shared_ptr<Data> &data, shared_ptr<ValidationRequest> nextStep)
{
  // Try to verify the certificate (data) according to the parameters in nextStep.
  verifyData(data, nextStep->onVerified_, nextStep->onVerifyFailed_, nextStep->stepCount_);
}

void
KeyChain::onCertificateInterestTimeout
  (const shared_ptr<const Interest> &interest, int retry, const OnVerifyFailed& onVerifyFailed, const shared_ptr<Data> &data, 
   shared_ptr<ValidationRequest> nextStep)
{
  if (retry > 0)
    // Issue the same expressInterest as in verifyData except decrement retry.
    face_->expressInterest
      (*interest, 
       bind(&KeyChain::onCertificateData, this, _1, _2, nextStep), 
       bind(&KeyChain::onCertificateInterestTimeout, this, _1, retry - 1, onVerifyFailed, data, nextStep));
  else
    onVerifyFailed(data);
}

}
