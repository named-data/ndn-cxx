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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BLOCK_CIPHER_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BLOCK_CIPHER_HPP

#include "transform-base.hpp"
#include "../security-common.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief The module to encrypt data using block cipher.
 *
 * The padding scheme of the block cipher is set to the OpenSSL default,
 * which is PKCS padding.
 */
class BlockCipher : public Transform
{
public:
  /**
   * @brief Create a block cipher
   *
   * @param algo   The block cipher algorithm to use.
   * @param op     Whether to encrypt or decrypt.
   * @param key    Pointer to the key.
   * @param keyLen Size of the key.
   * @param iv     Pointer to the initialization vector.
   * @param ivLen  Length of the initialization vector.
   */
  BlockCipher(BlockCipherAlgorithm algo, CipherOperator op,
              const uint8_t* key, size_t keyLen,
              const uint8_t* iv, size_t ivLen);

  ~BlockCipher();

private:
  /**
   * @brief Read partial transformation result (if exists) from BIO
   */
  void
  preTransform() final;

  /**
   * @brief Write @p data into the cipher
   *
   * @return number of bytes that are actually accepted
   */
  size_t
  convert(const uint8_t* data, size_t dataLen) final;

  /**
   * @brief Finalize the encryption
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
  initializeAesCbc(const uint8_t* key, size_t keyLen,
                   const uint8_t* iv, size_t ivLen, CipherOperator op);

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

unique_ptr<Transform>
blockCipher(BlockCipherAlgorithm algo, CipherOperator op,
            const uint8_t* key, size_t keyLen,
            const uint8_t* iv, size_t ivLen);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_BLOCK_CIPHER_HPP
