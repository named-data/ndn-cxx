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

#include "nfd-face-status.hpp"
#include "encoding/tlv-nfd.hpp"
#include "encoding/block-helpers.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FaceStatus>));
BOOST_CONCEPT_ASSERT((WireEncodable<FaceStatus>));
BOOST_CONCEPT_ASSERT((WireDecodable<FaceStatus>));
static_assert(std::is_base_of<tlv::Error, FaceStatus::Error>::value,
              "FaceStatus::Error must inherit from tlv::Error");

FaceStatus::FaceStatus()
  : m_hasExpirationPeriod(false)
  , m_nInInterests(0)
  , m_nInDatas(0)
  , m_nOutInterests(0)
  , m_nOutDatas(0)
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

  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NOutBytes, m_nOutBytes);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NInBytes, m_nInBytes);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NOutDatas, m_nOutDatas);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NOutInterests, m_nOutInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NInDatas, m_nInDatas);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NInInterests, m_nInInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::LinkType, m_linkType);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FacePersistency, m_facePersistency);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceScope, m_faceScope);
  if (m_hasExpirationPeriod) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::ExpirationPeriod, m_expirationPeriod.count());
  }
  totalLength += encoder.prependByteArrayBlock(tlv::nfd::LocalUri,
                 reinterpret_cast<const uint8_t*>(m_localUri.c_str()), m_localUri.size());
  totalLength += encoder.prependByteArrayBlock(tlv::nfd::Uri,
                 reinterpret_cast<const uint8_t*>(m_remoteUri.c_str()), m_remoteUri.size());
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceId, m_faceId);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::FaceStatus);
  return totalLength;
}

template size_t
FaceStatus::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& block) const;

template size_t
FaceStatus::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& block) const;

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
    m_remoteUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required Uri field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required LocalUri field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::ExpirationPeriod) {
    m_expirationPeriod = time::milliseconds(readNonNegativeInteger(*val));
    m_hasExpirationPeriod = true;
    ++val;
  }
  else {
    m_hasExpirationPeriod = false;
    // ExpirationPeriod is optional
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceScope) {
    m_faceScope = static_cast<FaceScope>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required FaceScope field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FacePersistency) {
    m_facePersistency = static_cast<FacePersistency>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required FacePersistency field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LinkType) {
    m_linkType = static_cast<LinkType>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required LinkType field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInInterests) {
    m_nInInterests = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInDatas) {
    m_nInDatas = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInDatas field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutInterests) {
    m_nOutInterests = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutDatas) {
    m_nOutDatas = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutDatas field"));
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
}

FaceStatus&
FaceStatus::setExpirationPeriod(const time::milliseconds& expirationPeriod)
{
  m_wire.reset();
  m_expirationPeriod = expirationPeriod;
  m_hasExpirationPeriod = true;
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
FaceStatus::setNInDatas(uint64_t nInDatas)
{
  m_wire.reset();
  m_nInDatas = nInDatas;
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
FaceStatus::setNOutDatas(uint64_t nOutDatas)
{
  m_wire.reset();
  m_nOutDatas = nOutDatas;
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

void
FaceStatus::wireReset() const
{
  m_wire.reset();
}

std::ostream&
operator<<(std::ostream& os, const FaceStatus& status)
{
  os << "FaceStatus("
     << "FaceID: " << status.getFaceId() << ",\n"
     << "RemoteUri: " << status.getRemoteUri() << ",\n"
     << "LocalUri: " << status.getLocalUri() << ",\n";

  if (status.hasExpirationPeriod()) {
    os << "ExpirationPeriod: " << status.getExpirationPeriod() << ",\n";
  }
  else {
    os << "ExpirationPeriod: infinite,\n";
  }

  os << "FaceScope: " << status.getFaceScope() << ",\n"
     << "FacePersistency: " << status.getFacePersistency() << ",\n"
     << "LinkType: " << status.getLinkType() << ",\n"
     << "Counters: { Interests: {in: " << status.getNInInterests() << ", "
     << "out: " << status.getNOutInterests() << "},\n"
     << "            Data: {in: " << status.getNInDatas() << ", "
     << "out: " << status.getNOutDatas() << "},\n"
     << "            bytes: {in: " << status.getNInBytes() << ", "
     << "out: " << status.getNOutBytes() << "} }\n"
     << ")";
  return os;
}

} // namespace nfd
} // namespace ndn
