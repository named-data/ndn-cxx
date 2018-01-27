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

#include "face-status.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"
#include "util/string-helper.hpp"

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<FaceStatus>));

FaceStatus::FaceStatus()
  : m_nInInterests(0)
  , m_nInData(0)
  , m_nInNacks(0)
  , m_nOutInterests(0)
  , m_nOutData(0)
  , m_nOutNacks(0)
  , m_nInBytes(0)
  , m_nOutBytes(0)
{
}

FaceStatus::FaceStatus(const Block& block)
{
  this->wireDecode(block);
}

template<encoding::Tag TAG>
size_t
FaceStatus::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Flags, m_flags);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutBytes, m_nOutBytes);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInBytes, m_nInBytes);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutNacks, m_nOutNacks);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutData, m_nOutData);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutInterests, m_nOutInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInNacks, m_nInNacks);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInData, m_nInData);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInInterests, m_nInInterests);
  if (m_defaultCongestionThreshold) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::DefaultCongestionThreshold,
                                                  *m_defaultCongestionThreshold);
  }
  if (m_baseCongestionMarkingInterval) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::BaseCongestionMarkingInterval,
                                                  m_baseCongestionMarkingInterval->count());
  }
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::LinkType, m_linkType);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FacePersistency, m_facePersistency);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FaceScope, m_faceScope);
  if (m_expirationPeriod) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::ExpirationPeriod,
                                                  m_expirationPeriod->count());
  }
  totalLength += prependStringBlock(encoder, tlv::nfd::LocalUri, m_localUri);
  totalLength += prependStringBlock(encoder, tlv::nfd::Uri, m_remoteUri);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FaceId, m_faceId);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::FaceStatus);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(FaceStatus);

const Block&
FaceStatus::wireEncode() const
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
FaceStatus::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::FaceStatus) {
    BOOST_THROW_EXCEPTION(Error("expecting FaceStatus block"));
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

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Uri) {
    m_remoteUri = readString(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Uri field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri = readString(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required LocalUri field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::ExpirationPeriod) {
    m_expirationPeriod.emplace(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    m_expirationPeriod = nullopt;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceScope) {
    m_faceScope = readNonNegativeIntegerAs<FaceScope>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required FaceScope field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FacePersistency) {
    m_facePersistency = readNonNegativeIntegerAs<FacePersistency>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required FacePersistency field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LinkType) {
    m_linkType = readNonNegativeIntegerAs<LinkType>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required LinkType field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::BaseCongestionMarkingInterval) {
    m_baseCongestionMarkingInterval.emplace(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    m_baseCongestionMarkingInterval = nullopt;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::DefaultCongestionThreshold) {
    m_defaultCongestionThreshold = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    m_defaultCongestionThreshold = nullopt;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInInterests) {
    m_nInInterests = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInData) {
    m_nInData = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInData field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInNacks) {
    m_nInNacks = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInNacks field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutInterests) {
    m_nOutInterests = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutData) {
    m_nOutData = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutData field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutNacks) {
    m_nOutNacks = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutNacks field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInBytes) {
    m_nInBytes = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInBytes field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutBytes) {
    m_nOutBytes = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutBytes field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Flags) {
    m_flags = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Flags field"));
  }
}

FaceStatus&
FaceStatus::setExpirationPeriod(time::milliseconds expirationPeriod)
{
  m_wire.reset();
  m_expirationPeriod = expirationPeriod;
  return *this;
}

FaceStatus&
FaceStatus::unsetExpirationPeriod()
{
  m_wire.reset();
  m_expirationPeriod = nullopt;
  return *this;
}

FaceStatus&
FaceStatus::setBaseCongestionMarkingInterval(time::nanoseconds interval)
{
  m_wire.reset();
  m_baseCongestionMarkingInterval = interval;
  return *this;
}

FaceStatus&
FaceStatus::unsetBaseCongestionMarkingInterval()
{
  m_wire.reset();
  m_baseCongestionMarkingInterval = nullopt;
  return *this;
}

FaceStatus&
FaceStatus::setDefaultCongestionThreshold(uint64_t threshold)
{
  m_wire.reset();
  m_defaultCongestionThreshold = threshold;
  return *this;
}

FaceStatus&
FaceStatus::unsetDefaultCongestionThreshold()
{
  m_wire.reset();
  m_defaultCongestionThreshold = nullopt;
  return *this;
}

FaceStatus&
FaceStatus::setNInInterests(uint64_t nInInterests)
{
  m_wire.reset();
  m_nInInterests = nInInterests;
  return *this;
}

FaceStatus&
FaceStatus::setNInData(uint64_t nInData)
{
  m_wire.reset();
  m_nInData = nInData;
  return *this;
}

FaceStatus&
FaceStatus::setNInNacks(uint64_t nInNacks)
{
  m_wire.reset();
  m_nInNacks = nInNacks;
  return *this;
}

FaceStatus&
FaceStatus::setNOutInterests(uint64_t nOutInterests)
{
  m_wire.reset();
  m_nOutInterests = nOutInterests;
  return *this;
}

FaceStatus&
FaceStatus::setNOutData(uint64_t nOutData)
{
  m_wire.reset();
  m_nOutData = nOutData;
  return *this;
}

FaceStatus&
FaceStatus::setNOutNacks(uint64_t nOutNacks)
{
  m_wire.reset();
  m_nOutNacks = nOutNacks;
  return *this;
}

FaceStatus&
FaceStatus::setNInBytes(uint64_t nInBytes)
{
  m_wire.reset();
  m_nInBytes = nInBytes;
  return *this;
}

FaceStatus&
FaceStatus::setNOutBytes(uint64_t nOutBytes)
{
  m_wire.reset();
  m_nOutBytes = nOutBytes;
  return *this;
}

bool
operator==(const FaceStatus& a, const FaceStatus& b)
{
  return a.getFaceId() == b.getFaceId() &&
      a.getRemoteUri() == b.getRemoteUri() &&
      a.getLocalUri() == b.getLocalUri() &&
      a.getFaceScope() == b.getFaceScope() &&
      a.getFacePersistency() == b.getFacePersistency() &&
      a.getLinkType() == b.getLinkType() &&
      a.getFlags() == b.getFlags() &&
      a.hasExpirationPeriod() == b.hasExpirationPeriod() &&
      (!a.hasExpirationPeriod() || a.getExpirationPeriod() == b.getExpirationPeriod()) &&
      a.hasBaseCongestionMarkingInterval() == b.hasBaseCongestionMarkingInterval() &&
      (!a.hasBaseCongestionMarkingInterval() ||
       a.getBaseCongestionMarkingInterval() == b.getBaseCongestionMarkingInterval()) &&
      a.hasDefaultCongestionThreshold() == b.hasDefaultCongestionThreshold() &&
      (!a.hasDefaultCongestionThreshold() ||
       a.getDefaultCongestionThreshold() == b.getDefaultCongestionThreshold()) &&
      a.getNInInterests() == b.getNInInterests() &&
      a.getNInData() == b.getNInData() &&
      a.getNInNacks() == b.getNInNacks() &&
      a.getNOutInterests() == b.getNOutInterests() &&
      a.getNOutData() == b.getNOutData() &&
      a.getNOutNacks() == b.getNOutNacks() &&
      a.getNInBytes() == b.getNInBytes() &&
      a.getNOutBytes() == b.getNOutBytes();
}

std::ostream&
operator<<(std::ostream& os, const FaceStatus& status)
{
  os << "Face(FaceId: " << status.getFaceId() << ",\n"
     << "     RemoteUri: " << status.getRemoteUri() << ",\n"
     << "     LocalUri: " << status.getLocalUri() << ",\n";

  if (status.hasExpirationPeriod()) {
    os << "     ExpirationPeriod: " << status.getExpirationPeriod() << ",\n";
  }
  else {
    os << "     ExpirationPeriod: infinite,\n";
  }

  os << "     FaceScope: " << status.getFaceScope() << ",\n"
     << "     FacePersistency: " << status.getFacePersistency() << ",\n"
     << "     LinkType: " << status.getLinkType() << ",\n";

  if (status.hasBaseCongestionMarkingInterval()) {
    os << "     BaseCongestionMarkingInterval: " << status.getBaseCongestionMarkingInterval() << ",\n";
  }

  if (status.hasDefaultCongestionThreshold()) {
    os << "     DefaultCongestionThreshold: " << status.getDefaultCongestionThreshold() << " bytes,\n";
  }

  os << "     Flags: " << AsHex{status.getFlags()} << ",\n"
     << "     Counters: {Interests: {in: " << status.getNInInterests() << ", "
     << "out: " << status.getNOutInterests() << "},\n"
     << "                Data: {in: " << status.getNInData() << ", "
     << "out: " << status.getNOutData() << "},\n"
     << "                Nacks: {in: " << status.getNInNacks() << ", "
     << "out: " << status.getNOutNacks() << "},\n"
     << "                bytes: {in: " << status.getNInBytes() << ", "
     << "out: " << status.getNOutBytes() << "}}\n";

  return os << "     )";
}

} // namespace nfd
} // namespace ndn
