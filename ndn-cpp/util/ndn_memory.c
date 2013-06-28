/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "ndn_memory.h"

#if !HAVE_MEMCPY
void ndn_memcpy(unsigned char *dest, unsigned char *src, unsigned int len)
{
  unsigned int i;
  
  for (i = 0; i < len; i++)
    dest[i] = src[i];
}
#endif

#if !HAVE_MEMSET
void ndn_memset(unsigned char *dest, int val, unsigned int len)
{
  unsigned int i;
  
  for (i = 0; i < len; i++)
    dest[i] = (unsigned char)val;
}
#endif