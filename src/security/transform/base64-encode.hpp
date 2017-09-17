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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BASE64_ENCODE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BASE64_ENCODE_HPP

#include "transform-base.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The module to perform Base64 encoding transformation.
 */
class Base64Encode : public Transform
{
public:
  /**
   * @brief Create a base64 encoding module
   *
   * @p needBreak if true, insert newline after every 64 bytes, otherwise no newline is inserted
   */
  explicit
  Base64Encode(bool needBreak = true);

  ~Base64Encode();

private:
  /**
   * @brief Read partial transformation result (if exists) from BIO
   */
  void
  preTransform() final;

  /**
   * @brief Encode @p data into base64 format.
   * @return The number of input bytes that have been accepted by the converter.
   */
  size_t
  convert(const uint8_t* data, size_t dataLen) final;

  /**
   * @brief Finalize base64 encoding
   *
   * This method with read all encoding results from the converter and write them into next module.
   */
  void
  finalize() final;

  /**
   * @brief Fill output buffer with the transformation result from BIO.
   */
  void
  fillOutputBuffer();

  /**
   * @return true if converter does not have partial result.
   */
  bool
  isConverterEmpty();

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

unique_ptr<Transform>
base64Encode(bool needBreak = true);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_BASE64_ENCODE_HPP
