/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "util/ndn_memory.h"
#include "name.h"

int ndn_Name_match(struct ndn_Name *self, struct ndn_Name *name)
{
	// This name is longer than the name we are checking it against.
	if (self->nComponents > name->nComponents)
    return 0;

	// Check if at least one of given components doesn't match.
  unsigned int i;
  for (i = 0; i < self->nComponents; ++i) {
    struct ndn_NameComponent *selfComponent = self->components + i;
    struct ndn_NameComponent *nameComponent = name->components + i;

    if (selfComponent->valueLength != nameComponent->valueLength ||
        ndn_memcmp(selfComponent->value, nameComponent->value, selfComponent->valueLength) != 0)
      return 0;
  }

	return 1;
}
