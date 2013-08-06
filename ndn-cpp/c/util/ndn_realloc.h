/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NDN_REALLOC_H
#define NDN_NDN_REALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wrap the C stdlib realloc to convert to/from void * to unsigned char *.
 * This can be used by ndn_DynamicUCharArray_init.
 * @param array the allocated array buffer to realloc
 * @param length the length for the new array buffer
 * @return the new allocated array buffer
 */
unsigned char *ndn_realloc(unsigned char *array, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif
