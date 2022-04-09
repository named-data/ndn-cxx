/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_ENCODING_ESTIMATOR_HPP
#define NDN_CXX_ENCODING_ESTIMATOR_HPP

#include "ndn-cxx/encoding/block.hpp"

namespace ndn {
namespace encoding {

/**
 * @brief Helper class to estimate size of TLV encoding.
 *
 * The interface of this class (mostly) matches that of the Encoder class.
 *
 * @sa Encoder
 */
class Estimator : noncopyable
{
public: // common interface between Encoder and Estimator
  /**
   * @brief Prepend a sequence of bytes
   */
  constexpr size_t
  prependBytes(span<const uint8_t> bytes) const noexcept
  {
    return bytes.size();
  }

  /**
   * @brief Append a sequence of bytes
   */
  constexpr size_t
  appendBytes(span<const uint8_t> bytes) const noexcept
  {
    return bytes.size();
  }

  /**
   * @brief Prepend bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  constexpr size_t
  prependRange(Iterator first, Iterator last) const noexcept
  {
    return static_cast<size_t>(std::distance(first, last));
  }

  /**
   * @brief Append bytes from the range [@p first, @p last)
   */
  template<class Iterator>
  constexpr size_t
  appendRange(Iterator first, Iterator last) const noexcept
  {
    return static_cast<size_t>(std::distance(first, last));
  }

  /**
   * @brief Prepend @p n in VarNumber encoding
   */
  constexpr size_t
  prependVarNumber(uint64_t n) const noexcept
  {
    return tlv::sizeOfVarNumber(n);
  }

  /**
   * @brief Append @p n in VarNumber encoding
   */
  constexpr size_t
  appendVarNumber(uint64_t n) const noexcept
  {
    return tlv::sizeOfVarNumber(n);
  }

  /**
   * @brief Prepend @p n in NonNegativeInteger encoding
   */
  constexpr size_t
  prependNonNegativeInteger(uint64_t n) const noexcept
  {
    return tlv::sizeOfNonNegativeInteger(n);
  }

  /**
   * @brief Append @p n in NonNegativeInteger encoding
   */
  constexpr size_t
  appendNonNegativeInteger(uint64_t n) const noexcept
  {
    return tlv::sizeOfNonNegativeInteger(n);
  }
};

} // namespace encoding
} // namespace ndn

#endif // NDN_CXX_ENCODING_ESTIMATOR_HPP
