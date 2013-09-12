/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <sstream>
#include "common.hpp"

using namespace std;

namespace ndn {

string toHex(const vector<unsigned char>& array) 
{
  if (!&array)
    return "";
  
  ostringstream result;
  result.flags(ios::hex | ios::uppercase);
  for (unsigned int i = 0; i < array.size(); ++i) {
    unsigned char x = array[i];
    if (x < 16)
      result << '0';
    result << (unsigned int)x;
  }

  return result.str();
}

}

