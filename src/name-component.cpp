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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Zhenkai Zhu <http://irl.cs.ucla.edu/~zhenkai/>
 */

#include "name-component.hpp"

#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "util/string-helper.hpp"
#include "security/cryptopp.hpp"
#include "util/crypto.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace name {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Component>));
BOOST_CONCEPT_ASSERT((WireEncodable<Component>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Component>));
BOOST_CONCEPT_ASSERT((WireDecodable<Component>));
static_assert(std::is_base_of<tlv::Error, Component::Error>::value,
              "name::Component::Error must inherit from tlv::Error");

static const std::string&
getSha256DigestUriPrefix()
{
  static const std::string prefix{"sha256digest="};
  return prefix;
}

Component::Component()
  : Block(tlv::NameComponent)
{
}

Component::Component(const Block& wire)
  : Block(wire)
{
  if (!isGeneric() && !isImplicitSha256Digest())
    BOOST_THROW_EXCEPTION(Error("Cannot construct name::Component from not a NameComponent "
                                "or ImplicitSha256DigestComponent TLV wire block"));
}

Component::Component(const ConstBufferPtr& buffer)
  : Block(tlv::NameComponent, buffer)
{
}

Component::Component(const Buffer& value)
  : Block(makeBinaryBlock(tlv::NameComponent, value.buf(), value.size()))
{
}

Component::Component(const uint8_t* value, size_t valueLen)
  : Block(makeBinaryBlock(tlv::NameComponent, value, valueLen))
{
}

Component::Component(const char* str)
  : Block(makeBinaryBlock(tlv::NameComponent, str, std::char_traits<char>::length(str)))
{
}

Component::Component(const std::string& str)
  : Block(makeStringBlock(tlv::NameComponent, str))
{
}


Component
Component::fromEscapedString(const char* escapedString, size_t beginOffset, size_t endOffset)
{
  std::string trimmedString(escapedString + beginOffset, escapedString + endOffset);
  trim(trimmedString);

  if (trimmedString.compare(0, getSha256DigestUriPrefix().size(),
                            getSha256DigestUriPrefix()) == 0) {
    if (trimmedString.size() != getSha256DigestUriPrefix().size() + crypto::SHA256_DIGEST_SIZE * 2)
      BOOST_THROW_EXCEPTION(Error("Cannot convert to ImplicitSha256DigestComponent"
                                  "(expected sha256 in hex encoding)"));

    try {
      trimmedString.erase(0, getSha256DigestUriPrefix().size());
      return fromImplicitSha256Digest(fromHex(trimmedString));
    }
    catch (StringHelperError& e) {
      BOOST_THROW_EXCEPTION(Error("Cannot convert to a ImplicitSha256DigestComponent (invalid hex "
                                  "encoding)"));
    }
  }
  else {
    std::string value = unescape(trimmedString);

    if (value.find_first_not_of(".") == std::string::npos) {
      // Special case for component of only periods.
      if (value.size() <= 2)
        // Zero, one or two periods is illegal.  Ignore this component.
        BOOST_THROW_EXCEPTION(Error("Illegal URI (name component cannot be . or ..)"));
      else
        // Remove 3 periods.
        return Component(reinterpret_cast<const uint8_t*>(&value[3]), value.size() - 3);
    }
    else
      return Component(reinterpret_cast<const uint8_t*>(&value[0]), value.size());
  }
}


void
Component::toUri(std::ostream& result) const
{
  if (type() == tlv::ImplicitSha256DigestComponent) {
    result << getSha256DigestUriPrefix();

    printHex(result, value(), value_size(), false);
  }
  else {
    const uint8_t* value = this->value();
    size_t valueSize = value_size();

    bool gotNonDot = false;
    for (size_t i = 0; i < valueSize; ++i) {
      if (value[i] != 0x2e) {
        gotNonDot = true;
        break;
      }
    }
    if (!gotNonDot) {
      // Special case for component of zero or more periods.  Add 3 periods.
      result << "...";
      for (size_t i = 0; i < valueSize; ++i)
        result << '.';
    }
    else {
      // In case we need to escape, set to upper case hex and save the previous flags.
      std::ios::fmtflags saveFlags = result.flags(std::ios::hex | std::ios::uppercase);

      for (size_t i = 0; i < valueSize; ++i) {
        uint8_t x = value[i];
        // Check for 0-9, A-Z, a-z, (+), (-), (.), (_)
        if ((x >= 0x30 && x <= 0x39) || (x >= 0x41 && x <= 0x5a) ||
            (x >= 0x61 && x <= 0x7a) || x == 0x2b || x == 0x2d ||
            x == 0x2e || x == 0x5f)
          result << x;
        else {
          result << '%';
          if (x < 16)
            result << '0';
          result << static_cast<uint32_t>(x);
        }
      }

      // Restore.
      result.flags(saveFlags);
    }
  }
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
  return makeNonNegativeIntegerBlock(tlv::NameComponent, number);
}

Component
Component::fromNumberWithMarker(uint8_t marker, uint64_t number)
{
  EncodingEstimator estimator;

  size_t valueLength = estimator.prependNonNegativeInteger(number);
  valueLength += estimator.prependByteArray(&marker, 1);
  size_t totalLength = valueLength;
  totalLength += estimator.prependVarNumber(valueLength);
  totalLength += estimator.prependVarNumber(tlv::NameComponent);

  EncodingBuffer encoder(totalLength, 0);
  encoder.prependNonNegativeInteger(number);
  encoder.prependByteArray(&marker, 1);
  encoder.prependVarNumber(valueLength);
  encoder.prependVarNumber(tlv::NameComponent);

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
  return (type() == tlv::NameComponent);
}

bool
Component::isImplicitSha256Digest() const
{
  return (type() == tlv::ImplicitSha256DigestComponent &&
          value_size() == crypto::SHA256_DIGEST_SIZE);
}

Component
Component::fromImplicitSha256Digest(const ConstBufferPtr& digest)
{
  if (digest->size() != crypto::SHA256_DIGEST_SIZE)
    BOOST_THROW_EXCEPTION(Error("Cannot create ImplicitSha256DigestComponent (input digest must be " +
                                std::to_string(crypto::SHA256_DIGEST_SIZE) + " octets)"));

  return Block(tlv::ImplicitSha256DigestComponent, digest);
}

Component
Component::fromImplicitSha256Digest(const uint8_t* digest, size_t digestSize)
{
  if (digestSize != crypto::SHA256_DIGEST_SIZE)
    BOOST_THROW_EXCEPTION(Error("Cannot create ImplicitSha256DigestComponent (input digest must be "
                                + std::to_string(crypto::SHA256_DIGEST_SIZE) + " octets)"));

  return makeBinaryBlock(tlv::ImplicitSha256DigestComponent, digest, digestSize);
}

////////////////////////////////////////////////////////////////////////////////

int
Component::compare(const Component& other) const
{
  // Imitate ndn_Exclude_compareComponents.
  if (type() < other.type())
    return -1;
  else if (type() > other.type())
    return 1;
  else if (value_size() < other.value_size())
    return -1;
  if (value_size() > other.value_size())
    return 1;

  if (value_size() == 0)
    return 0;

  // The components are equal length.  Just do a byte compare.
  return std::memcmp(value(), other.value(), value_size());
}

Component
Component::getSuccessor() const
{
  size_t totalLength = 0;
  EncodingBuffer encoder(size() + 1, 1); // + 1 in case there is an overflow
                                         // in unlikely case TLV length increases,
                                         // EncodingBuffer will take care of that

  bool isOverflow = true;
  size_t i = value_size();
  for (; isOverflow && i > 0; i--) {
    uint8_t newValue = static_cast<uint8_t>((value()[i - 1] + 1) & 0xFF);
    totalLength += encoder.prependByte(newValue);
    isOverflow = (newValue == 0);
  }
  totalLength += encoder.prependByteArray(value(), i);

  if (isOverflow) {
    // new name components has to be extended
    totalLength += encoder.appendByte(0);
  }

  encoder.prependVarNumber(totalLength);
  encoder.prependVarNumber(type());

  return encoder.block();
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

template size_t
Component::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Component::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

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
