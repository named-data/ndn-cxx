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

#include "additional-description.hpp"
#include "../util/concepts.hpp"
#include "../encoding/block-helpers.hpp"

namespace ndn {
namespace security {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<AdditionalDescription>));
BOOST_CONCEPT_ASSERT((WireEncodable<AdditionalDescription>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<AdditionalDescription>));
BOOST_CONCEPT_ASSERT((WireDecodable<AdditionalDescription>));
static_assert(std::is_base_of<tlv::Error, AdditionalDescription::Error>::value,
              "AdditionalDescription::Error must inherit from tlv::Error");

static const size_t KEY_OFFSET = 0;
static const size_t VALUE_OFFSET = 1;

AdditionalDescription::AdditionalDescription(const Block& block)
{
  wireDecode(block);
}

const std::string&
AdditionalDescription::get(const std::string& key) const
{
  auto it = m_info.find(key);
  if (it == m_info.end())
    BOOST_THROW_EXCEPTION(Error("Entry does not exist for key (" + key + ")"));

  return it->second;
}

void
AdditionalDescription::set(const std::string& key, const std::string& value)
{
  m_info[key] = value;
}

bool
AdditionalDescription::has(const std::string& key) const
{
  return (m_info.find(key) != m_info.end());
}

AdditionalDescription::iterator
AdditionalDescription::begin()
{
  return m_info.begin();
}

AdditionalDescription::iterator
AdditionalDescription::end()
{
  return m_info.end();
}

AdditionalDescription::const_iterator
AdditionalDescription::begin() const
{
  return m_info.begin();
}

AdditionalDescription::const_iterator
AdditionalDescription::end() const
{
  return m_info.end();
}

template<encoding::Tag TAG>
size_t
AdditionalDescription::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  for (auto it = m_info.rbegin(); it != m_info.rend(); it++) {
    size_t entryLength = 0;
    entryLength += prependStringBlock(encoder, tlv::DescriptionValue, it->second);
    entryLength += prependStringBlock(encoder, tlv::DescriptionKey, it->first);
    entryLength += encoder.prependVarNumber(entryLength);
    entryLength += encoder.prependVarNumber(tlv::DescriptionEntry);

    totalLength += entryLength;
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::AdditionalDescription);
  return totalLength;
}

template size_t
AdditionalDescription::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
AdditionalDescription::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
AdditionalDescription::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  m_wire.parse();

  return m_wire;
}

void
AdditionalDescription::wireDecode(const Block& wire)
{
   if (!wire.hasWire()) {
     BOOST_THROW_EXCEPTION(Error("The supplied block does not contain wire format"));
  }

  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::AdditionalDescription)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV type when decoding AdditionalDescription"));

  Block::element_const_iterator it = m_wire.elements_begin();
  while (it != m_wire.elements_end()) {
    const Block& entry = *it;
    entry.parse();

    if (entry.type() != tlv::DescriptionEntry)
      BOOST_THROW_EXCEPTION(Error("Unexpected TLV type when decoding DescriptionEntry"));

    if (entry.elements_size() != 2)
      BOOST_THROW_EXCEPTION(Error("DescriptionEntry does not have two sub-TLVs"));

    if (entry.elements()[KEY_OFFSET].type() != tlv::DescriptionKey ||
        entry.elements()[VALUE_OFFSET].type() != tlv::DescriptionValue)
      BOOST_THROW_EXCEPTION(Error("Invalid DescriptionKey or DescriptionValue field"));

    m_info[readString(entry.elements()[KEY_OFFSET])] = readString(entry.elements()[VALUE_OFFSET]);
    it++;
  }
}

bool
AdditionalDescription::operator==(const AdditionalDescription& other) const
{
  return (m_info == other.m_info);
}

bool
AdditionalDescription::operator!=(const AdditionalDescription& other) const
{
  return !(*this == other);
}

std::ostream&
operator<<(std::ostream& os, const AdditionalDescription& other)
{
  size_t count = 0;
  os << "(";
  for (const auto& entry : other) {
    if (count > 0)
      os << ", ";
    os << "(" << entry.first << ":" << entry.second << ")";
    count++;
  }
  os << ")";

  return os;
}

} // namespace security
} // namespace ndn
