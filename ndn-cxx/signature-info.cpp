/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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
#include "ndn-cxx/util/string-helper.hpp"

#include <boost/range/adaptor/reversed.hpp>

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

SignatureInfo::SignatureInfo(const Block& block, SignatureInfo::Type type)
{
  wireDecode(block, type);
}

template<encoding::Tag TAG>
size_t
SignatureInfo::wireEncode(EncodingImpl<TAG>& encoder, SignatureInfo::Type type) const
{
  if (m_type == -1) {
    NDN_THROW(Error("Cannot encode invalid SignatureInfo"));
  }

  // SignatureInfo = SIGNATURE-INFO-TYPE TLV-LENGTH
  //                   SignatureType
  //                   [KeyLocator]
  //                   [ValidityPeriod]
  //                   *OtherSubelements

  // InterestSignatureInfo = INTEREST-SIGNATURE-INFO-TYPE TLV-LENGTH
  //                           SignatureType
  //                           [KeyLocator]
  //                           [SignatureNonce]
  //                           [SignatureTime]
  //                           [SignatureSeqNum]
  //                           *OtherSubelements

  size_t totalLength = 0;

  // m_otherTlvs contains (if set) SignatureNonce, SignatureTime, SignatureSeqNum, ValidityPeriod,
  // and AdditionalDescription, as well as any custom elements added by the user
  for (const auto& block : m_otherTlvs | boost::adaptors::reversed) {
    totalLength += prependBlock(encoder, block);
  }

  if (m_keyLocator) {
    totalLength += m_keyLocator->wireEncode(encoder);
  }

  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::SignatureType,
                                                static_cast<uint64_t>(m_type));

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(to_underlying(type));

  return totalLength;
}

template size_t
SignatureInfo::wireEncode<encoding::EncoderTag>(EncodingBuffer&, SignatureInfo::Type) const;

template size_t
SignatureInfo::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, SignatureInfo::Type) const;

const Block&
SignatureInfo::wireEncode(SignatureInfo::Type type) const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator, type);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer, type);

  m_wire = buffer.block();
  return m_wire;
}

void
SignatureInfo::wireDecode(const Block& wire, SignatureInfo::Type type)
{
  m_type = -1;
  m_keyLocator = nullopt;
  m_otherTlvs.clear();

  m_wire = wire;
  m_wire.parse();

  if (m_wire.type() != to_underlying(type)) {
    NDN_THROW(Error("SignatureInfo", m_wire.type()));
  }

  size_t lastCriticalElement = 0;
  for (const auto& element : m_wire.elements()) {
    switch (element.type()) {
      case tlv::SignatureType: {
        if (lastCriticalElement > 0) {
          NDN_THROW(Error("SignatureType element is repeated or out-of-order"));
        }
        m_type = readNonNegativeIntegerAs<tlv::SignatureTypeValue>(element);
        lastCriticalElement = 1;
        break;
      }
      case tlv::KeyLocator: {
        if (lastCriticalElement > 1) {
          NDN_THROW(Error("KeyLocator element is repeated or out-of-order"));
        }
        m_keyLocator.emplace(element);
        lastCriticalElement = 2;
        break;
      }
      case tlv::SignatureNonce: {
        // Must handle SignatureNonce specifically because we must check that its length is >0
        if (element.value_size() < 1) {
          NDN_THROW(Error("SignatureNonce element cannot be empty"));
        }
        m_otherTlvs.push_back(element);
        break;
      }
      case tlv::ValidityPeriod:
        // ValidityPeriod is treated differently than other "extension" TLVs for historical reasons:
        // It is intended to be non-critical, but its TLV-TYPE is in the critical range. Therefore,
        // we must handle it specifically.
        m_otherTlvs.push_back(element);
        break;
      default: {
        // If the TLV-TYPE is unrecognized and critical, abort decoding
        if (tlv::isCriticalType(element.type())) {
          NDN_THROW(Error("Unrecognized element of critical type " + to_string(element.type())));
        }
        // Otherwise, store in m_otherTlvs
        m_otherTlvs.push_back(element);
      }
    }
  }

  if (m_type == -1) {
    NDN_THROW(Error("Missing SignatureType in SignatureInfo"));
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

security::ValidityPeriod
SignatureInfo::getValidityPeriod() const
{
  auto it = findOtherTlv(tlv::ValidityPeriod);
  if (it == m_otherTlvs.end()) {
    NDN_THROW(Error("ValidityPeriod does not exist in SignatureInfo"));
  }
  return security::ValidityPeriod(*it);
}

SignatureInfo&
SignatureInfo::setValidityPeriod(optional<security::ValidityPeriod> validityPeriod)
{
  if (!validityPeriod) {
    removeCustomTlv(tlv::ValidityPeriod);
  }
  else {
    addCustomTlv(validityPeriod->wireEncode());
  }
  return *this;
}

optional<std::vector<uint8_t>>
SignatureInfo::getNonce() const
{
  auto it = findOtherTlv(tlv::SignatureNonce);
  if (it == m_otherTlvs.end()) {
    return nullopt;
  }
  return std::vector<uint8_t>(it->value_begin(), it->value_end());
}

SignatureInfo&
SignatureInfo::setNonce(optional<span<const uint8_t>> nonce)
{
  if (!nonce) {
    removeCustomTlv(tlv::SignatureNonce);
  }
  else {
    addCustomTlv(makeBinaryBlock(tlv::SignatureNonce, *nonce));
  }
  return *this;
}

optional<time::system_clock::time_point>
SignatureInfo::getTime() const
{
  auto it = findOtherTlv(tlv::SignatureTime);
  if (it == m_otherTlvs.end()) {
    return nullopt;
  }
  return time::fromUnixTimestamp(time::milliseconds(readNonNegativeInteger(*it)));
}

SignatureInfo&
SignatureInfo::setTime(optional<time::system_clock::time_point> time)
{
  if (!time) {
    removeCustomTlv(tlv::SignatureTime);
  }
  else {
    addCustomTlv(makeNonNegativeIntegerBlock(tlv::SignatureTime,
                                             static_cast<uint64_t>(time::toUnixTimestamp(*time).count())));
  }
  return *this;
}

optional<uint64_t>
SignatureInfo::getSeqNum() const
{
  auto it = findOtherTlv(tlv::SignatureSeqNum);
  if (it == m_otherTlvs.end()) {
    return nullopt;
  }
  return readNonNegativeInteger(*it);
}

SignatureInfo&
SignatureInfo::setSeqNum(optional<uint64_t> seqNum)
{
  if (!seqNum) {
    removeCustomTlv(tlv::SignatureSeqNum);
  }
  else {
    addCustomTlv(makeNonNegativeIntegerBlock(tlv::SignatureSeqNum, *seqNum));
  }
  return *this;
}

optional<Block>
SignatureInfo::getCustomTlv(uint32_t type) const
{
  auto it = findOtherTlv(type);
  if (it == m_otherTlvs.end()) {
    return nullopt;
  }
  return *it;
}

void
SignatureInfo::addCustomTlv(Block block)
{
  auto existingIt = std::find_if(m_otherTlvs.begin(), m_otherTlvs.end(), [&block] (const Block& b) {
    return b.type() == block.type();
  });
  if (existingIt == m_otherTlvs.end()) {
    m_otherTlvs.push_back(std::move(block));
    m_wire.reset();
  }
  else if (*existingIt != block) {
    *existingIt = std::move(block);
    m_wire.reset();
  }
}

void
SignatureInfo::removeCustomTlv(uint32_t type)
{
  auto it = std::remove_if(m_otherTlvs.begin(), m_otherTlvs.end(), [type] (const Block& block) {
    return block.type() == type;
  });

  if (it != m_otherTlvs.end()) {
    m_otherTlvs.erase(it, m_otherTlvs.end());
    m_wire.reset();
  }
}

std::vector<Block>::const_iterator
SignatureInfo::findOtherTlv(uint32_t type) const
{
  return std::find_if(m_otherTlvs.begin(), m_otherTlvs.end(), [type] (const Block& block) {
    return block.type() == type;
  });
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
  if (info.getSignatureType() == -1) {
    return os << "Invalid SignatureInfo";
  }

  os << static_cast<tlv::SignatureTypeValue>(info.getSignatureType());
  if (info.hasKeyLocator()) {
    os << " " << info.getKeyLocator();
  }
  if (!info.m_otherTlvs.empty()) {
    os << " { ";
    for (const auto& block : info.m_otherTlvs) {
      switch (block.type()) {
        case tlv::SignatureNonce: {
          os << "Nonce=";
          auto nonce = *info.getNonce();
          printHex(os, nonce, false);
          os << " ";
          break;
        }
        case tlv::SignatureTime:
          os << "Time=" << time::toUnixTimestamp(*info.getTime()).count() << " ";
          break;
        case tlv::SignatureSeqNum:
          os << "SeqNum=" << *info.getSeqNum() << " ";
          break;
        case tlv::ValidityPeriod:
          os << "ValidityPeriod=" << info.getValidityPeriod() << " ";
          break;
        default:
          os << block.type() << " ";
          break;
      }
    }
    os << "}";
  }

  return os;
}

} // namespace ndn
