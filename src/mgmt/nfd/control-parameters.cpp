/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "control-parameters.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"
#include "util/string-helper.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ControlParameters>));
BOOST_CONCEPT_ASSERT((WireEncodable<ControlParameters>));
BOOST_CONCEPT_ASSERT((WireDecodable<ControlParameters>));
static_assert(std::is_base_of<tlv::Error, ControlParameters::Error>::value,
              "ControlParameters::Error must inherit from tlv::Error");

ControlParameters::ControlParameters()
  : m_hasFields(CONTROL_PARAMETER_UBOUND)
{
}

ControlParameters::ControlParameters(const Block& block)
  : m_hasFields(CONTROL_PARAMETER_UBOUND)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
ControlParameters::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  if (this->hasDefaultCongestionThreshold()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::DefaultCongestionThreshold,
                                                  m_defaultCongestionThreshold);
  }
  if (this->hasBaseCongestionMarkingInterval()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::BaseCongestionMarkingInterval,
                                                  m_baseCongestionMarkingInterval.count());
  }
  if (this->hasFacePersistency()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FacePersistency, m_facePersistency);
  }
  if (this->hasExpirationPeriod()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::ExpirationPeriod,
                                                  m_expirationPeriod.count());
  }
  if (this->hasStrategy()) {
    totalLength += prependNestedBlock(encoder, tlv::nfd::Strategy, m_strategy);
  }
  if (this->hasMask()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Mask, m_mask);
  }
  if (this->hasFlags()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Flags, m_flags);
  }
  if (this->hasCount()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Count, m_count);
  }
  if (this->hasCapacity()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Capacity, m_capacity);
  }
  if (this->hasCost()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Cost, m_cost);
  }
  if (this->hasOrigin()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Origin, m_origin);
  }
  if (this->hasLocalUri()) {
    totalLength += prependStringBlock(encoder, tlv::nfd::LocalUri, m_localUri);
  }
  if (this->hasUri()) {
    totalLength += prependStringBlock(encoder, tlv::nfd::Uri, m_uri);
  }
  if (this->hasFaceId()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FaceId, m_faceId);
  }
  if (this->hasName()) {
    totalLength += m_name.wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::ControlParameters);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(ControlParameters);

Block
ControlParameters::wireEncode() const
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
ControlParameters::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::ControlParameters) {
    BOOST_THROW_EXCEPTION(Error("Expecting TLV-TYPE ControlParameters"));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val;

  val = m_wire.find(tlv::Name);
  m_hasFields[CONTROL_PARAMETER_NAME] = val != m_wire.elements_end();
  if (this->hasName()) {
    m_name.wireDecode(*val);
  }

  val = m_wire.find(tlv::nfd::FaceId);
  m_hasFields[CONTROL_PARAMETER_FACE_ID] = val != m_wire.elements_end();
  if (this->hasFaceId()) {
    m_faceId = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::nfd::Uri);
  m_hasFields[CONTROL_PARAMETER_URI] = val != m_wire.elements_end();
  if (this->hasUri()) {
    m_uri = readString(*val);
  }

  val = m_wire.find(tlv::nfd::LocalUri);
  m_hasFields[CONTROL_PARAMETER_LOCAL_URI] = val != m_wire.elements_end();
  if (this->hasLocalUri()) {
    m_localUri = readString(*val);
  }

  val = m_wire.find(tlv::nfd::Origin);
  m_hasFields[CONTROL_PARAMETER_ORIGIN] = val != m_wire.elements_end();
  if (this->hasOrigin()) {
    m_origin = readNonNegativeIntegerAs<RouteOrigin>(*val);
  }

  val = m_wire.find(tlv::nfd::Cost);
  m_hasFields[CONTROL_PARAMETER_COST] = val != m_wire.elements_end();
  if (this->hasCost()) {
    m_cost = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::nfd::Capacity);
  m_hasFields[CONTROL_PARAMETER_CAPACITY] = val != m_wire.elements_end();
  if (this->hasCapacity()) {
    m_capacity = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::nfd::Count);
  m_hasFields[CONTROL_PARAMETER_COUNT] = val != m_wire.elements_end();
  if (this->hasCount()) {
    m_count = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::nfd::Flags);
  m_hasFields[CONTROL_PARAMETER_FLAGS] = val != m_wire.elements_end();
  if (this->hasFlags()) {
    m_flags = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::nfd::Mask);
  m_hasFields[CONTROL_PARAMETER_MASK] = val != m_wire.elements_end();
  if (this->hasMask()) {
    m_mask = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::nfd::Strategy);
  m_hasFields[CONTROL_PARAMETER_STRATEGY] = val != m_wire.elements_end();
  if (this->hasStrategy()) {
    val->parse();
    if (val->elements().empty()) {
      BOOST_THROW_EXCEPTION(Error("Expecting Strategy/Name"));
    }
    else {
      m_strategy.wireDecode(*val->elements_begin());
    }
  }

  val = m_wire.find(tlv::nfd::ExpirationPeriod);
  m_hasFields[CONTROL_PARAMETER_EXPIRATION_PERIOD] = val != m_wire.elements_end();
  if (this->hasExpirationPeriod()) {
    m_expirationPeriod = time::milliseconds(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::FacePersistency);
  m_hasFields[CONTROL_PARAMETER_FACE_PERSISTENCY] = val != m_wire.elements_end();
  if (this->hasFacePersistency()) {
    m_facePersistency = readNonNegativeIntegerAs<FacePersistency>(*val);
  }

  val = m_wire.find(tlv::nfd::BaseCongestionMarkingInterval);
  m_hasFields[CONTROL_PARAMETER_BASE_CONGESTION_MARKING_INTERVAL] = val != m_wire.elements_end();
  if (this->hasBaseCongestionMarkingInterval()) {
    m_baseCongestionMarkingInterval = time::nanoseconds(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::DefaultCongestionThreshold);
  m_hasFields[CONTROL_PARAMETER_DEFAULT_CONGESTION_THRESHOLD] = val != m_wire.elements_end();
  if (this->hasDefaultCongestionThreshold()) {
    m_defaultCongestionThreshold = readNonNegativeInteger(*val);
  }
}

bool
ControlParameters::hasFlagBit(size_t bit) const
{
  if (bit >= 64) {
    BOOST_THROW_EXCEPTION(std::out_of_range("bit must be within range [0, 64)"));
  }

  if (!hasMask()) {
    return false;
  }

  return getMask() & (1 << bit);
}

bool
ControlParameters::getFlagBit(size_t bit) const
{
  if (bit >= 64) {
    BOOST_THROW_EXCEPTION(std::out_of_range("bit must be within range [0, 64)"));
  }

  if (!hasFlags()) {
    return false;
  }

  return getFlags() & (1 << bit);
}

ControlParameters&
ControlParameters::setFlagBit(size_t bit, bool value, bool wantMask/* = true*/)
{
  if (bit >= 64) {
    BOOST_THROW_EXCEPTION(std::out_of_range("bit must be within range [0, 64)"));
  }

  uint64_t flags = hasFlags() ? getFlags() : 0;
  if (value) {
    flags |= (1 << bit);
  }
  else {
    flags &= ~(1 << bit);
  }
  setFlags(flags);

  if (wantMask) {
    uint64_t mask = hasMask() ? getMask() : 0;
    mask |= (1 << bit);
    setMask(mask);
  }

  return *this;
}

ControlParameters&
ControlParameters::unsetFlagBit(size_t bit)
{
  if (bit >= 64) {
    BOOST_THROW_EXCEPTION(std::out_of_range("bit must be within range [0, 64)"));
  }

  uint64_t mask = hasMask() ? getMask() : 0;
  mask &= ~(1 << bit);
  if (mask == 0) {
    unsetMask();
    unsetFlags();
  }
  else {
    setMask(mask);
  }

  return *this;
}

std::ostream&
operator<<(std::ostream& os, const ControlParameters& parameters)
{
  os << "ControlParameters(";

  if (parameters.hasName()) {
    os << "Name: " << parameters.getName() << ", ";
  }

  if (parameters.hasFaceId()) {
    os << "FaceId: " << parameters.getFaceId() << ", ";
  }

  if (parameters.hasUri()) {
    os << "Uri: " << parameters.getUri() << ", ";
  }

  if (parameters.hasLocalUri()) {
    os << "LocalUri: " << parameters.getLocalUri() << ", ";
  }

  if (parameters.hasOrigin()) {
    os << "Origin: " << parameters.getOrigin() << ", ";
  }

  if (parameters.hasCost()) {
    os << "Cost: " << parameters.getCost() << ", ";
  }

  if (parameters.hasCapacity()) {
    os << "Capacity: " << parameters.getCapacity() << ", ";
  }

  if (parameters.hasCount()) {
    os << "Count: " << parameters.getCount() << ", ";
  }

  if (parameters.hasFlags()) {
    os << "Flags: " << AsHex{parameters.getFlags()} << ", ";
  }

  if (parameters.hasMask()) {
    os << "Mask: " << AsHex{parameters.getMask()} << ", ";
  }

  if (parameters.hasStrategy()) {
    os << "Strategy: " << parameters.getStrategy() << ", ";
  }

  if (parameters.hasExpirationPeriod()) {
    os << "ExpirationPeriod: " << parameters.getExpirationPeriod() << ", ";
  }

  if (parameters.hasFacePersistency()) {
    os << "FacePersistency: " << parameters.getFacePersistency() << ", ";
  }

  if (parameters.hasBaseCongestionMarkingInterval()) {
    os << "BaseCongestionMarkingInterval: " << parameters.getBaseCongestionMarkingInterval() << ", ";
  }

  if (parameters.hasDefaultCongestionThreshold()) {
    os << "DefaultCongestionThreshold: " << parameters.getDefaultCongestionThreshold() << ", ";
  }

  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn
