/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_FACE_STATUS_HPP
#define NDN_MANAGEMENT_NFD_FACE_STATUS_HPP

#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"

namespace ndn {
namespace nfd {

class FaceStatus
{
public:
  class Error : public Tlv::Error
  {
  public:
    Error(const std::string& what) : Tlv::Error(what) { }
  };

  FaceStatus(const uint64_t faceId,
             const std::string& uri,
             uint64_t inInterest, uint64_t inData, uint64_t outInterest, uint64_t outData);

  explicit
  FaceStatus(const Block& block);

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

  const uint64_t
  getInInterest() const
  {
    return m_inInterest;
  }

  const uint64_t
  getInData() const
  {
    return m_inData;
  }

  const uint64_t
  getOutInterest() const
  {
    return m_outInterest;
  }

  const uint64_t
  getOutData() const
  {
    return m_outData;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& buffer) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

private:
  uint64_t m_faceId;
  std::string m_uri;
  uint64_t m_inInterest;
  uint64_t m_inData;
  uint64_t m_outInterest;
  uint64_t m_outData;

  mutable Block m_wire;
};

inline
FaceStatus::FaceStatus(const uint64_t faceId,
             const std::string& uri,
             uint64_t inInterest, uint64_t inData, uint64_t outInterest, uint64_t outData)
  : m_faceId(faceId)
  , m_uri(uri)
  , m_inInterest(inInterest)
  , m_inData(inData)
  , m_outInterest(outInterest)
  , m_outData(outData)
{
}

inline
FaceStatus::FaceStatus(const Block& block)
{
  wireDecode(block);
}

template<bool T>
size_t
FaceStatus::wireEncode(EncodingImpl<T>& buffer) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::TotalOutgoingDataCounter,
                                                m_outData);

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::TotalOutgoingInterestCounter,
                                                m_outInterest);

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::TotalIncomingDataCounter,
                                                m_inData);

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::TotalIncomingInterestCounter,
                                                m_inInterest);

  totalLength += prependByteArrayBlock(buffer,
                                       tlv::nfd::Uri,
                                       reinterpret_cast<const uint8_t*>(m_uri.c_str()),
                                       m_uri.size());

  totalLength += prependNonNegativeIntegerBlock(buffer,
                                                tlv::nfd::FaceId,
                                                m_faceId);

  totalLength += buffer.prependVarNumber(totalLength);
  totalLength += buffer.prependVarNumber(tlv::nfd::FaceStatus);

  return totalLength;
}

const Block&
FaceStatus::wireEncode() const
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
FaceStatus::wireDecode(const Block &wire)
{
  m_wire = wire;

  if (m_wire.type() != tlv::nfd::FaceStatus)
    throw Error("Requested decoding of FaceStatus, but Block is of different type");

  m_wire.parse();

  // FaceKind
  Block::element_const_iterator val = m_wire.elements_begin();
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::FaceId)
    throw Error("Missing required FaceId block");
  m_faceId = readNonNegativeInteger(*val);

  // URI
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::Uri)
    throw Error("Missing required Uri block");
  m_uri = std::string(reinterpret_cast<const char*>(val->value()), val->value_size());

  // TotalIncomingInterestCounter
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::TotalIncomingInterestCounter)
    throw Error("Missing required FaceId block");
  m_inInterest = readNonNegativeInteger(*val);

  // TotalIncomingDataCounter
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::TotalIncomingDataCounter)
    throw Error("Missing required FaceId block");
  m_inData = readNonNegativeInteger(*val);

  // TotalOutgoingInterestCounter
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::TotalOutgoingInterestCounter)
    throw Error("Missing required FaceId block");
  m_outInterest = readNonNegativeInteger(*val);

  // TotalOutgoingDataCounter
  ++val;
  if (val == m_wire.elements_end() || val->type() != tlv::nfd::TotalOutgoingDataCounter)
    throw Error("Missing required FaceId block");
  m_outData = readNonNegativeInteger(*val);
}

inline std::ostream&
operator << (std::ostream& os, const FaceStatus& status)
{
  os << "FaceStatus(";

  // FaceID
  os << "FaceID: " << status.getFaceId() << ", ";

  // URI
  os << "Uri: " << status.getUri() << ", ";

  os << "Counters: " << status.getInInterest() << "|" << status.getInData()
     << "|" << status.getOutInterest() << "|" << status.getOutData();

  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_EVENT_STATUS_HPP
