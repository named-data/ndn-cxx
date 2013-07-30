/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLENCODER_HPP
#define	NDN_BINARYXMLENCODER_HPP

#include <vector>
#include "../common.hpp"
#include "../c/util/ndn_realloc.h"
#include "../c/encoding/BinaryXMLEncoder.h"

namespace ndn {
  
/**
 * A BinaryXMLEncoder extends a C ndn_BinaryXMLEncoder struct and wraps related functions.
 */
class BinaryXMLEncoder : public ndn_BinaryXMLEncoder {
public:
  /**
   * Initialize the base ndn_BinaryXMLEncoder struct with an initial array of 16 bytes.  Use simpleRealloc.
   */
  BinaryXMLEncoder() 
  {
    const unsigned int initialLength = 16;
    ndn_BinaryXMLEncoder_init(this, (unsigned char *)malloc(initialLength), initialLength, ndn_realloc);
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
