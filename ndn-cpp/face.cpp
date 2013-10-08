/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "face.hpp"

using namespace std;

namespace ndn {
  
uint64_t 
Face::expressInterest(const Name& name, const Interest *interestTemplate, const OnData& onData, const OnTimeout& onTimeout)
{
  if (interestTemplate)
    return node_.expressInterest(Interest
      (name, interestTemplate->getMinSuffixComponents(), interestTemplate->getMaxSuffixComponents(),
       interestTemplate->getPublisherPublicKeyDigest(), interestTemplate->getExclude(),
       interestTemplate->getChildSelector(), interestTemplate->getAnswerOriginKind(),
       interestTemplate->getScope(), interestTemplate->getInterestLifetimeMilliseconds()), onData, onTimeout);
  else
    return node_.expressInterest(Interest(name, 4000.0), onData, onTimeout);  
}

void 
Face::shutdown()
{
  node_.shutdown();
}

}
