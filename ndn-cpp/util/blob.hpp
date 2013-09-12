/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BLOB_HPP
#define	NDN_BLOB_HPP

#include "../common.hpp"

namespace ndn {

/**
 * A Blob holds a pointer to an immutable byte array.  This is like a JavaScript string which is a pointer
 * to an immutable string.  It is OK to pass a pointer to the string because the new owner can't change the bytes
 * of the string.  However, like a JavaScript string, it is possible to change the pointer, and so this does allow
 * the copy constructor and assignment to change the pointer.
 */
class Blob {
public:
  /**
   * Create a new Blob with an immutable copy of the given array.
   * @param value A pointer to the byte array which is copied.
   * @param valueLength The length of value.
   */
  Blob(const unsigned char* value, unsigned int valueLength)
  : value_(new std::vector<unsigned char>(value, value + valueLength))
  {
  }
  
  /**
   * Create a new Blob with an immutable copy of the array in the given vector.
   * If you want to transfer the array without copying, the the vector has to start as a 
   * ptr_lib::shared_ptr<std::vector<unsigned char> > and you can use the Blob constructor with this type.
   * @param value A reference to a vector which is copied.
   */
  Blob(const std::vector<unsigned char> &value)
  : value_(new std::vector<unsigned char>(value))
  {
  }
  
  /**
   * Create a new Blob to point to an existing byte array.  IMPORTANT: After calling this constructor,
   * you must treat the array as immutable and promise not to change it.
   * @param value A pointer to a vector with the byte array.  This takes another reference and does not copy the bytes.
   */
  Blob(const ptr_lib::shared_ptr<std::vector<unsigned char> > &value)
  : value_(value)
  {
  }
  
  /**
   * Return a const reference to the immutable byte array.
   */
  const std::vector<unsigned char>& getValue() const
  {
    return *value_;
  }

  /**
   * Return the length of the immutable byte array.
   */
  unsigned int size() const
  {
    return value_->size();
  }

  /**
   * Get const pointer to the first byte of the immutable byte array.
   */
  const unsigned char* buffer() const
  {
    return &value_->front ();
  }
  
private:
  ptr_lib::shared_ptr<std::vector<unsigned char> > value_;
};

}

#endif
