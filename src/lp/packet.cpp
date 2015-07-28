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

#include "packet.hpp"
#include "detail/field-info.hpp"

#include <boost/range/adaptor/reversed.hpp>

namespace ndn {
namespace lp {

Packet::Packet()
  : m_wire(Block(tlv::LpPacket))
{
}

Packet::Packet(const Block& wire)
{
  wireDecode(wire);
}

template<encoding::Tag TAG>
size_t
Packet::wireEncode(EncodingImpl<TAG>& encoder) const
{
  if (m_wire.hasWire()) {
    return m_wire.size();
  }

  size_t length = 0;

  for (const Block& element : boost::adaptors::reverse(m_wire.elements())) {
    length += encoder.prependBlock(element);
  }

  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(tlv::LpPacket);

  return length;
}

template size_t
Packet::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Packet::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
Packet::wireEncode() const
{
  if (m_wire.hasWire()) {
    return m_wire;
  }

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
Packet::wireDecode(const Block& wire)
{
  if (wire.type() == ndn::tlv::Interest || wire.type() == ndn::tlv::Data) {
    m_wire = Block(tlv::LpPacket);
    add<FragmentField>(make_pair(wire.begin(), wire.end()));
    return;
  }

  wire.parse();

  bool isFirst = true;
  detail::FieldInfo prev;
  for (const Block& element : wire.elements()) {
    detail::FieldInfo info(element.type());

    if (!info.isRecognized && !info.canIgnore) {
      BOOST_THROW_EXCEPTION(Error("unknown field cannot be ignored"));
    }

    if (!isFirst) {
      if (info.tlvType == prev.tlvType && !info.isRepeatable) {
        BOOST_THROW_EXCEPTION(Error("non-repeatable field cannot be repeated"));
      }

      else if (info.tlvType != prev.tlvType && !detail::compareFieldSortOrder(prev, info)) {
        BOOST_THROW_EXCEPTION(Error("fields are not in correct sort order"));
      }
    }

    isFirst = false;
    prev = info;
  }

  m_wire = wire;
}

bool
Packet::comparePos(const Block& first, const uint64_t second)
{
  detail::FieldInfo firstInfo(first.type());
  detail::FieldInfo secondInfo(second);
  return detail::compareFieldSortOrder(firstInfo, secondInfo);
}

} // namespace lp
} // namespace ndn
