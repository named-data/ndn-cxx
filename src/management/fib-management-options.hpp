/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 *
 * @author: Wentao Shang <wentao@cs.ucla.edu>
 *
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FIB_MANAGEMENT_HPP
#define NDN_FIB_MANAGEMENT_HPP

#include "../encoding/block.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd-control.hpp"
#include "../name.hpp"

namespace ndn {

class FibManagementOptions {
public:
  FibManagementOptions ()
    : faceId_ (-1)
    , cost_ (-1)
  {
  }
  
  const Name& 
  getName () const { return name_; }
  
  void
  setName (const Name &name) { name_ = name; wire_.reset (); }
  
  int 
  getFaceId () const { return faceId_; }

  void 
  setFaceId (int faceId) { faceId_ = faceId; wire_.reset (); }

  int 
  getCost () const { return cost_; }

  void 
  setCost (int cost) { cost_ = cost; wire_.reset (); }

  inline size_t
  wireEncode (EncodingBuffer& blk);
  
  inline const Block&
  wireEncode () const;
  
  inline void 
  wireDecode (const Block &wire);
  
private:
  Name name_;
  int faceId_;
  int cost_;
  //Name strategy_;
  //TODO: implement strategy after its use is properly defined

  mutable Block wire_;
};

inline size_t
FibManagementOptions::wireEncode (EncodingBuffer& blk)
{
  size_t total_len = 0;
  if (cost_ != -1)
    {
      size_t var_len = blk.prependNonNegativeInteger (cost_);
      total_len += var_len;
      total_len += blk.prependVarNumber (var_len);
      total_len += blk.prependVarNumber (tlv::nfd_control::Cost);
    }

  if (faceId_ != -1)
    {
      size_t var_len = blk.prependNonNegativeInteger (faceId_);
      total_len += var_len;
      total_len += blk.prependVarNumber (var_len);
      total_len += blk.prependVarNumber (tlv::nfd_control::FaceId);
    }

  total_len += name_.wireEncode (blk);

  total_len += blk.prependVarNumber (total_len);
  total_len += blk.prependVarNumber (tlv::nfd_control::FibManagementOptions);
  return total_len;
}

inline const Block&
FibManagementOptions::wireEncode () const
{
  if (wire_.hasWire ())
    return wire_;
  
  wire_ = Block (tlv::nfd_control::FibManagementOptions);

  // Name
  wire_.push_back (name_.wireEncode ());
  
  // FaceId
  if (faceId_ != -1)
    wire_.push_back (nonNegativeIntegerBlock (tlv::nfd_control::FaceId, faceId_));

  // Cost
  if (cost_ != -1)
    wire_.push_back (nonNegativeIntegerBlock (tlv::nfd_control::Cost, cost_));

  //TODO: Strategy
  
  wire_.encode ();
  return wire_;
}
  
inline void 
FibManagementOptions::wireDecode (const Block &wire)
{
  name_.clear ();
  faceId_ = -1;
  cost_ = -1;

  wire_ = wire;
  wire_.parse ();

  // Name
  Block::element_iterator val = wire_.find(Tlv::Name);
  if (val != wire_.getAll().end())
    {
      name_.wireDecode(*val);
    }

  // FaceID
  val = wire_.find(tlv::nfd_control::FaceId);
  if (val != wire_.getAll().end())
    {
      faceId_ = readNonNegativeInteger(*val);
    }

  // Cost
  val = wire_.find(tlv::nfd_control::Cost);
  if (val != wire_.getAll().end())
    {
      cost_ = readNonNegativeInteger(*val);
    }

  //TODO: Strategy
}

inline std::ostream&
operator << (std::ostream &os, const FibManagementOptions &option)
{
  os << "ForwardingEntry(";
  
  // Name
  os << "Prefix: " << option.getName () << ", ";

  // FaceID
  os << "FaceID: " << option.getFaceId () << ", ";

  // Cost
  os << "Cost: " << option.getCost ();

  os << ")";
  return os;
}

}

#endif
