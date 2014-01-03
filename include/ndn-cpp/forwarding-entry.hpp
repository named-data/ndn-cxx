/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_ENTRY_HPP
#define NDN_FORWARDING_ENTRY_HPP

#include <string>
#include "name.hpp"
#include "forwarding-flags.hpp"
#include "encoding/block.hpp"

namespace ndn {

/**
 * An ForwardingEntry holds an action and  Name prefix and other fields for an forwarding entry.
 */
class ForwardingEntry {
public:    
  ForwardingEntry
    (const std::string& action,
       const Name& prefix,
       int faceId,
       const ForwardingFlags& forwardingFlags,
       int freshnessPeriod) 
  : action_(action)
  , prefix_(prefix)
  , faceId_(faceId)
  , forwardingFlags_(forwardingFlags)
  , freshnessPeriod_(freshnessPeriod)
  {
  }

  ForwardingEntry()
  : faceId_(-1), freshnessPeriod_(-1)
  {
    forwardingFlags_.setActive(true);
    forwardingFlags_.setChildInherit(true);
  }
  
  Block 
  wireEncode() const 
  {
    return wire_;
    // return wireFormat.encodeForwardingEntry(*this);
  }
  
  void 
  wireDecode(const Block &wire) 
  {
    // wireFormat.decodeForwardingEntry(*this, input, inputLength);
  }
  
  const std::string& 
  getAction() const { return action_; }
  
  Name& 
  getPrefix() { return prefix_; }
  
  const Name& 
  getPrefix() const { return prefix_; }
  
  int 
  getFaceId() const { return faceId_; }

  const ForwardingFlags& 
  getForwardingFlags() const { return forwardingFlags_; }

  int 
  getFreshnessPeriod() const { return freshnessPeriod_; }

  void 
  setAction(const std::string& action) { action_ = action; }
  
  void 
  setFaceId(int faceId) { faceId_ = faceId; }
      
  void 
  setForwardingFlags(const ForwardingFlags& forwardingFlags) { forwardingFlags_ = forwardingFlags; }
      
  void 
  setFreshnessPeriod(int freshnessPeriod) { freshnessPeriod_ = freshnessPeriod; }
      
private:
  std::string action_;   /**< empty for none. */
  Name prefix_;
  int faceId_;           /**< -1 for none. */
  ForwardingFlags forwardingFlags_;
  int freshnessPeriod_; /**< -1 for none. */

  Block wire_;
};

}

#endif
