/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BASE64_DECODE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BASE64_DECODE_HPP

#include "transform-base.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The module to perform Base64 decoding transformation.
 */
class Base64Decode : public Transform
{
public:
  /**
   * @brief Create a base64 decoding module
   *
   * @p expectNewlineEvery64Bytes if true, expect newline after every 64 bytes, otherwise expect
   *                              all input to be in a single line. Output is undefined if input
   *                              does not conform to the requirement.
   */
  explicit
  Base64Decode(bool expectNewlineEvery64Bytes = true);

  ~Base64Decode();

private:
  /**
   * @brief Read partial transformation results into output buffer and write them into next module.
   */
  void
  preTransform() final;

  /**
   * @brief Decode data @p buf in base64 format
   *
   * @return number of bytes that have been accepted by the converter
   */
  size_t
  convert(const uint8_t* buf, size_t size) final;

  /**
   * @brief Finalize base64 decoding
   *
   * This method with read all decoding results from the converter and write them into next module.
   */
  void
  finalize() final;

  /**
   * @brief Try to fill partial decoding result into output buffer.
   */
  void
  fillOutputBuffer();

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

unique_ptr<Transform>
base64Decode(bool expectNewlineEvery64Bytes = true);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_BASE64_DECODE_HPP
