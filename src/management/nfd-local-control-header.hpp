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

#ifndef NDN_MANAGEMENT_NFD_LOCAL_CONTROL_HEADER_HPP
#define NDN_MANAGEMENT_NFD_LOCAL_CONTROL_HEADER_HPP

#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"
#include "../encoding/block-helpers.hpp"

namespace ndn {
namespace nfd {

/**
 * @ingroup management
 * @brief Class to handle work with LocalControlHeader
 * @sa http://redmine.named-data.net/projects/nfd/wiki/LocalControlHeader
 */
class LocalControlHeader
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  enum EncodeFlags : uint8_t {
    ENCODE_NONE             = 0,
    ENCODE_INCOMING_FACE_ID = (1 << 0),
    ENCODE_NEXT_HOP         = (1 << 1),
    ENCODE_CACHING_POLICY   = (1 << 2),
    ENCODE_ALL              = 0xff
  };

  enum CachingPolicy : uint8_t {
    INVALID_POLICY = 0,
    NO_CACHE       = 1
  };

  LocalControlHeader()
    : m_incomingFaceId(INVALID_FACE_ID)
    , m_nextHopFaceId(INVALID_FACE_ID)
    , m_cachingPolicy(CachingPolicy::INVALID_POLICY)
  {
  }

  /**
   * @brief Create from wire encoding
   *
   * @sa wireDecode
   */
  explicit
  LocalControlHeader(const Block& wire, uint8_t encodeMask = ENCODE_ALL)
  {
    wireDecode(wire, encodeMask);
  }

  /**
   * @brief Create wire encoding with options LocalControlHeader and the supplied item
   *
   * The caller is responsible of checking whether LocalControlHeader contains
   * any information.
   *
   * !It is an error to call this method if none of IncomingFaceId, NextHopFaceId and CachingPolicy
   * are set, or neither of them are enabled.
   *
   * @throws LocalControlHeader::Error when empty LocalControlHeader be produced
   *
   * @returns Block, containing LocalControlHeader. Top-level length field of the
   *          returned LocalControlHeader includes payload length, but the memory
   *          block is independent of the payload's wire buffer.  It is expected
   *          that both LocalControlHeader's and payload's wire will be send out
   *          together within a single send call.
   *
   * @see http://redmine.named-data.net/projects/nfd/wiki/LocalControlHeader
   */
  template<class U>
  inline Block
  wireEncode(const U& payload, uint8_t encodeMask = ENCODE_ALL) const;

  /**
   * @brief Decode from the wire format and set LocalControlHeader on the supplied item
   *
   * The supplied wire MUST contain LocalControlHeader.  Determination whether the optional
   * LocalControlHeader should be done before calling this method.
   */
  inline void
  wireDecode(const Block& wire, uint8_t encodeMask = ENCODE_ALL);

  inline static const Block&
  getPayload(const Block& wire);

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // Getters/setters

  bool
  empty(uint8_t encodeMask) const
  {
    bool needIncomingFaceId = encodeMask & ENCODE_INCOMING_FACE_ID;
    bool needNextHopFaceId = encodeMask & ENCODE_NEXT_HOP;
    bool needCachingPolicy = encodeMask & ENCODE_CACHING_POLICY;

    return !((needIncomingFaceId && hasIncomingFaceId()) ||
             (needNextHopFaceId  && hasNextHopFaceId())  ||
             (needCachingPolicy  && hasCachingPolicy()));
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
  }

  //

  bool
  hasCachingPolicy() const
  {
    return m_cachingPolicy != CachingPolicy::INVALID_POLICY;
  }

  CachingPolicy
  getCachingPolicy() const
  {
    return m_cachingPolicy;
  }

  void
  setCachingPolicy(CachingPolicy cachingPolicy)
  {
    m_cachingPolicy = cachingPolicy;
  }

private:
  template<encoding::Tag TAG>
  inline size_t
  wireEncode(EncodingImpl<TAG>& block, size_t payloadSize, uint8_t encodeMask) const;

private:
  uint64_t m_incomingFaceId;
  uint64_t m_nextHopFaceId;
  CachingPolicy m_cachingPolicy;
};


/**
 * @brief Fast encoding or block size estimation
 */
template<encoding::Tag TAG>
inline size_t
LocalControlHeader::wireEncode(EncodingImpl<TAG>& block, size_t payloadSize,
                               uint8_t encodeMask) const
{
  bool needIncomingFaceId = encodeMask & ENCODE_INCOMING_FACE_ID;
  bool needNextHopFaceId = encodeMask & ENCODE_NEXT_HOP;
  bool needCachingPolicy = encodeMask & ENCODE_CACHING_POLICY;

  size_t totalLength = payloadSize;

  if (needIncomingFaceId && hasIncomingFaceId())
    {
      totalLength += prependNonNegativeIntegerBlock(block,
                                                    tlv::nfd::IncomingFaceId, getIncomingFaceId());
    }

  if (needNextHopFaceId && hasNextHopFaceId())
    {
      totalLength += prependNonNegativeIntegerBlock(block,
                                                    tlv::nfd::NextHopFaceId, getNextHopFaceId());
    }

  if (needCachingPolicy && hasCachingPolicy())
    {
      size_t cachingPolicyLength = 0;
      cachingPolicyLength += block.prependVarNumber(0);
      cachingPolicyLength += block.prependVarNumber(tlv::nfd::NoCache);
      cachingPolicyLength += block.prependVarNumber(cachingPolicyLength);
      cachingPolicyLength += block.prependVarNumber(tlv::nfd::CachingPolicy);

      totalLength += cachingPolicyLength;
    }

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(tlv::nfd::LocalControlHeader);
  return totalLength;
}

template<class U>
inline Block
LocalControlHeader::wireEncode(const U& payload, uint8_t encodeMask) const
{
  /// @todo should this be BOOST_ASSERT instead?  This is kind of unnecessary overhead
  if (empty(encodeMask))
    BOOST_THROW_EXCEPTION(Error("Requested wire for LocalControlHeader, but none of the fields are "
                                "set or enabled"));

  EncodingEstimator estimator;
  size_t length = wireEncode(estimator, payload.wireEncode().size(), encodeMask);

  EncodingBuffer buffer(length);
  wireEncode(buffer, payload.wireEncode().size(), encodeMask);

  return buffer.block(false);
}

inline void
LocalControlHeader::wireDecode(const Block& wire, uint8_t encodeMask)
{
  bool needIncomingFaceId = encodeMask & ENCODE_INCOMING_FACE_ID;
  bool needNextHopFaceId = encodeMask & ENCODE_NEXT_HOP;
  bool needCachingPolicy = encodeMask & ENCODE_CACHING_POLICY;

  BOOST_ASSERT(wire.type() == tlv::nfd::LocalControlHeader);
  wire.parse();

  m_incomingFaceId = INVALID_FACE_ID;
  m_nextHopFaceId = INVALID_FACE_ID;
  m_cachingPolicy = CachingPolicy::INVALID_POLICY;

  for (Block::element_const_iterator i = wire.elements_begin();
       i != wire.elements_end();
       ++i)
    {
      switch (i->type())
        {
        case tlv::nfd::IncomingFaceId:
          if (needIncomingFaceId)
            m_incomingFaceId = readNonNegativeInteger(*i);
          break;
        case tlv::nfd::NextHopFaceId:
          if (needNextHopFaceId)
            m_nextHopFaceId = readNonNegativeInteger(*i);
          break;
        case tlv::nfd::CachingPolicy:
          if (needCachingPolicy) {
            i->parse();
            Block::element_const_iterator it = i->elements_begin();
            if (it != i->elements_end() && it->type() == tlv::nfd::NoCache) {
              m_cachingPolicy = CachingPolicy::NO_CACHE;
            }
            else {
              BOOST_THROW_EXCEPTION(Error("CachingPolicy: Missing required NoCache field"));
            }
          }
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
