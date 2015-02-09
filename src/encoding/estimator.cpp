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

#include "estimator.hpp"

namespace ndn {
namespace encoding {

Estimator::Estimator(size_t totalReserve, size_t reserveFromBack)
{
}

size_t
Estimator::prependByte(uint8_t value)
{
  return 1;
}

size_t
Estimator::appendByte(uint8_t value)
{
  return 1;
}


size_t
Estimator::prependByteArray(const uint8_t* array, size_t length)
{
  return length;
}

size_t
Estimator::appendByteArray(const uint8_t* array, size_t length)
{
  return prependByteArray(array, length);
}

size_t
Estimator::prependVarNumber(uint64_t varNumber)
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

size_t
Estimator::appendVarNumber(uint64_t varNumber)
{
  return prependVarNumber(varNumber);
}


size_t
Estimator::prependNonNegativeInteger(uint64_t varNumber)
{
  if (varNumber <= std::numeric_limits<uint8_t>::max()) {
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

size_t
Estimator::appendNonNegativeInteger(uint64_t varNumber)
{
  return prependNonNegativeInteger(varNumber);
}


size_t
Estimator::prependByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize)
{
  size_t totalLength = arraySize;
  totalLength += prependVarNumber(arraySize);
  totalLength += prependVarNumber(type);

  return totalLength;
}

size_t
Estimator::appendByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize)
{
  return prependByteArrayBlock(type, array, arraySize);
}


size_t
Estimator::prependBlock(const Block& block)
{
  if (block.hasWire()) {
    return block.size();
  }
  else {
    return prependByteArrayBlock(block.type(), block.value(), block.value_size());
  }
}

size_t
Estimator::appendBlock(const Block& block)
{
  return prependBlock(block);
}


} // namespace encoding
} // namespace ndn
