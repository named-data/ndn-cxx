/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/encoding/estimator.hpp"

namespace ndn {
namespace encoding {

size_t
Estimator::prependVarNumber(uint64_t n) const noexcept
{
  return tlv::sizeOfVarNumber(n);
}

size_t
Estimator::appendVarNumber(uint64_t n) const noexcept
{
  return tlv::sizeOfVarNumber(n);
}

size_t
Estimator::prependNonNegativeInteger(uint64_t n) const noexcept
{
  return tlv::sizeOfNonNegativeInteger(n);
}

size_t
Estimator::appendNonNegativeInteger(uint64_t n) const noexcept
{
  return tlv::sizeOfNonNegativeInteger(n);
}

size_t
Estimator::prependByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize) const noexcept
{
  return tlv::sizeOfVarNumber(type) + tlv::sizeOfVarNumber(arraySize) + arraySize;
}

size_t
Estimator::appendByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize) const noexcept
{
  return prependByteArrayBlock(type, array, arraySize);
}

size_t
Estimator::prependBlock(const Block& block) const
{
  if (block.hasWire()) {
    return block.size();
  }
  else {
    return prependByteArrayBlock(block.type(), block.value(), block.value_size());
  }
}

size_t
Estimator::appendBlock(const Block& block) const
{
  return prependBlock(block);
}

} // namespace encoding
} // namespace ndn
