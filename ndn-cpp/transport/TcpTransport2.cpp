/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "TcpTransport.hpp"

using namespace std;

namespace ndn {

void TcpTransport::connect(char *host, unsigned short port)
{
  ndn_Error error;
  if (error = ndn_TcpTransport_connect(&transport_, host, port))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void TcpTransport::send(unsigned char *data, unsigned int dataLength)
{
  ndn_Error error;
  if (error = ndn_TcpTransport_send(&transport_, data, dataLength))
    throw std::runtime_error(ndn_getErrorString(error));  
}

unsigned int TcpTransport::receive(unsigned char *buffer, unsigned int bufferLength)
{
  ndn_Error error;
  unsigned int nBytes;
  if (error = ndn_TcpTransport_receive(&transport_, buffer, bufferLength, &nBytes))
    throw std::runtime_error(ndn_getErrorString(error));  
  
  return nBytes;
}

}
