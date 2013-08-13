/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "util/ndn_memory.h"
#include "interest.h"

int ndn_Exclude_compareComponents(struct ndn_NameComponent *component1, struct ndn_NameComponent *component2)
{
  if (component1->valueLength < component2->valueLength)
    return -1;
  if (component1->valueLength > component2->valueLength)
    return 1;

  // The components are equal length.  Just do a byte compare.  
  return ndn_memcmp(component1->value, component2->value, component1->valueLength);
}
