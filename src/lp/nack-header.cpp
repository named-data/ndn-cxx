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
 *
 * @author Eric Newberry <enewberry@email.arizona.edu>
 */

#include "nack-header.hpp"

namespace ndn {
namespace lp {

std::ostream&
operator<<(std::ostream& os, NackReason reason)
{
  switch (reason) {
  case NackReason::CONGESTION:
    os << "Congestion";
    break;
  case NackReason::DUPLICATE:
    os << "Duplicate";
    break;
  case NackReason::NO_ROUTE:
    os << "NoRoute";
    break;
  default:
    os << "None";
    break;
  }
  return os;
}

NackHeader::NackHeader()
  : m_reason(NackReason::NONE)
{
}

NackHeader::NackHeader(const Block& block)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
NackHeader::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t length = 0;
  length += prependNonNegativeIntegerBlock(encoder, tlv::NackReason,
                                           static_cast<uint32_t>(m_reason));
  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(tlv::Nack);
  return length;
}

template size_t
NackHeader::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
NackHeader::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
NackHeader::wireEncode() const
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
NackHeader::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::Nack) {
    BOOST_THROW_EXCEPTION(ndn::tlv::Error("expecting Nack block"));
  }

  m_wire = wire;
  m_wire.parse();
  m_reason = NackReason::NONE;

  if (m_wire.elements_size() > 0) {
    Block::element_const_iterator it = m_wire.elements_begin();

    if (it->type() == tlv::NackReason) {
      m_reason = static_cast<NackReason>(readNonNegativeInteger(*it));
    }
    else {
      BOOST_THROW_EXCEPTION(ndn::tlv::Error("expecting NackReason block"));
    }
  }
}

NackReason
NackHeader::getReason() const
{
  switch (m_reason) {
  case NackReason::CONGESTION:
  case NackReason::DUPLICATE:
  case NackReason::NO_ROUTE:
    return m_reason;
  default:
    return NackReason::NONE;
  }
}

NackHeader&
NackHeader::setReason(NackReason reason)
{
  m_reason = reason;
  m_wire.reset();
  return *this;
}

} // namespace lp
} // namespace ndn
