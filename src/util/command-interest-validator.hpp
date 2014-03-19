/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_COMMAND_INTEREST_VALIDATOR_HPP
#define NDN_UTIL_COMMAND_INTEREST_VALIDATOR_HPP

#include "../security/validator.hpp"
#include "../security/identity-certificate.hpp"
#include "../security/sec-rule-specific.hpp"

namespace ndn {

class CommandInterestValidator : public Validator
{
public:
  enum {
    POS_SIG_VALUE = -1,
    POS_SIG_INFO = -2,
    POS_RANDOM_VAL = -3,
    POS_TIMESTAMP = -4,

    GRACE_INTERVAL = 3000 // ms
  };

  CommandInterestValidator(const time::milliseconds& graceInterval =
                           time::milliseconds(static_cast<int>(GRACE_INTERVAL)))
    : m_graceInterval(graceInterval < time::milliseconds::zero() ?
                      time::milliseconds(static_cast<int>(GRACE_INTERVAL)) : graceInterval)
  {
  }

  virtual
  ~CommandInterestValidator()
  {
  }

  void
  addInterestRule(const std::string& regex, const IdentityCertificate& certificate);

  void
  addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey);

protected:
  virtual void
  checkPolicy(const Data& data,
              int stepCount,
              const OnDataValidated& onValidated,
              const OnDataValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps)
  {
    onValidationFailed(data.shared_from_this(), "No policy for data checking");
  }

  virtual void
  checkPolicy(const Interest& interest,
              int stepCount,
              const OnInterestValidated& onValidated,
              const OnInterestValidationFailed& onValidationFailed,
              std::vector<shared_ptr<ValidationRequest> >& nextSteps);
private:
  time::milliseconds m_graceInterval; //ms
  std::map<Name, PublicKey> m_trustAnchorsForInterest;
  std::list<SecRuleSpecific> m_trustScopeForInterest;

  typedef std::map<Name, time::system_clock::TimePoint> LastTimestampMap;
  LastTimestampMap m_lastTimestamp;
};

inline void
CommandInterestValidator::addInterestRule(const std::string& regex,
                                          const IdentityCertificate& certificate)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());
  addInterestRule(regex, keyName, certificate.getPublicKeyInfo());
}

inline void
CommandInterestValidator::addInterestRule(const std::string& regex,
                                          const Name& keyName,
                                          const PublicKey& publicKey)
{
  m_trustAnchorsForInterest[keyName] = publicKey;
  shared_ptr<Regex> interestRegex = make_shared<Regex>(regex);
  shared_ptr<Regex> signerRegex = Regex::fromName(keyName, true);
  m_trustScopeForInterest.push_back(SecRuleSpecific(interestRegex, signerRegex));
}

inline void
CommandInterestValidator::checkPolicy(const Interest& interest,
                                      int stepCount,
                                      const OnInterestValidated& onValidated,
                                      const OnInterestValidationFailed& onValidationFailed,
                                      std::vector<shared_ptr<ValidationRequest> >& nextSteps)
{
  const Name& interestName = interest.getName();

  //Prepare
  if (interestName.size() < 4)
    return onValidationFailed(interest.shared_from_this(),
                              "Interest is not signed: " + interest.getName().toUri());

  Signature signature(interestName[POS_SIG_INFO].blockFromValue(),
                      interestName[POS_SIG_VALUE].blockFromValue());

  SignatureSha256WithRsa sig(signature);
  const Name& keyLocatorName = sig.getKeyLocator().getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(keyLocatorName);

  //Check if command is in the trusted scope
  bool isInScope = false;
  for (std::list<SecRuleSpecific>::iterator scopeIt = m_trustScopeForInterest.begin();
      scopeIt != m_trustScopeForInterest.end();
      ++scopeIt)
    {
      if (scopeIt->satisfy(interestName, keyName))
        {
          isInScope = true;
          break;
        }
    }

  if (isInScope == false)
    return onValidationFailed(interest.shared_from_this(),
                              "Signer cannot be authorized for the command: " + keyName.toUri());

  //Check if timestamp is valid
  time::system_clock::TimePoint interestTime =
    time::fromUnixTimestamp(time::milliseconds(interestName.get(POS_TIMESTAMP).toNumber()));

  time::system_clock::TimePoint currentTime = time::system_clock::now();

  LastTimestampMap::iterator timestampIt = m_lastTimestamp.find(keyName);
  if (timestampIt == m_lastTimestamp.end())
    {
      if (!(currentTime - m_graceInterval <= interestTime &&
            interestTime <= currentTime + m_graceInterval))
        {
          return onValidationFailed(interest.shared_from_this(),
                                    "The command is not in grace interval: " +
                                    interest.getName().toUri());
        }
    }
  else
    {
      if(interestTime <= timestampIt->second)
        return onValidationFailed(interest.shared_from_this(),
                                  "The command is outdated: " + interest.getName().toUri());
    }

  //Check signature
  if (!Validator::verifySignature(interestName.wireEncode().value(),
                                  interestName.wireEncode().value_size() -
                                  interestName[-1].size(),
                                  sig, m_trustAnchorsForInterest[keyName]))
    return onValidationFailed(interest.shared_from_this(),
                              "Signature cannot be validated: " + interest.getName().toUri());

  //Update timestamp
  if (timestampIt == m_lastTimestamp.end())
    {
      m_lastTimestamp[keyName] = interestTime;
    }
  else
    {
      timestampIt->second = interestTime;
    }

  return onValidated(interest.shared_from_this());
}

} // namespace ndn

#endif // NDN_UTIL_COMMAND_INTEREST_VALIDATOR_HPP
