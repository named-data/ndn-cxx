/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef BINARYXMLSTRUCTUREDECODER_HPP
#define	BINARYXMLSTRUCTUREDECODER_HPP

#include <stdexcept>
#include "../c/encoding/BinaryXMLStructureDecoder.h"

namespace ndn {
  
/**
 * A BinaryXMLStructureDecoder wraps a C ndn_BinaryXMLStructureDecoder struct and related functions.
 */
class BinaryXMLStructureDecoder {
public:
  BinaryXMLStructureDecoder() 
  {
    ndn_BinaryXMLStructureDecoder_init(&base_);
  }
  
  /**
   * Continue scanning input starting from getOffset() to find the element end.  
   * If the end of the element which started at offset 0 is found, then return true and getOffset() is the length of 
   *   the element.  Otherwise return false, which means you should read more into input and call again.
   * @param input the input buffer. You have to pass in input each time because the buffer could be reallocated.
   * @param inputLength the number of bytes in input.
   * @return true if found the element end, false if need to read more. (This is the same as returning gotElementEnd().)
   */
  bool findElementEnd(unsigned char *input, unsigned int inputLength) 
  {
    char *error;
    if (error = ndn_BinaryXMLStructureDecoder_findElementEnd(&base_, input, inputLength))
      throw std::runtime_error(error);
    return gotElementEnd();
  }
  
  unsigned int getOffset() { return base_.offset; }
  bool gotElementEnd() { return base_.gotElementEnd != 0; }
  
private:
  struct ndn_BinaryXMLStructureDecoder base_;
};

}

#endif	/* BINARYXMLSTRUCTUREDECODER_HPP */

