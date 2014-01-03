/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FORWARDING_FLAGS_HPP
#define NDN_FORWARDING_FLAGS_HPP

#include "encoding/block.hpp"
#include "encoding/tlv-face-management.hpp"

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
  void setActive(bool active) { this->active_ = active; wire_.reset(); }
  
  /**
   * Set the value of the "childInherit" flag
   * @param childInherit true to set the flag, false to clear it.
   */  
  void setChildInherit(bool childInherit) { this->childInherit_ = childInherit; wire_.reset(); }
  
  /**
   * Set the value of the "advertise" flag
   * @param advertise true to set the flag, false to clear it.
   */  
  void setAdvertise(bool advertise) { this->advertise_ = advertise; wire_.reset(); }
  
  /**
   * Set the value of the "last" flag
   * @param last true to set the flag, false to clear it.
   */  
  void setLast(bool last) { this->last_ = last; wire_.reset(); }
  
  /**
   * Set the value of the "capture" flag
   * @param capture true to set the flag, false to clear it.
   */  
  void setCapture(bool capture) { this->capture_ = capture; wire_.reset(); }
  
  /**
   * Set the value of the "local" flag
   * @param local true to set the flag, false to clear it.
   */  
  void setLocal(bool local) { this->local_ = local; wire_.reset(); }
  
  /**
   * Set the value of the "tap" flag
   * @param tap true to set the flag, false to clear it.
   */  
  void setTap(bool tap) { this->tap_ = tap; wire_.reset(); }
  
  /**
   * Set the value of the "captureOk" flag
   * @param captureOk true to set the flag, false to clear it.
   */  
  void setCaptureOk(bool captureOk) { this->captureOk_ = captureOk; wire_.reset(); }

  inline const Block&
  wireEncode() const;

  inline void
  wireDecode(const Block &block);
  
private:
  bool active_;
  bool childInherit_;
  bool advertise_;
  bool last_;
  bool capture_;
  bool local_;
  bool tap_;
  bool captureOk_;

  mutable Block wire_;
};

inline const Block&
ForwardingFlags::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  uint32_t result = 0;
  if (active_)
    result |= Tlv::FaceManagement::FORW_ACTIVE;
  if (childInherit_)
    result |= Tlv::FaceManagement::FORW_CHILD_INHERIT;
  if (advertise_)
    result |= Tlv::FaceManagement::FORW_ADVERTISE;
  if (last_)
    result |= Tlv::FaceManagement::FORW_LAST;
  if (capture_)
    result |= Tlv::FaceManagement::FORW_CAPTURE;
  if (local_)
    result |= Tlv::FaceManagement::FORW_LOCAL;
  if (tap_)
    result |= Tlv::FaceManagement::FORW_TAP;
  if (captureOk_)
    result |= Tlv::FaceManagement::FORW_CAPTURE_OK;
  
  wire_ = nonNegativeIntegerBlock(Tlv::FaceManagement::ForwardingFlags, result);

  return wire_;
}

inline void
ForwardingFlags::wireDecode(const Block &wire)
{
  wire_ = wire;

  uint32_t flags = readNonNegativeInteger(wire_);
  
  active_       = (flags & Tlv::FaceManagement::FORW_ACTIVE)        ? true : false;
  childInherit_ = (flags & Tlv::FaceManagement::FORW_CHILD_INHERIT) ? true : false;
  advertise_    = (flags & Tlv::FaceManagement::FORW_ADVERTISE)     ? true : false;
  last_         = (flags & Tlv::FaceManagement::FORW_LAST)          ? true : false;
  capture_      = (flags & Tlv::FaceManagement::FORW_CAPTURE)       ? true : false;
  local_        = (flags & Tlv::FaceManagement::FORW_LOCAL)         ? true : false;
  tap_          = (flags & Tlv::FaceManagement::FORW_TAP)           ? true : false;
  captureOk_    = (flags & Tlv::FaceManagement::FORW_CAPTURE_OK)    ? true : false;
}

inline std::ostream&
operator << (std::ostream &os, const ForwardingFlags &flags)
{
  if (flags.getActive())
    os << "ACTIVE ";
  if (flags.getChildInherit())
    os << "CHILE_INHERIT ";
  if (flags.getAdvertise())
    os << "ADVERTISE ";
  if (flags.getLast())
    os << "LAST ";
  if (flags.getCapture())
    os << "CAPTURE ";
  if (flags.getLocal())
    os << "LOCAL ";
  if (flags.getTap())
    os << "TAP ";
  if (flags.getCaptureOk())
    os << "CAPTURE_OK ";

  return os;
}

}

#endif
