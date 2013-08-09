/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLENCODER_HPP
#define NDN_BINARYXMLENCODER_HPP

#include <vector>
#include "../common.hpp"
#include "../c/util/ndn_realloc.h"
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
  {
    const unsigned int initialLength = 16;
    ndn_BinaryXmlEncoder_init(this, (unsigned char *)malloc(initialLength), initialLength, ndn_realloc);
  }
  
  /**
   * Return the output as a shared_ptr.
   */
  ptr_lib::shared_ptr<std::vector<unsigned char> > getOutput() 
  {
    return ptr_lib::shared_ptr<std::vector<unsigned char> >(new std::vector<unsigned char>(output.array, output.array + offset));
  }
};

}

#endif
