/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_ENTRY_HPP
#define	NDN_FORWARDING_ENTRY_HPP

#include <string>
#include "name.hpp"
#include "publisher-public-key-digest.hpp"
#include "forwarding-flags.hpp"
#include "c/forwarding-entry.h"

namespace ndn {

/**
 * An ForwardingEntry holds an action and  Name prefix and other fields for an forwarding entry.
 */
class ForwardingEntry {
public:    
  ForwardingEntry
    (const std::string& action, const Name& prefix, const PublisherPublicKeyDigest publisherPublicKeyDigest,
     int faceId, const ForwardingFlags& forwardingFlags, int freshnessSeconds) 
  : action_(action), prefix_(prefix), publisherPublicKeyDigest_(publisherPublicKeyDigest), 
    faceId_(faceId), forwardingFlags_(forwardingFlags), freshnessSeconds_(freshnessSeconds)
  {
  }

  ForwardingEntry()
  : faceId_(-1), freshnessSeconds_(-1)
  {
    forwardingFlags_.setActive(true);
    forwardingFlags_.setChildInherit(true);
  }
  
  Blob 
  wireEncode(WireFormat& wireFormat = *WireFormat::getDefaultWireFormat()) const 
  {
    return wireFormat.encodeForwardingEntry(*this);
  }
  
  void 
  wireDecode(const uint8_t *input, size_t inputLength, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat()) 
  {
    wireFormat.decodeForwardingEntry(*this, input, inputLength);
  }
  
  void 
  wireDecode(const std::vector<uint8_t>& input, WireFormat& wireFormat = *WireFormat::getDefaultWireFormat()) 
  {
    wireDecode(&input[0], input.size(), wireFormat);
  }
  
  /**
   * Set the forwardingEntryStruct to point to the components in this forwarding entry, without copying any memory.
   * WARNING: The resulting pointers in forwardingEntryStruct are invalid after a further use of this object which could reallocate memory.
   * @param forwardingEntryStruct a C ndn_ForwardingEntry struct where the prefix name components array is already allocated.
   */
  void 
  get(struct ndn_ForwardingEntry& forwardingEntryStruct) const;

  const std::string& 
  getAction() const { return action_; }
  
  Name& 
  getPrefix() { return prefix_; }
  
  const Name& 
  getPrefix() const { return prefix_; }
  
  PublisherPublicKeyDigest& 
  getPublisherPublicKeyDigest() { return publisherPublicKeyDigest_; }
  
  const PublisherPublicKeyDigest& 
  getPublisherPublicKeyDigest() const { return publisherPublicKeyDigest_; }
  
  int 
  getFaceId() const { return faceId_; }

  const ForwardingFlags& 
  getForwardingFlags() const { return forwardingFlags_; }

  int 
  getFreshnessSeconds() const { return freshnessSeconds_; }
  
  /**
   * Clear this forwarding entry, and set the values by copying from forwardingEntryStruct.
   * @param forwardingEntryStruct a C ndn_ForwardingEntry struct.
   */
  void 
  set(const struct ndn_ForwardingEntry& forwardingEntryStruct);

  void 
  setAction(const std::string& value) { action_ = value; }
  
  void 
  setFaceId(int value) { faceId_ = value; }
      
  void 
  setForwardingFlags(const ForwardingFlags& value) { forwardingFlags_ = value; }
      
  void 
  setFreshnessSeconds(int value) { freshnessSeconds_ = value; }
      
private:
  std::string action_;   /**< empty for none. */
  Name prefix_;
  PublisherPublicKeyDigest publisherPublicKeyDigest_;
  int faceId_;           /**< -1 for none. */
  ForwardingFlags forwardingFlags_;
  int freshnessSeconds_; /**< -1 for none. */
};

}

#endif
