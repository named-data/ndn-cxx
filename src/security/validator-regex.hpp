/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_VALIDATOR_REGEX_HPP
#define NDN_SECURITY_VALIDATOR_REGEX_HPP

#include "validator.hpp"
#include "identity-certificate.hpp"
#include "sec-rule-relative.hpp"
#include "certificate-cache.hpp"
#include "../util/regex.hpp"

namespace ndn {

class ValidatorRegex : public Validator
{
public:
  struct Error : public Validator::Error { Error(const std::string &what) : Validator::Error(what) {} };

  static const shared_ptr<CertificateCache> DefaultCertificateCache;
  
  ValidatorRegex(shared_ptr<Face> face,
                 shared_ptr<CertificateCache> certificateCache = DefaultCertificateCache, 
                 const int stepLimit = 3);
  
  virtual 
  ~ValidatorRegex() {}
  
  /**
   * @brief Add a rule for data verification.
   *
   * @param policy The verification rule
   */
  inline void
  addDataVerificationRule (shared_ptr<SecRuleRelative> rule);
    
  /**
   * @brief Add a trust anchor
   *
   * @param certificate The trust anchor 
   */
  inline void 
  addTrustAnchor(shared_ptr<IdentityCertificate> certificate);

protected:
  virtual void
  checkPolicy (const Data& data, 
               int stepCount, 
               const OnDataValidated &onValidated, 
               const OnDataValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps);

  virtual void
  checkPolicy (const Interest& interest, 
               int stepCount, 
               const OnInterestValidated &onValidated, 
               const OnInterestValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps)
  { onValidationFailed(interest.shared_from_this()); }

  void
  onCertificateValidated(const shared_ptr<const Data> &signCertificate, 
                         const shared_ptr<const Data> &data, 
                         const OnDataValidated &onValidated, 
                         const OnDataValidationFailed &onValidationFailed);
  
  void
  onCertificateValidationFailed(const shared_ptr<const Data> &signCertificate, 
                                const shared_ptr<const Data> &data, 
                                const OnDataValidationFailed &onValidationFailed);
  
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
ValidatorRegex::addDataVerificationRule (shared_ptr<SecRuleRelative> rule)
{ rule->isPositive() ? m_verifyPolicies.push_back(rule) : m_mustFailVerify.push_back(rule); }
      
void  
ValidatorRegex::addTrustAnchor(shared_ptr<IdentityCertificate> certificate)
{ m_trustAnchors[certificate->getName().getPrefix(-1)] = certificate; }

} // namespace ndn

#endif //NDN_SECURITY_VALIDATOR_REGEX_HPP
