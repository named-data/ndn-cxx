/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "common.hpp"
#include "forwarding-entry.hpp"

using namespace std;

namespace ndn {
  
void 
ForwardingEntry::set(const struct ndn_ForwardingEntry& forwardingEntryStruct) 
{
  if (forwardingEntryStruct.action && forwardingEntryStruct.actionLength > 0)
    action_ = string(forwardingEntryStruct.action, forwardingEntryStruct.action + forwardingEntryStruct.actionLength);
  else
    action_ = "";
            
  prefix_.set(forwardingEntryStruct.prefix);
  publisherPublicKeyDigest_.set(forwardingEntryStruct.publisherPublicKeyDigest);  
  faceId_ = forwardingEntryStruct.faceId;
  forwardingFlags_ = forwardingEntryStruct.forwardingFlags;
  freshnessSeconds_ = forwardingEntryStruct.freshnessSeconds;
}

void 
ForwardingEntry::get(struct ndn_ForwardingEntry& forwardingEntryStruct) const 
{
  prefix_.get(forwardingEntryStruct.prefix);
  publisherPublicKeyDigest_.get(forwardingEntryStruct.publisherPublicKeyDigest);
  forwardingEntryStruct.faceId = faceId_;
  forwardingEntryStruct.forwardingFlags = forwardingFlags_;
  forwardingEntryStruct.freshnessSeconds = freshnessSeconds_;

  forwardingEntryStruct.actionLength = action_.size();
  if (action_.size() > 0)
    forwardingEntryStruct.action = (unsigned char *)&action_[0];
  else
    forwardingEntryStruct.action = 0;
}

}
