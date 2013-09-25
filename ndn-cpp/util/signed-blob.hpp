/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SIGNED_BLOB_HPP
#define	NDN_SIGNED_BLOB_HPP

#include "blob.hpp"

namespace ndn {

/**
 * A SignedBlob extends Blob to keep the offsets of a signed portion (e.g., the bytes of Data packet).
 */
class SignedBlob : public Blob {
public:
  /**
   * Create a new SignedBlob with a null pointer.
   */
  SignedBlob()
  : signedPortionBeginOffset_(0), signedPortionEndOffset_(0)
  {  
  }
  
  /**
   * Create a new SignedBlob with an immutable copy of the given array.
   * @param value A pointer to the byte array which is copied.
   * @param valueLength The length of value.
   * @param signedPortionBeginOffset The offset in the encoding of the beginning of the signed portion.
   * @param signedPortionEndOffset The offset in the encoding of the end of the signed portion.
   */
  SignedBlob
    (const uint8_t* value, size_t valueLength, size_t signedPortionBeginOffset, size_t signedPortionEndOffset)
  : Blob(value, valueLength), signedPortionBeginOffset_(signedPortionBeginOffset), signedPortionEndOffset_(signedPortionEndOffset)
  {
  }
  
  /**
   * Create a new SignedBlob with an immutable copy of the array in the given vector.
   * If you want to transfer the array without copying, the the vector has to start as a 
   * ptr_lib::shared_ptr<std::vector<uint8_t> > and you can use the SignedBlob constructor with this type.
   * @param value A reference to a vector which is copied.
   * @param signedPortionBeginOffset The offset in the encoding of the beginning of the signed portion.
   * @param signedPortionEndOffset The offset in the encoding of the end of the signed portion.
   */
  SignedBlob
    (const std::vector<uint8_t> &value, size_t signedPortionBeginOffset, size_t signedPortionEndOffset)
  : Blob(value), signedPortionBeginOffset_(signedPortionBeginOffset), signedPortionEndOffset_(signedPortionEndOffset)
  {
  }
  
  /**
   * Create a new SignedBlob to point to an existing byte array.  IMPORTANT: After calling this constructor,
   * if you keep a pointer to the array then you must treat the array as immutable and promise not to change it.
   * @param value A pointer to a vector with the byte array.  This takes another reference and does not copy the bytes.
   * @param signedPortionBeginOffset The offset in the array of the beginning of the signed portion.
   * @param signedPortionEndOffset The offset in the array of the end of the signed portion.
   */
  SignedBlob
    (const ptr_lib::shared_ptr<std::vector<uint8_t> > &value, 
     size_t signedPortionBeginOffset, size_t signedPortionEndOffset)
  : Blob(value), signedPortionBeginOffset_(signedPortionBeginOffset), signedPortionEndOffset_(signedPortionEndOffset)
  {
  }
  SignedBlob
    (const ptr_lib::shared_ptr<const std::vector<uint8_t> > &value, 
     size_t signedPortionBeginOffset, size_t signedPortionEndOffset)
  : Blob(value), signedPortionBeginOffset_(signedPortionBeginOffset), signedPortionEndOffset_(signedPortionEndOffset)
  {
  }
    
  /**
   * Return the length of the signed portion of the immutable byte array, or 0 of the pointer to the array is null.
   */
  size_t 
  signedSize() const
  {
    if (*this)
      return signedPortionEndOffset_ - signedPortionBeginOffset_;
    else
      return 0;
  }

  /**
   * Return a const pointer to the first byte of the signed portion of the immutable byte array, or 0 if the 
   * pointer to the array is null.
   */
  const uint8_t*
  signedBuf() const
  {
    if (*this)
      return &(*this)->front() + signedPortionBeginOffset_;
    else
      return 0;
  }

  /**
   * Return the offset in the array of the beginning of the signed portion.
   */  
  size_t 
  getSignedPortionBeginOffset() { return signedPortionBeginOffset_; }

  /**
   * Return the offset in the array of the end of the signed portion.
   */  
  size_t 
  getSignedPortionEndOffset() { return signedPortionEndOffset_; }
  
private:
  size_t signedPortionBeginOffset_;
  size_t signedPortionEndOffset_;
};

}

#endif
