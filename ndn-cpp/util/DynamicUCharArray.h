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
  unsigned char *array; /**< the allocated array buffer */
  unsigned int length;  /**< the length of the allocated array buffer */
  unsigned char (*realloc)(unsigned char *array, unsigned int length); /**< a pointer to a function that reallocates array and returns a new buffer of
                                                                        * length bytes or 0 for error.  The original array pointer is no longer used.
                                                                        * This may be 0 (which causes an error if a reallocate is necessary). */
};

/**
 * Initialize an ndn_DynamicUCharArray struct with the given array buffer.
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param array the allocated array buffer
 * @param length the length of the allocated array buffer
 * @param reallocFunction see ndn_DynamicUCharArray_ensureLength.  This may be 0.
 */
static inline void ndn_DynamicUCharArray_init
  (struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length, unsigned char (*reallocFunction)(unsigned char *, unsigned int)) 
{
  self->array = array;
  self->length = length;
  self->realloc = reallocFunction;
}

char *ndn_DynamicUCharArray_reallocArray(struct ndn_DynamicUCharArray *self, unsigned int length);

/**
 * Ensure that self->length is greater than or equal to length.  If it is, just return 0 for success.
 * Otherwise, if the self->realloc function pointer is null, then return an error.
 * If not null, call self->realloc to reallocate self->array, and update self->length (which may be greater than length).
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param length the needed minimum size for self->length
 * @return 0 for success, else an error string if need to reallocate the array but can't
 */
static inline char *ndn_DynamicUCharArray_ensureLength(struct ndn_DynamicUCharArray *self, unsigned int length) 
{
  if (self->length >= length)
    return 0;

  return ndn_DynamicUCharArray_reallocArray(self, length);
}

/**
 * Copy value into self->array at offset, using ndn_DynamicUCharArray_ensureLength to make sure self->array has enough length.
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param value the buffer to copy from
 * @param valueLength the length of the value buffer
 * @param offset the offset in self->array to copy to
 * @return 0 for success, else an error string if need to reallocate the array but can't
 */
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

