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

#ifndef NDN_MANAGEMENT_NFD_FACE_EVENT_NOTIFICATION_HPP
#define NDN_MANAGEMENT_NFD_FACE_EVENT_NOTIFICATION_HPP

// This include must be kept as the first one, to ensure nfd-face-flags.hpp compiles on its own.
#include "nfd-face-flags.hpp"

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 */
enum FaceEventKind {
  FACE_EVENT_CREATED = 1,
  FACE_EVENT_DESTROYED = 2
};

/**
 * \ingroup management
 * \brief represents a Face status change notification
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Status-Change-Notification
 */
class FaceEventNotification : public FaceFlagsTraits<FaceEventNotification>
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

  FaceEventNotification();

  explicit
  FaceEventNotification(const Block& block)
  {
    this->wireDecode(block);
  }

  /** \brief prepend FaceEventNotification to the encoder
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  /** \brief encode FaceEventNotification
   */
  const Block&
  wireEncode() const;

  /** \brief decode FaceEventNotification
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  FaceEventKind
  getKind() const
  {
    return m_kind;
  }

  FaceEventNotification&
  setKind(FaceEventKind kind)
  {
    m_wire.reset();
    m_kind = kind;
    return *this;
  }

  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  FaceEventNotification&
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

  FaceEventNotification&
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

  FaceEventNotification&
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

  FaceEventNotification&
  setFlags(uint64_t flags)
  {
    m_wire.reset();
    m_flags = flags;
    return *this;
  }

private:
  FaceEventKind m_kind;
  uint64_t m_faceId;
  std::string m_remoteUri;
  std::string m_localUri;
  uint64_t m_flags;

  mutable Block m_wire;
};

inline
FaceEventNotification::FaceEventNotification()
  : m_kind(static_cast<FaceEventKind>(0))
  , m_faceId(0)
  , m_flags(0)
{
}

template<bool T>
inline size_t
FaceEventNotification::wireEncode(EncodingImpl<T>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceFlags, m_flags);
  totalLength += prependByteArrayBlock(encoder, tlv::nfd::LocalUri,
                 reinterpret_cast<const uint8_t*>(m_localUri.c_str()), m_localUri.size());
  totalLength += prependByteArrayBlock(encoder, tlv::nfd::Uri,
                 reinterpret_cast<const uint8_t*>(m_remoteUri.c_str()), m_remoteUri.size());
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceId, m_faceId);
  totalLength += prependNonNegativeIntegerBlock(encoder,
                 tlv::nfd::FaceEventKind, m_kind);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::FaceEventNotification);
  return totalLength;
}

inline const Block&
FaceEventNotification::wireEncode() const
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
FaceEventNotification::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::FaceEventNotification) {
    throw Error("expecting FaceEventNotification block");
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::FaceEventKind) {
    m_kind = static_cast<FaceEventKind>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required FaceEventKind field");
  }

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
}

inline std::ostream&
operator<<(std::ostream& os, const FaceEventNotification& notification)
{
  os << "FaceEventNotification(";

  switch (notification.getKind())
    {
    case FACE_EVENT_CREATED:
      os << "Kind: created, ";
      break;
    case FACE_EVENT_DESTROYED:
      os << "Kind: destroyed, ";
      break;
    }

  os << "FaceID: " << notification.getFaceId() << ", "
     << "RemoteUri: " << notification.getRemoteUri() << ", "
     << "LocalUri: " << notification.getLocalUri() << ", "
     << "Flags: " << notification.getFlags()
     << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_EVENT_NOTIFICATION_HPP
