/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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
#include "ndn-cxx/encoding/block-helpers.hpp"
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
  , m_content(tlv::Content)
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
  // Data ::= DATA-TLV TLV-LENGTH
  //            Name
  //            MetaInfo?
  //            Content?
  //            SignatureInfo
  //            SignatureValue

  size_t totalLength = 0;

  // SignatureValue
  if (!wantUnsignedPortionOnly) {
    if (!m_signature) {
      NDN_THROW(Error("Requested wire format, but Data has not been signed"));
    }
    totalLength += encoder.prependBlock(m_signature.getValue());
  }

  // SignatureInfo
  totalLength += encoder.prependBlock(m_signature.getInfo());

  // Content
  totalLength += encoder.prependBlock(getContent());

  // MetaInfo
  totalLength += getMetaInfo().wireEncode(encoder);

  // Name
  totalLength += getName().wireEncode(encoder);

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
Data::wireEncode(EncodingBuffer& encoder, const Block& signatureValue) const
{
  size_t totalLength = encoder.size();
  totalLength += encoder.appendBlock(signatureValue);

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
  // Data ::= DATA-TLV TLV-LENGTH
  //            Name
  //            MetaInfo?
  //            Content?
  //            SignatureInfo
  //            SignatureValue

  m_wire = wire;
  m_wire.parse();

  auto element = m_wire.elements_begin();
  if (element == m_wire.elements_end() || element->type() != tlv::Name) {
    NDN_THROW(Error("Name element is missing or out of order"));
  }
  m_name.wireDecode(*element);
  int lastElement = 1; // last recognized element index, in spec order

  m_metaInfo = MetaInfo();
  m_content = Block(tlv::Content);
  m_signature = Signature();
  m_fullName.clear();

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
        m_signature.setInfo(*element);
        lastElement = 4;
        break;
      }
      case tlv::SignatureValue: {
        if (lastElement >= 5) {
          NDN_THROW(Error("SignatureValue element is out of order"));
        }
        m_signature.setValue(*element);
        lastElement = 5;
        break;
      }
      default: {
        if (tlv::isCriticalType(element->type())) {
          NDN_THROW(Error("unrecognized element of critical type " + to_string(element->type())));
        }
        break;
      }
    }
  }

  if (!m_signature) {
    NDN_THROW(Error("SignatureInfo element is missing"));
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
    m_fullName.appendImplicitSha256Digest(util::Sha256::computeDigest(m_wire.wire(), m_wire.size()));
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
  resetWire();
  m_name = name;
  return *this;
}

Data&
Data::setMetaInfo(const MetaInfo& metaInfo)
{
  resetWire();
  m_metaInfo = metaInfo;
  return *this;
}

const Block&
Data::getContent() const
{
  if (!m_content.hasWire()) {
    const_cast<Block&>(m_content).encode();
  }
  return m_content;
}

Data&
Data::setContent(const Block& block)
{
  resetWire();

  if (block.type() == tlv::Content) {
    m_content = block;
  }
  else {
    m_content = Block(tlv::Content, block);
  }

  return *this;
}

Data&
Data::setContent(const uint8_t* value, size_t valueSize)
{
  resetWire();
  m_content = makeBinaryBlock(tlv::Content, value, valueSize);
  return *this;
}

Data&
Data::setContent(ConstBufferPtr value)
{
  resetWire();
  m_content = Block(tlv::Content, std::move(value));
  return *this;
}

Data&
Data::setSignature(const Signature& signature)
{
  resetWire();
  m_signature = signature;
  return *this;
}

Data&
Data::setSignatureValue(const Block& value)
{
  resetWire();
  m_signature.setValue(value);
  return *this;
}

Data&
Data::setContentType(uint32_t type)
{
  resetWire();
  m_metaInfo.setType(type);
  return *this;
}

Data&
Data::setFreshnessPeriod(time::milliseconds freshnessPeriod)
{
  resetWire();
  m_metaInfo.setFreshnessPeriod(freshnessPeriod);
  return *this;
}

Data&
Data::setFinalBlock(optional<name::Component> finalBlockId)
{
  resetWire();
  m_metaInfo.setFinalBlock(std::move(finalBlockId));
  return *this;
}

bool
operator==(const Data& lhs, const Data& rhs)
{
  return lhs.getName() == rhs.getName() &&
         lhs.getMetaInfo().wireEncode() == rhs.getMetaInfo().wireEncode() &&
         lhs.getContent() == rhs.getContent() &&
         lhs.getSignature().getSignatureInfo() == rhs.getSignature().getSignatureInfo() &&
         lhs.getSignature().getValue() == rhs.getSignature().getValue();
}

std::ostream&
operator<<(std::ostream& os, const Data& data)
{
  os << "Name: " << data.getName() << "\n";
  os << "MetaInfo: " << data.getMetaInfo() << "\n";
  os << "Content: (size: " << data.getContent().value_size() << ")\n";
  os << "Signature: (type: " << data.getSignature().getType()
     << ", value_length: "<< data.getSignature().getValue().value_size() << ")";
  os << std::endl;

  return os;
}

} // namespace ndn
