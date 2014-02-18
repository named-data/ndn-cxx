/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_META_INFO_HPP
#define NDN_META_INFO_HPP

#include "encoding/encoding-buffer.hpp"

namespace ndn {

/**
 * An MetaInfo holds the meta info which is signed inside the data packet.
 */
class MetaInfo {
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

  MetaInfo(const Block& block)
  {
    wireDecode(block);
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;

  const Block& 
  wireEncode() const;
  
  void
  wireDecode(const Block &wire);  
  
  ///////////////////////////////////////////////////////////////////////////////
  // Gettest/setters
  
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
  
  Milliseconds 
  getFreshnessPeriod() const
  {
    return m_freshnessPeriod;
  }
  
  MetaInfo&
  setFreshnessPeriod(Milliseconds freshnessPeriod)
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
  
private:
  uint32_t m_type;
  Milliseconds m_freshnessPeriod;
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
  
  size_t total_len = 0;

  // FinalBlockId
  if (!m_finalBlockId.empty())
    {
      total_len += prependNestedBlock(blk, Tlv::FinalBlockId, m_finalBlockId);
    }
  
  // FreshnessPeriod
  if (m_freshnessPeriod >= 0)
    {
      total_len += prependNonNegativeIntegerBlock(blk, Tlv::FreshnessPeriod, m_freshnessPeriod);
    }

  // ContentType
  if (m_type != TYPE_DEFAULT)
    {
      total_len += prependNonNegativeIntegerBlock(blk, Tlv::ContentType, m_type);
    }

  total_len += blk.prependVarNumber (total_len);
  total_len += blk.prependVarNumber (Tlv::MetaInfo);
  return total_len;
}

inline const Block& 
MetaInfo::wireEncode() const
{
  if (m_wire.hasWire ())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);
  
  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}
  
inline void
MetaInfo::wireDecode(const Block &wire)
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
      m_freshnessPeriod = readNonNegativeInteger(*val);
    }
  else
    m_freshnessPeriod = -1;

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
operator << (std::ostream &os, const MetaInfo &info)
{
  // ContentType
  os << "ContentType: " << info.getType();

  // FreshnessPeriod
  if (info.getFreshnessPeriod() >= 0) {
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
