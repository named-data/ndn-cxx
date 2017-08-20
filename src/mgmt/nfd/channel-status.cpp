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

#include "channel-status.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<ChannelStatus>));

ChannelStatus::ChannelStatus() = default;

ChannelStatus::ChannelStatus(const Block& payload)
{
  this->wireDecode(payload);
}

template<encoding::Tag TAG>
size_t
ChannelStatus::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;
  totalLength += prependStringBlock(encoder, tlv::nfd::LocalUri, m_localUri);
  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::ChannelStatus);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(ChannelStatus);

const Block&
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

void
ChannelStatus::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::ChannelStatus) {
    BOOST_THROW_EXCEPTION(Error("Expecting ChannelStatus block"));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::LocalUri) {
    m_localUri = readString(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("Missing required LocalUri field"));
  }
}

ChannelStatus&
ChannelStatus::setLocalUri(const std::string localUri)
{
  m_wire.reset();
  m_localUri = localUri;
  return *this;
}

bool
operator==(const ChannelStatus& a, const ChannelStatus& b)
{
  return a.getLocalUri() == b.getLocalUri();
}

std::ostream&
operator<<(std::ostream& os, const ChannelStatus& status)
{
  return os << "Channel(LocalUri: " << status.getLocalUri() << ")";
}

} // namespace nfd
} // namespace ndn
