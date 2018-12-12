/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "ndn-cxx/name-component.hpp"
#include "ndn-cxx/impl/name-component-types.hpp"

#include <cstdlib>
#include <cstring>
#include <sstream>

namespace ndn {
namespace name {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Component>));
BOOST_CONCEPT_ASSERT((WireEncodable<Component>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Component>));
BOOST_CONCEPT_ASSERT((WireDecodable<Component>));
static_assert(std::is_base_of<tlv::Error, Component::Error>::value,
              "name::Component::Error must inherit from tlv::Error");

void
Component::ensureValid() const
{
  if (type() < tlv::NameComponentMin || type() > tlv::NameComponentMax) {
    BOOST_THROW_EXCEPTION(Error("TLV-TYPE " + to_string(type()) + " is not a valid NameComponent"));
  }
  detail::getComponentTypeTable().get(type()).check(*this);
}

Component::Component(uint32_t type)
  : Block(type)
{
  ensureValid();
}

Component::Component(const Block& wire)
  : Block(wire)
{
  ensureValid();
}

Component::Component(uint32_t type, ConstBufferPtr buffer)
  : Block(type, std::move(buffer))
{
  ensureValid();
}

Component::Component(uint32_t type, const uint8_t* value, size_t valueLen)
  : Block(makeBinaryBlock(type, value, valueLen))
{
  ensureValid();
}

Component::Component(const char* str)
  : Block(makeBinaryBlock(tlv::GenericNameComponent, str, std::char_traits<char>::length(str)))
{
}

Component::Component(const std::string& str)
  : Block(makeStringBlock(tlv::GenericNameComponent, str))
{
}

static Component
parseUriEscapedValue(uint32_t type, const char* input, size_t len)
{
  std::ostringstream oss;
  unescape(oss, input, len);
  std::string value = oss.str();
  if (value.find_first_not_of('.') == std::string::npos) { // all periods
    if (value.size() < 3) {
      BOOST_THROW_EXCEPTION(Component::Error("Illegal URI (name component cannot be . or ..)"));
    }
    return Component(type, reinterpret_cast<const uint8_t*>(value.data()), value.size() - 3);
  }
  return Component(type, reinterpret_cast<const uint8_t*>(value.data()), value.size());
}

Component
Component::fromEscapedString(const std::string& input)
{
  size_t equalPos = input.find('=');
  if (equalPos == std::string::npos) {
    return parseUriEscapedValue(tlv::GenericNameComponent, input.data(), input.size());
  }

  long type = std::strtol(input.data(), nullptr, 10);
  if (type >= tlv::NameComponentMin && type <= tlv::NameComponentMax &&
      to_string(type).size() == equalPos) {
    size_t valuePos = equalPos + 1;
    return parseUriEscapedValue(static_cast<uint32_t>(type), input.data() + valuePos,
                                input.size() - valuePos);
  }

  auto typePrefix = input.substr(0, equalPos);
  auto ct = detail::getComponentTypeTable().findByUriPrefix(typePrefix);
  if (ct == nullptr) {
    BOOST_THROW_EXCEPTION(Error("Incorrect TLV-TYPE '" + typePrefix + "' in NameComponent URI"));
  }
  return ct->parseAltUriValue(input.substr(equalPos + 1));
}

void
Component::toUri(std::ostream& os) const
{
  detail::getComponentTypeTable().get(type()).writeUri(os, *this);
}

std::string
Component::toUri() const
{
  std::ostringstream os;
  toUri(os);
  return os.str();
}

////////////////////////////////////////////////////////////////////////////////

bool
Component::isNumber() const
{
  return (value_size() == 1 || value_size() == 2 ||
          value_size() == 4 || value_size() == 8);
}

bool
Component::isNumberWithMarker(uint8_t marker) const
{
  return (!empty() && value()[0] == marker &&
          (value_size() == 2 || value_size() == 3 ||
           value_size() == 5 || value_size() == 9));
}

bool
Component::isVersion() const
{
  return isNumberWithMarker(VERSION_MARKER);
}

bool
Component::isSegment() const
{
  return isNumberWithMarker(SEGMENT_MARKER);
}

bool
Component::isSegmentOffset() const
{
  return isNumberWithMarker(SEGMENT_OFFSET_MARKER);
}

bool
Component::isTimestamp() const
{
  return isNumberWithMarker(TIMESTAMP_MARKER);
}

bool
Component::isSequenceNumber() const
{
  return isNumberWithMarker(SEQUENCE_NUMBER_MARKER);
}

////////////////////////////////////////////////////////////////////////////////

uint64_t
Component::toNumber() const
{
  if (!isNumber())
    BOOST_THROW_EXCEPTION(Error("Name component does not have nonNegativeInteger value"));

  return readNonNegativeInteger(*this);
}

uint64_t
Component::toNumberWithMarker(uint8_t marker) const
{
  if (!isNumberWithMarker(marker))
    BOOST_THROW_EXCEPTION(Error("Name component does not have the requested marker "
                                "or the value is not a nonNegativeInteger"));

  Buffer::const_iterator valueBegin = value_begin() + 1;
  return tlv::readNonNegativeInteger(value_size() - 1, valueBegin, value_end());
}

uint64_t
Component::toVersion() const
{
  return toNumberWithMarker(VERSION_MARKER);
}

uint64_t
Component::toSegment() const
{
  return toNumberWithMarker(SEGMENT_MARKER);
}

uint64_t
Component::toSegmentOffset() const
{
  return toNumberWithMarker(SEGMENT_OFFSET_MARKER);
}

time::system_clock::TimePoint
Component::toTimestamp() const
{
  uint64_t value = toNumberWithMarker(TIMESTAMP_MARKER);
  return time::getUnixEpoch() + time::microseconds(value);
}

uint64_t
Component::toSequenceNumber() const
{
  return toNumberWithMarker(SEQUENCE_NUMBER_MARKER);
}

////////////////////////////////////////////////////////////////////////////////

Component
Component::fromNumber(uint64_t number)
{
  return makeNonNegativeIntegerBlock(tlv::GenericNameComponent, number);
}

Component
Component::fromNumberWithMarker(uint8_t marker, uint64_t number)
{
  EncodingEstimator estimator;

  size_t valueLength = estimator.prependNonNegativeInteger(number);
  valueLength += estimator.prependByteArray(&marker, 1);
  size_t totalLength = valueLength;
  totalLength += estimator.prependVarNumber(valueLength);
  totalLength += estimator.prependVarNumber(tlv::GenericNameComponent);

  EncodingBuffer encoder(totalLength, 0);
  encoder.prependNonNegativeInteger(number);
  encoder.prependByteArray(&marker, 1);
  encoder.prependVarNumber(valueLength);
  encoder.prependVarNumber(tlv::GenericNameComponent);

  return encoder.block();
}

Component
Component::fromVersion(uint64_t version)
{
  return fromNumberWithMarker(VERSION_MARKER, version);
}

Component
Component::fromSegment(uint64_t segmentNo)
{
  return fromNumberWithMarker(SEGMENT_MARKER, segmentNo);
}

Component
Component::fromSegmentOffset(uint64_t offset)
{
  return fromNumberWithMarker(SEGMENT_OFFSET_MARKER, offset);
}

Component
Component::fromTimestamp(const time::system_clock::TimePoint& timePoint)
{
  using namespace time;
  uint64_t value = duration_cast<microseconds>(timePoint - getUnixEpoch()).count();
  return fromNumberWithMarker(TIMESTAMP_MARKER, value);
}

Component
Component::fromSequenceNumber(uint64_t seqNo)
{
  return fromNumberWithMarker(SEQUENCE_NUMBER_MARKER, seqNo);
}

////////////////////////////////////////////////////////////////////////////////

bool
Component::isGeneric() const
{
  return type() == tlv::GenericNameComponent;
}

bool
Component::isImplicitSha256Digest() const
{
  return detail::getComponentType1().match(*this);
}

Component
Component::fromImplicitSha256Digest(ConstBufferPtr digest)
{
  return detail::getComponentType1().create(digest);
}

Component
Component::fromImplicitSha256Digest(const uint8_t* digest, size_t digestSize)
{
  return detail::getComponentType1().create(digest, digestSize);
}

bool
Component::isParametersSha256Digest() const
{
  return detail::getComponentType2().match(*this);
}

Component
Component::fromParametersSha256Digest(ConstBufferPtr digest)
{
  return detail::getComponentType2().create(digest);
}

Component
Component::fromParametersSha256Digest(const uint8_t* digest, size_t digestSize)
{
  return detail::getComponentType2().create(digest, digestSize);
}

////////////////////////////////////////////////////////////////////////////////

bool
Component::equals(const Component& other) const
{
  return type() == other.type() &&
         value_size() == other.value_size() &&
         (empty() || // needed with Apple clang < 9.0.0 due to libc++ bug
          std::equal(value_begin(), value_end(), other.value_begin()));
}

int
Component::compare(const Component& other) const
{
  if (this->hasWire() && other.hasWire()) {
    // In the common case where both components have wire encoding,
    // it's more efficient to simply compare the wire encoding.
    // This works because lexical order of TLV encoding happens to be
    // the same as canonical order of the value.
    return std::memcmp(wire(), other.wire(), std::min(size(), other.size()));
  }

  int cmpType = type() - other.type();
  if (cmpType != 0)
    return cmpType;

  int cmpSize = value_size() - other.value_size();
  if (cmpSize != 0)
    return cmpSize;

  if (empty())
    return 0;

  return std::memcmp(value(), other.value(), value_size());
}

Component
Component::getSuccessor() const
{
  bool isOverflow = false;
  Component successor;
  std::tie(isOverflow, successor) =
    detail::getComponentTypeTable().get(type()).getSuccessor(*this);
  if (!isOverflow) {
    return successor;
  }

  uint32_t type = this->type() + 1;
  const std::vector<uint8_t>& value = detail::getComponentTypeTable().get(type).getMinValue();
  return Component(type, value.data(), value.size());
}

template<encoding::Tag TAG>
size_t
Component::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;
  if (value_size() > 0)
    totalLength += encoder.prependByteArray(value(), value_size());
  totalLength += encoder.prependVarNumber(value_size());
  totalLength += encoder.prependVarNumber(type());
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(Component);

const Block&
Component::wireEncode() const
{
  if (this->hasWire())
    return *this;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Component&>(*this) = buffer.block();
  return *this;
}

void
Component::wireDecode(const Block& wire)
{
  *this = wire;
  // validity check is done within Component(const Block& wire)
}

} // namespace name
} // namespace ndn
