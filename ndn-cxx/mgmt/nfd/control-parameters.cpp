/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#include "ndn-cxx/mgmt/nfd/control-parameters.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/encoding/tlv-nfd.hpp"
#include "ndn-cxx/util/string-helper.hpp"

namespace ndn::nfd {

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

  if (this->hasMtu()) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Mtu, m_mtu);
  }
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
    NDN_THROW(Error("ControlParameters", block.type()));
  }

  *this = {};
  m_wire = block;
  m_wire.parse();

  for (const auto& e : m_wire.elements()) {
    switch (e.type()) {
    case tlv::Name:
      m_hasFields[CONTROL_PARAMETER_NAME] = true;
      m_name.wireDecode(e);
      break;
    case tlv::nfd::FaceId:
      m_hasFields[CONTROL_PARAMETER_FACE_ID] = true;
      m_faceId = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Uri:
      m_hasFields[CONTROL_PARAMETER_URI] = true;
      m_uri = readString(e);
      break;
    case tlv::nfd::LocalUri:
      m_hasFields[CONTROL_PARAMETER_LOCAL_URI] = true;
      m_localUri = readString(e);
      break;
    case tlv::nfd::Origin:
      m_hasFields[CONTROL_PARAMETER_ORIGIN] = true;
      m_origin = readNonNegativeIntegerAs<RouteOrigin>(e);
      break;
    case tlv::nfd::Cost:
      m_hasFields[CONTROL_PARAMETER_COST] = true;
      m_cost = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Capacity:
      m_hasFields[CONTROL_PARAMETER_CAPACITY] = true;
      m_capacity = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Count:
      m_hasFields[CONTROL_PARAMETER_COUNT] = true;
      m_count = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Flags:
      m_hasFields[CONTROL_PARAMETER_FLAGS] = true;
      m_flags = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Mask:
      m_hasFields[CONTROL_PARAMETER_MASK] = true;
      m_mask = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Strategy:
      m_hasFields[CONTROL_PARAMETER_STRATEGY] = true;
      e.parse();
      if (e.elements().empty()) {
        NDN_THROW(Error("Expecting Strategy.Name"));
      }
      m_strategy.wireDecode(e.elements().front());
      break;
    case tlv::nfd::ExpirationPeriod:
      m_hasFields[CONTROL_PARAMETER_EXPIRATION_PERIOD] = true;
      m_expirationPeriod = time::milliseconds(readNonNegativeInteger(e));
      break;
    case tlv::nfd::FacePersistency:
      m_hasFields[CONTROL_PARAMETER_FACE_PERSISTENCY] = true;
      m_facePersistency = readNonNegativeIntegerAs<FacePersistency>(e);
      break;
    case tlv::nfd::BaseCongestionMarkingInterval:
      m_hasFields[CONTROL_PARAMETER_BASE_CONGESTION_MARKING_INTERVAL] = true;
      m_baseCongestionMarkingInterval = time::nanoseconds(readNonNegativeInteger(e));
      break;
    case tlv::nfd::DefaultCongestionThreshold:
      m_hasFields[CONTROL_PARAMETER_DEFAULT_CONGESTION_THRESHOLD] = true;
      m_defaultCongestionThreshold = readNonNegativeInteger(e);
      break;
    case tlv::nfd::Mtu:
      m_hasFields[CONTROL_PARAMETER_MTU] = true;
      m_mtu = readNonNegativeInteger(e);
      break;
    default:
      // ignore unrecognized elements
      break;
    }
  }
}

bool
ControlParameters::hasFlagBit(size_t bit) const
{
  if (bit >= 64) {
    NDN_THROW(std::out_of_range("bit must be within range [0, 64)"));
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
    NDN_THROW(std::out_of_range("bit must be within range [0, 64)"));
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
    NDN_THROW(std::out_of_range("bit must be within range [0, 64)"));
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
    NDN_THROW(std::out_of_range("bit must be within range [0, 64)"));
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

  if (parameters.hasMtu()) {
    os << "Mtu: " << parameters.getMtu() << ", ";
  }

  os << ")";
  return os;
}

} // namespace ndn::nfd
