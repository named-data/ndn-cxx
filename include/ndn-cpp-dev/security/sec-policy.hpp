/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_POLICY_HPP
#define NDN_SEC_POLICY_HPP

#include "../data.hpp"
#include "validation-request.hpp"

namespace ndn {
  
/**
 * A SecPolicy is an abstract base class to represent the policy for verifying data packets.
 * You must create an object of a subclass.
 */
class SecPolicy {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * The virtual destructor.
   */
  virtual
  ~SecPolicy() {}

  /**
   * Check if the received data packet can escape from verification and be trusted as valid.
   * @param data The received data packet.
   * @return true if the data does not need to be verified to be trusted as valid, otherwise false.
   */
  virtual bool 
  skipVerifyAndTrust(const Data& data) = 0;

  /**
   * Check if this SecPolicy has a verification rule for the received data.
   * @param data The received data packet.
   * @return true if the data must be verified, otherwise false.
   */
  virtual bool
  requireVerify(const Data& data) = 0;

  /**
   * Check whether the received data packet complies with the verification policy, and get the indication of the next verification step.
   * @param data The Data object with the signature to check.
   * @param stepCount The number of verification steps that have been done, used to track the verification progress.
   * @param onVerified If the signature is verified, this calls onVerified(data).
   * @param onVerifyFailed If the signature check fails, this calls onVerifyFailed(data).
   * @return the indication of next verification step, null if there is no further step.
   */
  virtual ptr_lib::shared_ptr<ValidationRequest>
  checkVerificationPolicy
    (const ptr_lib::shared_ptr<Data>& data, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed) = 0;
    
  /**
   * Check if the signing certificate name and data name satisfy the signing policy.
   * @param dataName The name of data to be signed.
   * @param certificateName The name of signing certificate.
   * @return true if the signing certificate can be used to sign the data, otherwise false.
   */
  virtual bool 
  checkSigningPolicy(const Name& dataName, const Name& certificateName) = 0;
    
  /**
   * Infer the signing identity name according to the policy. If the signing identity cannot be inferred, return an empty name.
   * @param dataName The name of data to be signed.
   * @return The signing identity or an empty name if cannot infer. 
   */
  virtual Name 
  inferSigningIdentity(const Name& dataName) = 0;
};

}

#endif
