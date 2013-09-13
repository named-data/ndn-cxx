/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CRYPTO_H
#define NDN_CRYPTO_H

#include <openssl/ssl.h>
#include <openssl/rsa.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compute the sha-256 digest of data.
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @param digest A pointer to a buffer of size SHA256_DIGEST_LENGTH to receive the data.
 */
void ndn_digestSha256(const unsigned char *data, unsigned int dataLength, unsigned char *digest);

#ifdef __cplusplus
}
#endif

#endif
