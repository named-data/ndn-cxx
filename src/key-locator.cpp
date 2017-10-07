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

#include "key-locator.hpp"
#include "encoding/block-helpers.hpp"
#include "util/string-helper.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<KeyLocator>));
BOOST_CONCEPT_ASSERT((WireEncodable<KeyLocator>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<KeyLocator>));
BOOST_CONCEPT_ASSERT((WireDecodable<KeyLocator>));
static_assert(std::is_base_of<tlv::Error, KeyLocator::Error>::value,
              "KeyLocator::Error must inherit from tlv::Error");

KeyLocator::KeyLocator()
  : m_type(KeyLocator_None)
{
}

KeyLocator::KeyLocator(const Block& wire)
{
  wireDecode(wire);
}

KeyLocator::KeyLocator(const Name& name)
{
  setName(name);
}

template<encoding::Tag TAG>
size_t
KeyLocator::wireEncode(EncodingImpl<TAG>& encoder) const
{
  // KeyLocator ::= KEY-LOCATOR-TYPE TLV-LENGTH (Name | KeyDigest)
  // KeyDigest ::= KEY-DIGEST-TYPE TLV-LENGTH BYTE+

  size_t totalLength = 0;

  switch (m_type) {
  case KeyLocator_None:
    break;
  case KeyLocator_Name:
    totalLength += m_name.wireEncode(encoder);
    break;
  case KeyLocator_KeyDigest:
    totalLength += encoder.prependBlock(m_keyDigest);
    break;
  default:
    BOOST_THROW_EXCEPTION(Error("Unsupported KeyLocator type"));
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::KeyLocator);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(KeyLocator);

const Block&
KeyLocator::wireEncode() const
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
KeyLocator::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::KeyLocator)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV type during KeyLocator decoding"));

  m_wire = wire;
  m_wire.parse();

  if (m_wire.elements().empty()) {
    m_type = KeyLocator_None;
    return;
  }

  switch (m_wire.elements_begin()->type()) {
  case tlv::Name:
    m_type = KeyLocator_Name;
    m_name.wireDecode(*m_wire.elements_begin());
    break;
  case tlv::KeyDigest:
    m_type = KeyLocator_KeyDigest;
    m_keyDigest = *m_wire.elements_begin();
    break;
  default:
    m_type = KeyLocator_Unknown;
    break;
  }
}

KeyLocator&
KeyLocator::clear()
{
  m_wire.reset();
  m_type = KeyLocator_None;
  m_name.clear();
  m_keyDigest.reset();
  return *this;
}

const Name&
KeyLocator::getName() const
{
  if (m_type != KeyLocator_Name)
    BOOST_THROW_EXCEPTION(Error("KeyLocator type is not Name"));

  return m_name;
}

KeyLocator&
KeyLocator::setName(const Name& name)
{
  this->clear();
  m_type = KeyLocator_Name;
  m_name = name;
  return *this;
}

const Block&
KeyLocator::getKeyDigest() const
{
  if (m_type != KeyLocator_KeyDigest)
    BOOST_THROW_EXCEPTION(Error("KeyLocator type is not KeyDigest"));

  return m_keyDigest;
}

KeyLocator&
KeyLocator::setKeyDigest(const Block& keyDigest)
{
  if (keyDigest.type() != tlv::KeyDigest)
    BOOST_THROW_EXCEPTION(Error("expecting KeyDigest block"));

  this->clear();
  m_type = KeyLocator_KeyDigest;
  m_keyDigest = keyDigest;
  return *this;
}

KeyLocator&
KeyLocator::setKeyDigest(const ConstBufferPtr& keyDigest)
{
  // WARNING: ConstBufferPtr is shared_ptr<const Buffer>
  // This function takes a constant reference of a shared pointer.
  // It MUST NOT change the reference count of that shared pointer.

  return this->setKeyDigest(makeBinaryBlock(tlv::KeyDigest, keyDigest->data(), keyDigest->size()));
}

bool
KeyLocator::operator==(const KeyLocator& other) const
{
  return wireEncode() == other.wireEncode();
}

std::ostream&
operator<<(std::ostream& os, const KeyLocator& keyLocator)
{
  switch (keyLocator.getType()) {
    case KeyLocator::KeyLocator_Name: {
      return os << "Name=" << keyLocator.getName();
    }
    case KeyLocator::KeyLocator_KeyDigest: {
      const size_t MAX_DIGEST_OCTETS_TO_SHOW = 5;
      const Block& digest = keyLocator.getKeyDigest();
      os << "KeyDigest=" << toHex(digest.value(), digest.value_size()).substr(0, MAX_DIGEST_OCTETS_TO_SHOW * 2);
      if (digest.value_size() > MAX_DIGEST_OCTETS_TO_SHOW) {
        os << "...";
      }
      return os;
    }
    case KeyLocator::KeyLocator_None: {
      return os << "None";
    }
    case KeyLocator::KeyLocator_Unknown: {
      return os << "Unknown";
    }
  }
  return os << "Unknown";
}

} // namespace ndn
