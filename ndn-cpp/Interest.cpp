/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "Interest.hpp"

using namespace std;

namespace ndn {
  
void Interest::set(struct ndn_Interest &interestStruct) 
{
  name_.set(interestStruct.name);
	maxSuffixComponents_ = interestStruct.maxSuffixComponents;
	minSuffixComponents_ = interestStruct.minSuffixComponents;
	
	publisherPublicKeyDigest_.clear();
  if (interestStruct.publisherPublicKeyDigest)
    publisherPublicKeyDigest_.insert
      (publisherPublicKeyDigest_.begin(), interestStruct.publisherPublicKeyDigest, interestStruct.publisherPublicKeyDigest + interestStruct.publisherPublicKeyDigestLength);
	// TODO: implement exclude
	childSelector_ = interestStruct.childSelector;
	answerOriginKind_ = interestStruct.answerOriginKind;
	scope_ = interestStruct.scope;
	interestLifetime_ = interestStruct.interestLifetime;
	nonce_.clear();
  if (interestStruct.nonce)
    nonce_.insert
      (nonce_.begin(), interestStruct.nonce, interestStruct.nonce + interestStruct.nonceLength);
}
  
}

