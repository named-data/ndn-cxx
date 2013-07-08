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
	minSuffixComponents_ = interestStruct.minSuffixComponents;
	maxSuffixComponents_ = interestStruct.maxSuffixComponents;
	
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

void Interest::get(struct ndn_Interest &interestStruct) const 
{
  name_.get(interestStruct.name);
  interestStruct.minSuffixComponents = minSuffixComponents_;
  interestStruct.maxSuffixComponents = maxSuffixComponents_;
  
  interestStruct.publisherPublicKeyDigestLength = publisherPublicKeyDigest_.size();
  if (publisherPublicKeyDigest_.size() > 0)
    interestStruct.publisherPublicKeyDigest = (unsigned char *)&publisherPublicKeyDigest_[0];
  else
    interestStruct.publisherPublicKeyDigest = 0;
  
  // TODO: implement exclude.
  
  interestStruct.childSelector = childSelector_;
  interestStruct.answerOriginKind = answerOriginKind_;
  interestStruct.scope = scope_;
  interestStruct.interestLifetime = interestLifetime_;

  interestStruct.nonceLength = nonce_.size();
  if (nonce_.size() > 0)
    interestStruct.nonce = (unsigned char *)&nonce_[0];
  else
    interestStruct.nonce = 0;
}
  
}

