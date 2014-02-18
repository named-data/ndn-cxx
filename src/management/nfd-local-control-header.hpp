/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_LOCAL_CONTROL_HEADER_HPP
#define NDN_MANAGEMENT_NFD_LOCAL_CONTROL_HEADER_HPP

#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"

namespace ndn {
namespace nfd {

const uint64_t INVALID_FACE_ID = std::numeric_limits<uint64_t>::max();
const size_t   ESTIMATED_LOCAL_HEADER_RESERVE = 10;

class LocalControlHeader
{
public:
  LocalControlHeader()
    : m_incomingFaceId(INVALID_FACE_ID)
    , m_nextHopFaceId(INVALID_FACE_ID)
  {
  }

  /**
   * @brief Create wire encoding with options LocalControlHeader and the supplied item
   *
   * This method will return wire encoding of the item if none of the LocalControlHeader
   * fields are set, otherwise it will encapsulate the item inside LocalControlHeader
   *
   * Note that this method will use default maximum packet size (8800 bytes) during the
   * encoding process.
   *
   * @see http://redmine.named-data.net/projects/nfd/wiki/LocalControlHeader
   */
  template<class U>
  inline const Block&
  wireEncode(const U& item) const;
  
  /**
   * @brief Decode from the wire format and set LocalControlHeader on the supplied item
   *
   * The supplied wire MUST contain LocalControlHeader.  Determination whether the optional
   * LocalControlHeader should be done before calling this method.
   */
  inline void 
  wireDecode(const Block& wire);

  inline static const Block&
  getPayload(const Block& wire);
  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // Gettest/setters

  bool
  empty() const
  {
    return (!hasIncomingFaceId() && !hasNextHopFaceId());
  }
  
  //
  
  bool
  hasIncomingFaceId() const
  {
    return m_incomingFaceId != INVALID_FACE_ID;
  }
  
  uint64_t
  getIncomingFaceId() const
  {
    return m_incomingFaceId;
  }

  void
  setIncomingFaceId(uint64_t incomingFaceId)
  {
    m_incomingFaceId = incomingFaceId;
    m_wire.reset();
  }

  //

  bool
  hasNextHopFaceId() const
  {
    return m_nextHopFaceId != INVALID_FACE_ID;
  }
  
  uint64_t
  getNextHopFaceId() const
  {
    return m_nextHopFaceId;
  }

  void
  setNextHopFaceId(uint64_t nextHopFaceId)
  {
    m_nextHopFaceId = nextHopFaceId;
    m_wire.reset();
  }

private:
  template<bool T, class U>
  inline size_t
  wireEncode(EncodingImpl<T>& block, const U& item) const;
  
private:
  uint64_t m_incomingFaceId;
  uint64_t m_nextHopFaceId;

  mutable Block m_wire;
};


/**
 * @brief Fast encoding or block size estimation
 */
template<bool T, class U>
inline size_t
LocalControlHeader::wireEncode(EncodingImpl<T>& block, const U& item) const
{
  size_t total_len = item.wireEncode().size();

  if (hasIncomingFaceId())
    {
      total_len += prependNonNegativeIntegerBlock(block,
                                                  tlv::nfd::IncomingFaceId, getIncomingFaceId());
    }

  if (hasNextHopFaceId())
    {
      total_len += prependNonNegativeIntegerBlock(block,
                                                  tlv::nfd::NextHopFaceId, getNextHopFaceId());
    }
  
  total_len += block.prependVarNumber(total_len);
  total_len += block.prependVarNumber(tlv::nfd::LocalControlHeader);
  return total_len;
}

template<class U>
inline const Block&
LocalControlHeader::wireEncode(const U& item) const
{
  if (item.hasWire() && m_wire.hasWire())
    return m_wire;

  if (empty())
    {
      if (item.hasWire())
        return item.wireEncode();
      else
        {
          EncodingBuffer buffer; // use default (maximum) packet size here
          item.wireEncode(buffer);
          item.m_wire = buffer.block();
          m_wire = buffer.block();
        }
    }
  else
    {
      if (item.hasWire())
        {
          // extend the existing buffer
          EncodingBuffer buffer(item.wireEncode());
          wireEncode(buffer, item);
          m_wire = buffer.block();
        }
      else
        {
          EncodingBuffer buffer;
          item.wireEncode(buffer);
          item.m_wire = buffer.block();

          wireEncode(buffer, item);
          m_wire = buffer.block();
        }
    }
  return m_wire;
}

inline void 
LocalControlHeader::wireDecode(const Block& wire)
{
  BOOST_ASSERT(wire.type() == tlv::nfd::LocalControlHeader);
  m_wire = wire;
  m_wire.parse();

  m_incomingFaceId = INVALID_FACE_ID;
  m_nextHopFaceId = INVALID_FACE_ID;

  for (Block::element_const_iterator i = m_wire.elements_begin();
       i != m_wire.elements_end();
       ++i)
    {
      switch(i->type())
        {
        case tlv::nfd::IncomingFaceId:
          m_incomingFaceId = readNonNegativeInteger(*i);
          break;
        case tlv::nfd::NextHopFaceId:
          m_nextHopFaceId = readNonNegativeInteger(*i);
          break;
        default:
          // ignore all unsupported
          break;
        }
    }
}

inline const Block&
LocalControlHeader::getPayload(const Block& wire)
{
  if (wire.type() == tlv::nfd::LocalControlHeader)
    {
      wire.parse();
      if (wire.elements_size() < 1)
        return wire; // don't throw an error, but don't continue processing

      return wire.elements()[wire.elements().size()-1];
    }
  else
    {
      return wire;
    }
}


} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_LOCAL_CONTROL_HEADER_HPP
