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

#include <iterator>

namespace ndn {

/**
 * @deprecated Use Encoder::prependBlock and Estimator::prependBlock instead
 */
template<bool P>
inline size_t
prependBlock(EncodingImpl<P>& encoder, const Block& block)
{
  return encoder.prependBlock(block);
}

/**
 * @deprecated Use Encoder::prependByteArrayBlock and Estimator::prependByteArrayBlock instead
 */
template<bool P>
inline size_t
prependByteArrayBlock(EncodingImpl<P>& encoder,
                      uint32_t type, const uint8_t* array, size_t arraySize)
{
  return encoder.prependByteArrayBlock(type, array, arraySize);
}

template<bool P>
inline size_t
prependNonNegativeIntegerBlock(EncodingImpl<P>& encoder, uint32_t type, uint64_t number)
{
  size_t valueLength = encoder.prependNonNegativeInteger(number);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template<bool P>
inline size_t
prependBooleanBlock(EncodingImpl<P>& encoder, uint32_t type)
{
  size_t totalLength = encoder.prependVarNumber(0);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template<bool P, class U>
inline size_t
prependNestedBlock(EncodingImpl<P>& encoder, uint32_t type, const U& nestedBlock)
{
  size_t valueLength = nestedBlock.wireEncode(encoder);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}


inline Block
nonNegativeIntegerBlock(uint32_t type, uint64_t value)
{
  EncodingEstimator estimator;
  size_t totalLength = prependNonNegativeIntegerBlock(estimator, type, value);

  EncodingBuffer encoder(totalLength, 0);
  prependNonNegativeIntegerBlock(encoder, type, value);

  return encoder.block();
}

inline uint64_t
readNonNegativeInteger(const Block& block)
{
  Buffer::const_iterator begin = block.value_begin();
  return tlv::readNonNegativeInteger(block.value_size(), begin, block.value_end());
}

inline Block
booleanBlock(uint32_t type)
{
  EncodingEstimator estimator;
  size_t totalLength = prependBooleanBlock(estimator, type);

  EncodingBuffer encoder(totalLength, 0);
  prependBooleanBlock(encoder, type);

  return encoder.block();
}

inline Block
dataBlock(uint32_t type, const uint8_t* data, size_t dataSize)
{
  EncodingEstimator estimator;
  size_t totalLength = estimator.prependByteArrayBlock(type, data, dataSize);

  EncodingBuffer encoder(totalLength, 0);
  encoder.prependByteArrayBlock(type, data, dataSize);

  return encoder.block();
}

inline Block
dataBlock(uint32_t type, const char* data, size_t dataSize)
{
  return dataBlock(type, reinterpret_cast<const uint8_t*>(data), dataSize);
}

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
dataBlock(uint32_t type, Iterator first, Iterator last)
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

} // namespace ndn

#endif // NDN_ENCODING_BLOCK_HELPERS_HPP
