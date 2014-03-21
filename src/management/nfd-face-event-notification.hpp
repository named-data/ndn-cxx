/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_FACE_EVENT_NOTIFICATION_HPP
#define NDN_MANAGEMENT_NFD_FACE_EVENT_NOTIFICATION_HPP

#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"

namespace ndn {
namespace nfd {

enum FaceEventKind
  {
    FACE_EVENT_CREATED = 1,
    FACE_EVENT_DESTROYED = 2
  };

enum FaceFlags
  {
    FACE_IS_LOCAL = 1,
    FACE_IS_ON_DEMAND = 2
    // FACE_? = 4
    // FACE_? = 8
  };

class FaceEventNotification
{
public:
  class Error : public Tlv::Error
  {
  public:
    Error(const std::string& what) : Tlv::Error(what) { }
  };

  FaceEventNotification(FaceEventKind eventKind,
                        uint64_t faceId,
                        const std::string& uri,
                        uint64_t flags);

  explicit
  FaceEventNotification(const Block& block);

  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  const std::string&
  getUri() const
  {
    return m_uri;
  }

  FaceEventKind
  getEventKind() const
  {
    return m_kind;
  }

  uint64_t
  getFlags() const
  {
    return m_flags;
  }

  bool
  isLocal() const
  {
    return m_flags & FACE_IS_LOCAL;
  }

  bool
  isOnDemand() const
  {
    return m_flags & FACE_IS_ON_DEMAND;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& buffer) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

private:
  FaceEventKind m_kind;
  uint64_t m_faceId;
  std::string m_uri;
  uint64_t m_flags;

  mutable Block m_wire;
};

inline
FaceEventNotification::FaceEventNotification(FaceEventKind eventKind,
                                             uint64_t faceId,
                                             const std::string& uri,
                                             uint64_t flags)
  : m_kind(eventKind)
  , m_faceId(faceId)
  , m_uri(uri)
  , m_flags(flags)
{
}

inline
FaceEventNotification::FaceEventNotification(const Block& block)
{
  wireDecode(block);
}

template<bool T>
size_t
FaceEventNotification::wireEncode(EncodingImpl<T>& buffer) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::FaceFlags,
                                                m_flags);

  totalLength += prependByteArrayBlock(buffer,
                                       tlv::nfd::Uri,
                                       reinterpret_cast<const uint8_t*>(m_uri.c_str()),
                                       m_uri.size());

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::FaceId,
                                                m_faceId);

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::FaceEventKind,
                                                static_cast<uint32_t>(m_kind));

  totalLength += buffer.prependVarNumber(totalLength);
  totalLength += buffer.prependVarNumber(tlv::nfd::FaceEventNotification);

  return totalLength;
}

inline const Block&
FaceEventNotification::wireEncode() const
{
  if (m_wire.hasWire ())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

inline void
FaceEventNotification::wireDecode (const Block &wire)
{
  m_wire = wire;

  if (m_wire.type() != tlv::nfd::FaceEventNotification)
    throw Error("Requested decoding of FaceEventNotification, but Block is of different type");

  m_wire.parse();

  // FaceKind
  Block::element_const_iterator val = m_wire.elements_begin();
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::FaceEventKind)
    throw Error("Missing required Uri block");
  m_kind = static_cast<FaceEventKind>(readNonNegativeInteger(*val));

  // FaceID
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::FaceId)
    throw Error("Missing required FaceId block");
  m_faceId = readNonNegativeInteger(*val);

  // URI
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::Uri)
    throw Error("Missing required Uri block");
  m_uri = std::string(reinterpret_cast<const char*>(val->value()), val->value_size());

  // FaceFlags
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::FaceFlags)
    throw Error("Missing required FaceFlags block");
  m_flags = readNonNegativeInteger(*val);
}

inline std::ostream&
operator << (std::ostream& os, const FaceEventNotification& event)
{
  os << "FaceEventNotification(";

  os << "Kind: ";
  switch (event.getEventKind())
    {
    case FACE_EVENT_CREATED:
      os << "created";
      break;
    case FACE_EVENT_DESTROYED:
      os << "destroyed";
      break;
    }
  os << ", ";

  // FaceID
  os << "FaceID: " << event.getFaceId() << ", ";

  // URI
  os << "Uri: " << event.getUri() << ", ";

  // Flags
  os << "Flags: " << event.getFlags();

  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_EVENT_NOTIFICATION_HPP
