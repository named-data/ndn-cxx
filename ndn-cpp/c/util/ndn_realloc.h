/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NDN_REALLOC_H
#define NDN_NDN_REALLOC_H

#include "dynamic-uchar-array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wrap the C stdlib realloc to convert to/from void * to unsigned char *.
 * This can be used by ndn_DynamicUCharArray_initialize.
 * @param self This is ignored.
 * @param array the allocated array buffer to realloc.
 * @param length the length for the new array buffer.
 * @return the new allocated array buffer.
 */
unsigned char *ndn_realloc(struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif
