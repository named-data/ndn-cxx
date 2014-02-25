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
#include "../encoding/tlv-nfd.hpp"
#include "../name.hpp"

namespace ndn {
namespace nfd {

class FibManagementOptions {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };

  FibManagementOptions ()
    : m_faceId (std::numeric_limits<uint64_t>::max())
    , m_cost (0)
  {
  }

  FibManagementOptions(const Block& block)
  {
    wireDecode(block);
  }

  const Name&
  getName () const
  {
    return m_name;
  }

  FibManagementOptions&
  setName (const Name &name)
  {
    m_name = name;
    m_wire.reset ();
    return *this;
  }

  uint64_t
  getFaceId () const
  {
    return m_faceId;
  }

  FibManagementOptions&
  setFaceId (uint64_t faceId)
  {
    m_faceId = faceId;
    m_wire.reset ();
    return *this;
  }

  uint64_t
  getCost () const
  {
    return m_cost;
  }

  FibManagementOptions&
  setCost (uint64_t cost)
  {
    m_cost = cost;
    m_wire.reset ();
    return *this;
  }

  const Name&
  getStrategy () const
  {
    return m_strategy;
  }

  FibManagementOptions&
  setStrategy (const Name& strategy)
  {
    m_strategy = strategy;
    m_wire.reset ();
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
  Name m_name;
  uint64_t m_faceId;
  uint64_t m_cost;
  Name m_strategy;

  mutable Block m_wire;
};

template<bool T>
inline size_t
FibManagementOptions::wireEncode(EncodingImpl<T>& blk) const
{
  size_t total_len = 0;

  if (!m_strategy.empty())
    {
      total_len += prependNestedBlock(blk, tlv::nfd::Strategy, m_strategy);
    }

  if (m_cost != 0)
    {
      total_len += prependNonNegativeIntegerBlock(blk, tlv::nfd::Cost, m_cost);
    }

  if (m_faceId != std::numeric_limits<uint64_t>::max())
    {
      total_len += prependNonNegativeIntegerBlock(blk, tlv::nfd::FaceId, m_faceId);
    }

  total_len += m_name.wireEncode(blk);

  total_len += blk.prependVarNumber(total_len);
  total_len += blk.prependVarNumber(tlv::nfd::FibManagementOptions);
  return total_len;
}

inline const Block&
FibManagementOptions::wireEncode () const
{
  if (m_wire.hasWire ())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

inline void
FibManagementOptions::wireDecode (const Block &wire)
{
  m_name.clear();
  m_faceId = std::numeric_limits<uint64_t>::max();
  m_cost = 0;
  m_strategy.clear();

  m_wire = wire;

  if (m_wire.type() != tlv::nfd::FibManagementOptions)
    throw Error("Requested decoding of FibManagementOptions, but Block is of different type");

  m_wire.parse ();

  // Name
  Block::element_const_iterator val = m_wire.find(Tlv::Name);
  if (val != m_wire.elements_end())
    {
      m_name.wireDecode(*val);
    }

  // FaceID
  val = m_wire.find(tlv::nfd::FaceId);
  if (val != m_wire.elements_end())
    {
      m_faceId = readNonNegativeInteger(*val);
    }

  // Cost
  val = m_wire.find(tlv::nfd::Cost);
  if (val != m_wire.elements_end())
    {
      m_cost = readNonNegativeInteger(*val);
    }

  // Strategy
  val = m_wire.find(tlv::nfd::Strategy);
  if (val != m_wire.elements_end())
    {
      val->parse();
      if (!val->elements().empty())
        m_strategy.wireDecode(*val->elements_begin());
    }
}

inline std::ostream&
operator << (std::ostream &os, const FibManagementOptions &option)
{
  os << "ForwardingEntry(";

  // Name
  os << "Prefix: " << option.getName() << ", ";

  // FaceID
  os << "FaceID: " << option.getFaceId() << ", ";

  // Cost
  os << "Cost: " << option.getCost() << ", ";

  // Strategy
  os << "Strategy: " << option.getStrategy() << ", ";

  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FIB_MANAGEMENT_OPTIONS_HPP
