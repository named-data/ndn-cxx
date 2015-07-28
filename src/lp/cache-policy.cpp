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

#include "cache-policy.hpp"

namespace ndn {
namespace lp {

std::ostream&
operator<<(std::ostream& os, CachePolicyType policy)
{
  switch (policy) {
  case CachePolicyType::NO_CACHE:
    os << "NoCache";
    break;
  default:
    os << "None";
    break;
  }

  return os;
}

CachePolicy::CachePolicy()
  : m_policy(CachePolicyType::NONE)
{
}

CachePolicy::CachePolicy(const Block& block)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
CachePolicy::wireEncode(EncodingImpl<TAG>& encoder) const
{
  if (m_policy == CachePolicyType::NONE) {
    BOOST_THROW_EXCEPTION(Error("CachePolicyType must be set"));
  }
  size_t length = 0;
  length += prependNonNegativeIntegerBlock(encoder, tlv::CachePolicyType,
                                           static_cast<uint32_t>(m_policy));
  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(tlv::CachePolicy);
  return length;
}

template size_t
CachePolicy::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
CachePolicy::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
CachePolicy::wireEncode() const
{
  if (m_policy == CachePolicyType::NONE) {
    BOOST_THROW_EXCEPTION(Error("CachePolicyType must be set"));
  }

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
CachePolicy::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::CachePolicy) {
    BOOST_THROW_EXCEPTION(Error("expecting CachePolicy block"));
  }

  m_wire = wire;
  m_wire.parse();

  Block::element_const_iterator it = m_wire.elements_begin();
  if (it != m_wire.elements_end() && it->type() == tlv::CachePolicyType) {
    m_policy = static_cast<CachePolicyType>(readNonNegativeInteger(*it));
    if (this->getPolicy() == CachePolicyType::NONE) {
      BOOST_THROW_EXCEPTION(Error("unknown CachePolicyType"));
    }
  }
  else {
    BOOST_THROW_EXCEPTION(Error("expecting CachePolicyType block"));
  }
}

CachePolicyType
CachePolicy::getPolicy() const
{
  switch (m_policy) {
  case CachePolicyType::NO_CACHE:
    return m_policy;
  default:
    return CachePolicyType::NONE;
  }
}

CachePolicy&
CachePolicy::setPolicy(CachePolicyType policy)
{
  m_policy = policy;
  m_wire.reset();
  return *this;
}

} // namespace lp
} // namespace ndn
