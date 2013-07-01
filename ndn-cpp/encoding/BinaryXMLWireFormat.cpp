/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include <stdexcept>
#include "../c/encoding/BinaryXMLName.h"
#include "../Name.hpp"
#include "BinaryXMLWireFormat.hpp"

namespace ndn {

BinaryXMLWireFormat BinaryXMLWireFormat::instance_;

void BinaryXMLWireFormat::decodeName(Name &name, const unsigned char *input, unsigned int inputLength)
{
  struct ndn_NameComponent components[100];
  struct ndn_Name nameStruct;
  ndn_Name_init(&nameStruct, components, sizeof(components) / sizeof(components[0]));
    
  char *error;
  if (error = ndn_decodeBinaryXMLName(&nameStruct, (unsigned char *)input, inputLength))
    throw std::runtime_error(error);
  
  name.clear();
  for (int i = 0; i < nameStruct.nComponents; ++i)
    name.addComponent(nameStruct.components[i].value, nameStruct.components[i].valueLength);
}

}
