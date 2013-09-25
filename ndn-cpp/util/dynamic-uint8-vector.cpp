/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "dynamic-uint8-vector.hpp"

using namespace std;

namespace ndn {

DynamicUInt8Vector::DynamicUInt8Vector(size_t initialLength)
: vector_(new vector<uint8_t>(initialLength))
{
  ndn_DynamicUInt8Array_initialize(this, &vector_->front(), initialLength, DynamicUInt8Vector::realloc);
}

uint8_t*
DynamicUInt8Vector::realloc(struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length)
{
  // Because this method is private, assume there is not a problem with upcasting.
  DynamicUInt8Vector *thisObject = (DynamicUInt8Vector *)self;
  
  if (array != &thisObject->vector_->front())
    // We don't expect this to ever happen. The caller didn't pass the array from this object.
    return 0;
  
  thisObject->vector_->resize(length);
  return &thisObject->vector_->front();
}

}
