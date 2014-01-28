/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "security/sec-policy-no-verify.hpp"

using namespace std;

namespace ndn {

SecPolicyNoVerify::~SecPolicyNoVerify()
{
}
    
ptr_lib::shared_ptr<ValidationRequest>
SecPolicyNoVerify::checkVerificationPolicy
  (const ptr_lib::shared_ptr<const Data>& data, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed)
{ 
  onVerified(); 
  return ptr_lib::shared_ptr<ValidationRequest>();
}

ptr_lib::shared_ptr<ValidationRequest>
SecPolicyNoVerify::checkVerificationPolicy
  (const ptr_lib::shared_ptr<const Interest>& interest, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed)
{ 
  onVerified(); 
  return ptr_lib::shared_ptr<ValidationRequest>();
}


}
