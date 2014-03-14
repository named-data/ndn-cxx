/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
* Copyright (C) 2014 Named Data Networking Project
* See COPYING for copyright and distribution information.
*/

#ifndef NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_OPTIONS_HPP
#define NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_OPTIONS_HPP

#include "../encoding/block.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"
#include "../name.hpp"

namespace ndn {
namespace nfd {

class StrategyChoiceOptions{
public:
  struct Error : public Tlv::Error
  {
    Error(const std::string& what) : Tlv::Error(what)
    {
    }
  };
  
  StrategyChoiceOptions()
  {
  }
  
  StrategyChoiceOptions(const Block& block)
  {
    wireDecode(block);
  }
  
  const Name&
  getName() const
  {
    return m_name;
  }
  
  StrategyChoiceOptions&
  setName(const Name& name)
  {
    m_name = name;
    m_wire.reset();
    return *this;
  }
  
  const Name&
  getStrategy() const
  {
    return m_strategy;
  }
  
  StrategyChoiceOptions&
  setStrategy(const Name& strategy)
  {
    m_strategy = strategy;
    m_wire.reset();
    return *this;
  }
  
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;
  
  const Block&
  wireEncode() const;
  
  void
  wireDecode(const Block& wire);
  
private:
  Name m_name;
  Name m_strategy;
  
  mutable Block m_wire;
};

template<bool T>
inline size_t
StrategyChoiceOptions::wireEncode(EncodingImpl<T>& block) const
{
  size_t totalLength = 0;
  
  if (!m_strategy.empty())
  {
    totalLength += prependNestedBlock(block, tlv::nfd::Strategy, m_strategy);
  }
  
  totalLength += m_name.wireEncode(block);
  
  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(tlv::nfd::StrategyChoiceOptions);
  return totalLength;
}

inline const Block&
StrategyChoiceOptions::wireEncode() const
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
StrategyChoiceOptions::wireDecode(const Block& wire)
{
  m_name.clear();
  m_strategy.clear();
  
  m_wire = wire;
  
  if (m_wire.type() != tlv::nfd::StrategyChoiceOptions)
    throw Error("Requested decoding of StrategyChoiceOptions, but Block is of different type");
  
  m_wire.parse();
  
  // Name
  Block::element_const_iterator val = m_wire.find(Tlv::Name);
  if (val != m_wire.elements_end())
  {
    m_name.wireDecode(*val);
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
operator << (std::ostream& os, const StrategyChoiceOptions& option)
{
  os << "StrategyChoiceOptions(";
  
  // Name
  os << "Name: " << option.getName() << ", ";
  
  // Strategy
  os << "Strategy: " << option.getStrategy() << ", ";
  
  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_MANAGEMENT_OPTIONS_HPP
