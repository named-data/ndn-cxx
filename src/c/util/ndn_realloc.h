/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NDN_REALLOC_H
#define NDN_NDN_REALLOC_H

#include "dynamic-uint8-array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wrap the C stdlib realloc to convert to/from void * to uint8_t *.
 * This can be used by ndn_DynamicUInt8Array_initialize.
 * @param self This is ignored.
 * @param array the allocated array buffer to realloc.
 * @param length the length for the new array buffer.
 * @return the new allocated array buffer.
 */
uint8_t *ndn_realloc(struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length);

#ifdef __cplusplus
}
#endif

#endif
