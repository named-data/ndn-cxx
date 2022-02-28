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

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/util/sha256.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Data>));
BOOST_CONCEPT_ASSERT((WireEncodable<Data>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Data>));
BOOST_CONCEPT_ASSERT((WireDecodable<Data>));
static_assert(std::is_base_of<tlv::Error, Data::Error>::value,
              "Data::Error must inherit from tlv::Error");

Data::Data(const Name& name)
  : m_name(name)
{
}

Data::Data(const Block& wire)
{
  wireDecode(wire);
}

template<encoding::Tag TAG>
size_t
Data::wireEncode(EncodingImpl<TAG>& encoder, bool wantUnsignedPortionOnly) const
{
  // Data = DATA-TYPE TLV-LENGTH
  //          Name
  //          [MetaInfo]
  //          [Content]
  //          SignatureInfo
  //          SignatureValue
  // (elements are encoded in reverse order)

  size_t totalLength = 0;

  // SignatureValue
  if (!wantUnsignedPortionOnly) {
    if (!m_signatureInfo) {
      NDN_THROW(Error("Requested wire format, but Data has not been signed"));
    }
    totalLength += prependBlock(encoder, m_signatureValue);
  }

  // SignatureInfo
  totalLength += m_signatureInfo.wireEncode(encoder, SignatureInfo::Type::Data);

  // Content
  if (hasContent()) {
    totalLength += prependBlock(encoder, m_content);
  }

  // MetaInfo
  totalLength += m_metaInfo.wireEncode(encoder);

  // Name
  totalLength += m_name.wireEncode(encoder);

  if (!wantUnsignedPortionOnly) {
    totalLength += encoder.prependVarNumber(totalLength);
    totalLength += encoder.prependVarNumber(tlv::Data);
  }
  return totalLength;
}

template size_t
Data::wireEncode<encoding::EncoderTag>(EncodingBuffer&, bool) const;

template size_t
Data::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, bool) const;

const Block&
Data::wireEncode(EncodingBuffer& encoder, span<const uint8_t> signature) const
{
  size_t totalLength = encoder.size();
  totalLength += encoder.appendVarNumber(tlv::SignatureValue);
  totalLength += encoder.appendVarNumber(signature.size());
  totalLength += encoder.appendBytes(signature);

  encoder.prependVarNumber(totalLength);
  encoder.prependVarNumber(tlv::Data);

  const_cast<Data*>(this)->wireDecode(encoder.block());
  return m_wire;
}

const Block&
Data::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Data*>(this)->wireDecode(buffer.block());
  return m_wire;
}

void
Data::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::Data) {
    NDN_THROW(Error("Data", wire.type()));
  }
  m_wire = wire;
  m_wire.parse();

  // Data = DATA-TYPE TLV-LENGTH
  //          Name
  //          [MetaInfo]
  //          [Content]
  //          SignatureInfo
  //          SignatureValue

  auto element = m_wire.elements_begin();
  if (element == m_wire.elements_end() || element->type() != tlv::Name) {
    NDN_THROW(Error("Name element is missing or out of order"));
  }
  m_name.wireDecode(*element);

  m_metaInfo = {};
  m_content = {};
  m_signatureInfo = {};
  m_signatureValue = {};
  m_fullName.clear();

  int lastElement = 1; // last recognized element index, in spec order
  for (++element; element != m_wire.elements_end(); ++element) {
    switch (element->type()) {
      case tlv::MetaInfo: {
        if (lastElement >= 2) {
          NDN_THROW(Error("MetaInfo element is out of order"));
        }
        m_metaInfo.wireDecode(*element);
        lastElement = 2;
        break;
      }
      case tlv::Content: {
        if (lastElement >= 3) {
          NDN_THROW(Error("Content element is out of order"));
        }
        m_content = *element;
        lastElement = 3;
        break;
      }
      case tlv::SignatureInfo: {
        if (lastElement >= 4) {
          NDN_THROW(Error("SignatureInfo element is out of order"));
        }
        m_signatureInfo.wireDecode(*element);
        lastElement = 4;
        break;
      }
      case tlv::SignatureValue: {
        if (lastElement >= 5) {
          NDN_THROW(Error("SignatureValue element is out of order"));
        }
        m_signatureValue = *element;
        lastElement = 5;
        break;
      }
      default: { // unrecognized element
        // if the TLV-TYPE is critical, abort decoding
        if (tlv::isCriticalType(element->type())) {
          NDN_THROW(Error("Unrecognized element of critical type " + to_string(element->type())));
        }
        // otherwise, ignore it
        break;
      }
    }
  }

  if (!m_signatureInfo) {
    NDN_THROW(Error("SignatureInfo element is missing"));
  }
  if (!m_signatureValue.isValid()) {
    NDN_THROW(Error("SignatureValue element is missing"));
  }
}

const Name&
Data::getFullName() const
{
  if (m_fullName.empty()) {
    if (!m_wire.hasWire()) {
      NDN_THROW(Error("Cannot compute full name because Data has no wire encoding (not signed)"));
    }
    m_fullName = m_name;
    m_fullName.appendImplicitSha256Digest(util::Sha256::computeDigest(m_wire));
  }

  return m_fullName;
}

void
Data::resetWire()
{
  m_wire.reset();
  m_fullName.clear();
}

Data&
Data::setName(const Name& name)
{
  if (name != m_name) {
    m_name = name;
    resetWire();
  }
  return *this;
}

Data&
Data::setMetaInfo(const MetaInfo& metaInfo)
{
  m_metaInfo = metaInfo;
  resetWire();
  return *this;
}

Data&
Data::setContent(const Block& block)
{
  if (!block.isValid()) {
    NDN_THROW(std::invalid_argument("Content block must be valid"));
  }

  if (block.type() == tlv::Content) {
    m_content = block;
  }
  else {
    m_content = Block(tlv::Content, block);
  }
  m_content.encode();
  resetWire();
  return *this;
}

Data&
Data::setContent(span<const uint8_t> value)
{
  m_content = makeBinaryBlock(tlv::Content, value);
  resetWire();
  return *this;
}

Data&
Data::setContent(const uint8_t* value, size_t length)
{
  if (value == nullptr && length != 0) {
    NDN_THROW(std::invalid_argument("Content buffer cannot be nullptr"));
  }

  return setContent(make_span(value, length));
}

Data&
Data::setContent(ConstBufferPtr value)
{
  if (value == nullptr) {
    NDN_THROW(std::invalid_argument("Content buffer cannot be nullptr"));
  }

  m_content = Block(tlv::Content, std::move(value));
  resetWire();
  return *this;
}

Data&
Data::unsetContent()
{
  m_content = {};
  resetWire();
  return *this;
}

Data&
Data::setSignatureInfo(const SignatureInfo& info)
{
  m_signatureInfo = info;
  resetWire();
  return *this;
}

Data&
Data::setSignatureValue(ConstBufferPtr value)
{
  if (value == nullptr) {
    NDN_THROW(std::invalid_argument("SignatureValue buffer cannot be nullptr"));
  }

  m_signatureValue = Block(tlv::SignatureValue, std::move(value));
  resetWire();
  return *this;
}

InputBuffers
Data::extractSignedRanges() const
{
  InputBuffers bufs;
  bufs.reserve(1); // One range containing data value up to, but not including, SignatureValue

  wireEncode();
  auto lastSignedIt = std::prev(m_wire.find(tlv::SignatureValue));
  // Note: we assume that both iterators point to the same underlying buffer
  bufs.emplace_back(m_wire.value_begin(), lastSignedIt->end());

  return bufs;
}

Data&
Data::setContentType(uint32_t type)
{
  if (type != m_metaInfo.getType()) {
    m_metaInfo.setType(type);
    resetWire();
  }
  return *this;
}

Data&
Data::setFreshnessPeriod(time::milliseconds freshnessPeriod)
{
  if (freshnessPeriod != m_metaInfo.getFreshnessPeriod()) {
    m_metaInfo.setFreshnessPeriod(freshnessPeriod);
    resetWire();
  }
  return *this;
}

Data&
Data::setFinalBlock(optional<name::Component> finalBlockId)
{
  if (finalBlockId != m_metaInfo.getFinalBlock()) {
    m_metaInfo.setFinalBlock(std::move(finalBlockId));
    resetWire();
  }
  return *this;
}

bool
operator==(const Data& lhs, const Data& rhs)
{
  return lhs.getName() == rhs.getName() &&
         lhs.getMetaInfo().wireEncode() == rhs.getMetaInfo().wireEncode() &&
         lhs.getContent() == rhs.getContent() &&
         lhs.getSignatureInfo() == rhs.getSignatureInfo() &&
         lhs.getSignatureValue() == rhs.getSignatureValue();
}

std::ostream&
operator<<(std::ostream& os, const Data& data)
{
  os << "Name: " << data.getName() << "\n"
     << "MetaInfo: [" << data.getMetaInfo() << "]\n";

  if (data.hasContent()) {
    os << "Content: [" << data.getContent().value_size() << " bytes]\n";
  }

  os << "Signature: [type: " << static_cast<tlv::SignatureTypeValue>(data.getSignatureType())
     << ", length: "<< data.getSignatureValue().value_size() << "]\n";

  return os;
}

} // namespace ndn
