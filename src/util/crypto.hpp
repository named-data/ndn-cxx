/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
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
void ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest);

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
