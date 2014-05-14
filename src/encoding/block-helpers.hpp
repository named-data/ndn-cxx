/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_ENCODING_BLOCK_HELPERS_HPP
#define NDN_ENCODING_BLOCK_HELPERS_HPP

#include "block.hpp"
#include "encoding-buffer.hpp"

namespace ndn {

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
  return Tlv::readNonNegativeInteger(block.value_size(), begin, block.value_end());
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
  size_t totalLength = prependByteArrayBlock(estimator, type, data, dataSize);

  EncodingBuffer encoder(totalLength, 0);
  prependByteArrayBlock(encoder, type, data, dataSize);

  return encoder.block();
}

inline Block
dataBlock(uint32_t type, const char* data, size_t dataSize)
{
  return dataBlock(type, reinterpret_cast<const uint8_t*>(data), dataSize);
}

// template<class InputIterator>
// inline Block
// dataBlock(uint32_t type, InputIterator first, InputIterator last)
// {
//   size_t dataSize = 0;
//   for (InputIterator i = first; i != last; i++)
//     ++dataSize;

//   OBufferStream os;
//   Tlv::writeVarNumber(os, type);
//   Tlv::writeVarNumber(os, dataSize);
//   std::copy(first, last, std::ostream_iterator<uint8_t>(os));

//   return Block(os.buf());
// }

} // namespace ndn

#endif // NDN_ENCODING_BLOCK_HELPERS_HPP
