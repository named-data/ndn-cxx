/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLENCODER_HPP
#define	NDN_BINARYXMLENCODER_HPP

#include <vector>
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
   * Copy the encoded bytes to the end of the buffer.
   * @param buffer a vector to receive the copy
   */
  void appendTo(std::vector<unsigned char> &buffer) 
  {
    buffer.insert(buffer.end(), output.array, output.array + offset);
  }
};

}

#endif
