/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DYNAMICUCHARARRAY_H
#define NDN_DYNAMICUCHARARRAY_H

#include "../errors.h"
#include "ndn_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_DynamicUCharArray {
  unsigned char *array; /**< the allocated array buffer */
  unsigned int length;  /**< the length of the allocated array buffer */
  unsigned char * (*realloc)
    (struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length); /**< a pointer to a function that reallocates array and returns a new pointer to a buffer of
                                                                                      * length bytes, or 0 for error.  On success, the contents of the old buffer are copied to the new one.
                                                                                      * On success, the original array pointer will no longer be used.
                                                                                      * self is a pointer to the struct ndn_DynamicUCharArray which is calling realloc.
                                                                                      * This function pointer may be 0 (which causes an error if a reallocate is necessary). */
};

/**
 * Initialize an ndn_DynamicUCharArray struct with the given array buffer.
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param array the allocated array buffer
 * @param length the length of the allocated array buffer
 * @param reallocFunction see ndn_DynamicUCharArray_ensureLength.  This may be 0.
 */
static inline void ndn_DynamicUCharArray_initialize
  (struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length, 
   unsigned char * (*reallocFunction)(struct ndn_DynamicUCharArray *self, unsigned char *, unsigned int)) 
{
  self->array = array;
  self->length = length;
  self->realloc = reallocFunction;
}

/**
 * Do the work of ndn_DynamicUCharArray_ensureLength if realloc is necessary.
 * If the self->realloc function pointer is null, then return an error.
 * If not null, call self->realloc to reallocate self->array, and update self->length (which may be greater than length).
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param length the needed minimum size for self->length
 * @return 0 for success, else an error code if can't reallocate the array
 */
ndn_Error ndn_DynamicUCharArray_reallocArray(struct ndn_DynamicUCharArray *self, unsigned int length);

/**
 * Ensure that self->length is greater than or equal to length.  If it is, just return 0 for success.
 * Otherwise, if the self->realloc function pointer is null, then return an error.
 * If not null, call self->realloc to reallocate self->array, and update self->length (which may be greater than length).
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param length the needed minimum size for self->length
 * @return 0 for success, else an error code if need to reallocate the array but can't
 */
static inline ndn_Error ndn_DynamicUCharArray_ensureLength(struct ndn_DynamicUCharArray *self, unsigned int length) 
{
  if (self->length >= length)
    return NDN_ERROR_success;

  return ndn_DynamicUCharArray_reallocArray(self, length);
}

/**
 * Copy value into self->array at offset, using ndn_DynamicUCharArray_ensureLength to make sure self->array has enough length.
 * @param self pointer to the ndn_DynamicUCharArray struct
 * @param value the buffer to copy from
 * @param valueLength the length of the value buffer
 * @param offset the offset in self->array to copy to
 * @return 0 for success, else an error code if need to reallocate the array but can't
 */
static inline ndn_Error ndn_DynamicUCharArray_set
  (struct ndn_DynamicUCharArray *self, unsigned char *value, unsigned int valueLength, unsigned int offset) 
{
  ndn_Error error;
  if ((error = ndn_DynamicUCharArray_ensureLength(self, valueLength + offset)))
    return error;
  ndn_memcpy(self->array + offset, value, valueLength);
  return NDN_ERROR_success;
};

#ifdef __cplusplus
}
#endif

#endif

