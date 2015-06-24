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
 */

#ifndef NDN_ENCODING_BLOCK_HELPERS_HPP
#define NDN_ENCODING_BLOCK_HELPERS_HPP

#include "block.hpp"
#include "encoding-buffer.hpp"
#include "../util/concepts.hpp"

#include <iterator>

namespace ndn {
namespace encoding {

/**
 * @brief Helper to prepend TLV block type @p type containing non-negative integer @p value
 * @see makeNonNegativeIntegerBlock, readNonNegativeInteger
 */
template<Tag TAG>
size_t
prependNonNegativeIntegerBlock(EncodingImpl<TAG>& encoder, uint32_t type, uint64_t value);

/**
 * @brief Create a TLV block type @p type containing non-negative integer @p value
 * @see prependNonNegativeIntegerBlock, readNonNegativeInteger
 */
Block
makeNonNegativeIntegerBlock(uint32_t type, uint64_t value);

/**
 * @brief Helper to read a non-negative integer from a block
 * @see prependNonNegativeIntegerBlock, makeNonNegativeIntegerBlock
 * @throw tlv::Error if block does not contain a valid nonNegativeInteger
 */
uint64_t
readNonNegativeInteger(const Block& block);

////////

/**
 * @brief Helper to prepend TLV block type @p type containing no value (i.e., a boolean block)
 * @see makeEmptyBlock
 */
template<Tag TAG>
size_t
prependEmptyBlock(EncodingImpl<TAG>& encoder, uint32_t type);

/**
 * @brief Create a TLV block type @p type containing no value (i.e., a boolean block)
 * @see prependEmptyBlock
 */
Block
makeEmptyBlock(uint32_t type);

////////

/**
 * @brief Helper to prepend TLV block type @p type with value from a string @p value
 * @see makeStringBlock, readString
 */
template<Tag TAG>
size_t
prependStringBlock(EncodingImpl<TAG>& encoder, uint32_t type, const std::string& value);

/**
 * @brief Create a TLV block type @p type with value from a string @p
 * @see prependStringBlock, readString
 */
Block
makeStringBlock(uint32_t type, const std::string& value);

/**
 * @brief Helper to read a string value from a block
 * @see prependStringBlock, makeStringBlock
 */
std::string
readString(const Block& block);

////////

/**
 * @brief Create a TLV block type @p type with value from a buffer @p value of size @p length
 */
Block
makeBinaryBlock(uint32_t type, const uint8_t* value, size_t length);

/**
 * @brief Create a TLV block type @p type with value from a buffer @p value of size @p length
 */
Block
makeBinaryBlock(uint32_t type, const char* value, size_t length);

/**
 * @brief Helper class template to create a data block when RandomAccessIterator is used
 */
template<class Iterator>
class DataBlockFast
{
public:
  BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<Iterator>));

  static Block
  makeBlock(uint32_t type, Iterator first, Iterator last)
  {
    EncodingEstimator estimator;
    size_t valueLength = last - first;
    size_t totalLength = valueLength;
    totalLength += estimator.prependVarNumber(valueLength);
    totalLength += estimator.prependVarNumber(type);

    EncodingBuffer encoder(totalLength, 0);
    encoder.prependRange(first, last);
    encoder.prependVarNumber(valueLength);
    encoder.prependVarNumber(type);

    return encoder.block();
  }
};

/**
 * @brief Helper class template to create a data block when generic InputIterator is used
 */
template<class Iterator>
class DataBlockSlow
{
public:
  BOOST_CONCEPT_ASSERT((boost::InputIterator<Iterator>));

  static Block
  makeBlock(uint32_t type, Iterator first, Iterator last)
  {
    // reserve 4 bytes in front (common for 1(type)-3(length) encoding
    // Actual size will be adjusted as necessary by the encoder
    EncodingBuffer encoder(4, 4);
    size_t valueLength = encoder.appendRange(first, last);
    encoder.prependVarNumber(valueLength);
    encoder.prependVarNumber(type);

    return encoder.block();
  }
};

/**
 * @brief Free function to create a block given @p type and range [@p first, @p last) of bytes
 * @tparam Iterator iterator type satisfying at least InputIterator concept.  Implementation
 *                  is more optimal when the iterator type satisfies RandomAccessIterator concept
 *                  It is required that sizeof(std::iterator_traits<Iterator>::value_type) == 1.
 */
template<class Iterator>
inline Block
makeBinaryBlock(uint32_t type, Iterator first, Iterator last)
{
  static_assert(sizeof(typename std::iterator_traits<Iterator>::value_type) == 1,
                "Iterator should point only to char or unsigned char");

  typedef typename boost::mpl::if_<
    std::is_base_of<std::random_access_iterator_tag,
                    typename std::iterator_traits<Iterator>::iterator_category>,
    DataBlockFast<Iterator>,
    DataBlockSlow<Iterator>>::type DataBlock;

  return DataBlock::makeBlock(type, first, last);
}

////////

/**
 * @brief Prepend a TLV block of type @p type with WireEncodable @p value as a value
 * @tparam U type that satisfies WireEncodableWithEncodingBuffer concept
 * @see makeNestedBlock
 */
template<Tag TAG, class U>
inline size_t
prependNestedBlock(EncodingImpl<TAG>& encoder, uint32_t type, const U& value)
{
  BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<U>));

  size_t valueLength = value.wireEncode(encoder);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

/**
 * @brief Create a TLV block of type @p type with WireEncodable @p value as a value
 * @tparam U type that satisfies WireEncodableWithEncodingBuffer concept
 * @see prependNestedBlock
 */
template<class U>
inline Block
makeNestedBlock(uint32_t type, const U& value)
{
  EncodingEstimator estimator;
  size_t totalLength = prependNestedBlock(estimator, type, value);

  EncodingBuffer encoder(totalLength, 0);
  prependNestedBlock(encoder, type, value);

  return encoder.block();
}

#define NDN_CXX_ENABLE_DEPRECATED_BLOCK_HELPERS
#ifdef NDN_CXX_ENABLE_DEPRECATED_BLOCK_HELPERS

/**
 * @deprecated Use Encoder::prependBlock and Estimator::prependBlock instead
 */
template<Tag TAG>
inline size_t
prependBlock(EncodingImpl<TAG>& encoder, const Block& block)
{
  return encoder.prependBlock(block);
}

/**
 * @deprecated Use Encoder::prependByteArrayBlock and Estimator::prependByteArrayBlock instead
 */
template<Tag TAG>
inline size_t
prependByteArrayBlock(EncodingImpl<TAG>& encoder,
                      uint32_t type, const uint8_t* array, size_t arraySize)
{
  return encoder.prependByteArrayBlock(type, array, arraySize);
}

/**
 * @deprecated Use makeNonNegativeIntegerBlock instead
 */
inline Block
nonNegativeIntegerBlock(uint32_t type, uint64_t value)
{
  return makeNonNegativeIntegerBlock(type, value);
}

/**
 * @deprecated Use prependEmptyBlock instead
 */
template<Tag TAG>
size_t
prependBooleanBlock(EncodingImpl<TAG>& encoder, uint32_t type)
{
  return prependEmptyBlock(encoder, type);
}

/**
 * @deprecated Use makeEmptyBlock instead
 */
inline Block
booleanBlock(uint32_t type)
{
  return makeEmptyBlock(type);
}

/**
 * @deprecated Use makeBinaryBlock instead
 */
inline Block
dataBlock(uint32_t type, const uint8_t* data, size_t dataSize)
{
  return makeBinaryBlock(type, data, dataSize);
}

/**
 * @deprecated Use makeBinaryBlock instead
 */
inline Block
dataBlock(uint32_t type, const char* data, size_t dataSize)
{
  return makeBinaryBlock(type, data, dataSize);
}

/**
 * @deprecated Use makeBinaryBlock instead
 */
template<class Iterator>
inline Block
dataBlock(uint32_t type, Iterator first, Iterator last)
{
  return makeBinaryBlock(type, first, last);
}

/**
 * @deprecated Use makeNestedBlock instead
 */
template<class U>
inline Block
nestedBlock(uint32_t type, const U& value)
{
  return makeNestedBlock(type, value);
}

#endif // NDN_CXX_ENABLE_DEPRECATED_BLOCK_HELPERS

} // namespace encoding

using encoding::makeNonNegativeIntegerBlock;
using encoding::readNonNegativeInteger;
using encoding::makeEmptyBlock;
using encoding::makeStringBlock;
using encoding::readString;
using encoding::makeBinaryBlock;
using encoding::makeNestedBlock;

#ifdef NDN_CXX_ENABLE_DEPRECATED_BLOCK_HELPERS

using encoding::nonNegativeIntegerBlock;
using encoding::booleanBlock;
using encoding::dataBlock;

#endif // NDN_CXX_ENABLE_DEPRECATED_BLOCK_HELPERS

} // namespace ndn

#endif // NDN_ENCODING_BLOCK_HELPERS_HPP
