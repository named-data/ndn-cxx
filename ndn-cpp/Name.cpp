/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include <sstream>
#include "Name.hpp"

using namespace std;

namespace ndn {

void Name::get(struct ndn_Name &nameStruct) 
{
  if (nameStruct.maxComponents < components_.size())
    throw runtime_error("nameStruct.maxComponents must be >= this name getNComponents()");
  
  nameStruct.nComponents = components_.size();
  for (unsigned int i = 0; i < nameStruct.nComponents; ++i)
    components_[i].get(nameStruct.components[i]);
}
  
void Name::set(struct ndn_Name &nameStruct) 
{
  clear();
  for (unsigned int i = 0; i < nameStruct.nComponents; ++i)
    addComponent(nameStruct.components[i].value, nameStruct.components[i].valueLength);  
}

std::string Name::to_uri()
{
  // TODO: implement fully.
  ostringstream output;
  for (unsigned int i = 0; i < components_.size(); ++i) {
    output << "/";
    for (unsigned int j = 0; j < components_[i].getValue().size(); ++j)
      output << components_[i].getValue()[j];
  }
  
  return output.str();
}

}
