/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdlib.h>
#include "ndn_realloc.h"

unsigned char *ndn_realloc(struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length)
{
  return (unsigned char *)realloc(array, length);
}
