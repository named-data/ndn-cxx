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
    if (nextStep) {
#if 0 // TODO: implement
      Ptr<Closure> closure = Ptr<Closure> (new Closure(nextStep->m_verifiedCallback,
                                                       boost::bind(&Keychain::onCertificateInterestTimeout, 
                                                                   this, 
                                                                   _1, 
                                                                   _2, 
                                                                   nextStep->m_retry,
                                                                   unverifiedCallback,
                                                                   data),
                                                       nextStep->m_unverifiedCallback,
                                                       nextStep->m_stepCount)
                                           );
            
      face_->expressInterest(nextStep->m_interest, closure);
#else
      throw SecurityException("KeyChain::verifyData: Use of ValidationRequest not implemented.");
#endif
    }
  }
  else if (policyManager_->skipVerifyAndTrust(*data))
    onVerified(data);
  else
    onVerifyFailed(data);
}

}
