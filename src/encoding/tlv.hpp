/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include <stdexcept>
#include <iostream>
#include <iterator>
#include <limits>

#include "buffer.hpp"
#include "endian.hpp"

namespace ndn {

/** @brief practical limit of network layer packet size
 *
 *  If a packet is longer than this size, library and application MAY drop it.
 */
const size_t MAX_NDN_PACKET_SIZE = 8800;

/**
 * @brief Namespace defining NDN-TLV related constants and procedures
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

enum {
  Interest      = 5,
  Data          = 6,
  Name          = 7,
  ImplicitSha256DigestComponent = 1,
  NameComponent = 8,
  Selectors     = 9,
  Nonce         = 10,
  // <Unassigned> = 11,
  InterestLifetime          = 12,
  MinSuffixComponents       = 13,
  MaxSuffixComponents       = 14,
  PublisherPublicKeyLocator = 15,
  Exclude       = 16,
  ChildSelector = 17,
  MustBeFresh   = 18,
  Any           = 19,
  MetaInfo      = 20,
  Content       = 21,
  SignatureInfo = 22,
  SignatureValue  = 23,
  ContentType     = 24,
  FreshnessPeriod = 25,
  FinalBlockId  = 26,
  SignatureType = 27,
  KeyLocator    = 28,
  KeyDigest     = 29,
  LinkPreference = 30,
  LinkDelegation = 31,
  SelectedDelegation = 32,

  AppPrivateBlock1 = 128,
  AppPrivateBlock2 = 32767
};

enum SignatureTypeValue {
  DigestSha256 = 0,
  SignatureSha256WithRsa = 1,
  // <Unassigned> = 2,
  SignatureSha256WithEcdsa = 3
};

/** @brief TLV codes for SignatureInfo features
 *  @sa docs/tutorials/certificate-format.rst
 */
enum {
  // SignatureInfo TLVs
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
   *  @warning Experimental. Not defined in NDN-TLV spec.
   */
  ContentType_Nack = 3
};

/**
 * @brief Read VAR-NUMBER in NDN-TLV encoding
 *
 * @param [in]  begin  Begin (pointer or iterator) of the buffer
 * @param [in]  end    End (pointer or iterator) of the buffer
 * @param [out] number Read number
 *
 * @throws This call never throws exception
 *
 * @return true if number successfully read from input, false otherwise
 */
template<class InputIterator>
inline bool
readVarNumber(InputIterator& begin, const InputIterator& end, uint64_t& number);

/**
 * @brief Read TLV Type
 *
 * @param [in]  begin  Begin (pointer or iterator) of the buffer
 * @param [in]  end    End (pointer or iterator) of the buffer
 * @param [out] type   Read type number
 *
 * @throws This call never throws exception
 *
 * This call is largely equivalent to tlv::readVarNumber, but exception will be thrown if type
 * is larger than 2^32-1 (type in this library is implemented as uint32_t)
 */
template<class InputIterator>
inline bool
readType(InputIterator& begin, const InputIterator& end, uint32_t& type);


/**
 * @brief Read VAR-NUMBER in NDN-TLV encoding
 *
 * @throws This call will throw ndn::tlv::Error (aka std::runtime_error) if number cannot be read
 *
 * Note that after call finished, begin will point to the first byte after the read VAR-NUMBER
 */
template<class InputIterator>
inline uint64_t
readVarNumber(InputIterator& begin, const InputIterator& end);

/**
 * @brief Read TLV Type
 *
 * @throws This call will throw ndn::tlv::Error (aka std::runtime_error) if number cannot be read
 *
 * This call is largely equivalent to tlv::readVarNumber, but exception will be thrown if type
 * is larger than 2^32-1 (type in this library is implemented as uint32_t)
 */
template<class InputIterator>
inline uint32_t
readType(InputIterator& begin, const InputIterator& end);

/**
 * @brief Get number of bytes necessary to hold value of VAR-NUMBER
 */
inline size_t
sizeOfVarNumber(uint64_t varNumber);

/**
 * @brief Write VAR-NUMBER to the specified stream
 */
inline size_t
writeVarNumber(std::ostream& os, uint64_t varNumber);

/**
 * @brief Read nonNegativeInteger in NDN-TLV encoding
 *
 * This call will throw ndn::tlv::Error (aka std::runtime_error) if number cannot be read
 *
 * Note that after call finished, begin will point to the first byte after the read VAR-NUMBER
 *
 * How many bytes will be read is directly controlled by the size parameter, which can be either
 * 1, 2, 4, or 8.  If the value of size is different, then an exception will be thrown.
 */
template<class InputIterator>
inline uint64_t
readNonNegativeInteger(size_t size, InputIterator& begin, const InputIterator& end);

/**
 * @brief Get number of bytes necessary to hold value of nonNegativeInteger
 */
inline size_t
sizeOfNonNegativeInteger(uint64_t varNumber);

/**
 * @brief Write nonNegativeInteger to the specified stream
 */
inline size_t
writeNonNegativeInteger(std::ostream& os, uint64_t varNumber);

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Inline implementations

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

template<class InputIterator>
inline bool
readVarNumber(InputIterator& begin, const InputIterator& end, uint64_t& number)
{
  if (begin == end)
    return false;

  uint8_t firstOctet = *begin;
  ++begin;
  if (firstOctet < 253)
    {
      number = firstOctet;
    }
  else if (firstOctet == 253)
    {
      if (end - begin < 2)
        return false;

      uint16_t value = *reinterpret_cast<const uint16_t*>(&*begin);
      begin += 2;
      number = be16toh(value);
    }
  else if (firstOctet == 254)
    {
      if (end - begin < 4)
        return false;

      uint32_t value = *reinterpret_cast<const uint32_t*>(&*begin);
      begin += 4;
      number = be32toh(value);
    }
  else // if (firstOctet == 255)
    {
      if (end - begin < 8)
        return false;

      uint64_t value = *reinterpret_cast<const uint64_t*>(&*begin);
      begin += 8;

      number = be64toh(value);
    }

  return true;
}

template<class InputIterator>
inline bool
readType(InputIterator& begin, const InputIterator& end, uint32_t& type)
{
  uint64_t number = 0;
  bool isOk = readVarNumber(begin, end, number);
  if (!isOk || number > std::numeric_limits<uint32_t>::max())
    {
      return false;
    }

  type = static_cast<uint64_t>(number);
  return true;
}

template<class InputIterator>
inline uint64_t
readVarNumber(InputIterator& begin, const InputIterator& end)
{
  if (begin == end)
    BOOST_THROW_EXCEPTION(Error("Empty buffer during TLV processing"));

  uint64_t value;
  bool isOk = readVarNumber(begin, end, value);
  if (!isOk)
    BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

  return value;
}

template<>
inline bool
readVarNumber<std::istream_iterator<uint8_t>>(std::istream_iterator<uint8_t>& begin,
                                              const std::istream_iterator<uint8_t>& end,
                                              uint64_t& value)
{
  if (begin == end)
    return false;

  uint8_t firstOctet = *begin;
  ++begin;
  if (firstOctet < 253)
    {
      value = firstOctet;
    }
  else if (firstOctet == 253)
    {
      value = 0;
      size_t count = 0;
      for (; begin != end && count < 2; ++count)
        {
          value = ((value << 8) | *begin);
          begin++;
        }

      if (count != 2)
        return false;
    }
  else if (firstOctet == 254)
    {
      value = 0;
      size_t count = 0;
      for (; begin != end && count < 4; ++count)
        {
          value = ((value << 8) | *begin);
          begin++;
        }

      if (count != 4)
        return false;
    }
  else // if (firstOctet == 255)
    {
      value = 0;
      size_t count = 0;
      for (; begin != end && count < 8; ++count)
        {
          value = ((value << 8) | *begin);
          begin++;
        }

      if (count != 8)
        return false;
    }

  return true;
}

template<class InputIterator>
inline uint32_t
readType(InputIterator& begin, const InputIterator& end)
{
  uint64_t type = readVarNumber(begin, end);
  if (type > std::numeric_limits<uint32_t>::max())
    {
      BOOST_THROW_EXCEPTION(Error("TLV type code exceeds allowed maximum"));
    }

  return static_cast<uint32_t>(type);
}

size_t
sizeOfVarNumber(uint64_t varNumber)
{
  if (varNumber < 253) {
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    return 5;
  }
  else {
    return 9;
  }
}

inline size_t
writeVarNumber(std::ostream& os, uint64_t varNumber)
{
  if (varNumber < 253) {
    os.put(static_cast<char>(varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    os.put(static_cast<char>(253));
    uint16_t value = htobe16(static_cast<uint16_t>(varNumber));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    os.put(static_cast<char>(254));
    uint32_t value = htobe32(static_cast<uint32_t>(varNumber));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 5;
  }
  else {
    os.put(static_cast<char>(255));
    uint64_t value = htobe64(varNumber);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 9;
  }
}

template<class InputIterator>
inline uint64_t
readNonNegativeInteger(size_t size, InputIterator& begin, const InputIterator& end)
{
  switch (size) {
  case 1:
    {
      if (end - begin < 1)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      uint8_t value = *begin;
      begin++;
      return value;
    }
  case 2:
    {
      if (end - begin < 2)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      uint16_t value = *reinterpret_cast<const uint16_t*>(&*begin);
      begin += 2;
      return be16toh(value);
    }
  case 4:
    {
      if (end - begin < 4)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      uint32_t value = *reinterpret_cast<const uint32_t*>(&*begin);
      begin += 4;
      return be32toh(value);
    }
  case 8:
    {
      if (end - begin < 8)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      uint64_t value = *reinterpret_cast<const uint64_t*>(&*begin);
      begin += 8;
      return be64toh(value);
    }
  }
  BOOST_THROW_EXCEPTION(Error("Invalid length for nonNegativeInteger (only 1, 2, 4, and 8 are allowed)"));
}

template<>
inline uint64_t
readNonNegativeInteger<std::istream_iterator<uint8_t> >(size_t size,
                                                        std::istream_iterator<uint8_t>& begin,
                                                        const std::istream_iterator<uint8_t>& end)
{
  switch (size) {
  case 1:
    {
      if (begin == end)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      uint64_t value = *begin;
      begin++;
      return value;
    }
  case 2:
    {
      uint64_t value = 0;
      size_t count = 0;
      for (; begin != end && count < 2; ++count)
        {
          value = ((value << 8) | *begin);
          begin++;
        }

      if (count != 2)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      return value;
    }
  case 4:
    {
      uint64_t value = 0;
      size_t count = 0;
      for (; begin != end && count < 4; ++count)
        {
          value = ((value << 8) | *begin);
          begin++;
        }

      if (count != 4)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      return value;
    }
  case 8:
    {
      uint64_t value = 0;
      size_t count = 0;
      for (; begin != end && count < 8; ++count)
        {
          value = ((value << 8) | *begin);
          begin++;
        }

      if (count != 8)
        BOOST_THROW_EXCEPTION(Error("Insufficient data during TLV processing"));

      return value;
    }
  }
  BOOST_THROW_EXCEPTION(Error("Invalid length for nonNegativeInteger (only 1, 2, 4, and 8 are allowed)"));
}

inline size_t
sizeOfNonNegativeInteger(uint64_t varNumber)
{
  if (varNumber < 253) {
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    return 2;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    return 4;
  }
  else {
    return 8;
  }
}


inline size_t
writeNonNegativeInteger(std::ostream& os, uint64_t varNumber)
{
  if (varNumber < 253) {
    os.put(static_cast<char>(varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = htobe16(static_cast<uint16_t>(varNumber));
    os.write(reinterpret_cast<const char*>(&value), 2);
    return 2;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = htobe32(static_cast<uint32_t>(varNumber));
    os.write(reinterpret_cast<const char*>(&value), 4);
    return 4;
  }
  else {
    uint64_t value = htobe64(varNumber);
    os.write(reinterpret_cast<const char*>(&value), 8);
    return 8;
  }
}


} // namespace tlv
} // namespace ndn

#endif // NDN_ENCODING_TLV_HPP
