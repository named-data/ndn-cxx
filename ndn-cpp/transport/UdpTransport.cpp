/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../NDN.hpp"
#include "../c/util/ndn_realloc.h"
#include "UdpTransport.hpp"

using namespace std;

namespace ndn {

void UdpTransport::connect(NDN &ndn)
{
  ndn_Error error;
  if (error = ndn_UdpTransport_connect(&transport_, (char *)ndn.getHost(), ndn.getPort()))
    throw std::runtime_error(ndn_getErrorString(error)); 

  // TODO: This belongs in the socket listener.
  const unsigned int initialLength = 1000;
  // Automatically cast ndn_ to (struct ndn_ElementListener *)
  ndn_BinaryXMLElementReader_init
    (&elementReader_, &ndn, (unsigned char *)malloc(initialLength), initialLength, ndn_realloc);
  
  // TODO: Properly indicate connected status.
  ndn_ = &ndn;
}

void UdpTransport::send(unsigned char *data, unsigned int dataLength)
{
  ndn_Error error;
  if (error = ndn_UdpTransport_send(&transport_, data, dataLength))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void UdpTransport::tempReceive()
{
  try {   
    ndn_Error error;
    unsigned char buffer[8000];
    unsigned int nBytes;
    if (error = ndn_UdpTransport_receive(&transport_, buffer, sizeof(buffer), &nBytes))
      throw std::runtime_error(ndn_getErrorString(error));  

    ndn_BinaryXMLElementReader_onReceivedData(&elementReader_, buffer, nBytes);
  } catch (...) {
    // This function is called by the socket callback, so don't send an exception back to it.
    // TODO: Log the exception?
  }
}

}
