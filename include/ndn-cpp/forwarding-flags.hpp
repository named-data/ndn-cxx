/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_FLAGS_HPP
#define NDN_FORWARDING_FLAGS_HPP

#include "c/forwarding-flags.h"

namespace ndn {

/**
 * A ForwardingFlags object holds the flags which specify how the forwarding daemon should forward an interest for
 * a registered prefix.  We use a separate ForwardingFlags object to retain future compatibility if the daemon forwarding
 * bits are changed, amended or deprecated.
 */
class ForwardingFlags : public ndn_ForwardingFlags {
public:
  /**
   * Create a new ForwardingFlags with "active" and "childInherit" set and all other flags cleared.
   */
  ForwardingFlags() 
  {
    ndn_ForwardingFlags_initialize(this);
  }
  
  ForwardingFlags(const struct ndn_ForwardingFlags &forwardingFlagsStruct)
  : ndn_ForwardingFlags(forwardingFlagsStruct)
  {
  }

  /**
   * Get the value of the "active" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getActive() const { return active; }
  
  /**
   * Get the value of the "childInherit" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getChildInherit() const { return childInherit; }
  
  /**
   * Get the value of the "advertise" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getAdvertise() const { return advertise; }
  
  /**
   * Get the value of the "last" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getLast() const { return last; }
  
  /**
   * Get the value of the "capture" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getCapture() const { return capture; }
  
  /**
   * Get the value of the "local" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getLocal() const { return local; }
  
  /**
   * Get the value of the "tap" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getTap() const { return tap; }
  
  /**
   * Get the value of the "captureOk" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getCaptureOk() const { return captureOk; }

  /**
   * Set the value of the "active" flag
   * @param active true to set the flag, false to clear it.
   */  
  void setActive(bool active) { this->active = active ? 1 : 0; }
  
  /**
   * Set the value of the "childInherit" flag
   * @param childInherit true to set the flag, false to clear it.
   */  
  void setChildInherit(bool childInherit) { this->childInherit = childInherit ? 1 : 0; }
  
  /**
   * Set the value of the "advertise" flag
   * @param advertise true to set the flag, false to clear it.
   */  
  void setAdvertise(bool advertise) { this->advertise = advertise ? 1 : 0; }
  
  /**
   * Set the value of the "last" flag
   * @param last true to set the flag, false to clear it.
   */  
  void setLast(bool last) { this->last = last ? 1 : 0; }
  
  /**
   * Set the value of the "capture" flag
   * @param capture true to set the flag, false to clear it.
   */  
  void setCapture(bool capture) { this->capture = capture ? 1 : 0; }
  
  /**
   * Set the value of the "local" flag
   * @param local true to set the flag, false to clear it.
   */  
  void setLocal(bool local) { this->local = local ? 1 : 0; }
  
  /**
   * Set the value of the "tap" flag
   * @param tap true to set the flag, false to clear it.
   */  
  void setTap(bool tap) { this->tap = tap ? 1 : 0; }
  
  /**
   * Set the value of the "captureOk" flag
   * @param captureOk true to set the flag, false to clear it.
   */  
  void setCaptureOk(bool captureOk) { this->captureOk = captureOk ? 1 : 0; }
};

}

#endif
