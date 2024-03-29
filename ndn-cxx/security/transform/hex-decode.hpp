/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_HEX_DECODE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_HEX_DECODE_HPP

#include "ndn-cxx/security/transform/transform-base.hpp"

namespace ndn::security::transform {

/**
 * @brief The module to perform hexadecimal decoding transformation.
 *
 * For example, if the input is a string "012345", the output will be
 * a byte stream: 0x01, 0x23, 0x45.
 *
 * If the total length of input is not even (2n + 1), the module will throw Error.
 */
class HexDecode final : public Transform
{
private:
  /**
   * @brief Decode data @p buf, and write the result into output buffer directly.
   *
   * @return Number of input bytes that are accepted.
   */
  size_t
  convert(span<const uint8_t> hex) final;

  /**
   * @throws Error if pending byte exists.
   */
  void
  finalize() final;

  /**
   * @return results of decoding concatenation of @p oddByte and @p hex.
   */
  unique_ptr<Transform::OBuffer>
  toBytes(const uint8_t* hex, size_t hexLen);

private:
  bool m_hasOddByte = false;
  uint8_t m_oddByte = 0;
};

unique_ptr<Transform>
hexDecode();

} // namespace ndn::security::transform

#endif // NDN_CXX_SECURITY_TRANSFORM_HEX_DECODE_HPP
