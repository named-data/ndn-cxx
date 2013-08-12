/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "dynamic-uchar-array.h"

ndn_Error ndn_DynamicUCharArray_reallocArray(struct ndn_DynamicUCharArray *self, unsigned int length)
{
  if (!self->realloc)
    return NDN_ERROR_DynamicUCharArray_realloc_function_pointer_not_supplied;
  
  // See if double is enough.
  unsigned int newLength = self->length * 2;
  if (length > newLength)
    // The needed length is much greater, so use it.
    newLength = length;
    
  unsigned char *newArray = (*self->realloc)(self, self->array, newLength);
  if (!newArray)
    return NDN_ERROR_DynamicUCharArray_realloc_failed;
  
  self->array = newArray;
  self->length = newLength;
  
  return NDN_ERROR_success;
}