/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "transport.hpp"

using namespace std;

namespace ndn {

void Transport::connect(Face &face) 
{
  throw logic_error("unimplemented");
}
  
void Transport::send(const unsigned char *data, unsigned int dataLength)
{
  throw logic_error("unimplemented");
}
 
void Transport::close()
{
}

}
