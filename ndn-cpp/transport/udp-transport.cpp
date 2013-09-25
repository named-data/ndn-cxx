/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../face.hpp"
#include "../c/util/ndn_realloc.h"
#include "udp-transport.hpp"

using namespace std;

namespace ndn {

UdpTransport::ConnectionInfo::~ConnectionInfo()
{  
}

void 
UdpTransport::connect(const Transport::ConnectionInfo& connectionInfo, ElementListener& elementListener)
{
  const UdpTransport::ConnectionInfo& udpConnectionInfo = dynamic_cast<const UdpTransport::ConnectionInfo&>(connectionInfo);
  
  ndn_Error error;
  if ((error = ndn_UdpTransport_connect(&transport_, (char *)udpConnectionInfo.getHost().c_str(), udpConnectionInfo.getPort())))
    throw std::runtime_error(ndn_getErrorString(error)); 

  // TODO: This belongs in the socket listener.
  const size_t initialLength = 1000;
  // Automatically cast elementReader_ to (struct ndn_ElementListener *)
  ndn_BinaryXmlElementReader_initialize
    (&elementReader_, &elementListener, (uint8_t *)malloc(initialLength), initialLength, ndn_realloc);
  
  isConnected_ = true;
  elementListener_ = &elementListener;
}

void 
UdpTransport::send(const uint8_t *data, size_t dataLength)
{
  ndn_Error error;
  if ((error = ndn_UdpTransport_send(&transport_, (uint8_t *)data, dataLength)))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void 
UdpTransport::processEvents()
{
  int receiveIsReady;
  ndn_Error error;
  if ((error = ndn_UdpTransport_receiveIsReady(&transport_, &receiveIsReady)))
    throw std::runtime_error(ndn_getErrorString(error));  
  if (!receiveIsReady)
    return;

  uint8_t buffer[8000];
  size_t nBytes;
  if ((error = ndn_UdpTransport_receive(&transport_, buffer, sizeof(buffer), &nBytes)))
    throw std::runtime_error(ndn_getErrorString(error));  

  ndn_BinaryXmlElementReader_onReceivedData(&elementReader_, buffer, nBytes);
}

bool 
UdpTransport::getIsConnected()
{
  return isConnected_;
}

void 
UdpTransport::close()
{
  ndn_Error error;
  if ((error = ndn_UdpTransport_close(&transport_)))
    throw std::runtime_error(ndn_getErrorString(error));  
}

UdpTransport::~UdpTransport()
{
  if (elementReader_.partialData.array)
    // Free the memory allocated in connect.
    free(elementReader_.partialData.array);
}

}
