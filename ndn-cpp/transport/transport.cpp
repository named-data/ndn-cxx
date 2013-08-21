/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "transport.hpp"

using namespace std;

namespace ndn {

void Transport::connect(Node &node) 
{
  throw logic_error("unimplemented");
}
  
void Transport::send(const unsigned char *data, unsigned int dataLength)
{
  throw logic_error("unimplemented");
}

void Transport::processEvents()
{
  throw logic_error("unimplemented");
}

void Transport::close()
{
}

Transport::~Transport()
{
}

}
