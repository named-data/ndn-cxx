/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "ndn-cpp/c/util/crypto.h"

void ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, dataLength);
  SHA256_Final(digest, &sha256);
}
