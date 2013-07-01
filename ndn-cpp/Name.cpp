/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include <sstream>
#include "Name.hpp"

using namespace std;

namespace ndn {

Name::Name() 
{
}
  
std::string Name::to_uri()
{
  // TODO: implement fully.
  ostringstream output;
  for (int i = 0; i < components_.size(); ++i) {
    output << "/";
    for (int j = 0; j < components_[i].size(); ++j)
      output << components_[i][j];
  }
  
  return output.str();
}

}
