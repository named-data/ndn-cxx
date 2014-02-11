/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMAND_INTEREST_VALIDATOR_HPP
#define NDN_COMMAND_INTEREST_VALIDATOR_HPP

#include "../security/validator.hpp"
#include "../security/identity-certificate.hpp"
#include "../security/sec-rule-specific.hpp"


namespace ndn
{

class CommandInterestValidator : public Validator
{
public:
  static const ssize_t POS_SIG_VALUE;
  static const ssize_t POS_SIG_INFO;
  static const ssize_t POS_RANDOM_VAL;
  static const ssize_t POS_TIMESTAMP;
  static const int64_t GRACE_INTERVAL;

  CommandInterestValidator(int64_t graceInterval = GRACE_INTERVAL) 
  { m_graceInterval = (graceInterval < 0 ? GRACE_INTERVAL : graceInterval); }

  virtual
  ~CommandInterestValidator() {}

  inline void
  addInterestRule(const std::string& regex, const IdentityCertificate& certificate);

  inline void
  addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey);

protected:
  virtual void
  checkPolicy (const Data& data, 
               int stepCount, 
               const OnDataValidated &onValidated, 
               const OnDataValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps)
  { onValidationFailed(data.shared_from_this()); }
  
  virtual void
  checkPolicy (const Interest& interest, 
               int stepCount, 
               const OnInterestValidated &onValidated, 
               const OnInterestValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps);
private:
  int64_t m_graceInterval; //ms
  std::map<Name, PublicKey> m_trustAnchorsForInterest;
  std::list<SecRuleSpecific> m_trustScopeForInterest;
  std::map<Name, uint64_t> m_lastTimestamp;
};

void
CommandInterestValidator::addInterestRule(const std::string& regex, const IdentityCertificate& certificate)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());
  addInterestRule(regex, keyName, certificate.getPublicKeyInfo());
}

void
CommandInterestValidator::addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey)
{
  m_trustAnchorsForInterest[keyName] = publicKey;
  shared_ptr<Regex> interestRegex = make_shared<Regex>(regex);
  shared_ptr<Regex> signerRegex = Regex::fromName(keyName, true);
  m_trustScopeForInterest.push_back(SecRuleSpecific(interestRegex, signerRegex));
}

}//ndn

#endif
