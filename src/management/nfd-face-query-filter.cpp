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

#include "nfd-face-query-filter.hpp"
#include "encoding/tlv-nfd.hpp"
#include "encoding/block-helpers.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FaceQueryFilter>));
BOOST_CONCEPT_ASSERT((WireEncodable<FaceQueryFilter>));
BOOST_CONCEPT_ASSERT((WireDecodable<FaceQueryFilter>));
static_assert(std::is_base_of<tlv::Error, FaceQueryFilter::Error>::value,
              "FaceQueryFilter::Error must inherit from tlv::Error");

FaceQueryFilter::FaceQueryFilter()
  : m_hasFaceId(false)
  , m_hasUriScheme(false)
  , m_hasRemoteUri(false)
  , m_hasLocalUri(false)
  , m_hasFaceScope(false)
  , m_hasFacePersistency(false)
  , m_hasLinkType(false)
{
}

FaceQueryFilter::FaceQueryFilter(const Block& block)
{
  this->wireDecode(block);
}

template<encoding::Tag TAG>
size_t
FaceQueryFilter::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  if (m_hasLinkType) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::LinkType, m_linkType);
  }

  if (m_hasFacePersistency) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::FacePersistency, m_facePersistency);
  }

  if (m_hasFaceScope) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::FaceScope, m_faceScope);
  }

  if (m_hasLocalUri) {
    totalLength += encoder.prependByteArrayBlock(tlv::nfd::LocalUri,
                   reinterpret_cast<const uint8_t*>(m_localUri.c_str()), m_localUri.size());
  }

  if (m_hasRemoteUri) {
    totalLength += encoder.prependByteArrayBlock(tlv::nfd::Uri,
                   reinterpret_cast<const uint8_t*>(m_remoteUri.c_str()), m_remoteUri.size());
  }

  if (m_hasUriScheme) {
    totalLength += encoder.prependByteArrayBlock(tlv::nfd::UriScheme,
                   reinterpret_cast<const uint8_t*>(m_uriScheme.c_str()), m_uriScheme.size());
  }

  if (m_hasFaceId) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::FaceId, m_faceId);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::FaceQueryFilter);
  return totalLength;
}

template size_t
FaceQueryFilter::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>&) const;

template size_t
FaceQueryFilter::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>&) const;

const Block&
FaceQueryFilter::wireEncode() const
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
FaceQueryFilter::wireDecode(const Block& block)
{
  //all fields are optional
  if (block.type() != tlv::nfd::FaceQueryFilter) {
    BOOST_THROW_EXCEPTION(Error("expecting FaceQueryFilter block"));
  }

  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceId) {
    m_faceId = readNonNegativeInteger(*val);
    m_hasFaceId = true;
    ++val;
  }
  else {
    m_hasFaceId = false;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::UriScheme) {
    m_uriScheme.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    m_hasUriScheme = true;
    ++val;
  }
  else {
    m_hasUriScheme = false;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Uri) {
    m_remoteUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    m_hasRemoteUri = true;
    ++val;
  }
  else {
    m_hasRemoteUri = false;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    m_hasLocalUri = true;
    ++val;
  }
  else {
    m_hasLocalUri = false;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceScope) {
    m_faceScope = static_cast<FaceScope>(readNonNegativeInteger(*val));
    m_hasFaceScope = true;
    ++val;
  }
  else {
    m_hasFaceScope = false;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FacePersistency) {
    m_facePersistency = static_cast<FacePersistency>(readNonNegativeInteger(*val));
    m_hasFacePersistency = true;
    ++val;
  }
  else {
    m_hasFacePersistency = false;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LinkType) {
    m_linkType = static_cast<LinkType>(readNonNegativeInteger(*val));
    m_hasLinkType = true;
    ++val;
  }
  else {
    m_hasLinkType = false;
  }

}

FaceQueryFilter&
FaceQueryFilter::setFaceId(uint64_t faceId)
{
  m_wire.reset();
  m_faceId = faceId;
  m_hasFaceId = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetFaceId()
{
  m_wire.reset();
  m_hasFaceId = false;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setUriScheme(const std::string& uriScheme)
{
  m_wire.reset();
  m_uriScheme = uriScheme;
  m_hasUriScheme = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetUriScheme()
{
  m_wire.reset();
  m_hasUriScheme = false;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setRemoteUri(const std::string& remoteUri)
{
  m_wire.reset();
  m_remoteUri = remoteUri;
  m_hasRemoteUri = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetRemoteUri()
{
  m_wire.reset();
  m_hasRemoteUri = false;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setLocalUri(const std::string& localUri)
{
  m_wire.reset();
  m_localUri = localUri;
  m_hasLocalUri = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetLocalUri()
{
  m_wire.reset();
  m_hasLocalUri = false;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setFaceScope(FaceScope faceScope)
{
  m_wire.reset();
  m_faceScope = faceScope;
  m_hasFaceScope = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetFaceScope()
{
  m_wire.reset();
  m_hasFaceScope = false;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setFacePersistency(FacePersistency facePersistency)
{
  m_wire.reset();
  m_facePersistency = facePersistency;
  m_hasFacePersistency = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetFacePersistency()
{
  m_wire.reset();
  m_hasFacePersistency = false;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setLinkType(LinkType linkType)
{
  m_wire.reset();
  m_linkType = linkType;
  m_hasLinkType = true;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetLinkType()
{
  m_wire.reset();
  m_hasLinkType = false;
  return *this;
}

std::ostream&
operator<<(std::ostream& os, const FaceQueryFilter& filter)
{
  os << "FaceQueryFilter(";
  if (filter.hasFaceId()) {
    os << "FaceID: " << filter.getFaceId() << ",\n";
  }

  if (filter.hasUriScheme()) {
    os << "UriScheme: " << filter.getUriScheme() << ",\n";
  }

  if (filter.hasRemoteUri()) {
    os << "RemoteUri: " << filter.getRemoteUri() << ",\n";
  }

  if (filter.hasLocalUri()) {
    os << "LocalUri: " << filter.getLocalUri() << ",\n";
  }

  if (filter.hasFaceScope()) {
    os << "FaceScope: " << filter.getFaceScope() << ",\n";
  }

  if (filter.hasFacePersistency()) {
    os << "FacePersistency: " << filter.getFacePersistency() << ",\n";
  }

  if (filter.hasLinkType()) {
    os << "LinkType: " << filter.getLinkType() << ",\n";
  }
  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn
