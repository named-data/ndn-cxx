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

#include "nfd-rib-entry.hpp"
#include "encoding/tlv-nfd.hpp"
#include "encoding/block-helpers.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Route>));
BOOST_CONCEPT_ASSERT((WireEncodable<Route>));
BOOST_CONCEPT_ASSERT((WireDecodable<Route>));
static_assert(std::is_base_of<tlv::Error, Route::Error>::value,
              "Route::Error must inherit from tlv::Error");

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<RibEntry>));
BOOST_CONCEPT_ASSERT((WireEncodable<RibEntry>));
BOOST_CONCEPT_ASSERT((WireDecodable<RibEntry>));
static_assert(std::is_base_of<tlv::Error, RibEntry::Error>::value,
              "RibEntry::Error must inherit from tlv::Error");

const time::milliseconds Route::INFINITE_EXPIRATION_PERIOD(time::milliseconds::max());

Route::Route()
  : m_faceId(0)
  , m_origin(0)
  , m_cost(0)
  , m_flags(ROUTE_FLAG_CHILD_INHERIT)
  , m_expirationPeriod(INFINITE_EXPIRATION_PERIOD)
  , m_hasInfiniteExpirationPeriod(true)
{
}

Route::Route(const Block& block)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
Route::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;

  // Absence of an ExpirationPeriod signifies non-expiration
  if (!m_hasInfiniteExpirationPeriod) {
    totalLength += prependNonNegativeIntegerBlock(block,
                                                  ndn::tlv::nfd::ExpirationPeriod,
                                                  m_expirationPeriod.count());
  }

  totalLength += prependNonNegativeIntegerBlock(block,
                                                ndn::tlv::nfd::Flags,
                                                m_flags);

  totalLength += prependNonNegativeIntegerBlock(block,
                                                ndn::tlv::nfd::Cost,
                                                m_cost);

  totalLength += prependNonNegativeIntegerBlock(block,
                                                ndn::tlv::nfd::Origin,
                                                m_origin);

  totalLength += prependNonNegativeIntegerBlock(block,
                                                ndn::tlv::nfd::FaceId,
                                                m_faceId);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(ndn::tlv::nfd::Route);

  return totalLength;
}

template size_t
Route::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& block) const;

template size_t
Route::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& block) const;

const Block&
Route::wireEncode() const
{
  if (m_wire.hasWire()) {
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
Route::wireDecode(const Block& wire)
{
  m_faceId = 0;
  m_origin = 0;
  m_cost = 0;
  m_flags = 0;
  m_expirationPeriod = time::milliseconds::min();

  m_wire = wire;

  if (m_wire.type() != tlv::nfd::Route) {
    std::stringstream error;
    error << "Expected Route Block, but Block is of a different type: #"
          << m_wire.type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }

  m_wire.parse();

  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceId) {
    m_faceId = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Missing required FaceId field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Origin) {
    m_origin = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Missing required Origin field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Cost) {
    m_cost = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Missing required Cost field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Flags) {
    m_flags = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Missing required Flags field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::ExpirationPeriod) {
    m_expirationPeriod = time::milliseconds(readNonNegativeInteger(*val));
    m_hasInfiniteExpirationPeriod = false;
  }
  else {
    m_expirationPeriod = INFINITE_EXPIRATION_PERIOD;
    m_hasInfiniteExpirationPeriod = true;
  }
}

std::ostream&
operator<<(std::ostream& os, const Route& route)
{
  os << "Route("
     << "FaceId: " << route.getFaceId() << ", "
     << "Origin: " << route.getOrigin() << ", "
     << "Cost: " << route.getCost() << ", "
     << "Flags: " << route.getFlags() << ", ";

  if (!route.hasInfiniteExpirationPeriod()) {
    os << "ExpirationPeriod: " << route.getExpirationPeriod();
  }
  else {
    os << "ExpirationPeriod: Infinity";
  }

  os << ")";

  return os;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


RibEntry::RibEntry()
{
}

RibEntry::RibEntry(const Block& block)
{
  wireDecode(block);
}


template<encoding::Tag TAG>
size_t
RibEntry::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;

  for (std::list<Route>::const_reverse_iterator it = m_routes.rbegin();
       it != m_routes.rend(); ++it)
    {
      totalLength += it->wireEncode(block);
    }

  totalLength += m_prefix.wireEncode(block);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(tlv::nfd::RibEntry);

  return totalLength;
}

template size_t
RibEntry::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& block) const;

template size_t
RibEntry::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& block) const;

const Block&
RibEntry::wireEncode() const
{
  if (m_wire.hasWire()) {
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
RibEntry::wireDecode(const Block& wire)
{
  m_prefix.clear();
  m_routes.clear();

  m_wire = wire;

  if (m_wire.type() != tlv::nfd::RibEntry) {
    std::stringstream error;
    error << "Expected RibEntry Block, but Block is of a different type: #"
          << m_wire.type();
    BOOST_THROW_EXCEPTION(Error(error.str()));
  }

  m_wire.parse();

  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::Name) {
    m_prefix.wireDecode(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Missing required Name field"));
  }

  for (; val != m_wire.elements_end(); ++val) {

    if (val->type() == tlv::nfd::Route) {
      m_routes.push_back(Route(*val));
    }
    else {
      std::stringstream error;
      error << "Expected Route Block, but Block is of a different type: #"
            << m_wire.type();
      BOOST_THROW_EXCEPTION(Error(error.str()));
    }
  }
}

std::ostream&
operator<<(std::ostream& os, const RibEntry& entry)
{
  os << "RibEntry{\n"
     << "  Name: " << entry.getName() << "\n";

  for (RibEntry::iterator it = entry.begin(); it != entry.end(); ++it) {
    os << "  " << *it << "\n";
  }

  os << "}";

  return os;
}

} // namespace nfd
} // namespace ndn
