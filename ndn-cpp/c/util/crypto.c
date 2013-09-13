/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "crypto.h"

void ndn_digestSha256(const unsigned char *data, unsigned int dataLength, unsigned char *digest)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, dataLength);
  SHA256_Final(digest, &sha256);
}
