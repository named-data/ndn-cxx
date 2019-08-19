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

#ifndef NDN_ENCODING_TLV_HPP
#define NDN_ENCODING_TLV_HPP

#include "ndn-cxx/detail/common.hpp"

#include <cstring>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vector>

#include <boost/endian/conversion.hpp>

namespace ndn {

/** @brief practical limit of network layer packet size
 *
 *  If a packet is longer than this size, library and application MAY drop it.
 */
const size_t MAX_NDN_PACKET_SIZE = 8800;

/**
 * @brief Namespace defining NDN Packet Format related constants and procedures
 */
namespace tlv {

/** @brief represents an error in TLV encoding or decoding
 *
 *  Element::Error SHOULD inherit from this Error class.
 */
class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;

  Error(const char* expectedType, uint32_t actualType);
};

/** @brief TLV-TYPE numbers defined in NDN Packet Format v0.3
 *  @sa https://named-data.net/doc/NDN-packet-spec/current/types.html
 */
enum : uint32_t {
  Invalid                         = 0,
  Interest                        = 5,
  Data                            = 6,
  Name                            = 7,
  GenericNameComponent            = 8,
  ImplicitSha256DigestComponent   = 1,
  ParametersSha256DigestComponent = 2,
  CanBePrefix                     = 33,
  MustBeFresh                     = 18,
  ForwardingHint                  = 30,
  Nonce                           = 10,
  InterestLifetime                = 12,
  HopLimit                        = 34,
  ApplicationParameters           = 36,
  MetaInfo                        = 20,
  Content                         = 21,
  SignatureInfo                   = 22,
  SignatureValue                  = 23,
  ContentType                     = 24,
  FreshnessPeriod                 = 25,
  FinalBlockId                    = 26,
  SignatureType                   = 27,
  KeyLocator                      = 28,
  KeyDigest                       = 29,
  LinkDelegation                  = 31,
  LinkPreference                  = 30,

  NameComponentMin = 1,
  NameComponentMax = 65535,

  AppPrivateBlock1 = 128,
  AppPrivateBlock2 = 32767
};

/** @brief TLV-TYPE numbers defined in NDN Packet Format v0.2 but not in v0.3
 *  @sa https://named-data.net/doc/NDN-packet-spec/0.2.1/types.html
 */
enum : uint32_t {
  Selectors                 = 9,
  MinSuffixComponents       = 13,
  MaxSuffixComponents       = 14,
  PublisherPublicKeyLocator = 15,
  Exclude                   = 16,
  ChildSelector             = 17,
  Any                       = 19,
};

[[deprecated("use GenericNameComponent")]]
constexpr int NameComponent = GenericNameComponent;

/** @brief TLV-TYPE numbers for typed name components.
 *  @sa https://redmine.named-data.net/projects/ndn-tlv/wiki/NameComponentType
 */
enum : uint32_t {
  KeywordNameComponent     = 32,
  SegmentNameComponent     = 33,
  ByteOffsetNameComponent  = 34,
  VersionNameComponent     = 35,
  TimestampNameComponent   = 36,
  SequenceNumNameComponent = 37,
};

/** @brief SignatureType values
 *  @sa https://named-data.net/doc/NDN-packet-spec/current/signature.html
 */
enum SignatureTypeValue : uint16_t {
  DigestSha256             = 0,
  SignatureSha256WithRsa   = 1,
  SignatureSha256WithEcdsa = 3,
  SignatureHmacWithSha256  = 4,
};

std::ostream&
operator<<(std::ostream& os, SignatureTypeValue st);

/** @brief TLV-TYPE numbers for SignatureInfo features
 *  @sa docs/specs/certificate-format.rst
 */
enum {
  ValidityPeriod = 253,
  NotBefore = 254,
  NotAfter = 255,

  AdditionalDescription = 258,
  DescriptionEntry = 512,
  DescriptionKey = 513,
  DescriptionValue = 514
};

/** @brief ContentType values
 *  @sa https://redmine.named-data.net/projects/ndn-tlv/wiki/ContentType
 */
enum ContentTypeValue : uint32_t {
  ContentType_Blob      = 0,    ///< payload
  ContentType_Link      = 1,    ///< another name that identifies the actual data content
  ContentType_Key       = 2,    ///< public key, certificate
  ContentType_Nack      = 3,    ///< application-level nack
  ContentType_Manifest  = 4,
  ContentType_PrefixAnn = 5,    ///< prefix announcement
  ContentType_Flic      = 1024, ///< File-Like ICN Collection
};

std::ostream&
operator<<(std::ostream& os, ContentTypeValue ct);

/**
 * @brief Determine whether a TLV-TYPE is "critical" for evolvability purpose.
 * @sa https://named-data.net/doc/NDN-packet-spec/0.3/tlv.html#considerations-for-evolvability-of-tlv-based-encoding
 */
constexpr bool
isCriticalType(uint32_t type)
{
  return type <= 31 || (type & 0x01);
}

/**
 * @brief Read VAR-NUMBER in NDN-TLV encoding.
 * @tparam Iterator an iterator or pointer that dereferences to uint8_t or compatible type
 *
 * @param [inout] begin  Begin of the buffer, will be incremented to point to the first byte after
 *                       the read VAR-NUMBER
 * @param [in]    end    End of the buffer
 * @param [out]   number Read VAR-NUMBER
 *
 * @return true if number was successfully read from input, false otherwise
 */
template<typename Iterator>
NDN_CXX_NODISCARD bool
readVarNumber(Iterator& begin, Iterator end, uint64_t& number) noexcept;

/**
 * @brief Read TLV-TYPE.
 * @tparam Iterator an iterator or pointer that dereferences to uint8_t or compatible type
 *
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read TLV-TYPE
 * @param [in]    end   End of the buffer
 * @param [out]   type  Read TLV-TYPE
 *
 * @return true if TLV-TYPE was successfully read from input, false otherwise
 * @note This function is largely equivalent to readVarNumber(), except that it returns false if
 *       the TLV-TYPE is zero or larger than 2^32-1 (maximum allowed by the packet format).
 */
template<typename Iterator>
NDN_CXX_NODISCARD bool
readType(Iterator& begin, Iterator end, uint32_t& type) noexcept;

/**
 * @brief Read VAR-NUMBER in NDN-TLV encoding.
 * @tparam Iterator an iterator or pointer that dereferences to uint8_t or compatible type
 *
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read VAR-NUMBER
 * @param [in]    end   End of the buffer
 *
 * @throw tlv::Error VAR-NUMBER cannot be read
 */
template<typename Iterator>
uint64_t
readVarNumber(Iterator& begin, Iterator end);

/**
 * @brief Read TLV-TYPE.
 * @tparam Iterator an iterator or pointer that dereferences to uint8_t or compatible type
 *
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read TLV-TYPE
 * @param [in]    end   End of the buffer
 *
 * @throw tlv::Error TLV-TYPE cannot be read
 * @note This function is largely equivalent to readVarNumber(), except that it throws if
 *       the TLV-TYPE is zero or larger than 2^32-1 (maximum allowed by the packet format).
 */
template<typename Iterator>
uint32_t
readType(Iterator& begin, Iterator end);

/**
 * @brief Get the number of bytes necessary to hold the value of @p number encoded as VAR-NUMBER.
 */
constexpr size_t
sizeOfVarNumber(uint64_t number) noexcept;

/**
 * @brief Write VAR-NUMBER to the specified stream.
 * @return length of written VAR-NUMBER
 */
size_t
writeVarNumber(std::ostream& os, uint64_t number);

/**
 * @brief Read nonNegativeInteger in NDN-TLV encoding.
 * @tparam Iterator an iterator or pointer that dereferences to uint8_t or compatible type
 *
 * @param [in]    size  size of the nonNegativeInteger
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read nonNegativeInteger
 * @param [in]    end   End of the buffer
 *
 * @throw tlv::Error number cannot be read
 * @note How many bytes to read is directly controlled by \p size, which can be either 1, 2, 4, or 8.
 *       If \p size differs from `std::distance(begin, end)`, tlv::Error exception will be thrown.
 */
template<typename Iterator>
uint64_t
readNonNegativeInteger(size_t size, Iterator& begin, Iterator end);

/**
 * @brief Get the number of bytes necessary to hold the value of @p integer encoded as nonNegativeInteger.
 */
constexpr size_t
sizeOfNonNegativeInteger(uint64_t integer) noexcept;

/**
 * @brief Write nonNegativeInteger to the specified stream.
 * @return length of written nonNegativeInteger
 */
size_t
writeNonNegativeInteger(std::ostream& os, uint64_t integer);

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Inline definitions

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

namespace detail {

/** @brief Function object to read a number from InputIterator
 */
template<typename Iterator>
class ReadNumberSlow
{
public:
  constexpr bool
  operator()(size_t size, Iterator& begin, Iterator end, uint64_t& number) const noexcept
  {
    number = 0;
    size_t count = 0;
    for (; begin != end && count < size; ++begin, ++count) {
      number = (number << 8) | *begin;
    }
    return count == size;
  }
};

/** @brief Function object to read a number from ContiguousIterator
 */
template<typename Iterator>
class ReadNumberFast
{
public:
  constexpr bool
  operator()(size_t size, Iterator& begin, Iterator end, uint64_t& number) const noexcept
  {
    if (begin + size > end) {
      return false;
    }

    switch (size) {
      case 1: {
        number = *begin;
        ++begin;
        return true;
      }
      case 2: {
        uint16_t value = 0;
        std::memcpy(&value, &*begin, 2);
        begin += 2;
        number = boost::endian::big_to_native(value);
        return true;
      }
      case 4: {
        uint32_t value = 0;
        std::memcpy(&value, &*begin, 4);
        begin += 4;
        number = boost::endian::big_to_native(value);
        return true;
      }
      case 8: {
        uint64_t value = 0;
        std::memcpy(&value, &*begin, 8);
        begin += 8;
        number = boost::endian::big_to_native(value);
        return true;
      }
      default: {
        NDN_CXX_UNREACHABLE;
      }
    }
  }
};

/** @brief Determine whether to select ReadNumber implementation for ContiguousIterator
 *
 *  This is not a full ContiguousIterator detection implementation. It returns true for the most
 *  common ContiguousIterator types used with TLV decoding function templates.
 */
template<typename Iterator,
         typename DecayedIterator = std::decay_t<Iterator>,
         typename ValueType = typename std::iterator_traits<DecayedIterator>::value_type>
constexpr bool
shouldSelectContiguousReadNumber()
{
  return (std::is_convertible<DecayedIterator, const ValueType*>::value ||
          std::is_convertible<DecayedIterator, typename std::basic_string<ValueType>::const_iterator>::value ||
          std::is_convertible<DecayedIterator, typename std::vector<ValueType>::const_iterator>::value) &&
         sizeof(ValueType) == 1 &&
         !std::is_same<ValueType, bool>::value;
}

template<typename Iterator>
class ReadNumber : public std::conditional_t<shouldSelectContiguousReadNumber<Iterator>(),
                                             ReadNumberFast<Iterator>, ReadNumberSlow<Iterator>>
{
};

} // namespace detail

template<typename Iterator>
bool
readVarNumber(Iterator& begin, Iterator end, uint64_t& number) noexcept
{
  if (begin == end)
    return false;

  uint8_t firstOctet = *begin;
  ++begin;
  if (firstOctet < 253) {
    number = firstOctet;
    return true;
  }

  size_t size = firstOctet == 253 ? 2 :
                firstOctet == 254 ? 4 : 8;
  return detail::ReadNumber<Iterator>()(size, begin, end, number);
}

template<typename Iterator>
bool
readType(Iterator& begin, Iterator end, uint32_t& type) noexcept
{
  uint64_t number = 0;
  bool isOk = readVarNumber(begin, end, number);
  if (!isOk || number == Invalid || number > std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  type = static_cast<uint32_t>(number);
  return true;
}

template<typename Iterator>
uint64_t
readVarNumber(Iterator& begin, Iterator end)
{
  if (begin == end) {
    NDN_THROW(Error("Empty buffer during TLV parsing"));
  }

  uint64_t value = 0;
  bool isOk = readVarNumber(begin, end, value);
  if (!isOk) {
    NDN_THROW(Error("Insufficient data during TLV parsing"));
  }

  return value;
}

template<typename Iterator>
uint32_t
readType(Iterator& begin, Iterator end)
{
  uint64_t type = readVarNumber(begin, end);
  if (type == Invalid || type > std::numeric_limits<uint32_t>::max()) {
    NDN_THROW(Error("Illegal TLV-TYPE " + to_string(type)));
  }

  return static_cast<uint32_t>(type);
}

constexpr size_t
sizeOfVarNumber(uint64_t number) noexcept
{
  return number < 253 ? 1 :
         number <= std::numeric_limits<uint16_t>::max() ? 3 :
         number <= std::numeric_limits<uint32_t>::max() ? 5 : 9;
}

inline size_t
writeVarNumber(std::ostream& os, uint64_t number)
{
  if (number < 253) {
    os.put(static_cast<char>(number));
    return 1;
  }
  else if (number <= std::numeric_limits<uint16_t>::max()) {
    os.put(static_cast<char>(253));
    uint16_t value = boost::endian::native_to_big(static_cast<uint16_t>(number));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 3;
  }
  else if (number <= std::numeric_limits<uint32_t>::max()) {
    os.put(static_cast<char>(254));
    uint32_t value = boost::endian::native_to_big(static_cast<uint32_t>(number));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 5;
  }
  else {
    os.put(static_cast<char>(255));
    uint64_t value = boost::endian::native_to_big(number);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 9;
  }
}

template<typename Iterator>
uint64_t
readNonNegativeInteger(size_t size, Iterator& begin, Iterator end)
{
  if (size != 1 && size != 2 && size != 4 && size != 8) {
    NDN_THROW(Error("Invalid length " + to_string(size) + " for nonNegativeInteger"));
  }

  uint64_t number = 0;
  bool isOk = detail::ReadNumber<Iterator>()(size, begin, end, number);
  if (!isOk) {
    NDN_THROW(Error("Insufficient data during nonNegativeInteger parsing"));
  }

  return number;
}

constexpr size_t
sizeOfNonNegativeInteger(uint64_t integer) noexcept
{
  return integer <= std::numeric_limits<uint8_t>::max() ? 1 :
         integer <= std::numeric_limits<uint16_t>::max() ? 2 :
         integer <= std::numeric_limits<uint32_t>::max() ? 4 : 8;
}

inline size_t
writeNonNegativeInteger(std::ostream& os, uint64_t integer)
{
  if (integer <= std::numeric_limits<uint8_t>::max()) {
    os.put(static_cast<char>(integer));
    return 1;
  }
  else if (integer <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = boost::endian::native_to_big(static_cast<uint16_t>(integer));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 2;
  }
  else if (integer <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = boost::endian::native_to_big(static_cast<uint32_t>(integer));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 4;
  }
  else {
    uint64_t value = boost::endian::native_to_big(integer);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 8;
  }
}

} // namespace tlv
} // namespace ndn

#endif // NDN_ENCODING_TLV_HPP
