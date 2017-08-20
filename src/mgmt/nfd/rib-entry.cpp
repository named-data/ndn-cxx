/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "rib-entry.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"
#include "util/string-helper.hpp"

#include <boost/range/adaptor/reversed.hpp>

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<Route>));
BOOST_CONCEPT_ASSERT((StatusDatasetItem<RibEntry>));

Route::Route()
  : m_faceId(INVALID_FACE_ID)
  , m_origin(ROUTE_ORIGIN_APP)
  , m_cost(0)
  , m_flags(ROUTE_FLAG_CHILD_INHERIT)
{
}

Route::Route(const Block& block)
{
  this->wireDecode(block);
}

Route&
Route::setFaceId(uint64_t faceId)
{
  m_faceId = faceId;
  m_wire.reset();
  return *this;
}

Route&
Route::setOrigin(RouteOrigin origin)
{
  m_origin = origin;
  m_wire.reset();
  return *this;
}

Route&
Route::setCost(uint64_t cost)
{
  m_cost = cost;
  m_wire.reset();
  return *this;
}

Route&
Route::setFlags(uint64_t flags)
{
  m_flags = flags;
  m_wire.reset();
  return *this;
}

Route&
Route::setExpirationPeriod(time::milliseconds expirationPeriod)
{
  if (expirationPeriod == time::milliseconds::max())
    return unsetExpirationPeriod();

  m_expirationPeriod = expirationPeriod;
  m_wire.reset();
  return *this;
}

Route&
Route::unsetExpirationPeriod()
{
  m_expirationPeriod = nullopt;
  m_wire.reset();
  return *this;
}

template<encoding::Tag TAG>
size_t
Route::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;

  if (m_expirationPeriod) {
    totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::ExpirationPeriod,
                                                  static_cast<uint64_t>(m_expirationPeriod->count()));
  }
  totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::Flags, m_flags);
  totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::Cost, m_cost);
  totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::Origin, m_origin);
  totalLength += prependNonNegativeIntegerBlock(block, ndn::tlv::nfd::FaceId, m_faceId);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(ndn::tlv::nfd::Route);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(Route);

const Block&
Route::wireEncode() const
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
Route::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::Route) {
    BOOST_THROW_EXCEPTION(Error("expecting Route, but Block has type " + to_string(block.type())));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceId) {
    m_faceId = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required FaceId field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Origin) {
    m_origin = readNonNegativeIntegerAs<RouteOrigin>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Origin field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Cost) {
    m_cost = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Cost field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Flags) {
    m_flags = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Flags field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::ExpirationPeriod) {
    m_expirationPeriod.emplace(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    m_expirationPeriod = nullopt;
  }
}

bool
operator==(const Route& a, const Route& b)
{
  return a.getFaceId() == b.getFaceId() &&
      a.getOrigin() == b.getOrigin() &&
      a.getCost() == b.getCost() &&
      a.getFlags() == b.getFlags() &&
      a.getExpirationPeriod() == b.getExpirationPeriod();
}

std::ostream&
operator<<(std::ostream& os, const Route& route)
{
  os << "Route("
     << "FaceId: " << route.getFaceId() << ", "
     << "Origin: " << route.getOrigin() << ", "
     << "Cost: " << route.getCost() << ", "
     << "Flags: " << AsHex{route.getFlags()} << ", ";

  if (route.hasExpirationPeriod()) {
    os << "ExpirationPeriod: " << route.getExpirationPeriod();
  }
  else {
    os << "ExpirationPeriod: infinite";
  }

  return os << ")";
}

////////////////////

RibEntry::RibEntry() = default;

RibEntry::RibEntry(const Block& block)
{
  this->wireDecode(block);
}

RibEntry&
RibEntry::setName(const Name& prefix)
{
  m_prefix = prefix;
  m_wire.reset();
  return *this;
}

RibEntry&
RibEntry::addRoute(const Route& route)
{
  m_routes.push_back(route);
  m_wire.reset();
  return *this;
}

RibEntry&
RibEntry::clearRoutes()
{
  m_routes.clear();
  m_wire.reset();
  return *this;
}

template<encoding::Tag TAG>
size_t
RibEntry::wireEncode(EncodingImpl<TAG>& block) const
{
  size_t totalLength = 0;

  for (const auto& route : m_routes | boost::adaptors::reversed) {
    totalLength += route.wireEncode(block);
  }
  totalLength += m_prefix.wireEncode(block);

  totalLength += block.prependVarNumber(totalLength);
  totalLength += block.prependVarNumber(tlv::nfd::RibEntry);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(RibEntry);

const Block&
RibEntry::wireEncode() const
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
RibEntry::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::RibEntry) {
    BOOST_THROW_EXCEPTION(Error("expecting RibEntry, but Block has type " + to_string(block.type())));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val == m_wire.elements_end()) {
    BOOST_THROW_EXCEPTION(Error("unexpected end of RibEntry"));
  }
  else if (val->type() != tlv::Name) {
    BOOST_THROW_EXCEPTION(Error("expecting Name, but Block has type " + to_string(val->type())));
  }
  m_prefix.wireDecode(*val);
  ++val;

  m_routes.clear();
  for (; val != m_wire.elements_end(); ++val) {
    if (val->type() != tlv::nfd::Route) {
      BOOST_THROW_EXCEPTION(Error("expecting Route, but Block has type " + to_string(val->type())));
    }
    m_routes.emplace_back(*val);
  }
}

bool
operator==(const RibEntry& a, const RibEntry& b)
{
  const auto& aRoutes = a.getRoutes();
  const auto& bRoutes = b.getRoutes();

  if (a.getName() != b.getName() ||
      aRoutes.size() != bRoutes.size())
    return false;

  std::vector<bool> matched(bRoutes.size(), false);
  return std::all_of(aRoutes.begin(), aRoutes.end(),
                     [&] (const Route& route) {
                       for (size_t i = 0; i < bRoutes.size(); ++i) {
                         if (!matched[i] && bRoutes[i] == route) {
                           matched[i] = true;
                           return true;
                         }
                       }
                       return false;
                     });
}

std::ostream&
operator<<(std::ostream& os, const RibEntry& entry)
{
  os << "RibEntry(Prefix: " << entry.getName() << ",\n"
     << "         Routes: [";

  std::copy(entry.getRoutes().begin(), entry.getRoutes().end(),
            make_ostream_joiner(os, ",\n                  "));

  os << "]\n";

  return os << "         )";
}

} // namespace nfd
} // namespace ndn
