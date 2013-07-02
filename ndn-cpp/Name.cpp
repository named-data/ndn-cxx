/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include <sstream>
#include "c/Name.h"
#include "Name.hpp"

using namespace std;

namespace ndn {

Name::Name() 
{
}
  
void Name::set(struct ndn_Name &nameStruct) 
{
  clear();
  for (unsigned int i = 0; i < nameStruct.nComponents; ++i)
    addComponent(nameStruct.components[i].value, nameStruct.components[i].valueLength);  
}

void Name::get(struct ndn_Name &nameStruct) 
{
  if (nameStruct.maxComponents < components_.size())
    throw runtime_error("nameStruct.maxComponents must be >= this name getNComponents()");
  
  nameStruct.nComponents = components_.size();
  for (unsigned int i = 0; i < nameStruct.nComponents; ++i) {
    nameStruct.components[i].value = &components_[i][0];
    nameStruct.components[i].valueLength = components_[i].size();
  }  
}

std::string Name::to_uri()
{
  // TODO: implement fully.
  ostringstream output;
  for (unsigned int i = 0; i < components_.size(); ++i) {
    output << "/";
    for (unsigned int j = 0; j < components_[i].size(); ++j)
      output << components_[i][j];
  }
  
  return output.str();
}

}
