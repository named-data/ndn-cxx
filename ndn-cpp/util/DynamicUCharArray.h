/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_DYNAMICUCHARARRAY_H
#define	NDN_DYNAMICUCHARARRAY_H

#include "ndn_memory.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_DynamicUCharArray {
  unsigned char *array;
  unsigned int length;
  unsigned char (*realloc)(unsigned char *array, unsigned int length);
};

static inline void ndn_DynamicUCharArray_init
  (struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length, unsigned char (*reallocFunction)(unsigned char *, unsigned int)) 
{
  self->array = array;
  self->length = length;
  self->realloc = reallocFunction;
}

char *ndn_DynamicUCharArray_reallocArray(struct ndn_DynamicUCharArray *self, unsigned int length);

static inline char *ndn_DynamicUCharArray_ensureLength(struct ndn_DynamicUCharArray *self, unsigned int length) 
{
  if (self->length >= length)
    return 0;

  return ndn_DynamicUCharArray_reallocArray(self, length);
}

static inline char *ndn_DynamicUCharArray_set
  (struct ndn_DynamicUCharArray *self, unsigned char *value, unsigned int valueLength, unsigned int offset) 
{
  char *error;
  if (error = ndn_DynamicUCharArray_ensureLength(self, valueLength + offset))
    return error;
  ndn_memcpy(self->array + offset, value, valueLength);
};

#ifdef	__cplusplus
}
#endif

#endif

