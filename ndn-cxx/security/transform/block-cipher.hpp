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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BLOCK_CIPHER_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BLOCK_CIPHER_HPP

#include "ndn-cxx/security/transform/transform-base.hpp"
#include "ndn-cxx/security/security-common.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The module to encrypt data using block cipher.
 *
 * The padding scheme of the block cipher is set to the OpenSSL default,
 * which is PKCS padding.
 */
class BlockCipher final : public Transform
{
public:
  /**
   * @brief Create a block cipher.
   *
   * @param algo The block cipher algorithm to use.
   * @param op   The operation to perform (encrypt or decrypt).
   * @param key  The symmetric key.
   * @param iv   The initialization vector.
   */
  BlockCipher(BlockCipherAlgorithm algo, CipherOperator op,
              span<const uint8_t> key, span<const uint8_t> iv);

  ~BlockCipher() final;

private:
  /**
   * @brief Read partial transformation result (if exists) from BIO.
   */
  void
  preTransform() final;

  /**
   * @brief Write @p data into the cipher.
   *
   * @return Number of bytes that are actually accepted.
   */
  size_t
  convert(span<const uint8_t> data) final;

  /**
   * @brief Finalize the encryption.
   */
  void
  finalize() final;

  /**
   * @brief Fill output buffer with the encryption result from BIO.
   */
  void
  fillOutputBuffer();

  /**
   * @return true if the cipher does not have partial result.
   */
  bool
  isConverterEmpty() const;

private:
  void
  initializeAesCbc(span<const uint8_t> key, span<const uint8_t> iv, CipherOperator op);

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

unique_ptr<Transform>
blockCipher(BlockCipherAlgorithm algo, CipherOperator op,
            span<const uint8_t> key, span<const uint8_t> iv);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_BLOCK_CIPHER_HPP
