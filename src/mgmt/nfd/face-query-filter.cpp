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

#include "face-query-filter.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FaceQueryFilter>));
BOOST_CONCEPT_ASSERT((WireEncodable<FaceQueryFilter>));
BOOST_CONCEPT_ASSERT((WireDecodable<FaceQueryFilter>));
static_assert(std::is_base_of<tlv::Error, FaceQueryFilter::Error>::value,
              "FaceQueryFilter::Error must inherit from tlv::Error");

FaceQueryFilter::FaceQueryFilter() = default;

FaceQueryFilter::FaceQueryFilter(const Block& block)
{
  this->wireDecode(block);
}

template<encoding::Tag TAG>
size_t
FaceQueryFilter::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  if (m_linkType) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::LinkType, *m_linkType);
  }

  if (m_facePersistency) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::FacePersistency, *m_facePersistency);
  }

  if (m_faceScope) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::FaceScope, *m_faceScope);
  }

  if (hasLocalUri()) {
    totalLength += prependStringBlock(encoder, tlv::nfd::LocalUri, m_localUri);
  }

  if (hasRemoteUri()) {
    totalLength += prependStringBlock(encoder, tlv::nfd::Uri, m_remoteUri);
  }

  if (hasUriScheme()) {
    totalLength += prependStringBlock(encoder, tlv::nfd::UriScheme, m_uriScheme);
  }

  if (m_faceId) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::FaceId, *m_faceId);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::FaceQueryFilter);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(FaceQueryFilter);

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
  // all fields are optional
  if (block.type() != tlv::nfd::FaceQueryFilter) {
    BOOST_THROW_EXCEPTION(Error("expecting FaceQueryFilter block"));
  }

  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceId) {
    m_faceId = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    m_faceId = nullopt;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::UriScheme) {
    m_uriScheme = readString(*val);
    ++val;
  }
  else {
    m_uriScheme.clear();
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Uri) {
    m_remoteUri = readString(*val);
    ++val;
  }
  else {
    m_remoteUri.clear();
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri = readString(*val);
    ++val;
  }
  else {
    m_localUri.clear();
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceScope) {
    m_faceScope = readNonNegativeIntegerAs<FaceScope>(*val);
    ++val;
  }
  else {
    m_faceScope = nullopt;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FacePersistency) {
    m_facePersistency = readNonNegativeIntegerAs<FacePersistency>(*val);
    ++val;
  }
  else {
    m_facePersistency = nullopt;
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LinkType) {
    m_linkType = readNonNegativeIntegerAs<LinkType>(*val);
    ++val;
  }
  else {
    m_linkType = nullopt;
  }
}

bool
FaceQueryFilter::empty() const
{
  return !this->hasFaceId() &&
         !this->hasUriScheme() &&
         !this->hasRemoteUri() &&
         !this->hasLocalUri() &&
         !this->hasFaceScope() &&
         !this->hasFacePersistency() &&
         !this->hasLinkType();
}

FaceQueryFilter&
FaceQueryFilter::setFaceId(uint64_t faceId)
{
  m_wire.reset();
  m_faceId = faceId;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetFaceId()
{
  m_wire.reset();
  m_faceId = nullopt;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setUriScheme(const std::string& uriScheme)
{
  m_wire.reset();
  m_uriScheme = uriScheme;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetUriScheme()
{
  return this->setUriScheme("");
}

FaceQueryFilter&
FaceQueryFilter::setRemoteUri(const std::string& remoteUri)
{
  m_wire.reset();
  m_remoteUri = remoteUri;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetRemoteUri()
{
  return this->setRemoteUri("");
}

FaceQueryFilter&
FaceQueryFilter::setLocalUri(const std::string& localUri)
{
  m_wire.reset();
  m_localUri = localUri;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetLocalUri()
{
  return this->setLocalUri("");
}

FaceQueryFilter&
FaceQueryFilter::setFaceScope(FaceScope faceScope)
{
  m_wire.reset();
  m_faceScope = faceScope;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetFaceScope()
{
  m_wire.reset();
  m_faceScope = nullopt;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setFacePersistency(FacePersistency facePersistency)
{
  m_wire.reset();
  m_facePersistency = facePersistency;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetFacePersistency()
{
  m_wire.reset();
  m_facePersistency = nullopt;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::setLinkType(LinkType linkType)
{
  m_wire.reset();
  m_linkType = linkType;
  return *this;
}

FaceQueryFilter&
FaceQueryFilter::unsetLinkType()
{
  m_wire.reset();
  m_linkType = nullopt;
  return *this;
}

bool
operator==(const FaceQueryFilter& a, const FaceQueryFilter& b)
{
  return a.hasFaceId() == b.hasFaceId() &&
         (!a.hasFaceId() || a.getFaceId() == b.getFaceId()) &&
         a.hasUriScheme() == b.hasUriScheme() &&
         (!a.hasUriScheme() || a.getUriScheme() == b.getUriScheme()) &&
         a.hasRemoteUri() == b.hasRemoteUri() &&
         (!a.hasRemoteUri() || a.getRemoteUri() == b.getRemoteUri()) &&
         a.hasLocalUri() == b.hasLocalUri() &&
         (!a.hasLocalUri() || a.getLocalUri() == b.getLocalUri()) &&
         a.hasFaceScope() == b.hasFaceScope() &&
         (!a.hasFaceScope() || a.getFaceScope() == b.getFaceScope()) &&
         a.hasFacePersistency() == b.hasFacePersistency() &&
         (!a.hasFacePersistency() || a.getFacePersistency() == b.getFacePersistency()) &&
         a.hasLinkType() == b.hasLinkType() &&
         (!a.hasLinkType() || a.getLinkType() == b.getLinkType());
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
