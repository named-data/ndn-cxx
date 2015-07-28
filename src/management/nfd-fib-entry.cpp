/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "nfd-fib-entry.hpp"
#include <sstream>
#include "encoding/tlv-nfd.hpp"
#include "encoding/block-helpers.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<NextHopRecord>));
BOOST_CONCEPT_ASSERT((WireEncodable<NextHopRecord>));
BOOST_CONCEPT_ASSERT((WireDecodable<NextHopRecord>));
static_assert(std::is_base_of<tlv::Error, NextHopRecord::Error>::value,
              "NextHopRecord::Error must inherit from tlv::Error");

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FibEntry>));
BOOST_CONCEPT_ASSERT((WireEncodable<FibEntry>));
BOOST_CONCEPT_ASSERT((WireDecodable<FibEntry>));
static_assert(std::is_base_of<tlv::Error, FibEntry::Error>::value,
              "FibEntry::Error must inherit from tlv::Error");

// NextHopRecord := NEXT-HOP-RECORD TLV-LENGTH
//                    FaceId
//                    Cost

NextHopRecord::NextHopRecord()
  : m_faceId(std::numeric_limits<uint64_t>::max())
  , m_cost(0)
{
}

NextHopRecord::NextHopRecord(const Block& block)
{
  this->wireDecode(block);
}

NextHopRecord&
NextHopRecord::setFaceId(uint64_t faceId)
{
  m_faceId = faceId;
  m_wire.reset();
  return *this;
}

NextHopRecord&
NextHopRecord::setCost(uint64_t cost)
{
  m_cost = cost;
  m_wire.reset();
  return *this;
}

template<encoding::Tag TAG>
size_t
NextHopRecord::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;
  totalLength += prependNonNegativeIntegerBlock(block,
                                                ndn::tlv::nfd::Cost,
                                                m_cost);

  totalLength += prependNonNegativeIntegerBlock(block,
                                                ndn::tlv::nfd::FaceId,
                                                m_faceId);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(ndn::tlv::nfd::NextHopRecord);
  return totalLength;
}

template size_t
NextHopRecord::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& block) const;

template size_t
NextHopRecord::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& block) const;

const Block&
NextHopRecord::wireEncode() const
{
  if (m_wire.hasWire()) {
    return m_wire;
  }

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
NextHopRecord::wireDecode(const Block& wire)
{
  m_faceId = std::numeric_limits<uint64_t>::max();
  m_cost = 0;

  m_wire = wire;

  if (m_wire.type() != tlv::nfd::NextHopRecord) {
    std::stringstream error;
    error << "Requested decoding of NextHopRecord, but Block is of a different type: #"
          << m_wire.type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }
  m_wire.parse();

  Block::element_const_iterator val = m_wire.elements_begin();
  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("Unexpected end of NextHopRecord"));
  }
  else if (val->type() != tlv::nfd::FaceId) {
    std::stringstream error;
    error << "Expected FaceId, but Block is of a different type: #"
          << val->type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }
  m_faceId = readNonNegativeInteger(*val);
  ++val;

  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("Unexpected end of NextHopRecord"));
  }
  else if (val->type() != tlv::nfd::Cost) {
    std::stringstream error;
    error << "Expected Cost, but Block is of a different type: #"
          << m_wire.type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }
  m_cost = readNonNegativeInteger(*val);
}

// FibEntry      := FIB-ENTRY-TYPE TLV-LENGTH
//                    Name
//                    NextHopRecord*

FibEntry::FibEntry()
{
}

FibEntry::FibEntry(const Block& block)
{
  this->wireDecode(block);
}

FibEntry&
FibEntry::setPrefix(const Name& prefix)
{
  m_prefix = prefix;
  m_wire.reset();
  return *this;
}

FibEntry&
FibEntry::addNextHopRecord(const NextHopRecord& nextHopRecord)
{
  m_nextHopRecords.push_back(nextHopRecord);
  m_wire.reset();
  return *this;
}

template<encoding::Tag TAG>
size_t
FibEntry::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;

  for (auto i = m_nextHopRecords.rbegin(); i != m_nextHopRecords.rend(); ++i) {
    totalLength += i->wireEncode(block);
  }

  totalLength += m_prefix.wireEncode(block);
  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(tlv::nfd::FibEntry);

  return totalLength;
}

template size_t
FibEntry::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& block) const;

template size_t
FibEntry::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& block) const;

const Block&
FibEntry::wireEncode() const
{
  if (m_wire.hasWire()) {
    return m_wire;
  }

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();

  return m_wire;
}

void
FibEntry::wireDecode(const Block& wire)
{
  m_prefix.clear();
  m_nextHopRecords.clear();

  m_wire = wire;

  if (m_wire.type() != tlv::nfd::FibEntry) {
    std::stringstream error;
    error << "Requested decoding of FibEntry, but Block is of a different type: #"
          << m_wire.type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }

  m_wire.parse();

  Block::element_const_iterator val = m_wire.elements_begin();
  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("Unexpected end of FibEntry"));
  }
  else if (val->type() != tlv::Name) {
    std::stringstream error;
    error << "Expected Name, but Block is of a different type: #"
          << val->type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }
  m_prefix.wireDecode(*val);
  ++val;

  for (; val != m_wire.elements_end(); ++val) {
    if (val->type() != tlv::nfd::NextHopRecord) {
      std::stringstream error;
      error << "Expected NextHopRecords, but Block is of a different type: #"
            << val->type();
      BOOST_THROW_EXCEPTION(Error(error.str()));
    }
    m_nextHopRecords.push_back(NextHopRecord(*val));
  }
}

} // namespace nfd
} // namespace ndn
