/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "common.hpp"
#include "Interest.hpp"

using namespace std;

namespace ndn {
  
void Exclude::get(struct ndn_Exclude &excludeStruct) const
{
  if (excludeStruct.maxEntries < entries_.size())
    throw runtime_error("excludeStruct.maxEntries must be >= this exclude getEntryCount()");
  
  excludeStruct.nEntries = entries_.size();
  for (unsigned int i = 0; i < excludeStruct.nEntries; ++i)
    entries_[i].get(excludeStruct.entries[i]);  
}

void Exclude::set(const struct ndn_Exclude &excludeStruct)
{
  entries_.clear();
  for (unsigned int i = 0; i < excludeStruct.nEntries; ++i) {
    ndn_ExcludeEntry *entry = &excludeStruct.entries[i];
    
    if (entry->type == ndn_Exclude_COMPONENT)
      addComponent(entry->component, entry->componentLength);
    else if (entry->type == ndn_Exclude_ANY)
      addAny();
    else
      throw runtime_error("unrecognized ndn_ExcludeType");
  }
}

void Interest::set(const struct ndn_Interest &interestStruct) 
{
  name_.set(interestStruct.name);
	minSuffixComponents_ = interestStruct.minSuffixComponents;
	maxSuffixComponents_ = interestStruct.maxSuffixComponents;
	
	publisherPublicKeyDigest_.set(interestStruct.publisherPublicKeyDigest);
  
  exclude_.set(interestStruct.exclude);
	childSelector_ = interestStruct.childSelector;
	answerOriginKind_ = interestStruct.answerOriginKind;
	scope_ = interestStruct.scope;
	interestLifetimeMilliseconds_ = interestStruct.interestLifetimeMilliseconds;
  setVector(nonce_, interestStruct.nonce, interestStruct.nonceLength);
}

void Interest::get(struct ndn_Interest &interestStruct) const 
{
  name_.get(interestStruct.name);
  interestStruct.minSuffixComponents = minSuffixComponents_;
  interestStruct.maxSuffixComponents = maxSuffixComponents_;
  publisherPublicKeyDigest_.get(interestStruct.publisherPublicKeyDigest);
  exclude_.get(interestStruct.exclude);
  interestStruct.childSelector = childSelector_;
  interestStruct.answerOriginKind = answerOriginKind_;
  interestStruct.scope = scope_;
  interestStruct.interestLifetimeMilliseconds = interestLifetimeMilliseconds_;

  interestStruct.nonceLength = nonce_.size();
  if (nonce_.size() > 0)
    interestStruct.nonce = (unsigned char *)&nonce_[0];
  else
    interestStruct.nonce = 0;
}
  
}

