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

#include "ndn-cxx/key-locator.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"
#include "ndn-cxx/util/overload.hpp"
#include "ndn-cxx/util/string-helper.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<KeyLocator>));
BOOST_CONCEPT_ASSERT((WireEncodable<KeyLocator>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<KeyLocator>));
BOOST_CONCEPT_ASSERT((WireDecodable<KeyLocator>));
static_assert(std::is_base_of<tlv::Error, KeyLocator::Error>::value,
              "KeyLocator::Error must inherit from tlv::Error");

const size_t MAX_KEY_DIGEST_OCTETS_TO_SHOW = 5;

KeyLocator::KeyLocator() = default;

KeyLocator::KeyLocator(const Block& wire)
{
  wireDecode(wire);
}

KeyLocator::KeyLocator(const Name& name)
  : m_locator(name)
{
}

template<encoding::Tag TAG>
size_t
KeyLocator::wireEncode(EncodingImpl<TAG>& encoder) const
{
  // KeyLocator = KEY-LOCATOR-TYPE TLV-LENGTH (Name / KeyDigest)
  // KeyDigest = KEY-DIGEST-TYPE TLV-LENGTH *OCTET

  size_t totalLength = 0;

  auto visitor = overload(
    []  (monostate)           {}, // nothing to encode, TLV-VALUE is empty
    [&] (const Name& name)    { totalLength += name.wireEncode(encoder); },
    [&] (const Block& digest) { totalLength += encoder.prependBlock(digest); },
    []  (uint32_t type)       { NDN_THROW(Error("Unsupported KeyLocator type " + to_string(type))); });
  visit(visitor, m_locator);

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
    NDN_THROW(Error("KeyLocator", wire.type()));

  clear();
  m_wire = wire;
  m_wire.parse();

  auto element = m_wire.elements_begin();
  if (element == m_wire.elements().end()) {
    return;
  }

  switch (element->type()) {
  case tlv::Name:
    m_locator.emplace<Name>(*element);
    break;
  case tlv::KeyDigest:
    m_locator.emplace<Block>(*element);
    break;
  default:
    m_locator = element->type();
    break;
  }
}

uint32_t
KeyLocator::getType() const
{
  switch (m_locator.index()) {
  case 0:
    return tlv::Invalid;
  case 1:
    return tlv::Name;
  case 2:
    return tlv::KeyDigest;
  case 3:
    return get<uint32_t>(m_locator);
  default:
    NDN_CXX_UNREACHABLE;
  }
}

KeyLocator&
KeyLocator::clear()
{
  m_locator = monostate{};
  m_wire.reset();
  return *this;
}

const Name&
KeyLocator::getName() const
{
  try {
    return get<Name>(m_locator);
  }
  catch (const bad_variant_access&) {
    NDN_THROW(Error("KeyLocator does not contain a Name"));
  }
}

KeyLocator&
KeyLocator::setName(const Name& name)
{
  m_locator = name;
  m_wire.reset();
  return *this;
}

const Block&
KeyLocator::getKeyDigest() const
{
  try {
    return get<Block>(m_locator);
  }
  catch (const bad_variant_access&) {
    NDN_THROW(Error("KeyLocator does not contain a KeyDigest"));
  }
}

KeyLocator&
KeyLocator::setKeyDigest(const Block& keyDigest)
{
  if (keyDigest.type() != tlv::KeyDigest) {
    NDN_THROW(std::invalid_argument("Invalid KeyDigest block of type " + to_string(keyDigest.type())));
  }
  m_locator = keyDigest;
  m_wire.reset();
  return *this;
}

KeyLocator&
KeyLocator::setKeyDigest(const ConstBufferPtr& keyDigest)
{
  BOOST_ASSERT(keyDigest != nullptr);
  m_locator = makeBinaryBlock(tlv::KeyDigest, keyDigest->data(), keyDigest->size());
  m_wire.reset();
  return *this;
}

std::ostream&
operator<<(std::ostream& os, const KeyLocator& keyLocator)
{
  auto visitor = overload(
    [&] (monostate) {
      os << "None";
    },
    [&] (const Name& name) {
      os << "Name=" << name;
    },
    [&] (const Block& digest) {
      os << "KeyDigest=";
      printHex(os, digest.value(), std::min(digest.value_size(), MAX_KEY_DIGEST_OCTETS_TO_SHOW));
      if (digest.value_size() > MAX_KEY_DIGEST_OCTETS_TO_SHOW) {
        os << "...";
      }
    },
    [&] (uint32_t type) {
      os << "Unknown(" << type << ")";
    });
  visit(visitor, keyLocator.m_locator);
  return os;
}

} // namespace ndn
