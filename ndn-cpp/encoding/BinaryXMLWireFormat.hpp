/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */


#ifndef NDN_BINARYXMLWIREFORMAT_HPP
#define	NDN_BINARYXMLWIREFORMAT_HPP

#include "WireFormat.hpp"

namespace ndn {

class BinaryXMLWireFormat : public WireFormat {
public:
  virtual void encodeName(Name &name, std::vector<unsigned char> &output);
  virtual void decodeName(Name &name, const unsigned char *input, unsigned int inputLength);

  virtual void encodeInterest(Interest &interest, std::vector<unsigned char> &output);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);
  
  static BinaryXMLWireFormat &instance() { return instance_; }
  
private:
  static BinaryXMLWireFormat instance_;
};
  
}

#endif

