/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include <sstream>
#include "Name.hpp"

using namespace std;

namespace ndn {

/**
 * Write the value to result, escaping characters according to the NDN URI Scheme.
 * This also adds "..." to a value with zero or more ".".
 * @param value the buffer with the value to escape
 * @param result the string stream to write to.
 */
static void toEscapedString(const vector<unsigned char> &value, ostringstream &result)
{
  bool gotNonDot = false;
  for (unsigned i = 0; i < value.size(); ++i) {
    if (value[i] != 0x2e) {
      gotNonDot = true;
      break;
    }
  }
  if (!gotNonDot) {
    // Special case for component of zero or more periods.  Add 3 periods.
    result << "...";
    for (unsigned int i = 0; i < value.size(); ++i)
      result << ".";
  }
  else {
    // In case we need to escape, set to upper case hex and save the previous flags.
    ios::fmtflags saveFlags = result.flags(ios::hex | ios::uppercase);
    
    for (unsigned int i = 0; i < value.size(); ++i) {
      unsigned char x = value[i];
      // Check for 0-9, A-Z, a-z, (+), (-), (.), (_)
      if (x >= 0x30 && x <= 0x39 || x >= 0x41 && x <= 0x5a ||
        x >= 0x61 && x <= 0x7a || x == 0x2b || x == 0x2d || 
        x == 0x2e || x == 0x5f)
        result << x;
      else {
        result << "%";
        if (x < 16)
          result << "0";
        result << (unsigned int)x;
      }
    }
    
    // Restore.
    result.flags(saveFlags);
  }  
}
  
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
  if (components_.size() == 0)
    return "/";
  
  ostringstream result;
  for (unsigned int i = 0; i < components_.size(); ++i) {
    result << "/";
    toEscapedString(components_[i].getValue(), result);
  }
  
  return result.str();
}

}
