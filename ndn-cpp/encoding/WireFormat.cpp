/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#include <stdexcept>
#include "WireFormat.hpp"

using namespace std;

namespace ndn {
  
void WireFormat::encodeName(Name &name, vector<unsigned char> &output) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeName(Name &name, vector<unsigned char> &input) 
{
  throw logic_error("unimplemented");
}

void WireFormat::encodeInterest(Interest &interest, vector<unsigned char> &output) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeInterest(Interest &interest, vector<unsigned char> &input) 
{
  throw logic_error("unimplemented");
}

}
