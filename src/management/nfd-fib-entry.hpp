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

#ifndef NDN_MANAGEMENT_NFD_FIB_ENTRY_HPP
#define NDN_MANAGEMENT_NFD_FIB_ENTRY_HPP

#include "../encoding/block.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"
#include "../name.hpp"

#include <list>

namespace ndn {
namespace nfd {

// NextHopRecord := NEXT-HOP-RECORD TLV-LENGTH
//                    FaceId
//                    Cost

/**
 * @ingroup management
 */
class NextHopRecord
{
public:
  class Error : public Tlv::Error
  {
  public:
    Error(const std::string& what) : Tlv::Error(what) {}
  };

  NextHopRecord()
    : m_faceId(std::numeric_limits<uint64_t>::max())
    , m_cost(0)
  {
  }

  explicit
  NextHopRecord(const Block& block)
  {
    wireDecode(block);
  }

  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  NextHopRecord&
  setFaceId(uint64_t faceId)
  {
    m_faceId = faceId;
    m_wire.reset();
    return *this;
  }

  uint64_t
  getCost() const
  {
    return m_cost;
  }

  NextHopRecord&
  setCost(uint64_t cost)
  {
    m_cost = cost;
    m_wire.reset();
    return *this;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const
  {
    size_t totalLength = 0;
    totalLength += prependNonNegativeIntegerBlock(block,
                                                  ndn::tlv::nfd::Cost,
                                                  m_cost);

    totalLength += prependNonNegativeIntegerBlock(block,
                                                  ndn::tlv::nfd::FaceId,
                                                  m_faceId);

    totalLength += block.prependVarNumber(totalLength);
    totalLength += block.prependVarNumber(ndn::tlv::nfd::NextHopRecord);
    return totalLength;
  }

  const Block&
  wireEncode() const
  {
    if (m_wire.hasWire())
      {
        return m_wire;
      }

    EncodingEstimator estimator;
    size_t estimatedSize = wireEncode(estimator);

    EncodingBuffer buffer(estimatedSize, 0);
    wireEncode(buffer);

    m_wire = buffer.block();
    return m_wire;
  }

  void
  wireDecode(const Block& wire)
  {
    m_faceId = std::numeric_limits<uint64_t>::max();
    m_cost = 0;

    m_wire = wire;

    if (m_wire.type() != tlv::nfd::NextHopRecord)
      {
        std::stringstream error;
        error << "Requested decoding of NextHopRecord, but Block is of a different type: #"
              << m_wire.type();
        throw Error(error.str());
      }
    m_wire.parse();

    Block::element_const_iterator val = m_wire.elements_begin();
    if (val == m_wire.elements_end())
      {
        throw Error("Unexpected end of NextHopRecord");
      }
    else if (val->type() != tlv::nfd::FaceId)
      {
        std::stringstream error;
        error << "Expected FaceId, but Block is of a different type: #"
              << val->type();
        throw Error(error.str());
      }
    m_faceId = readNonNegativeInteger(*val);
    ++val;

    if (val == m_wire.elements_end())
      {
        throw Error("Unexpected end of NextHopRecord");
      }
    else if (val->type() != tlv::nfd::Cost)
      {
        std::stringstream error;
        error << "Expected Cost, but Block is of a different type: #"
              << m_wire.type();
        throw Error(error.str());
      }
    m_cost = readNonNegativeInteger(*val);
  }


private:
  uint64_t m_faceId;
  uint64_t m_cost;

  mutable Block m_wire;
};


// FibEntry      := FIB-ENTRY-TYPE TLV-LENGTH
//                    Name
//                    NextHopRecord*

/**
 * @ingroup management
 */
class FibEntry
{
public:
    class Error : public Tlv::Error
    {
    public:
      Error(const std::string& what) : Tlv::Error(what)
      {
      }
    };

  FibEntry()
  {
  }

  explicit
  FibEntry(const Block& block)
  {
    wireDecode(block);
  }

  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  FibEntry&
  setPrefix(const Name& prefix)
  {
    m_prefix = prefix;
    m_wire.reset();
    return *this;
  }

  const std::list<NextHopRecord>&
  getNextHopRecords() const
  {
    return m_nextHopRecords;
  }

  FibEntry&
  addNextHopRecord(const NextHopRecord& nextHopRecord)
  {
    m_nextHopRecords.push_back(nextHopRecord);
    m_wire.reset();
    return *this;
  }

  template<typename T>
  FibEntry&
  setNextHopRecords(const T& begin, const T& end)
  {
    m_nextHopRecords.clear();
    m_nextHopRecords.assign(begin, end);
    m_wire.reset();
    return *this;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const
  {
    size_t totalLength = 0;

    for (std::list<NextHopRecord>::const_reverse_iterator i = m_nextHopRecords.rbegin();
         i != m_nextHopRecords.rend();
         ++i)
      {
        totalLength += i->wireEncode(block);
      }

    totalLength += m_prefix.wireEncode(block);
    totalLength += block.prependVarNumber(totalLength);
    totalLength += block.prependVarNumber(tlv::nfd::FibEntry);

    return totalLength;
  }

  const Block&
  wireEncode() const
  {
    if (m_wire.hasWire())
      {
        return m_wire;
      }

    EncodingEstimator estimator;
    size_t estimatedSize = wireEncode(estimator);

    EncodingBuffer buffer(estimatedSize, 0);
    wireEncode(buffer);

    m_wire = buffer.block();

    return m_wire;
  }

  void
  wireDecode(const Block& wire)
  {
    m_prefix.clear();
    m_nextHopRecords.clear();

    m_wire = wire;

    if (m_wire.type() != tlv::nfd::FibEntry)
      {
        std::stringstream error;
        error << "Requested decoding of FibEntry, but Block is of a different type: #"
              << m_wire.type();
        throw Error(error.str());
      }

    m_wire.parse();

    Block::element_const_iterator val = m_wire.elements_begin();
    if (val == m_wire.elements_end())
      {
        throw Error("Unexpected end of FibEntry");
      }
    else if (val->type() != Tlv::Name)
      {
        std::stringstream error;
        error << "Expected Name, but Block is of a different type: #"
              << val->type();
        throw Error(error.str());
      }
    m_prefix.wireDecode(*val);
    ++val;

    for (; val != m_wire.elements_end(); ++val)
      {
        if (val->type() != tlv::nfd::NextHopRecord)
          {
            std::stringstream error;
            error << "Expected NextHopRecords, but Block is of a different type: #"
                  << val->type();
            throw Error(error.str());
          }
        m_nextHopRecords.push_back(NextHopRecord(*val));
      }
  }

private:
  Name m_prefix;
  std::list<NextHopRecord> m_nextHopRecords;

  mutable Block m_wire;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FIB_ENTRY_HPP
