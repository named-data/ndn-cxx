/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UTIL_CRYPTO_HPP
#define NDN_UTIL_CRYPTO_HPP

#include "../common.hpp"

#include <openssl/ssl.h>
#include <openssl/rsa.h>

namespace ndn {

/**
 * Compute the sha-256 digest of data.
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @param digest A pointer to a buffer of size SHA256_DIGEST_LENGTH to receive the data.
 */
void ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest);

} // namespace ndn

#endif // NDN_UTIL_CRYPTO_HPP
