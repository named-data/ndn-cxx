/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "cs-info.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<CsInfo>));

CsInfo::CsInfo()
  : m_capacity(0)
  , m_nEntries(0)
  , m_nHits(0)
  , m_nMisses(0)
{
}

CsInfo::CsInfo(const Block& block)
{
  this->wireDecode(block);
}

template<encoding::Tag TAG>
size_t
CsInfo::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NMisses, m_nMisses);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NHits, m_nHits);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NCsEntries, m_nEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Flags, m_flags.to_ullong());
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Capacity, m_capacity);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::CsInfo);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(CsInfo);

const Block&
CsInfo::wireEncode() const
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
CsInfo::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::CsInfo) {
    BOOST_THROW_EXCEPTION(Error("expecting CsInfo block, got " + to_string(block.type())));
  }
  m_wire = block;
  m_wire.parse();
  auto val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Capacity) {
    m_capacity = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Capacity field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Flags) {
    m_flags = FlagsBitSet(static_cast<unsigned long long>(readNonNegativeInteger(*val)));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Flags field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NCsEntries) {
    m_nEntries = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NCsEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NHits) {
    m_nHits = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NHits field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NMisses) {
    m_nMisses = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NMisses field"));
  }
}

CsInfo&
CsInfo::setCapacity(uint64_t capacity)
{
  m_wire.reset();
  m_capacity = capacity;
  return *this;
}

CsInfo&
CsInfo::setEnableAdmit(bool enableAdmit)
{
  m_wire.reset();
  m_flags[BIT_CS_ENABLE_ADMIT] = enableAdmit;
  return *this;
}

CsInfo&
CsInfo::setEnableServe(bool enableServe)
{
  m_wire.reset();
  m_flags[BIT_CS_ENABLE_SERVE] = enableServe;
  return *this;
}

CsInfo&
CsInfo::setNEntries(uint64_t nEntries)
{
  m_wire.reset();
  m_nEntries = nEntries;
  return *this;
}

CsInfo&
CsInfo::setNHits(uint64_t nHits)
{
  m_wire.reset();
  m_nHits = nHits;
  return *this;
}

CsInfo&
CsInfo::setNMisses(uint64_t nMisses)
{
  m_wire.reset();
  m_nMisses = nMisses;
  return *this;
}

bool
operator==(const CsInfo& a, const CsInfo& b)
{
  return a.wireEncode() == b.wireEncode();
}

std::ostream&
operator<<(std::ostream& os, const CsInfo& csi)
{
  os << "CS: "
     << csi.getNEntries() << " entries, " << csi.getCapacity() << " max, "
     << (csi.getEnableAdmit() ? "admit enabled, " : "admit disabled, ")
     << (csi.getEnableServe() ? "serve enabled, " : "serve disabled, ")
     << csi.getNHits() << (csi.getNHits() == 1 ? " hit, " : " hits, ")
     << csi.getNMisses() << (csi.getNMisses() == 1 ? " miss" : " misses");
  return os;
}

} // namespace nfd
} // namespace ndn
