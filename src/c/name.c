/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "util/ndn_memory.h"
#include "name.h"

uint64_t ndn_NameComponent_toNumber(struct ndn_NameComponent *self)
{
  uint64_t result = 0;
  size_t i;
  for (i = 0; i < self->value.length; ++i) {
    result *= 256;
    result += (uint64_t)self->value.value[i];
  }
  
  return result;
}

ndn_Error ndn_NameComponent_toNumberWithMarker(struct ndn_NameComponent *self, uint8_t marker, uint64_t *result)
{
  if (self->value.length == 0 || self->value.value[0] != marker)
    return NDN_ERROR_Name_component_does_not_begin_with_the_expected_marker;
  
  uint64_t localResult = 0;
  size_t i;
  for (i = 1; i < self->value.length; ++i) {
    localResult *= 256;
    localResult += (uint64_t)self->value.value[i];
  }
  
  *result = localResult;
  return NDN_ERROR_success;
}

int ndn_Name_match(struct ndn_Name *self, struct ndn_Name *name)
{
  // This name is longer than the name we are checking it against.
  if (self->nComponents > name->nComponents)
    return 0;

  // Check if at least one of given components doesn't match.
  size_t i;
  for (i = 0; i < self->nComponents; ++i) {
    struct ndn_NameComponent *selfComponent = self->components + i;
    struct ndn_NameComponent *nameComponent = name->components + i;

    if (selfComponent->value.length != nameComponent->value.length ||
        ndn_memcmp(selfComponent->value.value, nameComponent->value.value, selfComponent->value.length) != 0)
      return 0;
  }

  return 1;
}
