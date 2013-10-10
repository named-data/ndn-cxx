/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DYNAMIC_UCHAR_VECTOR_HPP
#define NDN_DYNAMIC_UCHAR_VECTOR_HPP

#include <vector>
#include <ndn-cpp/common.hpp>
#include "../c/util/dynamic-uint8-array.h"

namespace ndn {

/**
 * A DynamicUInt8Vector extends ndn_DynamicUInt8Array to hold a shared_ptr<vector<uint8_t> > for use with
 * C functions which need an ndn_DynamicUInt8Array.
 */
class DynamicUInt8Vector : public ndn_DynamicUInt8Array {
public:
  /**
   * Create a new DynamicUInt8Vector with an initial length.
   * @param initialLength The initial size of the allocated vector.
   */
  DynamicUInt8Vector(size_t initialLength);
  
  /**
   * Get the shared_ptr to the allocated vector.
   * @return The shared_ptr to the allocated vector. 
   */
  const ptr_lib::shared_ptr<std::vector<uint8_t> >& 
  get() { return vector_; }
  
private:
  /**
   * Implement the static realloc function using vector resize.
   * @param self A pointer to this object.
   * @param array Should be the front of the vector.
   * @param length The new length for the vector.
   * @return The front of the allocated vector.
   */
  static uint8_t*
  realloc(struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length);
  
  ptr_lib::shared_ptr<std::vector<uint8_t> > vector_;
};

}

#endif
