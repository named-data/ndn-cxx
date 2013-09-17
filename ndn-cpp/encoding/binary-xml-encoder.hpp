/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLENCODER_HPP
#define NDN_BINARYXMLENCODER_HPP

#include <vector>
#include "../common.hpp"
#include "../util/dynamic-uchar-vector.hpp"
#include "../c/encoding/binary-xml-encoder.h"

namespace ndn {
  
/**
 * A BinaryXmlEncoder extends a C ndn_BinaryXmlEncoder struct and wraps related functions.
 */
class BinaryXmlEncoder : public ndn_BinaryXmlEncoder {
public:
  /**
   * Initialize the base ndn_BinaryXmlEncoder struct with an initial array of 16 bytes.  Use simpleRealloc.
   */
  BinaryXmlEncoder() 
  : output_(16)
  {
    ndn_BinaryXmlEncoder_initialize(this, &output_);
  }
  
  /**
   * Resize the output vector to the correct encoding length and return.
   * @return The encoding as a shared_ptr.  Assume that the caller now owns the vector.
   */
  const ptr_lib::shared_ptr<std::vector<unsigned char> >& 
  getOutput() 
  {
    output_.get()->resize(offset);
    return output_.get();
  }
  
  DynamicUCharVector output_;
};

}

#endif
