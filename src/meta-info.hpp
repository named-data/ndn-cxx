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

#ifndef NDN_META_INFO_HPP
#define NDN_META_INFO_HPP

#include "encoding/encoding-buffer.hpp"

namespace ndn {

/**
 * An MetaInfo holds the meta info which is signed inside the data packet.
 */
class MetaInfo
{
public:
  enum {
    TYPE_DEFAULT = 0,
    TYPE_LINK = 1,
    TYPE_KEY = 2
  };

  MetaInfo()
    : m_type(TYPE_DEFAULT)
    , m_freshnessPeriod(-1)
  {
  }

  /**
   * @brief Create from wire encoding
   */
  MetaInfo(const Block& block)
  {
    wireDecode(block);
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

  ///////////////////////////////////////////////////////////////////////////////
  // Getters/setters

  uint32_t
  getType() const
  {
    return m_type;
  }

  MetaInfo&
  setType(uint32_t type)
  {
    m_wire.reset();
    m_type = type;
    return *this;
  }

  const time::milliseconds&
  getFreshnessPeriod() const
  {
    return m_freshnessPeriod;
  }

  MetaInfo&
  setFreshnessPeriod(const time::milliseconds& freshnessPeriod)
  {
    m_wire.reset();
    m_freshnessPeriod = freshnessPeriod;
    return *this;
  }

  const name::Component&
  getFinalBlockId() const
  {
    return m_finalBlockId;
  }

  MetaInfo&
  setFinalBlockId(const name::Component& finalBlockId)
  {
    m_wire.reset();
    m_finalBlockId = finalBlockId;
    return *this;
  }

public: // EqualityComparable concept
  bool
  operator==(const MetaInfo& other) const
  {
    return wireEncode() == other.wireEncode();
  }

  bool
  operator!=(const MetaInfo& other) const
  {
    return !(*this == other);
  }

private:
  uint32_t m_type;
  time::milliseconds m_freshnessPeriod;
  name::Component m_finalBlockId;

  mutable Block m_wire;
};

template<bool T>
inline size_t
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
      totalLength += prependNestedBlock(blk, Tlv::FinalBlockId, m_finalBlockId);
    }

  // FreshnessPeriod
  if (m_freshnessPeriod >= time::milliseconds::zero())
    {
      totalLength += prependNonNegativeIntegerBlock(blk, Tlv::FreshnessPeriod,
                                                    m_freshnessPeriod.count());
    }

  // ContentType
  if (m_type != TYPE_DEFAULT)
    {
      totalLength += prependNonNegativeIntegerBlock(blk, Tlv::ContentType, m_type);
    }

  totalLength += blk.prependVarNumber(totalLength);
  totalLength += blk.prependVarNumber(Tlv::MetaInfo);
  return totalLength;
}

inline const Block&
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

inline void
MetaInfo::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // MetaInfo ::= META-INFO-TYPE TLV-LENGTH
  //                ContentType?
  //                FreshnessPeriod?

  // ContentType
  Block::element_const_iterator val = m_wire.find(Tlv::ContentType);
  if (val != m_wire.elements().end())
    {
      m_type = readNonNegativeInteger(*val);
    }
  else
    m_type = TYPE_DEFAULT;

  // FreshnessPeriod
  val = m_wire.find(Tlv::FreshnessPeriod);
  if (val != m_wire.elements().end())
    {
      m_freshnessPeriod = time::milliseconds(readNonNegativeInteger(*val));
    }
  else
    m_freshnessPeriod = time::milliseconds::min();

  // FinalBlockId
  val = m_wire.find(Tlv::FinalBlockId);
  if (val != m_wire.elements().end())
    {
      m_finalBlockId = val->blockFromValue();
      if (m_finalBlockId.type() != Tlv::NameComponent)
        {
          /// @todo May or may not throw exception later...
          m_finalBlockId.reset();
        }
    }
  else
    m_finalBlockId.reset();
}

inline std::ostream&
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

#endif // NDN_META_INFO_HPP
