/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "face.hpp"

using namespace std;

namespace ndn {
  
void Face::expressInterest(const Name &name, const Interest *interestTemplate, const OnData &onData, const OnTimeout &onTimeout)
{
  if (interestTemplate)
    node_.expressInterest(Interest
      (name, interestTemplate->getMinSuffixComponents(), interestTemplate->getMaxSuffixComponents(),
       interestTemplate->getPublisherPublicKeyDigest(), interestTemplate->getExclude(),
       interestTemplate->getChildSelector(), interestTemplate->getAnswerOriginKind(),
       interestTemplate->getScope(), interestTemplate->getInterestLifetimeMilliseconds()), onData, onTimeout);
  else
    node_.expressInterest(Interest(name, 4000.0), onData, onTimeout);  
}

void Face::shutdown()
{
  node_.shutdown();
}

}
