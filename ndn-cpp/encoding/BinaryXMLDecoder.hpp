/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BINARYXMLDECODER_HPP
#define	NDN_BINARYXMLDECODER_HPP

#include "../c/errors.h"
#include "../c/encoding/BinaryXMLDecoder.h"

namespace ndn {

  
/**
 * A BinaryXMLDecoder extends a C ndn_BinaryXMLDecoder struct and wraps related functions.
 */
class BinaryXMLDecoder : public ndn_BinaryXMLDecoder {
public:
  /**
   * Initialize the base ndn_BinaryXMLDecoder struct with the input.
   */
  BinaryXMLDecoder(const unsigned char *input, unsigned int inputLength) 
  {
    ndn_BinaryXMLDecoder_init(this, (unsigned char *)input, inputLength);
  }
  
  /**
   * Decode the header from the input starting at offset, and if it is a DTAG where the value is the expectedTag,
   * then return true, else false.  Do not update offset, including if throwing an exception.
   * @param expectedTag the expected value for DTAG
   * @return true if got the expected tag, else false
   */
  bool peekDTag(unsigned int expectedTag) 
  {
    int gotExpectedTag;
    ndn_Error error;
    if (error = ndn_BinaryXMLDecoder_peekDTag(this, expectedTag, &gotExpectedTag))
      throw std::runtime_error(ndn_getErrorString(error));
    
    return gotExpectedTag;
  }
};

}

#endif
