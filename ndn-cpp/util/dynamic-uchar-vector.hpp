/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DYNAMIC_UCHAR_VECTOR_HPP
#define NDN_DYNAMIC_UCHAR_VECTOR_HPP

#include <vector>
#include "../common.hpp"
#include "../c/util/dynamic-uchar-array.h"

namespace ndn {

/**
 * A DynamicUCharVector extends ndn_DynamicUCharArray to hold a shared_ptr<vector<unsigned char> > for use with
 * C functions which need an ndn_DynamicUCharArray.
 */
class DynamicUCharVector : public ndn_DynamicUCharArray {
public:
  /**
   * Create a new DynamicUCharVector with an initial length.
   * @param initialLength The initial size of the allocated vector.
   */
  DynamicUCharVector(unsigned int initialLength);
  
  /**
   * Get the shared_ptr to the allocated vector.
   * @return The shared_ptr to the allocated vector. 
   */
  const ptr_lib::shared_ptr<std::vector<unsigned char> >& 
  get() { return vector_; }
  
private:
  /**
   * Implement the static realloc function using vector resize.
   * @param self A pointer to this object.
   * @param array Should be the front of the vector.
   * @param length The new length for the vector.
   * @return The front of the allocated vector.
   */
  static unsigned char*
  realloc(struct ndn_DynamicUCharArray *self, unsigned char *array, unsigned int length);
  
  ptr_lib::shared_ptr<std::vector<unsigned char> > vector_;
};

}

#endif
