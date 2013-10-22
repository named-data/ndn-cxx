/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdlib.h>
#include "ndn_realloc.h"

uint8_t *ndn_realloc(struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length)
{
  return (uint8_t *)realloc(array, length);
}
