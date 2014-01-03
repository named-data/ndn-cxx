/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_FLAGS_HPP
#define NDN_FORWARDING_FLAGS_HPP

namespace ndn {

/**
 * A ForwardingFlags object holds the flags which specify how the forwarding daemon should forward an interest for
 * a registered prefix.  We use a separate ForwardingFlags object to retain future compatibility if the daemon forwarding
 * bits are changed, amended or deprecated.
 */
class ForwardingFlags {
public:
  /**
   * Create a new ForwardingFlags with "active" and "childInherit" set and all other flags cleared.
   */
  ForwardingFlags() 
    : active_(true)
    , childInherit_(true)
    , advertise_(false)
    , last_(false)
    , capture_(false)
    , local_(false)
    , tap_(false)
    , captureOk_(false)

  {
  }

  /**
   * Get the value of the "active" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getActive() const { return active_; }
  
  /**
   * Get the value of the "childInherit" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getChildInherit() const { return childInherit_; }
  
  /**
   * Get the value of the "advertise" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getAdvertise() const { return advertise_; }
  
  /**
   * Get the value of the "last" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getLast() const { return last_; }
  
  /**
   * Get the value of the "capture" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getCapture() const { return capture_; }
  
  /**
   * Get the value of the "local" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getLocal() const { return local_; }
  
  /**
   * Get the value of the "tap" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getTap() const { return tap_; }
  
  /**
   * Get the value of the "captureOk" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getCaptureOk() const { return captureOk_; }

  /**
   * Set the value of the "active" flag
   * @param active true to set the flag, false to clear it.
   */  
  void setActive(bool active) { this->active_ = active; }
  
  /**
   * Set the value of the "childInherit" flag
   * @param childInherit true to set the flag, false to clear it.
   */  
  void setChildInherit(bool childInherit) { this->childInherit_ = childInherit; }
  
  /**
   * Set the value of the "advertise" flag
   * @param advertise true to set the flag, false to clear it.
   */  
  void setAdvertise(bool advertise) { this->advertise_ = advertise; }
  
  /**
   * Set the value of the "last" flag
   * @param last true to set the flag, false to clear it.
   */  
  void setLast(bool last) { this->last_ = last; }
  
  /**
   * Set the value of the "capture" flag
   * @param capture true to set the flag, false to clear it.
   */  
  void setCapture(bool capture) { this->capture_ = capture; }
  
  /**
   * Set the value of the "local" flag
   * @param local true to set the flag, false to clear it.
   */  
  void setLocal(bool local) { this->local_ = local; }
  
  /**
   * Set the value of the "tap" flag
   * @param tap true to set the flag, false to clear it.
   */  
  void setTap(bool tap) { this->tap_ = tap; }
  
  /**
   * Set the value of the "captureOk" flag
   * @param captureOk true to set the flag, false to clear it.
   */  
  void setCaptureOk(bool captureOk) { this->captureOk_ = captureOk; }

private:
  bool active_;
  bool childInherit_;
  bool advertise_;
  bool last_;
  bool capture_;
  bool local_;
  bool tap_;
  bool captureOk_;
};

}

#endif
