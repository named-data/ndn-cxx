/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "dynamic-uchar-vector.hpp"

using namespace std;

namespace ndn {

DynamicUCharVector::DynamicUCharVector(unsigned int initialLength)
: vector_(new vector<unsigned char>(initialLength))
{
  ndn_DynamicUCharArray_init(this, &vector_->front(), initialLength, DynamicUCharVector::realloc);
}

unsigned char *DynamicUCharVector::realloc(struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length)
{
  // Because this method is private, assume there is not a problem with upcasting.
  DynamicUCharVector *thisObject = (DynamicUCharVector *)self;
  
  if (array != &thisObject->vector_->front())
    // We don't expect this to ever happen. The caller didn't pass the array from this object.
    return 0;
  
  thisObject->vector_->reserve(length);
  return &thisObject->vector_->front();
}

}
