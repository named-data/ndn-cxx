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

#include "block-helpers.hpp"

namespace ndn {
namespace encoding {

template<Tag TAG>
size_t
prependNonNegativeIntegerBlock(EncodingImpl<TAG>& encoder, uint32_t type, uint64_t value)
{
  size_t valueLength = encoder.prependNonNegativeInteger(value);
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template size_t
prependNonNegativeIntegerBlock<EstimatorTag>(EncodingImpl<EstimatorTag>& encoder,
                                             uint32_t type, uint64_t value);

template size_t
prependNonNegativeIntegerBlock<EncoderTag>(EncodingImpl<EncoderTag>& encoder,
                                           uint32_t type, uint64_t value);


Block
makeNonNegativeIntegerBlock(uint32_t type, uint64_t value)
{
  EncodingEstimator estimator;
  size_t totalLength = prependNonNegativeIntegerBlock(estimator, type, value);

  EncodingBuffer encoder(totalLength, 0);
  prependNonNegativeIntegerBlock(encoder, type, value);

  return encoder.block();
}

uint64_t
readNonNegativeInteger(const Block& block)
{
  Buffer::const_iterator begin = block.value_begin();
  return tlv::readNonNegativeInteger(block.value_size(), begin, block.value_end());
}

////////

template<Tag TAG>
size_t
prependEmptyBlock(EncodingImpl<TAG>& encoder, uint32_t type)
{
  size_t totalLength = encoder.prependVarNumber(0);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template size_t
prependEmptyBlock<EstimatorTag>(EncodingImpl<EstimatorTag>& encoder, uint32_t type);

template size_t
prependEmptyBlock<EncoderTag>(EncodingImpl<EncoderTag>& encoder, uint32_t type);


Block
makeEmptyBlock(uint32_t type)
{
  EncodingEstimator estimator;
  size_t totalLength = prependEmptyBlock(estimator, type);

  EncodingBuffer encoder(totalLength, 0);
  prependEmptyBlock(encoder, type);

  return encoder.block();
}

////////

template<Tag TAG>
size_t
prependStringBlock(EncodingImpl<TAG>& encoder, uint32_t type, const std::string& value)
{
  size_t valueLength = encoder.prependByteArray(reinterpret_cast<const uint8_t*>(value.data()),
                                                value.size());
  size_t totalLength = valueLength;
  totalLength += encoder.prependVarNumber(valueLength);
  totalLength += encoder.prependVarNumber(type);

  return totalLength;
}

template size_t
prependStringBlock<EstimatorTag>(EncodingImpl<EstimatorTag>& encoder,
                                 uint32_t type, const std::string& value);

template size_t
prependStringBlock<EncoderTag>(EncodingImpl<EncoderTag>& encoder,
                               uint32_t type, const std::string& value);


Block
makeStringBlock(uint32_t type, const std::string& value)
{
  EncodingEstimator estimator;
  size_t totalLength = prependStringBlock(estimator, type, value);

  EncodingBuffer encoder(totalLength, 0);
  prependStringBlock(encoder, type, value);

  return encoder.block();
}

std::string
readString(const Block& block)
{
  return std::string(reinterpret_cast<const char*>(block.value()), block.value_size());
}

////////

Block
makeBinaryBlock(uint32_t type, const uint8_t* value, size_t length)
{
  EncodingEstimator estimator;
  size_t totalLength = estimator.prependByteArrayBlock(type, value, length);

  EncodingBuffer encoder(totalLength, 0);
  encoder.prependByteArrayBlock(type, value, length);

  return encoder.block();
}

Block
makeBinaryBlock(uint32_t type, const char* value, size_t length)
{
  return makeBinaryBlock(type, reinterpret_cast<const uint8_t*>(value), length);
}

} // namespace encoding
} // namespace ndn
