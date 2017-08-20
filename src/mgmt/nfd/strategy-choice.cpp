/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "strategy-choice.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<StrategyChoice>));

StrategyChoice::StrategyChoice() = default;

StrategyChoice::StrategyChoice(const Block& payload)
{
  this->wireDecode(payload);
}

template<encoding::Tag TAG>
size_t
StrategyChoice::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNestedBlock(encoder, tlv::nfd::Strategy, m_strategy);
  totalLength += m_name.wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::StrategyChoice);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(StrategyChoice);

const Block&
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

void
StrategyChoice::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::StrategyChoice) {
    BOOST_THROW_EXCEPTION(Error("expecting StrategyChoice block"));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::Name) {
    m_name.wireDecode(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Name field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Strategy) {
    val->parse();
    if (val->elements().empty()) {
      BOOST_THROW_EXCEPTION(Error("expecting Strategy/Name"));
    }
    else {
      m_strategy.wireDecode(*val->elements_begin());
    }
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Strategy field"));
  }
}

StrategyChoice&
StrategyChoice::setName(const Name& name)
{
  m_wire.reset();
  m_name = name;
  return *this;
}

StrategyChoice&
StrategyChoice::setStrategy(const Name& strategy)
{
  m_wire.reset();
  m_strategy = strategy;
  return *this;
}

bool
operator==(const StrategyChoice& a, const StrategyChoice& b)
{
  return a.getName() == b.getName() && a.getStrategy() == b.getStrategy();
}

std::ostream&
operator<<(std::ostream& os, const StrategyChoice& sc)
{
  return os << "StrategyChoice("
            << "Name: " << sc.getName() << ", "
            << "Strategy: " << sc.getStrategy()
            << ")";
}

} // namespace nfd
} // namespace ndn
