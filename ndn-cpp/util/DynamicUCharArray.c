/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include "ndn_memory.h"
#include "DynamicUCharArray.h"

char *ndn_DynamicUCharArray_reallocArray(struct ndn_DynamicUCharArray *self, unsigned int length)
{
  return "ndn_DynamicUCharArray_reallocArray: realloc function pointer not supplied";
}