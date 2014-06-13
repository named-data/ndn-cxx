/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_UTIL_CRYPTO_HPP
#define NDN_UTIL_CRYPTO_HPP

#include "../common.hpp"
#include "../encoding/buffer.hpp"

namespace ndn {

/**
 * @brief Compute the sha-256 digest of data.
 *
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @param digest A pointer to a buffer of size crypto::SHA256_DIGEST_SIZE to receive the data.
 */
void
ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest);

namespace crypto {

/// @brief number of octets in a SHA256 digest
static const size_t SHA256_DIGEST_SIZE = 32;

/**
 * @brief Compute the sha-256 digest of data.
 *
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @return A pointer to a buffer of SHA256_DIGEST.
 */
ConstBufferPtr
sha256(const uint8_t* data, size_t dataLength);

} // namespace crypto

} // namespace ndn

#endif // NDN_UTIL_CRYPTO_HPP
