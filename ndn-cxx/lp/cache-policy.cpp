/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2025 Regents of the University of California.
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

#include "ndn-cxx/lp/cache-policy.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

namespace ndn::lp {

std::ostream&
operator<<(std::ostream& os, CachePolicyType policy)
{
  switch (policy) {
  case CachePolicyType::NO_CACHE:
    return os << "NoCache";
  default:
    return os << "None";
  }
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
    NDN_THROW(Error("CachePolicyType must be set"));
  }

  size_t length = 0;
  length += prependNonNegativeIntegerBlock(encoder, tlv::CachePolicyType, static_cast<uint64_t>(m_policy));
  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(tlv::CachePolicy);
  return length;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(CachePolicy);

const Block&
CachePolicy::wireEncode() const
{
  if (m_policy == CachePolicyType::NONE) {
    NDN_THROW(Error("CachePolicyType must be set"));
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
    NDN_THROW(Error("CachePolicy", wire.type()));
  }
  m_wire = wire;
  m_wire.parse();

  m_policy = CachePolicyType::NONE;

  auto it = m_wire.elements_begin();
  if (it != m_wire.elements_end() && it->type() == tlv::CachePolicyType) {
    m_policy = readNonNegativeIntegerAs<CachePolicyType>(*it);
    if (getPolicy() == CachePolicyType::NONE) {
      NDN_THROW(Error("Unknown CachePolicyType " + std::to_string(to_underlying(m_policy))));
    }
  }
  else {
    NDN_THROW(Error("CachePolicyType is missing or out of order"));
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

} // namespace ndn::lp
