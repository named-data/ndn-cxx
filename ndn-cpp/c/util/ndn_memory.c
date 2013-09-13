/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "ndn_memory.h"

#if !HAVE_MEMCMP
int ndn_memcmp(unsigned char *buf1, unsigned char *buf2, unsigned int len)
{
  unsigned int i;
  
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

#if !HAVE_MEMCPY
void ndn_memcpy(unsigned char *dest, unsigned char *src, unsigned int len)
{
  unsigned int i;
  
  for (i = 0; i < len; i++)
    dest[i] = src[i];
}
#else
int ndn_memcpy_stub_to_avoid_empty_file_warning = 0;
#endif

#if !HAVE_MEMSET
void ndn_memset(unsigned char *dest, int val, unsigned int len)
{
  unsigned int i;
  
  for (i = 0; i < len; i++)
    dest[i] = (unsigned char)val;
}
#else
int ndn_memset_stub_to_avoid_empty_file_warning = 0;
#endif
