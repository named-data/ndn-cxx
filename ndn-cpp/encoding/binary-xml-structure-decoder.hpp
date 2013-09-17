/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLSTRUCTUREDECODER_HPP
#define NDN_BINARYXMLSTRUCTUREDECODER_HPP

#include <stdexcept>
#include "../c/encoding/BinaryXMLStructureDecoder.h"

namespace ndn {
  
/**
 * A BinaryXmlStructureDecoder extends a C ndn_BinaryXmlStructureDecoder struct and wraps related functions.
 */
class BinaryXmlStructureDecoder : private ndn_BinaryXmlStructureDecoder {
public:
  BinaryXmlStructureDecoder() 
  {
    ndn_BinaryXmlStructureDecoder_initialize(this);
  }
  
  /**
   * Continue scanning input starting from getOffset() to find the element end.  
   * If the end of the element which started at offset 0 is found, then return true and getOffset() is the length of 
   *   the element.  Otherwise return false, which means you should read more into input and call again.
   * @param input the input buffer. You have to pass in input each time because the buffer could be reallocated.
   * @param inputLength the number of bytes in input.
   * @return true if found the element end, false if need to read more. (This is the same as returning gotElementEnd().)
   */
  bool 
  findElementEnd(unsigned char *input, unsigned int inputLength) 
  {
    ndn_Error error;
    if ((error = ndn_BinaryXmlStructureDecoder_findElementEnd(this, input, inputLength)))
      throw std::runtime_error(ndn_getErrorString(error));
    return gotElementEnd();
  }
  
  unsigned int 
  getOffset() const { return offset; }
  
  bool 
  gotElementEnd() const { return gotElementEnd != 0; }
};

}

#endif
