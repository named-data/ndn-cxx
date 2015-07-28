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

#include "meta-info.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<MetaInfo>));
BOOST_CONCEPT_ASSERT((WireEncodable<MetaInfo>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<MetaInfo>));
BOOST_CONCEPT_ASSERT((WireDecodable<MetaInfo>));
static_assert(std::is_base_of<tlv::Error, MetaInfo::Error>::value,
              "MetaInfo::Error must inherit from tlv::Error");

MetaInfo::MetaInfo()
  : m_type(tlv::ContentType_Blob)
  , m_freshnessPeriod(-1)
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
MetaInfo::setFreshnessPeriod(const time::milliseconds& freshnessPeriod)
{
  m_wire.reset();
  m_freshnessPeriod = freshnessPeriod;
  return *this;
}

MetaInfo&
MetaInfo::setFinalBlockId(const name::Component& finalBlockId)
{
  m_wire.reset();
  m_finalBlockId = finalBlockId;
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
  for (std::list<Block>::const_iterator i = info.begin(); i != info.end(); ++i) {
    if (!(128 <= i->type() && i->type() <= 252))
      BOOST_THROW_EXCEPTION(Error("AppMetaInfo block has type outside the application range "
                                  "[128, 252]"));
  }

  m_wire.reset();
  m_appMetaInfo = info;
  return *this;
}

MetaInfo&
MetaInfo::addAppMetaInfo(const Block& block)
{
  if (!(128 <= block.type() && block.type() <= 252))
    BOOST_THROW_EXCEPTION(Error("AppMetaInfo block has type outside the application range "
                                "[128, 252]"));

  m_wire.reset();
  m_appMetaInfo.push_back(block);
  return *this;
}

bool
MetaInfo::removeAppMetaInfo(uint32_t tlvType)
{
  for (std::list<Block>::iterator iter = m_appMetaInfo.begin();
       iter != m_appMetaInfo.end(); ++iter) {
    if (iter->type() == tlvType) {
      m_wire.reset();
      m_appMetaInfo.erase(iter);
      return true;
    }
  }
  return false;
}

const Block*
MetaInfo::findAppMetaInfo(uint32_t tlvType) const
{
  for (std::list<Block>::const_iterator iter = m_appMetaInfo.begin();
       iter != m_appMetaInfo.end(); ++iter) {
    if (iter->type() == tlvType) {
      return &*iter;
    }
  }
  return 0;
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

  for (std::list<Block>::const_reverse_iterator appMetaInfoItem = m_appMetaInfo.rbegin();
       appMetaInfoItem != m_appMetaInfo.rend(); ++appMetaInfoItem) {
    totalLength += encoder.prependBlock(*appMetaInfoItem);
  }

  // FinalBlockId
  if (!m_finalBlockId.empty())
    {
      totalLength += prependNestedBlock(encoder, tlv::FinalBlockId, m_finalBlockId);
    }

  // FreshnessPeriod
  if (m_freshnessPeriod >= time::milliseconds::zero())
    {
      totalLength += prependNonNegativeIntegerBlock(encoder, tlv::FreshnessPeriod,
                                                    m_freshnessPeriod.count());
    }

  // ContentType
  if (m_type != tlv::ContentType_Blob)
    {
      totalLength += prependNonNegativeIntegerBlock(encoder, tlv::ContentType, m_type);
    }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::MetaInfo);
  return totalLength;
}

template size_t
MetaInfo::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
MetaInfo::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

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


  Block::element_const_iterator val = m_wire.elements_begin();

  // ContentType
  if (val != m_wire.elements_end() && val->type() == tlv::ContentType) {
    m_type = readNonNegativeInteger(*val);
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
    m_freshnessPeriod = time::milliseconds::min();
  }

  // FinalBlockId
  if (val != m_wire.elements_end() && val->type() == tlv::FinalBlockId) {
    m_finalBlockId = val->blockFromValue();
    if (m_finalBlockId.type() != tlv::NameComponent)
      {
        /// @todo May or may not throw exception later...
        m_finalBlockId.reset();
      }
    ++val;
  }
  else {
    m_finalBlockId.reset();
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
  if (info.getFreshnessPeriod() >= time::milliseconds::zero()) {
    os << ", FreshnessPeriod: " << info.getFreshnessPeriod();
  }

  // FinalBlockId
  if (!info.getFinalBlockId().empty()) {
    os << ", FinalBlockId: ";
    info.getFinalBlockId().toUri(os);
  }

  // App-defined MetaInfo items
  for (std::list<Block>::const_iterator iter = info.getAppMetaInfo().begin();
       iter != info.getAppMetaInfo().end(); ++iter) {
    os << ", AppMetaInfoTlvType: " << iter->type();
  }

  return os;
}

} // namespace ndn
