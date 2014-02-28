/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_CRYPTO_HPP
#define NDN_UTIL_CRYPTO_HPP

#include "../common.hpp"
#include "../encoding/buffer.hpp"

namespace ndn {

/**
 * Compute the sha-256 digest of data.
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @param digest A pointer to a buffer of size SHA256_DIGEST_LENGTH to receive the data.
 */
void ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest);

namespace crypto {

static size_t SHA256_DIGEST_LENGTH = 32;

/**
 * Compute the sha-256 digest of data.
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @return A pointer to a buffer of SHA256_DIGEST.
 */
ConstBufferPtr
sha256(const uint8_t *data, size_t dataLength);

} // namespace crypto

} // namespace ndn

#endif // NDN_UTIL_CRYPTO_HPP
