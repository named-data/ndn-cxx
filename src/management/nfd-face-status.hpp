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

#ifndef NDN_MANAGEMENT_NFD_FACE_STATUS_HPP
#define NDN_MANAGEMENT_NFD_FACE_STATUS_HPP

// This include must be kept as the first one, to ensure nfd-face-flags.hpp compiles on its own.
#include "nfd-face-flags.hpp"

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents Face status
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceStatus : public FaceFlagsTraits<FaceStatus>
{
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  FaceStatus();

  explicit
  FaceStatus(const Block& block)
  {
    this->wireDecode(block);
  }

  /** \brief prepend FaceStatus to the encoder
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  /** \brief encode FaceStatus
   */
  const Block&
  wireEncode() const;

  /** \brief decode FaceStatus
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  FaceStatus&
  setFaceId(uint64_t faceId)
  {
    m_wire.reset();
    m_faceId = faceId;
    return *this;
  }

  const std::string&
  getRemoteUri() const
  {
    return m_remoteUri;
  }

  FaceStatus&
  setRemoteUri(const std::string& remoteUri)
  {
    m_wire.reset();
    m_remoteUri = remoteUri;
    return *this;
  }

  const std::string&
  getLocalUri() const
  {
    return m_localUri;
  }

  FaceStatus&
  setLocalUri(const std::string& localUri)
  {
    m_wire.reset();
    m_localUri = localUri;
    return *this;
  }

  uint64_t
  getFlags() const
  {
    return m_flags;
  }

  FaceStatus&
  setFlags(uint64_t flags)
  {
    m_wire.reset();
    m_flags = flags;
    return *this;
  }

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  FaceStatus&
  setNInInterests(uint64_t nInInterests)
  {
    m_wire.reset();
    m_nInInterests = nInInterests;
    return *this;
  }

  uint64_t
  getNInDatas() const
  {
    return m_nInDatas;
  }

  FaceStatus&
  setNInDatas(uint64_t nInDatas)
  {
    m_wire.reset();
    m_nInDatas = nInDatas;
    return *this;
  }

  uint64_t
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  FaceStatus&
  setNOutInterests(uint64_t nOutInterests)
  {
    m_wire.reset();
    m_nOutInterests = nOutInterests;
    return *this;
  }

  uint64_t
  getNOutDatas() const
  {
    return m_nOutDatas;
  }

  FaceStatus&
  setNOutDatas(uint64_t nOutDatas)
  {
    m_wire.reset();
    m_nOutDatas = nOutDatas;
    return *this;
  }

private:
  uint64_t m_faceId;
  std::string m_remoteUri;
  std::string m_localUri;
  uint64_t m_flags;
  uint64_t m_nInInterests;
  uint64_t m_nInDatas;
  uint64_t m_nOutInterests;
  uint64_t m_nOutDatas;

  mutable Block m_wire;
};

inline
FaceStatus::FaceStatus()
  : m_faceId(0)
  , m_flags(0)
  , m_nInInterests(0)
  , m_nInDatas(0)
  , m_nOutInterests(0)
  , m_nOutDatas(0)
{
}

template<bool T>
inline size_t
FaceStatus::wireEncode(EncodingImpl<T>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NOutDatas, m_nOutDatas);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NOutInterests, m_nOutInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NInDatas, m_nInDatas);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::NInInterests, m_nInInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceFlags, m_flags);
  totalLength += prependByteArrayBlock(encoder, tlv::nfd::LocalUri,
                 reinterpret_cast<const uint8_t*>(m_localUri.c_str()), m_localUri.size());
  totalLength += prependByteArrayBlock(encoder, tlv::nfd::Uri,
                 reinterpret_cast<const uint8_t*>(m_remoteUri.c_str()), m_remoteUri.size());
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceId, m_faceId);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::FaceStatus);
  return totalLength;
}

inline const Block&
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

inline void
FaceStatus::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::FaceStatus) {
    throw Error("expecting FaceStatus block");
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceId) {
    m_faceId = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required FaceId field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::Uri) {
    m_remoteUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    ++val;
  }
  else {
    throw Error("missing required Uri field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    ++val;
  }
  else {
    throw Error("missing required LocalUri field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceFlags) {
    m_flags = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required FaceFlags field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInInterests) {
    m_nInInterests = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NInInterests field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInDatas) {
    m_nInDatas = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NInDatas field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutInterests) {
    m_nOutInterests = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NOutInterests field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutDatas) {
    m_nOutDatas = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NOutDatas field");
  }
}

inline std::ostream&
operator<<(std::ostream& os, const FaceStatus& status)
{
  os << "FaceStatus("
     << "FaceID: " << status.getFaceId() << ", "
     << "RemoteUri: " << status.getRemoteUri() << ", "
     << "LocalUri: " << status.getLocalUri() << ", "
     << "Flags: " << status.getFlags() << ", "
     << "Counters: " << status.getNInInterests() << "|" << status.getNInDatas()
     << "|" << status.getNOutInterests() << "|" << status.getNOutDatas()
     << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_STATUS_HPP
