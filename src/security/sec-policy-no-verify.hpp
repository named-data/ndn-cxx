/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_POLICY_NO_VERIFY_HPP
#define NDN_SEC_POLICY_NO_VERIFY_HPP

#include "sec-policy.hpp"

namespace ndn {

class SecPolicyNoVerify : public SecPolicy {
public:
  /**
   * The virtual destructor.
   */
  virtual
  ~SecPolicyNoVerify();

  /**
   * Override to always skip verification and trust as valid.
   * @param data The received data packet.
   * @return true.
   */
  virtual bool 
  skipVerifyAndTrust(const Data& data);

  /**
   * Override to return false for no verification rule for the received data.
   * @param data The received data packet.
   * @return false.
   */
  virtual bool
  requireVerify(const Data& data);

  /**
   * Override to call onVerified(data) and to indicate no further verification step.
   * @param data The Data object with the signature to check.
   * @param stepCount The number of verification steps that have been done, used to track the verification progress.
   * @param onVerified This does override to call onVerified(data).
   * @param onVerifyFailed Override to ignore this.
   * @return null for no further step.
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
