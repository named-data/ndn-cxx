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
   * Check whether the received data packet complies with the verification policy, and get the indication of the next verification step.
   * If there is no next verification step, that imlies policy MUST have already made the verification decision.
   * i.e., either onVerified or onVerifyFailed callback is invoked.
   * @param data The Data object with the signature to check.
   * @param stepCount The number of verification steps that have been done, used to track the verification progress.
   * @param onVerified If the signature is verified, this calls onVerified(data).
   * @param onVerifyFailed If the signature check fails, this calls onVerifyFailed(data).
   * @return the indication of next verification step, null if there is no further step.
   */
  virtual ptr_lib::shared_ptr<ValidationRequest>
  checkVerificationPolicy
    (const ptr_lib::shared_ptr<const Data>& data, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed);

  /**
   * Check whether the received interest packet complies with the verification policy, and get the indication of the next verification step.
   * If there is no next verification step, that implies policy MUST have already made the verification decision.
   * i.e., either onVerified or onVerifyFailed callback is invoked.
   * @param data The Data object with the signature to check.
   * @param stepCount The number of verification steps that have been done, used to track the verification progress.
   * @param onVerified If the signature is verified, this calls onVerified(data).
   * @param onVerifyFailed If the signature check fails, this calls onVerifyFailed(data).
   * @return the indication of next verification step, null if there is no further step.
   */
  virtual ptr_lib::shared_ptr<ValidationRequest>
  checkVerificationPolicy
    (const ptr_lib::shared_ptr<const Interest>& interest, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed);
};

}

#endif
