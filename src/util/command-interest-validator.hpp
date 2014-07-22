/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#ifndef NDN_UTIL_COMMAND_INTEREST_VALIDATOR_HPP
#define NDN_UTIL_COMMAND_INTEREST_VALIDATOR_HPP

#include "../security/validator.hpp"
#include "../security/identity-certificate.hpp"
#include "../security/sec-rule-specific.hpp"

#include <list>

namespace ndn {

/**
 * @brief Helper class to validate CommandInterests
 *
 * @deprecated Use ValidatorConfig instead.
 * See http://redmine.named-data.net/projects/ndn-cxx/wiki/CommandValidatorConf
 * for more details about the configuration format of ValidatorConfig.
 *
 * @see http://redmine.named-data.net/projects/nfd/wiki/Command_Interests
 */
class CommandInterestValidator : public Validator
{
public:
  enum {
    POS_SIG_VALUE = -1,
    POS_SIG_INFO = -2,
    POS_RANDOM_VAL = -3,
    POS_TIMESTAMP = -4,

    MIN_LENGTH = 4,

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

  /**
   * @brief add an Interest rule that allows a specific certificate
   *
   * @param regex NDN Regex to match Interest Name
   * @param certificate trusted certificate
   */
  void
  addInterestRule(const std::string& regex, const IdentityCertificate& certificate);

  /**
   * @brief add an Interest rule that allows a specific public key
   *
   * @param regex NDN Regex to match Interest Name
   * @param keyName KeyLocator.Name
   * @param publicKey public key
   */
  void
  addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey);

  /**
   * @brief add an Interest rule that allows any signer
   *
   * @param regex NDN Regex to match Interest Name
   * @note Command Interest matched by regex that is signed by any key will be accepted.
   */
  void
  addInterestBypassRule(const std::string& regex);

  /**
   * @brief Remove all installed Interest rules (e.g., when reinitialization needed)
   */
  void
  reset();

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
CommandInterestValidator::addInterestBypassRule(const std::string& regex)
{
  shared_ptr<Regex> interestRegex = make_shared<Regex>(regex);
  m_trustScopeForInterest.push_back(SecRuleSpecific(interestRegex));
}

inline void
CommandInterestValidator::reset()
{
  m_trustAnchorsForInterest.clear();
  m_trustScopeForInterest.clear();
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
  if (interestName.size() < MIN_LENGTH)
    return onValidationFailed(interest.shared_from_this(),
                              "Interest is not signed: " + interest.getName().toUri());
  Name keyName;
  try
    {
      Signature signature(interestName[POS_SIG_INFO].blockFromValue(),
                          interestName[POS_SIG_VALUE].blockFromValue());

      if (signature.getType() != tlv::SignatureSha256WithRsa)
        return onValidationFailed(interest.shared_from_this(),
                                  "Require SignatureSha256WithRsa");

      SignatureSha256WithRsa sig(signature);

      const KeyLocator& keyLocator = sig.getKeyLocator();

      if (keyLocator.getType() != KeyLocator::KeyLocator_Name)
        return onValidationFailed(interest.shared_from_this(),
                                  "Key Locator is not a name");

      keyName = IdentityCertificate::certificateNameToPublicKeyName(keyLocator.getName());

      //Check if command is in the trusted scope
      bool isInScope = false;
      for (std::list<SecRuleSpecific>::iterator scopeIt = m_trustScopeForInterest.begin();
           scopeIt != m_trustScopeForInterest.end();
           ++scopeIt)
        {
          if (scopeIt->satisfy(interestName, keyName))
            {
              if (scopeIt->isExempted())
                {
                  return onValidated(interest.shared_from_this());
                }

              isInScope = true;
              break;
            }
        }
      if (isInScope == false)
        return onValidationFailed(interest.shared_from_this(),
                                  "Signer cannot be authorized for the command: " +
                                  keyName.toUri());

      //Check signature
      if (!Validator::verifySignature(interestName.wireEncode().value(),
                                      interestName.wireEncode().value_size() -
                                      interestName[-1].size(),
                                      sig, m_trustAnchorsForInterest[keyName]))
        return onValidationFailed(interest.shared_from_this(),
                                  "Signature cannot be validated: " +
                                  interest.getName().toUri());

      //Check if timestamp is valid
      time::system_clock::TimePoint interestTime =
        time::fromUnixTimestamp(time::milliseconds(interestName.get(POS_TIMESTAMP).toNumber()));

      time::system_clock::TimePoint currentTime = time::system_clock::now();

      LastTimestampMap::iterator timestampIt = m_lastTimestamp.find(keyName);
      if (timestampIt == m_lastTimestamp.end())
        {
          if (!(currentTime - m_graceInterval <= interestTime &&
                interestTime <= currentTime + m_graceInterval))
            return onValidationFailed(interest.shared_from_this(),
                                      "The command is not in grace interval: " +
                                      interest.getName().toUri());
        }
      else
        {
          if (interestTime <= timestampIt->second)
            return onValidationFailed(interest.shared_from_this(),
                                      "The command is outdated: " +
                                      interest.getName().toUri());
        }

      //Update timestamp
      if (timestampIt == m_lastTimestamp.end())
        {
          m_lastTimestamp[keyName] = interestTime;
        }
      else
        {
          timestampIt->second = interestTime;
        }
    }
  catch (Signature::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "No valid signature");
    }
  catch (IdentityCertificate::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "Cannot locate the signing key");
    }
  catch (tlv::Error& e)
    {
      return onValidationFailed(interest.shared_from_this(),
                                "Cannot decode signature related TLVs");
    }

  return onValidated(interest.shared_from_this());
}

} // namespace ndn

#endif // NDN_UTIL_COMMAND_INTEREST_VALIDATOR_HPP
