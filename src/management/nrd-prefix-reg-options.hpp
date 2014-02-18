/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2014 Regents of the University of California.
 *
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NRD_PREFIX_REG_OPTIONS_HPP
#define NDN_MANAGEMENT_NRD_PREFIX_REG_OPTIONS_HPP

#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nrd.hpp"
#include "../name.hpp"

namespace ndn {
namespace nrd {

const uint64_t INVALID_FACE_ID = std::numeric_limits<uint64_t>::max();
const uint64_t DEFAULT_COST = 0;
const uint64_t DEFAULT_FLAGS = tlv::nrd::NDN_FORW_CHILD_INHERIT;

/**
 * @brief Abstraction for prefix registration options for NRD Prefix registration protocol
 *
 * @see http://redmine.named-data.net/projects/nrd/wiki/NRD_Prefix_Registration_protocol
 */
class PrefixRegOptions {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };

  PrefixRegOptions()
    : m_faceId (INVALID_FACE_ID)
    , m_flags (DEFAULT_FLAGS)
    , m_cost (DEFAULT_COST)
    , m_expirationPeriod (-1)
  {
  }

  PrefixRegOptions(const Block& block)
  {
    wireDecode(block);
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;
  
  const Block&
  wireEncode() const;
  
  void 
  wireDecode(const Block& wire);
  
  ////////////////////////////////////////////////////////
  
  const Name& 
  getName() const
  {
    return m_name;
  }
  
  PrefixRegOptions&
  setName(const Name& name)
  {
    m_name = name;
    m_wire.reset();
    return *this;
  }

  //
  
  int
  getFaceId() const
  {
    return m_faceId;
  }

  PrefixRegOptions&
  setFaceId(int faceId)
  {
    m_faceId = faceId;
    m_wire.reset();
    return *this;
  }

  //
  
  uint64_t 
  getFlags() const
  {
    return m_flags;
  }

  PrefixRegOptions&
  setFlags(uint64_t flags)
  {
    m_flags = flags;
    m_wire.reset();
    return *this;
  }

  //
  
  uint64_t 
  getCost() const
  {
    return m_cost;
  }

  PrefixRegOptions&
  setCost(uint64_t cost)
  {
    m_cost = cost;
    m_wire.reset();
    return *this;
  }

  //
  
  Milliseconds
  getExpirationPeriod() const
  {
    return m_expirationPeriod;
  }

  PrefixRegOptions&
  setExpirationPeriod(Milliseconds expirationPeriod)
  {
    m_expirationPeriod = expirationPeriod;
    m_wire.reset();
    return *this;
  }

  //
  
  const Name& 
  getStrategy() const
  {
    return m_strategy;
  }

  PrefixRegOptions&
  setStrategy(const Name& strategy)
  {
    m_strategy = strategy;
    m_wire.reset();
    return *this;
  }

  //

  const std::string& 
  getProtocol() const
  {
    return m_protocol;
  }

  PrefixRegOptions&
  setProtocol(const std::string& protocol)
  {
    m_protocol = protocol;
    m_wire.reset();
    return *this;
  }
  
private:
  Name m_name;
  uint64_t m_faceId;
  uint64_t m_flags;
  uint64_t m_cost;
  Milliseconds m_expirationPeriod;
  Name m_strategy;
  std::string m_protocol;
  
  mutable Block m_wire;
};

template<bool T>
inline size_t
PrefixRegOptions::wireEncode(EncodingImpl<T>& block) const
{
  size_t total_len = 0;

  // PrefixRegOptions ::= PREFIX-REG-OPTIONS-TYPE TLV-LENGTH
  //                        Name
  //                        FaceId?
  //                        Flags?
  //                        Cost?    
  //                        ExpirationPeriod?
  //                        StrategyName?
  //                        Protocol?

  // (reverse encoding)

  // Protocol
  if (!m_protocol.empty())
    {
      total_len += prependByteArrayBlock(block,
                                         tlv::nrd::Protocol,
                                         reinterpret_cast<const uint8_t*>(m_protocol.c_str()),
                                         m_protocol.size());
    }

  // StrategyName
  if (!m_strategy.empty())
    {
      total_len += prependNestedBlock(block, tlv::nrd::StrategyName, m_strategy);
    }

  // ExpirationPeriod
  if (m_expirationPeriod > 0)
    {
      total_len += prependNonNegativeIntegerBlock(block,
                                                  tlv::nrd::ExpirationPeriod, m_expirationPeriod);
    }

  // Cost
  if (m_cost != DEFAULT_COST)
    {
      total_len += prependNonNegativeIntegerBlock(block, tlv::nrd::Cost, m_cost);
    }

  // Flags
  if (m_flags != DEFAULT_FLAGS)
    {
      total_len += prependNonNegativeIntegerBlock(block, tlv::nrd::Flags, m_flags);
    }

  // FaceId
  if (m_faceId != INVALID_FACE_ID)
    {
      total_len += prependNonNegativeIntegerBlock(block, tlv::nrd::FaceId, m_faceId);
    }

  // Name
  total_len += m_name.wireEncode(block);

  total_len += block.prependVarNumber(total_len);
  total_len += block.prependVarNumber(tlv::nrd::PrefixRegOptions);
  return total_len;
}

inline const Block&
PrefixRegOptions::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);
  
  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}
  
inline void 
PrefixRegOptions::wireDecode(const Block& wire)
{
  // PrefixRegOptions ::= PREFIX-REG-OPTIONS-TYPE TLV-LENGTH
  //                        Name
  //                        FaceId?
  //                        Flags?
  //                        Cost?    
  //                        ExpirationPeriod?
  //                        StrategyName?
  //                        Protocol?

  m_name.clear();
  m_faceId = INVALID_FACE_ID;
  m_flags = DEFAULT_FLAGS;
  m_cost = DEFAULT_COST;
  m_expirationPeriod = -1;
  m_strategy.clear();
  m_protocol.clear();

  m_wire = wire;

  if (m_wire.type() != tlv::nrd::PrefixRegOptions)
    throw Error("Requested decoding of PrefixRegOptions, but Block is of different type");
  
  m_wire.parse ();

  // Name
  Block::element_const_iterator val = m_wire.find(Tlv::Name);
  if (val != m_wire.elements_end())
    {
      m_name.wireDecode(*val);
    }

  // FaceId
  val = m_wire.find(tlv::nrd::FaceId);
  if (val != m_wire.elements_end())
    {
      m_faceId = readNonNegativeInteger(*val);
    }
  
  // Flags
  val = m_wire.find(tlv::nrd::Flags);
  if (val != m_wire.elements_end())
    {
      m_flags = readNonNegativeInteger(*val);
    }

  // Cost
  val = m_wire.find(tlv::nrd::Cost);
  if (val != m_wire.elements_end())
    {
      m_cost = readNonNegativeInteger(*val);
    }

  // ExpirationPeriod
  val = m_wire.find(tlv::nrd::ExpirationPeriod);
  if (val != m_wire.elements_end())
    {
      m_expirationPeriod = readNonNegativeInteger(*val);
    }

  // StrategyName
  val = m_wire.find(tlv::nrd::StrategyName);
  if (val != m_wire.elements_end())
    {
      val->parse();
      if (!val->elements().empty())
        m_strategy.wireDecode(*val->elements_begin());
    }

  // Protocol
  val = m_wire.find(tlv::nrd::Protocol);
  if (val != m_wire.elements_end())
    {
      m_protocol = std::string(reinterpret_cast<const char*>(val->value()),
                               val->value_size());
    }
}

// inline std::ostream&
// operator << (std::ostream &os, const PrefixRegOptions &option)
// {
//   os << "ForwardingEntry(";
  
//   // Name
//   os << "Prefix: " << option.getName() << ", ";

//   // FaceID
//   os << "FaceID: " << option.getFaceId() << ", ";

//   // Cost
//   os << "Cost: " << option.getCost() << ", ";

//   // Strategy
//   os << "Strategy: " << option.getStrategy() << ", ";
  
//   os << ")";
//   return os;
// }

} // namespace nrd
} // namespace ndn

#endif // NDN_MANAGEMENT_NRD_PREFIX_REG_OPTIONS_HPP
