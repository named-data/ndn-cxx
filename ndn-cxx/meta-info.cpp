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

#include "ndn-cxx/meta-info.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/encoding/encoding-buffer.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((WireEncodable<MetaInfo>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<MetaInfo>));
BOOST_CONCEPT_ASSERT((WireDecodable<MetaInfo>));
static_assert(std::is_base_of<tlv::Error, MetaInfo::Error>::value,
              "MetaInfo::Error must inherit from tlv::Error");

MetaInfo::MetaInfo()
  : m_type(tlv::ContentType_Blob)
  , m_freshnessPeriod(DEFAULT_FRESHNESS_PERIOD)
{
}

MetaInfo::MetaInfo(const Block& block)
{
  wireDecode(block);
}

MetaInfo&
MetaInfo::setType(uint32_t type)
{
  m_wire.reset();
  m_type = type;
  return *this;
}

MetaInfo&
MetaInfo::setFreshnessPeriod(time::milliseconds freshnessPeriod)
{
  if (freshnessPeriod < time::milliseconds::zero()) {
    NDN_THROW(std::invalid_argument("FreshnessPeriod must be >= 0"));
  }
  m_wire.reset();
  m_freshnessPeriod = freshnessPeriod;
  return *this;
}

MetaInfo&
MetaInfo::setFinalBlock(optional<name::Component> finalBlockId)
{
  m_wire.reset();
  m_finalBlockId = std::move(finalBlockId);
  return *this;
}

const std::list<Block>&
MetaInfo::getAppMetaInfo() const
{
  return m_appMetaInfo;
}

MetaInfo&
MetaInfo::setAppMetaInfo(const std::list<Block>& info)
{
  for (const auto& block : info) {
    if (block.type() < 128 || block.type() > 252)
      NDN_THROW(Error("AppMetaInfo block has type outside the application range [128, 252]"));
  }

  m_wire.reset();
  m_appMetaInfo = info;
  return *this;
}

MetaInfo&
MetaInfo::addAppMetaInfo(const Block& block)
{
  if (!(128 <= block.type() && block.type() <= 252))
    NDN_THROW(Error("AppMetaInfo block has type outside the application range [128, 252]"));

  m_wire.reset();
  m_appMetaInfo.push_back(block);
  return *this;
}

bool
MetaInfo::removeAppMetaInfo(uint32_t tlvType)
{
  for (auto it = m_appMetaInfo.begin(); it != m_appMetaInfo.end(); ++it) {
    if (it->type() == tlvType) {
      m_wire.reset();
      m_appMetaInfo.erase(it);
      return true;
    }
  }
  return false;
}

const Block*
MetaInfo::findAppMetaInfo(uint32_t tlvType) const
{
  auto it = std::find_if(m_appMetaInfo.begin(), m_appMetaInfo.end(),
                         [=] (const Block& b) { return b.type() == tlvType; });
  return it != m_appMetaInfo.end() ? &*it : nullptr;
}

template<encoding::Tag TAG>
size_t
MetaInfo::wireEncode(EncodingImpl<TAG>& encoder) const
{
  // MetaInfo ::= META-INFO-TYPE TLV-LENGTH
  //                ContentType?
  //                FreshnessPeriod?
  //                FinalBlockId?
  //                AppMetaInfo*

  size_t totalLength = 0;

  for (auto it = m_appMetaInfo.rbegin(); it != m_appMetaInfo.rend(); ++it) {
    totalLength += encoder.prependBlock(*it);
  }

  // FinalBlockId
  if (m_finalBlockId) {
    totalLength += prependNestedBlock(encoder, tlv::FinalBlockId, *m_finalBlockId);
  }

  // FreshnessPeriod
  if (m_freshnessPeriod != DEFAULT_FRESHNESS_PERIOD) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::FreshnessPeriod,
                                                  static_cast<uint64_t>(m_freshnessPeriod.count()));
  }

  // ContentType
  if (m_type != tlv::ContentType_Blob) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::ContentType, m_type);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::MetaInfo);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(MetaInfo);

const Block&
MetaInfo::wireEncode() const
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
MetaInfo::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // MetaInfo ::= META-INFO-TYPE TLV-LENGTH
  //                ContentType?
  //                FreshnessPeriod?
  //                FinalBlockId?
  //                AppMetaInfo*

  auto val = m_wire.elements_begin();

  // ContentType
  if (val != m_wire.elements_end() && val->type() == tlv::ContentType) {
    m_type = readNonNegativeIntegerAs<uint32_t>(*val);
    ++val;
  }
  else {
    m_type = tlv::ContentType_Blob;
  }

  // FreshnessPeriod
  if (val != m_wire.elements_end() && val->type() == tlv::FreshnessPeriod) {
    m_freshnessPeriod = time::milliseconds(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    m_freshnessPeriod = DEFAULT_FRESHNESS_PERIOD;
  }

  // FinalBlockId
  if (val != m_wire.elements_end() && val->type() == tlv::FinalBlockId) {
    m_finalBlockId.emplace(val->blockFromValue());
    ++val;
  }
  else {
    m_finalBlockId = nullopt;
  }

  // AppMetaInfo (if any)
  for (; val != m_wire.elements().end(); ++val) {
    m_appMetaInfo.push_back(*val);
  }
}

std::ostream&
operator<<(std::ostream& os, const MetaInfo& info)
{
  // ContentType
  os << "ContentType: " << info.getType();

  // FreshnessPeriod
  if (info.getFreshnessPeriod() > 0_ms) {
    os << ", FreshnessPeriod: " << info.getFreshnessPeriod();
  }

  // FinalBlockId
  if (info.getFinalBlock()) {
    os << ", FinalBlockId: ";
    info.getFinalBlock()->toUri(os);
  }

  // App-defined MetaInfo items
  for (const auto& block : info.getAppMetaInfo()) {
    os << ", AppMetaInfoTlvType: " << block.type();
  }

  return os;
}

} // namespace ndn
