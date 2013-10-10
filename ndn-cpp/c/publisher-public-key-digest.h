/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_PUBLISHERPUBLICKEYDIGEST_H
#define NDN_PUBLISHERPUBLICKEYDIGEST_H

#include <ndn-cpp/c/common.h>
#include "util/blob.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A PublisherPublicKeyDigest holds a pointer to the publisher public key digest value, if any.
 * We make a separate struct since this is used by multiple other structs.
 */
struct ndn_PublisherPublicKeyDigest {
  struct ndn_Blob publisherPublicKeyDigest; /**< A Blob whose value is a pointer to pre-allocated buffer.  0 for none */
};

/**
 * Initialize an ndn_PublisherPublicKeyDigest struct with 0 for none.
 */
static inline void ndn_PublisherPublicKeyDigest_initialize(struct ndn_PublisherPublicKeyDigest *self)
{
  ndn_Blob_initialize(&self->publisherPublicKeyDigest, 0, 0);
}

#ifdef __cplusplus
}
#endif

#endif
