/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "ndn_memory.h"

#if !NDN_CPP_HAVE_MEMCMP
int ndn_memcmp(const uint8_t *buf1, const uint8_t *buf2, size_t len)
{
  size_t i;
  
  for (i = 0; i < len; i++) {
    if (buf1[i] > buf2[i])
      return 1;
    else if (buf1[i] < buf2[i])
      return -1;
  }
  
  return 0;
}
#else
int ndn_memcmp_stub_to_avoid_empty_file_warning = 0;
#endif

#if !NDN_CPP_HAVE_MEMCPY
void ndn_memcpy(uint8_t *dest, const uint8_t *src, size_t len)
{
  size_t i;
  
  for (i = 0; i < len; i++)
    dest[i] = src[i];
}
#else
int ndn_memcpy_stub_to_avoid_empty_file_warning = 0;
#endif

#if !NDN_CPP_HAVE_MEMSET
void ndn_memset(uint8_t *dest, int val, size_t len)
{
  size_t i;
  
  for (i = 0; i < len; i++)
    dest[i] = (uint8_t)val;
}
#else
int ndn_memset_stub_to_avoid_empty_file_warning = 0;
#endif
