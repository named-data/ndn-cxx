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
 *
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Zhenkai Zhu <http://irl.cs.ucla.edu/~zhenkai/>
 */

#include "ndn-cxx/name.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/encoding-buffer.hpp"
#include "ndn-cxx/util/time.hpp"

#include <sstream>
#include <boost/functional/hash.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/concepts.hpp>

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Name>));
BOOST_CONCEPT_ASSERT((WireEncodable<Name>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Name>));
BOOST_CONCEPT_ASSERT((WireDecodable<Name>));
BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<Name::iterator>));
BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<Name::const_iterator>));
BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<Name::reverse_iterator>));
BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<Name::const_reverse_iterator>));
BOOST_CONCEPT_ASSERT((boost::RandomAccessRangeConcept<Name>));
static_assert(std::is_base_of<tlv::Error, Name::Error>::value,
              "Name::Error must inherit from tlv::Error");

const size_t Name::npos = std::numeric_limits<size_t>::max();

// ---- constructors, encoding, decoding ----

Name::Name()
  : m_wire(tlv::Name)
{
}

Name::Name(const Block& wire)
  : m_wire(wire)
{
  m_wire.parse();
}

Name::Name(const char* uri)
  : Name(std::string(uri))
{
}

Name::Name(std::string uri)
{
  if (uri.empty())
    return;

  size_t iColon = uri.find(':');
  if (iColon != std::string::npos) {
    // Make sure the colon came before a '/'.
    size_t iFirstSlash = uri.find('/');
    if (iFirstSlash == std::string::npos || iColon < iFirstSlash) {
      // Omit the leading protocol such as ndn:
      uri.erase(0, iColon + 1);
    }
  }

  // Trim the leading slash and possibly the authority.
  if (uri[0] == '/') {
    if (uri.size() >= 2 && uri[1] == '/') {
      // Strip the authority following "//".
      size_t iAfterAuthority = uri.find('/', 2);
      if (iAfterAuthority == std::string::npos)
        // Unusual case: there was only an authority.
        return;
      else {
        uri.erase(0, iAfterAuthority + 1);
      }
    }
    else {
      uri.erase(0, 1);
    }
  }

  size_t iComponentStart = 0;

  // Unescape the components.
  while (iComponentStart < uri.size()) {
    size_t iComponentEnd = uri.find("/", iComponentStart);
    if (iComponentEnd == std::string::npos)
      iComponentEnd = uri.size();

    append(Component::fromEscapedString(&uri[0], iComponentStart, iComponentEnd));
    iComponentStart = iComponentEnd + 1;
  }
}

template<encoding::Tag TAG>
size_t
Name::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;
  for (const Component& comp : *this | boost::adaptors::reversed) {
    totalLength += comp.wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Name);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(Name);

const Block&
Name::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  m_wire.parse();

  return m_wire;
}

void
Name::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::Name)
    NDN_THROW(tlv::Error("Name", wire.type()));

  m_wire = wire;
  m_wire.parse();
}

Name
Name::deepCopy() const
{
  Name copiedName(*this);
  copiedName.m_wire.resetWire();
  copiedName.wireEncode(); // "compress" the underlying buffer
  return copiedName;
}

// ---- accessors ----

const name::Component&
Name::at(ssize_t i) const
{
  if (i < 0) {
    i += static_cast<ssize_t>(size());
  }

  if (i < 0 || static_cast<size_t>(i) >= size()) {
    NDN_THROW(Error("Requested component does not exist (out of bounds)"));
  }

  return reinterpret_cast<const Component&>(m_wire.elements()[i]);
}

PartialName
Name::getSubName(ssize_t iStartComponent, size_t nComponents) const
{
  PartialName result;

  if (iStartComponent < 0)
    iStartComponent += static_cast<ssize_t>(size());
  size_t iStart = iStartComponent < 0 ? 0 : static_cast<size_t>(iStartComponent);

  size_t iEnd = size();
  if (nComponents != npos)
    iEnd = std::min(size(), iStart + nComponents);

  for (size_t i = iStart; i < iEnd; ++i)
    result.append(at(i));

  return result;
}

// ---- modifiers ----

Name&
Name::set(ssize_t i, const Component& component)
{
  if (i < 0) {
    i += static_cast<ssize_t>(size());
  }

  const_cast<Block::element_container&>(m_wire.elements())[i] = component;
  m_wire.resetWire();
  return *this;
}

Name&
Name::set(ssize_t i, Component&& component)
{
  if (i < 0) {
    i += static_cast<ssize_t>(size());
  }

  const_cast<Block::element_container&>(m_wire.elements())[i] = std::move(component);
  m_wire.resetWire();
  return *this;
}

Name&
Name::appendVersion(optional<uint64_t> version)
{
  return append(Component::fromVersion(version.value_or(time::toUnixTimestamp(time::system_clock::now()).count())));
}

Name&
Name::appendTimestamp(optional<time::system_clock::TimePoint> timestamp)
{
  return append(Component::fromTimestamp(timestamp.value_or(time::system_clock::now())));
}

Name&
Name::append(const PartialName& name)
{
  if (&name == this)
    // Copying from this name, so need to make a copy first.
    return append(PartialName(name));

  for (size_t i = 0; i < name.size(); ++i)
    append(name.at(i));

  return *this;
}

static constexpr uint8_t SHA256_OF_EMPTY_STRING[] = {
  0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
  0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
  0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
  0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
};

Name&
Name::appendParametersSha256DigestPlaceholder()
{
  static const Component placeholder(tlv::ParametersSha256DigestComponent,
                                     SHA256_OF_EMPTY_STRING, sizeof(SHA256_OF_EMPTY_STRING));
  return append(placeholder);
}

void
Name::erase(ssize_t i)
{
  if (i < 0) {
    i += static_cast<ssize_t>(size());
  }

  m_wire.erase(m_wire.elements_begin() + i);
}

void
Name::clear()
{
  m_wire = Block(tlv::Name);
}

// ---- algorithms ----

Name
Name::getSuccessor() const
{
  if (empty()) {
    static const Name n("/sha256digest=0000000000000000000000000000000000000000000000000000000000000000");
    return n;
  }

  return getPrefix(-1).append(get(-1).getSuccessor());
}

bool
Name::isPrefixOf(const Name& other) const
{
  // This name is longer than the name we are checking against.
  if (size() > other.size())
    return false;

  // Check if at least one of given components doesn't match.
  for (size_t i = 0; i < size(); ++i) {
    if (get(i) != other.get(i))
      return false;
  }

  return true;
}

bool
Name::equals(const Name& other) const
{
  if (size() != other.size())
    return false;

  for (size_t i = 0; i < size(); ++i) {
    if (get(i) != other.get(i))
      return false;
  }

  return true;
}

int
Name::compare(size_t pos1, size_t count1, const Name& other, size_t pos2, size_t count2) const
{
  count1 = std::min(count1, this->size() - pos1);
  count2 = std::min(count2, other.size() - pos2);
  size_t count = std::min(count1, count2);

  for (size_t i = 0; i < count; ++i) {
    int comp = get(pos1 + i).compare(other.get(pos2 + i));
    if (comp != 0) { // i-th component differs
      return comp;
    }
  }
  // [pos1, pos1+count) of this Name equals [pos2, pos2+count) of other Name
  return count1 - count2;
}

// ---- URI representation ----

void
Name::toUri(std::ostream& os, name::UriFormat format) const
{
  if (empty()) {
    os << "/";
    return;
  }

  for (const auto& component : *this) {
    os << "/";
    component.toUri(os, format);
  }
}

std::string
Name::toUri(name::UriFormat format) const
{
  std::ostringstream os;
  toUri(os, format);
  return os.str();
}

std::istream&
operator>>(std::istream& is, Name& name)
{
  std::string inputString;
  is >> inputString;
  name = Name(inputString);

  return is;
}

} // namespace ndn

namespace std {

size_t
hash<ndn::Name>::operator()(const ndn::Name& name) const
{
  return boost::hash_range(name.wireEncode().wire(),
                           name.wireEncode().wire() + name.wireEncode().size());
}

} // namespace std
