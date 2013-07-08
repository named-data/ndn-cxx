/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "DynamicUCharArray.h"

char *ndn_DynamicUCharArray_reallocArray(struct ndn_DynamicUCharArray *self, unsigned int length)
{
  if (!self->realloc)
    return "ndn_DynamicUCharArray_reallocArray: realloc function pointer not supplied";
  
  // See if double is enough.
  unsigned int newLength = self->length * 2;
  if (length > newLength)
    // The needed length is much greater, so use it.
    newLength = length;
    
  unsigned char *newArray = (*self->realloc)(self->array, newLength);
  if (!newArray)
    return "ndn_DynamicUCharArray_reallocArray: realloc failed";
  
  self->array = newArray;
  self->length = newLength;
  
  return 0;
}