/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "ndn_memory.h"

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