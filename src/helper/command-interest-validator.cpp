/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "command-interest-validator.hpp"

namespace ndn
{
const ssize_t CommandInterestValidator::POS_SIG_VALUE  = -1;
const ssize_t CommandInterestValidator::POS_SIG_INFO   = -2;
const ssize_t CommandInterestValidator::POS_RANDOM_VAL = -3;
const ssize_t CommandInterestValidator::POS_TIMESTAMP  = -4;
const int64_t CommandInterestValidator::GRACE_INTERVAL = 3000;

void
CommandInterestValidator::checkPolicy (const Interest& interest, 
					      int stepCount, 
					      const OnInterestValidated &onValidated, 
					      const OnInterestValidationFailed &onValidationFailed,
					      std::vector<shared_ptr<ValidationRequest> > &nextSteps)
{
  try{
    Name interestName  = interest.getName();

    Signature signature(interestName.get(POS_SIG_INFO).blockFromValue(), 
			interestName.get(POS_SIG_VALUE).blockFromValue());
    
    SignatureSha256WithRsa sig(signature);
    const Name & keyLocatorName = sig.getKeyLocator().getName();
    Name keyName = IdentityCertificate::certificateNameToPublicKeyName(keyLocatorName);

    //Check if command is in the trusted scope
    bool inScope = false;
    std::list<SecRuleSpecific>::iterator scopeIt = m_trustScopeForInterest.begin();
    for(; scopeIt != m_trustScopeForInterest.end(); scopeIt++)
      {
	if(scopeIt->satisfy(interestName, keyName))
	  {
	    inScope = true;
	    break;
	  }
      }
    if(inScope == false)
      {
	onValidationFailed(interest.shared_from_this());
	return;
      }

    //Check if timestamp is valid
    uint64_t timestamp = interestName.get(POS_TIMESTAMP).toNumber();
    uint64_t current = static_cast<uint64_t>(time::now()/1000000);
    std::map<Name, uint64_t>::const_iterator timestampIt = m_lastTimestamp.find(keyName);
    if(timestampIt == m_lastTimestamp.end())
      {
	if(timestamp > (current + m_graceInterval) || (timestamp + m_graceInterval) < current)
	  {
	    onValidationFailed(interest.shared_from_this());
	    return;
	  }
      }
    else if(m_lastTimestamp[keyName] >= timestamp)
      {
	onValidationFailed(interest.shared_from_this());
	return;
      }
    
    //Check if signature can be verified
    Name signedName = interestName.getPrefix(-1);
    Buffer signedBlob = Buffer(signedName.wireEncode().value(), signedName.wireEncode().value_size()); //These two lines could be optimized
    if(!Validator::verifySignature(signedBlob.buf(), signedBlob.size(), sig, m_trustAnchorsForInterest[keyName]))
      {
	onValidationFailed(interest.shared_from_this());
	return;
      }

    m_lastTimestamp[keyName] = timestamp;
    onValidated(interest.shared_from_this());
    return;

  }catch(...){
    onValidationFailed(interest.shared_from_this());
  }
}

}//ndn
