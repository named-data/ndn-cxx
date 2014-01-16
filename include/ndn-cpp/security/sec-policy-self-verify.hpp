/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_POLICY_SELF_VERIFY_HPP
#define NDN_SEC_POLICY_SELF_VERIFY_HPP

#include "sec-policy.hpp"

namespace ndn {

/**
 * A SecPolicySelfVerify implements a PolicyManager to use the public key DER in the data packet's KeyLocator (if available)
 * or look in the IdentityStorage for the public key with the name in the KeyLocator (if available) and use
 * it to verify the data packet, without searching a certificate chain.  If the public key can't be found, the
 * verification fails.
 */
class SecPolicySelfVerify : public SecPolicy {
public:
  /**
   * Create a new SecPolicySelfVerify which will look up the public key in the given identityManager.
   * @param identityManager (optional) The IdentityManager for looking up the public key.  This points to an object must which remain 
   * valid during the life of this SecPolicySelfVerify.  If omitted, then don't look for a public key with the name 
   * in the KeyLocator and rely on the KeyLocator having the full public key DER.
   */
  SecPolicySelfVerify()
  {
  }
  
  /**
   * The virtual destructor.
   */
  virtual
  ~SecPolicySelfVerify();

  /**
   * Never skip verification.
   * @param data The received data packet.
   * @return false.
   */
  virtual bool 
  skipVerifyAndTrust(const Data& data);

  /**
   * Always return true to use the self-verification rule for the received data.
   * @param data The received data packet.
   * @return true.
   */
  virtual bool
  requireVerify(const Data& data);

  /**
   * Use the public key DER in the data packet's KeyLocator (if available) or look in the IdentityStorage for the 
   * public key with the name in the KeyLocator (if available) and use it to verify the data packet.  If the public key can't 
   * be found, call onVerifyFailed.
   * @param data The Data object with the signature to check.
   * @param stepCount The number of verification steps that have been done, used to track the verification progress.
   * (stepCount is ignored.)
   * @param onVerified If the signature is verified, this calls onVerified(data).
   * @param onVerifyFailed If the signature check fails or can't find the public key, this calls onVerifyFailed(data).
   * @return null for no further step for looking up a certificate chain.
   */
  virtual ptr_lib::shared_ptr<ValidationRequest>
  checkVerificationPolicy
    (const ptr_lib::shared_ptr<Data>& data, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed);
    
  /**
   * Override to always indicate that the signing certificate name and data name satisfy the signing policy.
   * @param dataName The name of data to be signed.
   * @param certificateName The name of signing certificate.
   * @return true to indicate that the signing certificate can be used to sign the data.
   */
  virtual bool 
  checkSigningPolicy(const Name& dataName, const Name& certificateName);
    
  /**
   * Override to indicate that the signing identity cannot be inferred.
   * @param dataName The name of data to be signed.
   * @return An empty name because cannot infer. 
   */
  virtual Name 
  inferSigningIdentity(const Name& dataName);
  
};

}

#endif
