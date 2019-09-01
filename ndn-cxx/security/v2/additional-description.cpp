/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/security/v2/additional-description.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/util/concepts.hpp"
#include "ndn-cxx/util/ostream-joiner.hpp"

namespace ndn {
namespace security {
namespace v2 {

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
    NDN_THROW(Error("Entry does not exist for key (" + key + ")"));

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

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(AdditionalDescription);

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
     NDN_THROW(Error("The supplied block does not contain wire format"));
  }

  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::AdditionalDescription)
    NDN_THROW(Error("AdditionalDescription", m_wire.type()));

  auto it = m_wire.elements_begin();
  while (it != m_wire.elements_end()) {
    const Block& entry = *it;
    entry.parse();

    if (entry.type() != tlv::DescriptionEntry)
      NDN_THROW(Error("DescriptionEntry", entry.type()));

    if (entry.elements_size() != 2)
      NDN_THROW(Error("DescriptionEntry does not have two sub-TLVs"));

    if (entry.elements()[KEY_OFFSET].type() != tlv::DescriptionKey ||
        entry.elements()[VALUE_OFFSET].type() != tlv::DescriptionValue)
      NDN_THROW(Error("Invalid DescriptionKey or DescriptionValue field"));

    m_info[readString(entry.elements()[KEY_OFFSET])] = readString(entry.elements()[VALUE_OFFSET]);
    it++;
  }
}

std::ostream&
operator<<(std::ostream& os, const AdditionalDescription& desc)
{
  os << "[";

  auto join = make_ostream_joiner(os, ", ");
  for (const auto& entry : desc) {
    join = "(" + entry.first + ":" + entry.second + ")";
  }

  return os << "]";
}

} // namespace v2
} // namespace security
} // namespace ndn
