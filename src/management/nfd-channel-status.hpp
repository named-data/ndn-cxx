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

#ifndef NDN_MANAGEMENT_NFD_CHANNEL_STATUS_HPP
#define NDN_MANAGEMENT_NFD_CHANNEL_STATUS_HPP

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"

#include "../util/time.hpp"

namespace ndn {
namespace nfd {

/**
 * @ingroup management
 * @brief represents NFD Channel Status dataset
 * @sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Channel-Dataset
 */
class ChannelStatus
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

  ChannelStatus()
  {
  }

  explicit
  ChannelStatus(const Block& payload)
  {
    this->wireDecode(payload);
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

public: // getters & setters
  const std::string&
  getLocalUri() const
  {
    return m_localUri;
  }

  ChannelStatus&
  setLocalUri(const std::string localUri)
  {
    m_wire.reset();
    m_localUri = localUri;
    return *this;
  }

private:
  std::string m_localUri;

  mutable Block m_wire;
};


template<bool T>
inline size_t
ChannelStatus::wireEncode(EncodingImpl<T>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependByteArrayBlock(encoder, tlv::nfd::LocalUri,
                 reinterpret_cast<const uint8_t*>(m_localUri.c_str()), m_localUri.size());

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::ChannelStatus);
  return totalLength;
}

inline const Block&
ChannelStatus::wireEncode() const
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
ChannelStatus::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::ChannelStatus) {
    throw Error("expecting ChannelStatus block");
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    ++val;
  }
  else {
    throw Error("missing required LocalUri field");
  }
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CHANNEL_STATUS_HPP
