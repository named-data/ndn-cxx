/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXMLENCODER_HPP
#define	NDN_BINARYXMLENCODER_HPP

#include <cstdlib>
#include <stdexcept>
#include <vector>
#include "../c/encoding/BinaryXMLEncoder.h"

namespace ndn {
  
/**
 * A BinaryXMLEncoder wraps a C ndn_BinaryXMLEncoder struct and related functions.
 */
class BinaryXMLEncoder {
public:
  /**
   * Initialize the base ndn_BinaryXMLEncoder struct with an initial array of 16 bytes.  Use simpleRealloc.
   */
  BinaryXMLEncoder() 
  {
    const unsigned int initialLength = 16;
    ndn_BinaryXMLEncoder_init(&base_, (unsigned char *)malloc(initialLength), initialLength, simpleRealloc);
  }
  
  /**
   * Wrap the C stdlib realloc to convert to/from void * to unsigned char *.
   * @param array the allocated array buffer to realloc
   * @param length the length for the new array buffer
   * @return the new allocated array buffer
   */
  static unsigned char *simpleRealloc(unsigned char *array, unsigned int length)
  {
    return (unsigned char *)realloc(array, length);
  }
  
  /**
   * Return a pointer to the base ndn_BinaryXMLEncoder struct.
   * @return 
   */
  struct ndn_BinaryXMLEncoder *getEncoder() { return &base_; }
  
  /**
   * Copy the encoded bytes to the end of the buffer.
   * @param buffer a vector to receive the copy
   */
  void appendTo(std::vector<unsigned char> &buffer) 
  {
    buffer.insert(buffer.end(), base_.output.array, base_.output.array + base_.offset);
  }
  
private:
  struct ndn_BinaryXMLEncoder base_;
};

}

#endif
