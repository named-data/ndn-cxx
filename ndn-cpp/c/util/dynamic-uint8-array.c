/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "dynamic-uint8-array.h"

ndn_Error ndn_DynamicUInt8Array_reallocArray(struct ndn_DynamicUInt8Array *self, unsigned int length)
{
  if (!self->realloc)
    return NDN_ERROR_DynamicUInt8Array_realloc_function_pointer_not_supplied;
  
  // See if double is enough.
  unsigned int newLength = self->length * 2;
  if (length > newLength)
    // The needed length is much greater, so use it.
    newLength = length;
    
  uint8_t *newArray = (*self->realloc)(self, self->array, newLength);
  if (!newArray)
    return NDN_ERROR_DynamicUInt8Array_realloc_failed;
  
  self->array = newArray;
  self->length = newLength;
  
  return NDN_ERROR_success;
}
