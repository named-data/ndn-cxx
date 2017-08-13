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

#ifndef NDN_ENCODING_BLOCK_HELPERS_HPP
#define NDN_ENCODING_BLOCK_HELPERS_HPP

#include "block.hpp"
#include "encoding-buffer.hpp"
#include "../util/concepts.hpp"

namespace ndn {
namespace encoding {

/** @brief Prepend a TLV element containing a non-negative integer
 *  @param encoder an EncodingBuffer or EncodingEstimator
 *  @param type TLV-TYPE number
 *  @param value non-negative integer value
 *  @sa makeNonNegativeIntegerBlock, readNonNegativeInteger
 */
template<Tag TAG>
size_t
prependNonNegativeIntegerBlock(EncodingImpl<TAG>& encoder, uint32_t type, uint64_t value);

extern template size_t
prependNonNegativeIntegerBlock<EstimatorTag>(EncodingImpl<EstimatorTag>&, uint32_t, uint64_t);

extern template size_t
prependNonNegativeIntegerBlock<EncoderTag>(EncodingImpl<EncoderTag>&, uint32_t, uint64_t);

/** @brief Create a TLV block containing a non-negative integer
 *  @param type TLV-TYPE number
 *  @param value non-negative integer value
 *  @sa prependNonNegativeIntegerBlock, readNonNegativeInteger
 */
Block
makeNonNegativeIntegerBlock(uint32_t type, uint64_t value);

/** @brief Read a non-negative integer from a TLV element
 *  @param block the TLV element
 *  @throw tlv::Error block does not contain a non-negative integer
 *  @sa prependNonNegativeIntegerBlock, makeNonNegativeIntegerBlock
 */
uint64_t
readNonNegativeInteger(const Block& block);

/** @brief Read a non-negative integer from a TLV element and cast to the specified type
 *  @tparam R result type, must be an integral type
 *  @param block the TLV element
 *  @throw tlv::Error block does not contain a valid non-negative integer or the number cannot be
 *                    represented in R
 */
template<typename R>
typename std::enable_if<std::is_integral<R>::value, R>::type
readNonNegativeIntegerAs(const Block& block)
{
  uint64_t value = readNonNegativeInteger(block);
  if (value > std::numeric_limits<R>::max()) {
    BOOST_THROW_EXCEPTION(tlv::Error("Value in TLV element of type " + to_string(block.type()) +
                          " is too large"));
  }
  return static_cast<R>(value);
}

/** @brief Read a non-negative integer from a TLV element and cast to the specified type
 *  @tparam R result type, must be an enumeration type
 *  @param block the TLV element
 *  @throw tlv::Error block does not contain a valid non-negative integer or the number cannot be
 *                    represented in R
 *  @warning If R is an unscoped enum type, it must have a fixed underlying type. Otherwise, this
 *           function may trigger unspecified behavior.
 */
template<typename R>
typename std::enable_if<std::is_enum<R>::value, R>::type
readNonNegativeIntegerAs(const Block& block)
{
  return static_cast<R>(readNonNegativeIntegerAs<typename std::underlying_type<R>::type>(block));
}

/** @brief Prepend an empty TLV element
 *  @param encoder an EncodingBuffer or EncodingEstimator
 *  @param type TLV-TYPE number
 *  @details The TLV element has zero-length TLV-VALUE.
 *  @sa makeEmptyBlock
 */
template<Tag TAG>
size_t
prependEmptyBlock(EncodingImpl<TAG>& encoder, uint32_t type);

extern template size_t
prependEmptyBlock<EstimatorTag>(EncodingImpl<EstimatorTag>&, uint32_t);

extern template size_t
prependEmptyBlock<EncoderTag>(EncodingImpl<EncoderTag>&, uint32_t);

/** @brief Create an empty TLV block
 *  @param type TLV-TYPE number
 *  @return A TLV block with zero-length TLV-VALUE
 *  @sa prependEmptyBlock
 */
Block
makeEmptyBlock(uint32_t type);

/** @brief Prepend a TLV element containing a string.
 *  @param encoder an EncodingBuffer or EncodingEstimator
 *  @param type TLV-TYPE number
 *  @param value string value, may contain NUL octets
 *  @sa makeStringBlock, readString
 */
template<Tag TAG>
size_t
prependStringBlock(EncodingImpl<TAG>& encoder, uint32_t type, const std::string& value);

extern template size_t
prependStringBlock<EstimatorTag>(EncodingImpl<EstimatorTag>&, uint32_t, const std::string&);

extern template size_t
prependStringBlock<EncoderTag>(EncodingImpl<EncoderTag>&, uint32_t, const std::string&);

/** @brief Create a TLV block containing a string.
 *  @param type TLV-TYPE number
 *  @param value string value, may contain NUL octets
 *  @sa prependStringBlock, readString
 */
Block
makeStringBlock(uint32_t type, const std::string& value);

/** @brief Read TLV-VALUE of a TLV element as a string.
 *  @param block the TLV element
 *  @return a string, may contain NUL octets
 *  @sa prependStringBlock, makeStringBlock
 */
std::string
readString(const Block& block);

/** @brief Create a TLV block copying TLV-VALUE from raw buffer.
 *  @param type TLV-TYPE number
 *  @param value raw buffer as TLV-VALUE
 *  @param length length of value buffer
 *  @sa Encoder::prependByteArrayBlock
 */
Block
makeBinaryBlock(uint32_t type, const uint8_t* value, size_t length);

/** @brief Create a TLV block copying TLV-VALUE from raw buffer.
 *  @param type TLV-TYPE number
 *  @param value raw buffer as TLV-VALUE
 *  @param length length of value buffer
 *  @sa Encoder::prependByteArrayBlock
 */
Block
makeBinaryBlock(uint32_t type, const char* value, size_t length);

namespace detail {

/** @brief Create a binary block copying from RandomAccessIterator
 */
template<class Iterator>
class BinaryBlockFast
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

/** @brief Create a binary block copying from generic InputIterator
 */
template<class Iterator>
class BinaryBlockSlow
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

} // namespace detail

/** @brief Create a TLV block copying TLV-VALUE from iterators.
 *  @tparam Iterator an InputIterator dereferencable to an 1-octet type; faster implementation is
 *                   available for RandomAccessIterator
 *  @param type TLV-TYPE number
 *  @param first begin iterator
 *  @param last past-the-end iterator
 */
template<class Iterator>
Block
makeBinaryBlock(uint32_t type, Iterator first, Iterator last)
{
  using BinaryBlockHelper = typename std::conditional<
    std::is_base_of<std::random_access_iterator_tag,
                    typename std::iterator_traits<Iterator>::iterator_category>::value,
    detail::BinaryBlockFast<Iterator>,
    detail::BinaryBlockSlow<Iterator>>::type;

  return BinaryBlockHelper::makeBlock(type, first, last);
}

/** @brief Prepend a TLV element containing a nested TLV element.
 *  @tparam U type that satisfies WireEncodableWithEncodingBuffer concept
 *  @param encoder an EncodingBuffer or EncodingEstimator
 *  @param type TLV-TYPE number for outer TLV element
 *  @param value an object to be encoded as inner TLV element
 *  @sa makeNestedBlock
 */
template<Tag TAG, class U>
size_t
prependNestedBlock(EncodingImpl<TAG>& encoder, uint32_t type, const U& value)
{
  BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<U>));

  size_t valueLength = value.wireEncode(encoder);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

/** @brief Create a TLV block containing a nested TLV element.
 *  @tparam U type that satisfies WireEncodableWithEncodingBuffer concept
 *  @param type TLV-TYPE number for outer TLV element
 *  @param value an object to be encoded as inner TLV element
 *  @sa prependNestedBlock
 */
template<class U>
Block
makeNestedBlock(uint32_t type, const U& value)
{
  EncodingEstimator estimator;
  size_t totalLength = prependNestedBlock(estimator, type, value);

  EncodingBuffer encoder(totalLength, 0);
  prependNestedBlock(encoder, type, value);

  return encoder.block();
}

} // namespace encoding

using encoding::makeNonNegativeIntegerBlock;
using encoding::readNonNegativeInteger;
using encoding::readNonNegativeIntegerAs;
using encoding::makeEmptyBlock;
using encoding::makeStringBlock;
using encoding::readString;
using encoding::makeBinaryBlock;
using encoding::makeNestedBlock;

} // namespace ndn

#endif // NDN_ENCODING_BLOCK_HELPERS_HPP
