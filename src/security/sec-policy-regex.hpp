/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SEC_POLICY_REGEX_HPP
#define NDN_SEC_POLICY_REGEX_HPP

#include "sec-policy.hpp"
#include "identity-certificate.hpp"
#include "sec-rule-relative.hpp"
#include "certificate-cache.hpp"
#include "../util/regex.hpp"

#include <map>




namespace ndn {

class SecPolicyRegex : public SecPolicy
{
public:
  struct Error : public SecPolicy::Error { Error(const std::string &what) : SecPolicy::Error(what) {} };
  
  SecPolicyRegex(shared_ptr<CertificateCache> certificateCache, const int stepLimit = 10);
  
  virtual 
  ~SecPolicyRegex() {}
  
  virtual shared_ptr<ValidationRequest>
  checkVerificationPolicy(const shared_ptr<Data>& data, 
                          int stepCount, 
                          const OnVerified& onVerified, 
                          const OnVerifyFailed& onVerifyFailed);
      
  /**
   * @brief add a rule to check whether the data name and signing certificate name comply with the policy
   * @param policy the verification policy
   */
  inline virtual void
  addVerificationPolicyRule (shared_ptr<SecRuleRelative> rule);
    
  /**
   * @brief add a trust anchor
   * @param certificate the trust anchor 
   */
  inline virtual void 
  addTrustAnchor(shared_ptr<IdentityCertificate> certificate);

protected:
  virtual void
  onCertificateVerified(shared_ptr<Data> certificate, 
                        shared_ptr<Data> data, 
                        const OnVerified& onVerified, 
                        const OnVerifyFailed& onVerifyFailed);
  
  virtual void
  onCertificateVerifyFailed(shared_ptr<Data>signCertificate, 
                            shared_ptr<Data>data, 
                            const OnVerifyFailed& onVerifyFailed);
  
protected:
  typedef std::vector< shared_ptr<SecRuleRelative> > RuleList;
  typedef std::vector< shared_ptr<Regex> > RegexList;

  int m_stepLimit;
  shared_ptr<CertificateCache> m_certificateCache;
  RuleList m_mustFailVerify;
  RuleList m_verifyPolicies;
  std::map<Name, shared_ptr<IdentityCertificate> > m_trustAnchors;
};

void 
SecPolicyRegex::addVerificationPolicyRule (shared_ptr<SecRuleRelative> rule)
{ rule->isPositive() ? m_verifyPolicies.push_back(rule) : m_mustFailVerify.push_back(rule); }
      
void  
SecPolicyRegex::addTrustAnchor(shared_ptr<IdentityCertificate> certificate)
{ m_trustAnchors[certificate->getName().getPrefix(-1)] = certificate; }

}//ndn

#endif
