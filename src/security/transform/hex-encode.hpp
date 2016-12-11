/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_HEX_ENCODE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_HEX_ENCODE_HPP

#include "transform-base.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The module to perform hexadecimal encoding transformation.
 *
 * For example, if the input is a byte stream 0x01, 0x23, 0x45,
 * the output will be a string "012345".
 */
class HexEncode : public Transform
{
public:
  /**
   * @brief Create a hex encoding module
   *
   * @param useUpperCase if true, use upper case letters, otherwise lower case
   */
  explicit
  HexEncode(bool useUpperCase = false);

private:
  /**
   * @brief Encode @p data, and write the result into next module directly.
   *
   * @return The number of input bytes that have been accepted by the converter.
   */
  size_t
  convert(const uint8_t* data, size_t dataLen) final;

  /**
   * @return results of encoding @p data
   */
  unique_ptr<Transform::OBuffer>
  toHex(const uint8_t* data, size_t dataLen);

private:
  bool m_useUpperCase;
};

unique_ptr<Transform>
hexEncode(bool useUpperCase = false);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_HEX_ENCODE_HPP
