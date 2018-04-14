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
 */

#ifndef NDN_ENCODING_TLV_HPP
#define NDN_ENCODING_TLV_HPP

#include "buffer.hpp"
#include "endian.hpp"

#include <cstring>
#include <iterator>
#include <ostream>
#include <type_traits>

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
  explicit
  Error(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

/** @brief TLV-TYPE numbers defined in NDN Packet Format v0.3
 *  @sa https://named-data.net/doc/NDN-packet-spec/current/types.html
 */
enum {
  Interest                      = 5,
  Data                          = 6,
  Name                          = 7,
  GenericNameComponent          = 8,
  ImplicitSha256DigestComponent = 1,
  CanBePrefix                   = 33,
  MustBeFresh                   = 18,
  ForwardingHint                = 30,
  Nonce                         = 10,
  InterestLifetime              = 12,
  HopLimit                      = 34,
  Parameters                    = 35,
  MetaInfo                      = 20,
  Content                       = 21,
  SignatureInfo                 = 22,
  SignatureValue                = 23,
  ContentType                   = 24,
  FreshnessPeriod               = 25,
  FinalBlockId                  = 26,
  SignatureType                 = 27,
  KeyLocator                    = 28,
  KeyDigest                     = 29,
  LinkDelegation                = 31,
  LinkPreference                = 30,

  NameComponentMin = 1,
  NameComponentMax = 65535,

  AppPrivateBlock1 = 128,
  AppPrivateBlock2 = 32767
};

/** @brief TLV-TYPE numbers defined in NDN Packet Format v0.2 but not in v0.3
 *  @sa https://named-data.net/doc/NDN-packet-spec/0.2.1/types.html
 */
enum {
  Selectors                 = 9,
  MinSuffixComponents       = 13,
  MaxSuffixComponents       = 14,
  PublisherPublicKeyLocator = 15,
  Exclude                   = 16,
  ChildSelector             = 17,
  Any                       = 19,
};

constexpr int NameComponent NDN_CXX_DEPRECATED = GenericNameComponent;

enum SignatureTypeValue : uint16_t {
  DigestSha256 = 0,
  SignatureSha256WithRsa = 1,
  // <Unassigned> = 2,
  SignatureSha256WithEcdsa = 3
};

std::ostream&
operator<<(std::ostream& os, SignatureTypeValue signatureType);

/** @brief TLV-TYPE numbers for SignatureInfo features
 *  @sa docs/tutorials/certificate-format.rst
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

/** @brief indicates a possible value of ContentType field
 */
enum ContentTypeValue {
  /** @brief indicates content is the actual data bits
   */
  ContentType_Blob = 0,

  /** @brief indicates content is another name which identifies actual data content
   */
  ContentType_Link = 1,

  /** @brief indicates content is a public key
   */
  ContentType_Key = 2,

  /** @brief indicates a producer generated NACK
   */
  ContentType_Nack = 3
};

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
 * @brief Read VAR-NUMBER in NDN-TLV encoding
 * @tparam Iterator an iterator or pointer whose value is assignable to uint8_t
 *
 * @param [inout] begin  Begin of the buffer, will be incremented to point to the first byte after
 *                       the read VAR-NUMBER
 * @param [in]    end    End of the buffer
 * @param [out]   number Read VAR-NUMBER
 *
 * @return true if number was successfully read from input, false otherwise
 * @note This call never throws exceptions
 */
template<typename Iterator>
bool
readVarNumber(Iterator& begin, const Iterator& end, uint64_t& number);

/**
 * @brief Read TLV-TYPE
 * @tparam Iterator an iterator or pointer whose value is assignable to uint8_t
 *
 * @param [inout] begin  Begin of the buffer, will be incremented to point to the first byte after
 *                       the read TLV-TYPE
 * @param [in]    end   End of the buffer
 * @param [out]   type  Read TLV-TYPE
 *
 * @return true if TLV-TYPE was successfully read from input, false otherwise
 * @note This call never throws exceptions
 * @note This call is largely equivalent to tlv::readVarNumber, but it will return false if type
 *       is larger than 2^32-1 (TLV-TYPE in this library is implemented as uint32_t)
 */
template<typename Iterator>
bool
readType(Iterator& begin, const Iterator& end, uint32_t& type);

/**
 * @brief Read VAR-NUMBER in NDN-TLV encoding
 * @tparam Iterator an iterator or pointer whose value is assignable to uint8_t
 *
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read VAR-NUMBER
 * @param [in]    end   End of the buffer
 *
 * @throw tlv::Error VAR-NUMBER cannot be read
 */
template<typename Iterator>
uint64_t
readVarNumber(Iterator& begin, const Iterator& end);

/**
 * @brief Read TLV Type
 * @tparam Iterator an iterator or pointer whose value is assignable to uint8_t
 *
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read TLV-TYPE
 * @param [in]    end   End of the buffer
 *
 * @throw tlv::Error VAR-NUMBER cannot be read
 * @note This call is largely equivalent to tlv::readVarNumber, but exception will be thrown if type
 *       is larger than 2^32-1 (TLV-TYPE in this library is implemented as uint32_t)
 */
template<typename Iterator>
uint32_t
readType(Iterator& begin, const Iterator& end);

/**
 * @brief Get number of bytes necessary to hold value of VAR-NUMBER
 */
constexpr size_t
sizeOfVarNumber(uint64_t number);

/**
 * @brief Write VAR-NUMBER to the specified stream
 * @return length of written VAR-NUMBER
 */
size_t
writeVarNumber(std::ostream& os, uint64_t number);

/**
 * @brief Read nonNegativeInteger in NDN-TLV encoding
 * @tparam Iterator an iterator or pointer whose value is assignable to uint8_t
 *
 * @param [in]    size  size of the nonNegativeInteger
 * @param [inout] begin Begin of the buffer, will be incremented to point to the first byte after
 *                      the read nonNegativeInteger
 * @param [in]    end   End of the buffer
 *
 * @throw tlv::Error number cannot be read
 * @note How many bytes to read is directly controlled by \p size, which can be either 1, 2, 4, or 8.
 *       If \p size differs from \p std::distance(begin, end), tlv::Error exception will be thrown.
 */
template<typename Iterator>
uint64_t
readNonNegativeInteger(size_t size, Iterator& begin, const Iterator& end);

/**
 * @brief Get number of bytes necessary to hold value of nonNegativeInteger
 */
constexpr size_t
sizeOfNonNegativeInteger(uint64_t integer);

/**
 * @brief Write nonNegativeInteger to the specified stream
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
  bool
  operator()(size_t size, Iterator& begin, const Iterator& end, uint64_t& number) const
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
  bool
  operator()(size_t size, Iterator& begin, const Iterator& end, uint64_t& number) const
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
        number = be16toh(value);
        return true;
      }
      case 4: {
        uint32_t value = 0;
        std::memcpy(&value, &*begin, 4);
        begin += 4;
        number = be32toh(value);
        return true;
      }
      case 8: {
        uint64_t value = 0;
        std::memcpy(&value, &*begin, 8);
        begin += 8;
        number = be64toh(value);
        return true;
      }
      default: {
        BOOST_ASSERT(false);
        return false;
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
         typename DecayedIterator = typename std::decay<Iterator>::type,
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
class ReadNumber : public std::conditional<shouldSelectContiguousReadNumber<Iterator>(),
                                           ReadNumberFast<Iterator>, ReadNumberSlow<Iterator>>::type
{
};

} // namespace detail

template<typename Iterator>
bool
readVarNumber(Iterator& begin, const Iterator& end, uint64_t& number)
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
readType(Iterator& begin, const Iterator& end, uint32_t& type)
{
  uint64_t number = 0;
  bool isOk = readVarNumber(begin, end, number);
  if (!isOk || number > std::numeric_limits<uint32_t>::max()) {
    return false;
  }

  type = static_cast<uint32_t>(number);
  return true;
}

template<typename Iterator>
uint64_t
readVarNumber(Iterator& begin, const Iterator& end)
{
  if (begin == end)
    BOOST_THROW_EXCEPTION(Error("Empty buffer during TLV processing"));

  uint64_t value = 0;
  bool isOk = readVarNumber(begin, end, value);
  if (!isOk) {
    BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));
  }

  return value;
}

template<typename Iterator>
uint32_t
readType(Iterator& begin, const Iterator& end)
{
  uint64_t type = readVarNumber(begin, end);
  if (type > std::numeric_limits<uint32_t>::max()) {
    BOOST_THROW_EXCEPTION(Error("TLV-TYPE number exceeds allowed maximum"));
  }

  return static_cast<uint32_t>(type);
}

constexpr size_t
sizeOfVarNumber(uint64_t number)
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
    uint16_t value = htobe16(static_cast<uint16_t>(number));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 3;
  }
  else if (number <= std::numeric_limits<uint32_t>::max()) {
    os.put(static_cast<char>(254));
    uint32_t value = htobe32(static_cast<uint32_t>(number));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 5;
  }
  else {
    os.put(static_cast<char>(255));
    uint64_t value = htobe64(number);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 9;
  }
}

template<typename Iterator>
uint64_t
readNonNegativeInteger(size_t size, Iterator& begin, const Iterator& end)
{
  if (size != 1 && size != 2 && size != 4 && size != 8) {
    BOOST_THROW_EXCEPTION(Error("Invalid length for nonNegativeInteger "
                                "(only 1, 2, 4, and 8 are allowed)"));
  }

  uint64_t number = 0;
  bool isOk = detail::ReadNumber<Iterator>()(size, begin, end, number);
  if (!isOk) {
    BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));
  }

  return number;
}

constexpr size_t
sizeOfNonNegativeInteger(uint64_t integer)
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
    uint16_t value = htobe16(static_cast<uint16_t>(integer));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 2;
  }
  else if (integer <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = htobe32(static_cast<uint32_t>(integer));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 4;
  }
  else {
    uint64_t value = htobe64(integer);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 8;
  }
}


} // namespace tlv
} // namespace ndn

#endif // NDN_ENCODING_TLV_HPP
