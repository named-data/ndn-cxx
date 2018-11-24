/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "ndn-cxx/encoding/block.hpp"

namespace ndn {
namespace encoding {

/**
 * @brief Helper class to estimate size of TLV encoding
 * Interface of this class (mostly) matches interface of Encoder class
 * @sa Encoder
 */
class Estimator : noncopyable
{
public: // common interface between Encoder and Estimator
  /**
   * @brief Prepend a byte
   */
  constexpr size_t
  prependByte(uint8_t) const noexcept
  {
    return 1;
  }

  /**
   * @brief Append a byte
   */
  constexpr size_t
  appendByte(uint8_t) const noexcept
  {
    return 1;
  }

  /**
   * @brief Prepend a byte array @p array of length @p length
   */
  constexpr size_t
  prependByteArray(const uint8_t*, size_t length) const noexcept
  {
    return length;
  }

  /**
   * @brief Append a byte array @p array of length @p length
   */
  constexpr size_t
  appendByteArray(const uint8_t*, size_t length) const noexcept
  {
    return length;
  }

  /**
   * @brief Prepend range of bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  size_t
  prependRange(Iterator first, Iterator last) const noexcept
  {
    return std::distance(first, last);
  }

  /**
   * @brief Append range of bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  size_t
  appendRange(Iterator first, Iterator last) const noexcept
  {
    return std::distance(first, last);
  }

  /**
   * @brief Prepend VarNumber @p varNumber of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  prependVarNumber(uint64_t varNumber) const noexcept;

  /**
   * @brief Prepend VarNumber @p varNumber of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  appendVarNumber(uint64_t varNumber) const noexcept;

  /**
   * @brief Prepend non-negative integer @p integer of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  prependNonNegativeInteger(uint64_t integer) const noexcept;

  /**
   * @brief Append non-negative integer @p integer of NDN TLV encoding
   * @sa http://named-data.net/doc/ndn-tlv/
   */
  size_t
  appendNonNegativeInteger(uint64_t integer) const noexcept;

  /**
   * @brief Prepend TLV block of type @p type and value from buffer @p array of size @p arraySize
   */
  size_t
  prependByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize) const noexcept;

  /**
   * @brief Append TLV block of type @p type and value from buffer @p array of size @p arraySize
   */
  size_t
  appendByteArrayBlock(uint32_t type, const uint8_t* array, size_t arraySize) const noexcept;

  /**
   * @brief Prepend TLV block @p block
   */
  size_t
  prependBlock(const Block& block) const;

  /**
   * @brief Append TLV block @p block
   */
  size_t
  appendBlock(const Block& block) const;
};

} // namespace encoding
} // namespace ndn

#endif // NDN_ENCODING_ESTIMATOR_HPP
