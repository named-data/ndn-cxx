/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/policy/no-verify-policy-manager.hpp>

using namespace std;

namespace ndn {

NoVerifyPolicyManager::~NoVerifyPolicyManager()
{
}

bool 
NoVerifyPolicyManager::skipVerifyAndTrust(const Data& data)
{ 
  return true; 
}

bool
NoVerifyPolicyManager::requireVerify(const Data& data)
{ 
  return false; 
}
    
ptr_lib::shared_ptr<ValidationRequest>
NoVerifyPolicyManager::checkVerificationPolicy
  (const ptr_lib::shared_ptr<Data>& data, int stepCount, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed)
{ 
  onVerified(data); 
  return ptr_lib::shared_ptr<ValidationRequest>();
}

bool 
NoVerifyPolicyManager::checkSigningPolicy(const Name& dataName, const Name& certificateName)
{ 
  return true; 
}

Name 
NoVerifyPolicyManager::inferSigningIdentity(const Name& dataName)
{ 
  return Name(); 
}

}
