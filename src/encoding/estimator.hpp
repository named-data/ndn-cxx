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

#ifndef NDN_ENCODING_ESTIMATOR_HPP
#define NDN_ENCODING_ESTIMATOR_HPP

#include "../common.hpp"
#include "block.hpp"

namespace ndn {
namespace encoding {

/**
 * @brief Helper class to estimate size of TLV encoding
 * Interface of this class (mostly) matches interface of Encoder class
 * @sa Encoder
 */
class Estimator
{
public: // common interface between Encoder and Estimator
  /**
   * @brief Create instance of the estimator
   * @param totalReserve  not used (for compatibility with the Encoder)
   * @param totalFromBack not used (for compatibility with the Encoder)
   */
  explicit
  Estimator(size_t totalReserve = 0, size_t reserveFromBack = 0);

  Estimator(const Estimator&) = delete;

  Estimator&
  operator=(const Estimator&) = delete;

  /**
   * @brief Prepend a byte
   */
  size_t
  prependByte(uint8_t value);

  /**
   * @brief Append a byte
   */
  size_t
  appendByte(uint8_t value);

  /**
   * @brief Prepend a byte array @p array of length @p length
   */
  size_t
  prependByteArray(const uint8_t* array, size_t length);

  /**
   * @brief Append a byte array @p array of length @p length
   */
  size_t
  appendByteArray(const uint8_t* array, size_t length);

  /**
   * @brief Prepend range of bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  size_t
  prependRange(Iterator first, Iterator last);

  /**
   * @brief Append range of bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  size_t
  appendRange(Iterator first, Iterator last);

  /**
   * @brief Prepend VarNumber @p varNumber of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  prependVarNumber(uint64_t varNumber);

  /**
   * @brief Prepend VarNumber @p varNumber of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  appendVarNumber(uint64_t varNumber);

  /**
   * @brief Prepend non-negative integer @p integer of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  prependNonNegativeInteger(uint64_t integer);

  /**
   * @brief Append non-negative integer @p integer of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  appendNonNegativeInteger(uint64_t integer);

  /**
   * @brief Prepend TLV block of type @p type and value from buffer @p array of size @p arraySize
   */
  size_t
  prependByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize);

  /**
   * @brief Append TLV block of type @p type and value from buffer @p array of size @p arraySize
   */
  size_t
  appendByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize);

  /**
   * @brief Prepend TLV block @p block
   */
  size_t
  prependBlock(const Block& block);

  /**
   * @brief Append TLV block @p block
   */
  size_t
  appendBlock(const Block& block);
};


template<class Iterator>
inline size_t
Estimator::prependRange(Iterator first, Iterator last)
{
  return std::distance(first, last);
}


template<class Iterator>
inline size_t
Estimator::appendRange(Iterator first, Iterator last)
{
  return prependRange(first, last);
}

} // namespace encoding
} // namespace ndn

#endif // NDN_ENCODING_ESTIMATOR_HPP
