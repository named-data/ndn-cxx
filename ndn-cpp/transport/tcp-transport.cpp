/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../face.hpp"
#include "../c/util/ndn_realloc.h"
#include "tcp-transport.hpp"

using namespace std;

namespace ndn {

void TcpTransport::connect(Face &face)
{
  ndn_Error error;
  if (error = ndn_TcpTransport_connect(&transport_, (char *)face.getHost(), face.getPort()))
    throw std::runtime_error(ndn_getErrorString(error)); 

  // TODO: This belongs in the socket listener.
  const unsigned int initialLength = 1000;
  // Automatically cast ndn_ to (struct ndn_ElementListener *)
  ndn_BinaryXmlElementReader_init
    (&elementReader_, &face, (unsigned char *)malloc(initialLength), initialLength, ndn_realloc);
  
  // TODO: Properly indicate connected status.
  face_ = &face;
}

void TcpTransport::send(const unsigned char *data, unsigned int dataLength)
{
  ndn_Error error;
  if (error = ndn_TcpTransport_send(&transport_, (unsigned char *)data, dataLength))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void TcpTransport::tempReceive()
{
  try {   
    ndn_Error error;
    unsigned char buffer[8000];
    unsigned int nBytes;
    if (error = ndn_TcpTransport_receive(&transport_, buffer, sizeof(buffer), &nBytes))
      throw std::runtime_error(ndn_getErrorString(error));  

    ndn_BinaryXmlElementReader_onReceivedData(&elementReader_, buffer, nBytes);
  } catch (...) {
    // This function is called by the socket callback, so don't send an exception back to it.
    // TODO: Log the exception?
  }
}

void TcpTransport::close()
{
  ndn_Error error;
  if (error = ndn_TcpTransport_close(&transport_))
    throw std::runtime_error(ndn_getErrorString(error));  
}

}
