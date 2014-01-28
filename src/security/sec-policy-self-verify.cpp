/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifdef TEMPRORARILY_DISABLED

#include "../c/util/crypto.h"
#include "security/identity-storage.hpp"
#include "security/sec-policy-self-verify.hpp"

using namespace std;

namespace ndn {

SecPolicySelfVerify::~SecPolicySelfVerify()
{
}

ptr_lib::shared_ptr<ValidationRequest>
SecPolicySelfVerify::checkVerificationPolicy
  (const ptr_lib::shared_ptr<Data>& data, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed)
{ 
  // Cast to const Data* so that we use the const version of getSignature() and don't reset the default encoding.
  const Sha256WithRsaSignature *signature = dynamic_cast<const Sha256WithRsaSignature*>(((const Data*)data.get())->getSignature());
  if (!signature)
    throw SecurityException("SecPolicySelfVerify: Signature is not Sha256WithRsaSignature.");
  
  if (signature->getKeyLocator().getType() == ndn_KeyLocatorType_KEY) {
    // Use the public key DER directly.
    if (verifySha256WithRsaSignature(*data, signature->getKeyLocator().getKeyData()))
      onVerified(data);
    else
      onVerifyFailed(data); 
  }
  else if (signature->getKeyLocator().getType() == ndn_KeyLocatorType_KEYNAME && identityStorage_) {
    // Assume the key name is a certificate name.
    Blob publicKeyDer = identityStorage_->getKey
      (IdentityCertificate::certificateNameToPublicKeyName(signature->getKeyLocator().getKeyName()));
    if (!publicKeyDer)
      // Can't find the public key with the name.
      onVerifyFailed(data);
    
    if (verifySha256WithRsaSignature(*data, publicKeyDer))
      onVerified(data);
    else
      onVerifyFailed(data); 
  }
  else
    // Can't find a key to verify.
    onVerifyFailed(data); 
  
  // No more steps, so return a null ValidationRequest.
  return ptr_lib::shared_ptr<ValidationRequest>();
}

}

#endif // TEMPORARILY_DISABLED
