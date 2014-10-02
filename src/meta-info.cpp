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

#include "meta-info.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "util/time.hpp"

namespace ndn {

MetaInfo::MetaInfo()
  : m_type(TYPE_DEFAULT)
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

template<bool T>
size_t
MetaInfo::wireEncode(EncodingImpl<T>& blk) const
{
  // MetaInfo ::= META-INFO-TYPE TLV-LENGTH
  //                ContentType?
  //                FreshnessPeriod?
  //                FinalBlockId?

  size_t totalLength = 0;

  // FinalBlockId
  if (!m_finalBlockId.empty())
    {
      totalLength += prependNestedBlock(blk, tlv::FinalBlockId, m_finalBlockId);
    }

  // FreshnessPeriod
  if (m_freshnessPeriod >= time::milliseconds::zero())
    {
      totalLength += prependNonNegativeIntegerBlock(blk, tlv::FreshnessPeriod,
                                                    m_freshnessPeriod.count());
    }

  // ContentType
  if (m_type != TYPE_DEFAULT)
    {
      totalLength += prependNonNegativeIntegerBlock(blk, tlv::ContentType, m_type);
    }

  totalLength += blk.prependVarNumber(totalLength);
  totalLength += blk.prependVarNumber(tlv::MetaInfo);
  return totalLength;
}

template size_t
MetaInfo::wireEncode<true>(EncodingImpl<true>& block) const;

template size_t
MetaInfo::wireEncode<false>(EncodingImpl<false>& block) const;

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

  // ContentType
  Block::element_const_iterator val = m_wire.find(tlv::ContentType);
  if (val != m_wire.elements().end())
    {
      m_type = readNonNegativeInteger(*val);
    }
  else
    m_type = TYPE_DEFAULT;

  // FreshnessPeriod
  val = m_wire.find(tlv::FreshnessPeriod);
  if (val != m_wire.elements().end())
    {
      m_freshnessPeriod = time::milliseconds(readNonNegativeInteger(*val));
    }
  else
    m_freshnessPeriod = time::milliseconds::min();

  // FinalBlockId
  val = m_wire.find(tlv::FinalBlockId);
  if (val != m_wire.elements().end())
    {
      m_finalBlockId = val->blockFromValue();
      if (m_finalBlockId.type() != tlv::NameComponent)
        {
          /// @todo May or may not throw exception later...
          m_finalBlockId.reset();
        }
    }
  else
    m_finalBlockId.reset();
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

  if (!info.getFinalBlockId().empty()) {
    os << ", FinalBlockId: ";
    info.getFinalBlockId().toUri(os);
  }
  return os;
}

} // namespace ndn
