/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BLOB_H
#define	NDN_BLOB_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * An ndn_Blob holds a pointer to a read-only pre-allocated buffer and its length.
 */
struct ndn_Blob {
  uint8_t *value;     /**< pointer to the pre-allocated buffer for the value. Must be treated as read only. */
  size_t valueLength; /**< the number of bytes in value. */
};

/**
 * Initialize the ndn_Blob struct with the given value.
 * @param self pointer to the ndn_Blob struct.
 * @param value The pre-allocated buffer for the value, or 0 for none.
 * @param valueLength The number of bytes in value.
 */
static inline void ndn_Blob_initialize(struct ndn_Blob *self, uint8_t *value, size_t valueLength) 
{
  self->value = value;
  self->valueLength = valueLength;
}

#ifdef	__cplusplus
}
#endif

#endif
