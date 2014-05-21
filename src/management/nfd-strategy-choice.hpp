/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#ifndef NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_HPP
#define NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_HPP

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"
#include "../name.hpp"

namespace ndn {
namespace nfd {

/**
 * @ingroup management
 * @brief represents NFD StrategyChoice dataset
 * @sa http://redmine.named-data.net/projects/nfd/wiki/StrategyChoice#Strategy-Choice-Dataset
 */
class StrategyChoice
{
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  StrategyChoice()
  {
  }

  explicit
  StrategyChoice(const Block& payload)
  {
    this->wireDecode(payload);
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

public: // getters & setters
  const Name&
  getName() const
  {
    return m_name;
  }

  StrategyChoice&
  setName(const Name& name)
  {
    m_wire.reset();
    m_name = name;
    return *this;
  }

  const Name&
  getStrategy() const
  {
    return m_strategy;
  }

  StrategyChoice&
  setStrategy(const Name& strategy)
  {
    m_wire.reset();
    m_strategy = strategy;
    return *this;
  }

private:
  Name m_name; // namespace
  Name m_strategy; // strategy for the namespace

  mutable Block m_wire;
};


template<bool T>
inline size_t
StrategyChoice::wireEncode(EncodingImpl<T>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNestedBlock(encoder, tlv::nfd::Strategy, m_strategy);
  totalLength += m_name.wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::StrategyChoice);
  return totalLength;
}

inline const Block&
StrategyChoice::wireEncode() const
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
StrategyChoice::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::StrategyChoice) {
    throw Error("expecting StrategyChoice block");
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == Tlv::Name) {
    m_name.wireDecode(*val);
    ++val;
  }
  else {
    throw Error("missing required Name field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Strategy) {
    val->parse();
    if (val->elements().empty()) {
      throw Error("expecting Strategy/Name");
    }
    else {
      m_strategy.wireDecode(*val->elements_begin());
    }
    ++val;
  }
  else {
    throw Error("missing required Strategy field");
  }
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_STRATEGY_CHOICE_HPP
