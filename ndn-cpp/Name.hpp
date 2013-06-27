/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_NAME_HPP
#define	NDN_NAME_HPP

#include <vector>
#include "common.h"
#include "encoding/BinaryXMLWireFormat.hpp"

namespace ndn {
  
class Name {
public:
  Name();
  Name(const char *uri);
  
  void encode(std::vector<unsigned char> &output, WireFormat &wireFormat) {
    wireFormat.encodeName(*this, output);
  }
  void encode(std::vector<unsigned char> &output) {
    encode(output, BinaryXMLWireFormat::instance());
  }
  void decode(std::vector<unsigned char> &input, WireFormat &wireFormat) {
    wireFormat.decodeName(*this, input);
  }
  void decode(std::vector<unsigned char> &input) {
    decode(input, BinaryXMLWireFormat::instance());
  }
  
private:
  std::vector<std::vector<unsigned char> > components_;
};  

}

#endif

