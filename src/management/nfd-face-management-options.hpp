/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_FACE_MANAGEMENT_OPTIONS_HPP
#define NDN_MANAGEMENT_NFD_FACE_MANAGEMENT_OPTIONS_HPP

#include "../encoding/encoding-buffer.hpp"
#include "../encoding/tlv-nfd.hpp"

namespace ndn {
namespace nfd {

class FaceManagementOptions {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };

  FaceManagementOptions ()
    : m_faceId (INVALID_FACE_ID)
  {
  }

  FaceManagementOptions(const Block& block)
  {
    wireDecode(block);
  }
  
  uint64_t 
  getFaceId () const
  {
    return m_faceId;
  }
  
  FaceManagementOptions&
  setFaceId (uint64_t faceId)
  {
    m_faceId = faceId;
    wire_.reset ();
    return *this;
  }

  const std::string&
  getUri () const
  {
    return m_uri;
  }

  FaceManagementOptions&
  setUri (const std::string& uri)
  {
    m_uri = uri;
    wire_.reset ();
    return *this;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;
  
  const Block&
  wireEncode () const;
  
  void 
  wireDecode (const Block &wire);
  
private:
  uint64_t m_faceId;
  std::string m_uri;

  mutable Block wire_;
};


template<bool T>
inline size_t
FaceManagementOptions::wireEncode(EncodingImpl<T>& blk) const
{
  size_t total_len = 0;

  if (!m_uri.empty())
    {
      size_t var_len = blk.prependByteArray (reinterpret_cast<const uint8_t*>(m_uri.c_str()), m_uri.size());
      total_len += var_len;
      total_len += blk.prependVarNumber (var_len);
      total_len += blk.prependVarNumber (tlv::nfd::Uri);
    }

  if (m_faceId != INVALID_FACE_ID)
    {
      size_t var_len = blk.prependNonNegativeInteger (m_faceId);
      total_len += var_len;
      total_len += blk.prependVarNumber (var_len);
      total_len += blk.prependVarNumber (tlv::nfd::FaceId);
    }

  total_len += blk.prependVarNumber (total_len);
  total_len += blk.prependVarNumber (tlv::nfd::FaceManagementOptions);
  return total_len;
}

inline const Block&
FaceManagementOptions::wireEncode () const
{
  if (wire_.hasWire ())
    return wire_;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);
  
  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  wire_ = buffer.block();
  return wire_;
}

inline void 
FaceManagementOptions::wireDecode (const Block &wire)
{
  m_uri.clear ();
  m_faceId = INVALID_FACE_ID;

  wire_ = wire;

  if (wire_.type() != tlv::nfd::FaceManagementOptions)
    throw Error("Requested decoding of FaceManagementOptions, but Block is of different type");
  
  wire_.parse ();

  // FaceID
  Block::element_const_iterator val = wire_.find(tlv::nfd::FaceId);
  if (val != wire_.elements_end())
    {
      m_faceId = readNonNegativeInteger(*val);
    }

  // Uri
  val = wire_.find(tlv::nfd::Uri);
  if (val != wire_.elements_end())
    {
      m_uri.append(reinterpret_cast<const char*>(val->value()), val->value_size());
    }
}

inline std::ostream&
operator << (std::ostream &os, const FaceManagementOptions &option)
{
  os << "ForwardingEntry(";
  
  // FaceID
  os << "FaceID: " << option.getFaceId () << ", ";

  // Uri
  os << "Uri: " << option.getUri ();

  os << ")";
  return os;
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_MANAGEMENT_OPTIONS_HPP
