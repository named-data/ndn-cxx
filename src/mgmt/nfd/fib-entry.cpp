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

#include "fib-entry.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"

#include <boost/range/adaptor/reversed.hpp>

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<NextHopRecord>));
BOOST_CONCEPT_ASSERT((StatusDatasetItem<FibEntry>));

NextHopRecord::NextHopRecord()
  : m_faceId(INVALID_FACE_ID)
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

  totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::Cost, m_cost);
  totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::FaceId, m_faceId);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(ndn::tlv::nfd::NextHopRecord);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(NextHopRecord);

const Block&
NextHopRecord::wireEncode() const
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
NextHopRecord::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::NextHopRecord) {
    BOOST_THROW_EXCEPTION(Error("expecting NextHopRecord, but Block has type " + to_string(block.type())));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("unexpected end of NextHopRecord"));
  }
  else if (val->type() != tlv::nfd::FaceId) {
    BOOST_THROW_EXCEPTION(Error("expecting FaceId, but Block has type " + to_string(val->type())));
  }
  m_faceId = readNonNegativeInteger(*val);
  ++val;

  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("unexpected end of NextHopRecord"));
  }
  else if (val->type() != tlv::nfd::Cost) {
    BOOST_THROW_EXCEPTION(Error("expecting Cost, but Block has type " + to_string(val->type())));
  }
  m_cost = readNonNegativeInteger(*val);
  ++val;
}

bool
operator==(const NextHopRecord& a, const NextHopRecord& b)
{
  return a.getFaceId() == b.getFaceId() &&
      a.getCost() == b.getCost();
}

std::ostream&
operator<<(std::ostream& os, const NextHopRecord& nh)
{
  return os << "NextHopRecord("
            << "FaceId: " << nh.getFaceId() << ", "
            << "Cost: " << nh.getCost()
            << ")";
}

////////////////////

FibEntry::FibEntry() = default;

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
FibEntry::addNextHopRecord(const NextHopRecord& nh)
{
  m_nextHopRecords.push_back(nh);
  m_wire.reset();
  return *this;
}

FibEntry&
FibEntry::clearNextHopRecords()
{
  m_nextHopRecords.clear();
  m_wire.reset();
  return *this;
}

template<encoding::Tag TAG>
size_t
FibEntry::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;

  for (const auto& nh : m_nextHopRecords | boost::adaptors::reversed) {
    totalLength += nh.wireEncode(block);
  }
  totalLength += m_prefix.wireEncode(block);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(tlv::nfd::FibEntry);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(FibEntry);

const Block&
FibEntry::wireEncode() const
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
FibEntry::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::FibEntry) {
    BOOST_THROW_EXCEPTION(Error("expecting FibEntry, but Block has type " + to_string(block.type())));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("unexpected end of FibEntry"));
  }
  else if (val->type() != tlv::Name) {
    BOOST_THROW_EXCEPTION(Error("expecting Name, but Block has type " + to_string(val->type())));
  }
  m_prefix.wireDecode(*val);
  ++val;

  m_nextHopRecords.clear();
  for (; val != m_wire.elements_end(); ++val) {
    if (val->type() != tlv::nfd::NextHopRecord) {
      BOOST_THROW_EXCEPTION(Error("expecting NextHopRecord, but Block has type " + to_string(val->type())));
    }
    m_nextHopRecords.emplace_back(*val);
  }
}

bool
operator==(const FibEntry& a, const FibEntry& b)
{
  const auto& aNextHops = a.getNextHopRecords();
  const auto& bNextHops = b.getNextHopRecords();

  if (a.getPrefix() != b.getPrefix() ||
      aNextHops.size() != bNextHops.size())
    return false;

  std::vector<bool> matched(bNextHops.size(), false);
  return std::all_of(aNextHops.begin(), aNextHops.end(),
                     [&] (const NextHopRecord& nh) {
                       for (size_t i = 0; i < bNextHops.size(); ++i) {
                         if (!matched[i] && bNextHops[i] == nh) {
                           matched[i] = true;
                           return true;
                         }
                       }
                       return false;
                     });
}

std::ostream&
operator<<(std::ostream& os, const FibEntry& entry)
{
  os << "FibEntry(Prefix: " << entry.getPrefix() << ",\n"
     << "         NextHops: [";

  std::copy(entry.getNextHopRecords().begin(), entry.getNextHopRecords().end(),
            make_ostream_joiner(os, ",\n                    "));

  os << "]\n";

  return os << "         )";
}

} // namespace nfd
} // namespace ndn
