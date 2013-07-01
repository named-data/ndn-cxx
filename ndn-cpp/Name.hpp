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
  void decode(const unsigned char *input, unsigned int inputLength, WireFormat &wireFormat) {
    wireFormat.decodeName(*this, input, inputLength);
  }
  void decode(const unsigned char *input, unsigned int inputLength) {
    decode(input, inputLength, BinaryXMLWireFormat::instance());
  }
  
  /**
   * Add a new component, copying from value of length valueLength.
   */
  void addComponent(unsigned char *value, unsigned int valueLength) {
    components_.push_back(std::vector<unsigned char>(value, value + valueLength));
  }
  
  /**
   * Clear all the components.
   */
  void clear() {
    components_.clear();
  }
  
  /**
   * Get the number of components.
   * @return the number of components
   */
  unsigned int getComponentCount() {
    return components_.size();
  }
  
  /**
   * Encode this name as a URI.
   * @return the encoded URI.
   */
  std::string to_uri();
  
private:
  std::vector<std::vector<unsigned char> > components_;
};  

}

#endif

