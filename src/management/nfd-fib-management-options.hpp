/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 *
 * @author: Wentao Shang <wentao@cs.ucla.edu>
 *
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_FIB_MANAGEMENT_OPTIONS_HPP
#define NDN_MANAGEMENT_NFD_FIB_MANAGEMENT_OPTIONS_HPP

#include "../encoding/block.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd-control.hpp"
#include "../name.hpp"

namespace ndn {
namespace nfd {

class FibManagementOptions {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };

  FibManagementOptions ()
    : faceId_ (-1)
    , cost_ (-1)
  {
  }

  FibManagementOptions(const Block& block)
  {
    wireDecode(block);
  }
  
  const Name& 
  getName () const
  {
    return name_;
  }
  
  FibManagementOptions&
  setName (const Name &name)
  {
    name_ = name;
    wire_.reset ();
    return *this;
  }
  
  int 
  getFaceId () const
  {
    return faceId_;
  }

  FibManagementOptions&
  setFaceId (int faceId)
  {
    faceId_ = faceId;
    wire_.reset ();
    return *this;
  }

  int 
  getCost () const
  {
    return cost_;
  }

  FibManagementOptions&
  setCost (int cost)
  {
    cost_ = cost;
    wire_.reset ();
    return *this;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;
  
  const Block&
  wireEncode () const;
  
  void 
  wireDecode (const Block &wire);
  
private:
  Name name_;
  int faceId_;
  int cost_;
  //Name strategy_;
  //TODO: implement strategy after its use is properly defined

  mutable Block wire_;
};

template<bool T>
inline size_t
FibManagementOptions::wireEncode(EncodingImpl<T>& blk) const
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

template
size_t
FibManagementOptions::wireEncode<true>(EncodingBuffer& block) const;

template
size_t
FibManagementOptions::wireEncode<false>(EncodingEstimator& block) const;

inline const Block&
FibManagementOptions::wireEncode () const
{
  if (wire_.hasWire ())
    return wire_;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);
  
  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  wire_ = buffer.block();
  return wire_;
}
  
inline void 
FibManagementOptions::wireDecode (const Block &wire)
{
  name_.clear ();
  faceId_ = -1;
  cost_ = -1;

  wire_ = wire;

  if (wire_.type() != tlv::nfd_control::FibManagementOptions)
    throw Error("Requested decoding of FibManagementOptions, but Block is of different type");
  
  wire_.parse ();

  // Name
  Block::element_const_iterator val = wire_.find(Tlv::Name);
  if (val != wire_.elements_end())
    {
      name_.wireDecode(*val);
    }

  // FaceID
  val = wire_.find(tlv::nfd_control::FaceId);
  if (val != wire_.elements_end())
    {
      faceId_ = readNonNegativeInteger(*val);
    }

  // Cost
  val = wire_.find(tlv::nfd_control::Cost);
  if (val != wire_.elements_end())
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

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FIB_MANAGEMENT_OPTIONS_HPP
