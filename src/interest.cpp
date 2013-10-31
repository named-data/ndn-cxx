/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <ndn-cpp/common.hpp>
#include <ndn-cpp/interest.hpp>
#include "c/interest.h"

using namespace std;

namespace ndn {
  
void 
ExcludeEntry::get(struct ndn_ExcludeEntry& excludeEntryStruct) const 
{
  excludeEntryStruct.type = type_;
  if (type_ == ndn_Exclude_COMPONENT)
    component_.get(excludeEntryStruct.component);
}

void 
Exclude::get(struct ndn_Exclude& excludeStruct) const
{
  if (excludeStruct.maxEntries < entries_.size())
    throw runtime_error("excludeStruct.maxEntries must be >= this exclude getEntryCount()");
  
  excludeStruct.nEntries = entries_.size();
  for (size_t i = 0; i < excludeStruct.nEntries; ++i)
    entries_[i].get(excludeStruct.entries[i]);  
}

void 
Exclude::set(const struct ndn_Exclude& excludeStruct)
{
  entries_.clear();
  for (size_t i = 0; i < excludeStruct.nEntries; ++i) {
    ndn_ExcludeEntry *entry = &excludeStruct.entries[i];
    
    if (entry->type == ndn_Exclude_COMPONENT)
      appendComponent(entry->component.value.value, entry->component.value.length);
    else if (entry->type == ndn_Exclude_ANY)
      appendAny();
    else
      throw runtime_error("unrecognized ndn_ExcludeType");
  }
}

string 
Exclude::toUri() const
{
  if (entries_.size() == 0)
    return "";

  ostringstream result;
  for (unsigned i = 0; i < entries_.size(); ++i) {
    if (i > 0)
      result << ",";
        
    if (entries_[i].getType() == ndn_Exclude_ANY)
      result << "*";
    else
      Name::toEscapedString(*entries_[i].getComponent().getValue(), result);
  }
  
  return result.str();  
}

void 
Interest::set(const struct ndn_Interest& interestStruct) 
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
  nonce_ = Blob(interestStruct.nonce);
}

void 
Interest::get(struct ndn_Interest& interestStruct) const 
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
  nonce_.get(interestStruct.nonce);
}
  
}

