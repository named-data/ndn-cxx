/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#include "ndn-cxx/signature-info.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/util/concepts.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<SignatureInfo>));
BOOST_CONCEPT_ASSERT((WireEncodable<SignatureInfo>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<SignatureInfo>));
BOOST_CONCEPT_ASSERT((WireDecodable<SignatureInfo>));
static_assert(std::is_base_of<tlv::Error, SignatureInfo::Error>::value,
              "SignatureInfo::Error must inherit from tlv::Error");

SignatureInfo::SignatureInfo() = default;

SignatureInfo::SignatureInfo(tlv::SignatureTypeValue type, optional<KeyLocator> keyLocator)
  : m_type(type)
  , m_keyLocator(std::move(keyLocator))
{
}

SignatureInfo::SignatureInfo(const Block& block)
{
  wireDecode(block);
}

template<encoding::Tag TAG>
size_t
SignatureInfo::wireEncode(EncodingImpl<TAG>& encoder) const
{
  if (m_type == -1) {
    NDN_THROW(Error("Cannot encode invalid SignatureInfo"));
  }

  // SignatureInfo = SIGNATURE-INFO-TYPE TLV-LENGTH
  //                   SignatureType
  //                   [KeyLocator]
  //                   [ValidityPeriod] ; if present, stored as first item of m_otherTlvs
  //                   *OtherSubelements

  size_t totalLength = 0;

  for (auto i = m_otherTlvs.rbegin(); i != m_otherTlvs.rend(); i++) {
    totalLength += encoder.prependBlock(*i);
  }

  if (m_keyLocator) {
    totalLength += m_keyLocator->wireEncode(encoder);
  }

  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::SignatureType,
                                                static_cast<uint64_t>(m_type));
  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::SignatureInfo);

  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(SignatureInfo);

const Block&
SignatureInfo::wireEncode() const
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
SignatureInfo::wireDecode(const Block& wire)
{
  m_type = -1;
  m_keyLocator = nullopt;
  m_otherTlvs.clear();

  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != tlv::SignatureInfo)
    NDN_THROW(Error("SignatureInfo", m_wire.type()));

  auto it = m_wire.elements_begin();

  // the first sub-element must be SignatureType
  if (it == m_wire.elements_end() || it->type() != tlv::SignatureType)
    NDN_THROW(Error("Missing SignatureType in SignatureInfo"));

  m_type = readNonNegativeIntegerAs<tlv::SignatureTypeValue>(*it);
  ++it;

  // the second sub-element could be KeyLocator
  if (it != m_wire.elements_end() && it->type() == tlv::KeyLocator) {
    m_keyLocator.emplace(*it);
    ++it;
  }

  // store SignatureType-specific sub-elements, if any
  while (it != m_wire.elements_end()) {
    m_otherTlvs.push_back(*it);
    ++it;
  }
}

SignatureInfo&
SignatureInfo::setSignatureType(tlv::SignatureTypeValue type)
{
  if (type != m_type) {
    m_type = type;
    m_wire.reset();
  }
  return *this;
}

const KeyLocator&
SignatureInfo::getKeyLocator() const
{
  if (!hasKeyLocator()) {
    NDN_THROW(Error("KeyLocator does not exist in SignatureInfo"));
  }
  return *m_keyLocator;
}

SignatureInfo&
SignatureInfo::setKeyLocator(optional<KeyLocator> keyLocator)
{
  if (keyLocator != m_keyLocator) {
    m_keyLocator = std::move(keyLocator);
    m_wire.reset();
  }
  return *this;
}

void
SignatureInfo::unsetKeyLocator()
{
  setKeyLocator(nullopt);
}

security::ValidityPeriod
SignatureInfo::getValidityPeriod() const
{
  if (!hasValidityPeriod()) {
    NDN_THROW(Error("ValidityPeriod does not exist in SignatureInfo"));
  }
  return security::ValidityPeriod(m_otherTlvs.front());
}

SignatureInfo&
SignatureInfo::setValidityPeriod(optional<security::ValidityPeriod> validityPeriod)
{
  if (validityPeriod) {
    auto block = validityPeriod->wireEncode();
    if (!hasValidityPeriod()) {
      m_otherTlvs.push_front(std::move(block));
      m_wire.reset();
    }
    else if (m_otherTlvs.front() != block) {
      m_otherTlvs.front() = std::move(block);
      m_wire.reset();
    }
  }
  else if (hasValidityPeriod()) {
    m_otherTlvs.pop_front();
    m_wire.reset();
  }
  return *this;
}

void
SignatureInfo::unsetValidityPeriod()
{
  setValidityPeriod(nullopt);
}

const Block&
SignatureInfo::getTypeSpecificTlv(uint32_t type) const
{
  for (const Block& block : m_otherTlvs) {
    if (block.type() == type)
      return block;
  }

  NDN_THROW(Error("TLV-TYPE " + to_string(type) + " sub-element does not exist in SignatureInfo"));
}

void
SignatureInfo::appendTypeSpecificTlv(const Block& block)
{
  m_otherTlvs.push_back(block);
  m_wire.reset();
}

bool
operator==(const SignatureInfo& lhs, const SignatureInfo& rhs)
{
  return lhs.m_type == rhs.m_type &&
         lhs.m_keyLocator == rhs.m_keyLocator &&
         lhs.m_otherTlvs == rhs.m_otherTlvs;
}

std::ostream&
operator<<(std::ostream& os, const SignatureInfo& info)
{
  if (info.getSignatureType() == -1)
    return os << "Invalid SignatureInfo";

  os << static_cast<tlv::SignatureTypeValue>(info.getSignatureType());
  if (info.hasKeyLocator()) {
    os << " " << info.getKeyLocator();
  }
  if (!info.m_otherTlvs.empty()) {
    os << " { ";
    for (const auto& block : info.m_otherTlvs) {
      os << block.type() << " ";
    }
    os << "}";
  }

  return os;
}

} // namespace ndn
