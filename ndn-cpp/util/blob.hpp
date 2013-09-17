/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BLOB_HPP
#define	NDN_BLOB_HPP

#include "../common.hpp"

namespace ndn {

/**
 * A Blob holds a pointer to an immutable byte array implemented as const std::vector<unsigned char>.  
 * This is like a JavaScript string which is a pointer to an immutable string.  
 * It is OK to pass a pointer to the string because the new owner can't change the bytes
 * of the string.  However, like a JavaScript string, it is possible to change the pointer, and so this does allow
 * the copy constructor and assignment to change the pointer.  Also remember that the pointer can be null.
 * (Note that we could have made Blob derive directly from vector<unsigned char> and then explicitly use
 * a pointer to it like shared_ptr<Blob>, but this does not enforce immutability because we can't declare
 * Blob as derived from const vector<unsigned char>.)
 */
class Blob : public ptr_lib::shared_ptr<const std::vector<unsigned char> > {
public:
  /**
   * Create a new Blob with a null pointer.
   */
  Blob()
  {  
  }
  
  /**
   * Create a new Blob with an immutable copy of the given array.
   * @param value A pointer to the byte array which is copied.
   * @param valueLength The length of value.
   */
  Blob(const unsigned char* value, unsigned int valueLength)
  : ptr_lib::shared_ptr<const std::vector<unsigned char> >(new std::vector<unsigned char>(value, value + valueLength))
  {
  }
  
  /**
   * Create a new Blob with an immutable copy of the array in the given vector.
   * If you want to transfer the array without copying, the the vector has to start as a 
   * ptr_lib::shared_ptr<std::vector<unsigned char> > and you can use the Blob constructor with this type.
   * @param value A reference to a vector which is copied.
   */
  Blob(const std::vector<unsigned char> &value)
  : ptr_lib::shared_ptr<const std::vector<unsigned char> >(new std::vector<unsigned char>(value))
  {
  }
  
  /**
   * Create a new Blob to point to an existing byte array.  IMPORTANT: After calling this constructor,
   * if you keep a pointer to the array then you must treat the array as immutable and promise not to change it.
   * @param value A pointer to a vector with the byte array.  This takes another reference and does not copy the bytes.
   */
  Blob(const ptr_lib::shared_ptr<std::vector<unsigned char> > &value)
  : ptr_lib::shared_ptr<const std::vector<unsigned char> >(value)
  {
  }
  Blob(const ptr_lib::shared_ptr<const std::vector<unsigned char> > &value)
  : ptr_lib::shared_ptr<const std::vector<unsigned char> >(value)
  {
  }
  
  /**
   * Return the length of the immutable byte array.
   */
  unsigned int 
  size() const
  {
    if (*this)
      return (*this)->size();
    else
      return 0;
  }

  /**
   * Return a const pointer to the first byte of the immutable byte array, or 0 if the pointer is null.
   */
  const unsigned char* 
  buf() const
  {
    if (*this)
      return &(*this)->front();
    else
      return 0;
  }
};

}

#endif
