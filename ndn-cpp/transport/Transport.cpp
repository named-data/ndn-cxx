/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "Transport.hpp"

using namespace std;

namespace ndn {

void Transport::connect(NDN &ndn) 
{
  throw logic_error("unimplemented");
}
  
void Transport::send(unsigned char *data, unsigned int dataLength)
{
  throw logic_error("unimplemented");
}

}
