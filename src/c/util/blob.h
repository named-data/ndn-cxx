/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BLOB_H
#define NDN_BLOB_H

#include <ndn-cpp/c/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_Blob holds a pointer to a read-only pre-allocated buffer and its length.
 */
struct ndn_Blob {
  uint8_t *value; /**< pointer to the pre-allocated buffer for the value. Must be treated as read only. */
  size_t length;  /**< the number of bytes in value. */
};

/**
 * Initialize the ndn_Blob struct with the given value.
 * @param self pointer to the ndn_Blob struct.
 * @param value The pre-allocated buffer for the value, or 0 for none.
 * @param length The number of bytes in value.
 */
static inline void ndn_Blob_initialize(struct ndn_Blob *self, uint8_t *value, size_t length) 
{
  self->value = value;
  self->length = length;
}

#ifdef __cplusplus
}
#endif

#endif
