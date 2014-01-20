/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TLV_HPP
#define NDN_TLV_HPP

#include <stdexcept>
#include "buffer.hpp"
#include "endian.h"

namespace ndn {

/**
 * @brief Namespace defining NDN-TLV related constants and procedures
 */
namespace Tlv {

struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

enum {
  Interest      = 1,
  Data          = 2,
  Name          = 3,
  NameComponent = 4,
  Selectors     = 5,
  Nonce         = 6,
  Scope         = 7,
  InterestLifetime          = 8,
  MinSuffixComponents       = 9,
  MaxSuffixComponents       = 10,
  PublisherPublicKeyLocator = 11,
  Exclude       = 12,
  ChildSelector = 13,
  MustBeFresh   = 14,
  Any           = 15,
  MetaInfo      = 16,
  Content       = 17,
  SignatureInfo = 18,
  SignatureValue = 19,
  ContentType   = 20,
  FreshnessPeriod = 21,
  SignatureType = 22,
  KeyLocator    = 23,
  KeyLocatorDigest = 24,

  AppPrivateBlock1 = 128,
  AppPrivateBlock2 = 32767
};

enum SignatureType {
  DigestSha256 = 0,
  SignatureSha256WithRsa = 1,
};

enum ConentType {
  ContentType_Default = 0,
  ContentType_Link = 1,
  ContentType_Key = 2,
};

/**
 * @brief Read VAR-NUMBER in NDN-TLV encoding
 *
 * This call will throw ndn::Tlv::Error (aka std::runtime_error) if number cannot be read
 *
 * Note that after call finished, begin will point to the first byte after the read VAR-NUMBER
 */
template<class InputIterator>
inline uint64_t
readVarNumber(InputIterator &begin, const InputIterator &end);

/**
 * @brief Read TLV Type
 *
 * This call is largely equivalent to tlv::readVarNumber, but exception will be thrown if type
 * is larger than 2^32-1 (type in this library is implemented as uint32_t)
 */
template<class InputIterator>
inline uint32_t
readType(InputIterator &begin, const InputIterator &end);

/**
 * @brief Get number of bytes necessary to hold value of VAR-NUMBER
 */
inline size_t
sizeOfVarNumber(uint64_t varNumber);

/**
 * @brief Write VAR-NUMBER to the specified stream
 */
inline size_t
writeVarNumber(std::ostream &os, uint64_t varNumber);

/**
 * @brief Read nonNegativeInteger in NDN-TLV encoding
 *
 * This call will throw ndn::Tlv::Error (aka std::runtime_error) if number cannot be read
 *
 * Note that after call finished, begin will point to the first byte after the read VAR-NUMBER
 *
 * How many bytes will be read is directly controlled by the size parameter, which can be either
 * 1, 2, 4, or 8.  If the value of size is different, then an exception will be thrown.
 */
template<class InputIterator>
inline uint64_t
readNonNegativeInteger(size_t size, InputIterator &begin, const InputIterator &end);

/**
 * @brief Get number of bytes necessary to hold value of nonNegativeInteger
 */
inline size_t
sizeOfNonNegativeInteger(uint64_t varNumber);

/**
 * @brief Write nonNegativeInteger to the specified stream
 */
inline size_t
writeNonNegativeInteger(std::ostream &os, uint64_t varNumber);

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Inline implementations

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

template<class InputIterator>
inline uint64_t
readVarNumber(InputIterator &begin, const InputIterator &end)
{
  if (begin == end)
    throw Error("Empty buffer during TLV processing");
  
  uint8_t value = *begin;
  ++begin;
  if (value < 253)
    {
      return value;
    }
  else if (value == 253)
    {
      if (end - begin < 2)
        throw Error("Insufficient data during TLV processing");
      
      uint16_t value = *reinterpret_cast<const uint16_t*>(&*begin); // kind of dangerous... but should be efficient
      begin += 2;
      return be16toh(value);
    }
  else if (value == 254)
    {
      if (end - begin < 4)
        throw Error("Insufficient data during TLV processing");
      
      uint32_t value = *reinterpret_cast<const uint32_t*>(&*begin); // kind of dangerous... but should be efficient
      begin += 4;
      return be32toh(value);
    }
  else // if (value == 255)
    {
      if (end - begin < 8)
        throw Error("Insufficient data during TLV processing");
      
      uint64_t value = *reinterpret_cast<const uint64_t*>(&*begin);
      begin += 8;

      return be64toh(value);
    }
}

template<class InputIterator>
inline uint32_t
readType(InputIterator &begin, const InputIterator &end)
{
  uint64_t type   = readVarNumber(begin, end);
  if (type > std::numeric_limits<uint32_t>::max())
    {
      throw Error("TLV type code exceeds allowed maximum");
    }

  return static_cast<uint32_t> (type);
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
writeVarNumber(std::ostream &os, uint64_t varNumber)
{
  if (varNumber < 253) {
    os.put(static_cast<uint8_t> (varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    os.put(253);
    uint16_t value = htobe16(static_cast<uint16_t> (varNumber));
    os.write(reinterpret_cast<const char*> (&value), 2);
    return 3;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    os.put(254);
    uint32_t value = htobe32(static_cast<uint32_t> (varNumber));
    os.write(reinterpret_cast<const char*> (&value), 4);
    return 5;
  }
  else {
    os.put(255);
    uint64_t value = htobe64(varNumber);
    os.write(reinterpret_cast<const char*> (&value), 8);
    return 9;
  }
}

template<class InputIterator>
inline uint64_t
readNonNegativeInteger(size_t size, InputIterator &begin, const InputIterator &end)
{
  switch (size) {
  case 1:
    {
      if (end - begin < 1)
        throw Error("Insufficient data during TLV processing");
      
      uint8_t value = *begin;
      begin++;
      return value;
    }
  case 2:
    {
      if (end - begin < 2)
        throw Error("Insufficient data during TLV processing");
      
      uint16_t value = *reinterpret_cast<const uint16_t*>(&*begin); // kind of dangerous... but should be efficient
      begin += 2;
      return be16toh(value);
    }
  case 4:
    {
      if (end - begin < 4)
        throw Error("Insufficient data during TLV processing");
      
      uint32_t value = *reinterpret_cast<const uint32_t*>(&*begin); // kind of dangerous... but should be efficient
      begin += 4;
      return be32toh(value);
    }
  case 8:
    {
      if (end - begin < 8)
        throw Error("Insufficient data during TLV processing");
      
      uint64_t value = *reinterpret_cast<const uint64_t*>(&*begin);
      begin += 8;
      return be64toh(value);
    }
  }
  throw Error("Invalid length for nonNegativeInteger (only 1, 2, 4, and 8 are allowed)");
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
writeNonNegativeInteger(std::ostream &os, uint64_t varNumber)
{
  if (varNumber < 253) {
    os.put(static_cast<uint8_t> (varNumber));
    return 1;
  }
  else if (varNumber <= std::numeric_limits<uint16_t>::max()) {
    uint16_t value = htobe16(static_cast<uint16_t> (varNumber));
    os.write(reinterpret_cast<const char*> (&value), 2);
    return 2;
  }
  else if (varNumber <= std::numeric_limits<uint32_t>::max()) {
    uint32_t value = htobe32(static_cast<uint32_t> (varNumber));
    os.write(reinterpret_cast<const char*> (&value), 4);
    return 4;
  }
  else {
    uint64_t value = htobe64(varNumber);
    os.write(reinterpret_cast<const char*> (&value), 8);
    return 8;
  }
}


} // namespace tlv
} // namespace ndn

#endif // NDN_TLV_HPP
