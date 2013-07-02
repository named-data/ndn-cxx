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

extern "C" { struct ndn_Name; }

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
   * Clear the name, and set the components by copying from the name struct.
   * @param name a C ndn_Name struct
   */
  void set(struct ndn_Name &nameStruct);
  
  /**
   * Set the nameStruct to point to the components in this name, without copying any memory.
   * WARNING: The resulting pointers in nameStruct are invalid after a further use of this name which could reallocate the components memory.
   * @param nameStruct a C ndn_Name struct where the components array is already allocated.
   */
  void get(struct ndn_Name &nameStruct);
  
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

