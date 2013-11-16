/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <stdlib.h>
#include <ndn-cpp/face.hpp>
#include "../c/transport/udp-transport.h"
#include "../c/encoding/binary-xml-element-reader.h"
#include "../c/util/ndn_realloc.h"
#include <ndn-cpp/transport/udp-transport.hpp>

using namespace std;

namespace ndn {

UdpTransport::ConnectionInfo::~ConnectionInfo()
{  
}

UdpTransport::UdpTransport() 
  : isConnected_(false), transport_(new struct ndn_UdpTransport), elementReader_(new struct ndn_BinaryXmlElementReader)
{
  ndn_UdpTransport_initialize(transport_.get());
  elementReader_->partialData.array = 0;
}

void 
UdpTransport::connect(const Transport::ConnectionInfo& connectionInfo, ElementListener& elementListener)
{
  const UdpTransport::ConnectionInfo& udpConnectionInfo = dynamic_cast<const UdpTransport::ConnectionInfo&>(connectionInfo);
  
  ndn_Error error;
  if ((error = ndn_UdpTransport_connect(transport_.get(), (char *)udpConnectionInfo.getHost().c_str(), udpConnectionInfo.getPort())))
    throw runtime_error(ndn_getErrorString(error)); 

  // TODO: This belongs in the socket listener.
  const size_t initialLength = 1000;
  // Automatically cast elementReader_ to (struct ndn_ElementListener *)
  ndn_BinaryXmlElementReader_initialize
    (elementReader_.get(), &elementListener, (uint8_t *)malloc(initialLength), initialLength, ndn_realloc);
  
  isConnected_ = true;
}

void 
UdpTransport::send(const uint8_t *data, size_t dataLength)
{
  ndn_Error error;
  if ((error = ndn_UdpTransport_send(transport_.get(), (uint8_t *)data, dataLength)))
    throw runtime_error(ndn_getErrorString(error));  
}

void 
UdpTransport::processEvents()
{
  int receiveIsReady;
  ndn_Error error;
  if ((error = ndn_UdpTransport_receiveIsReady(transport_.get(), &receiveIsReady)))
    throw runtime_error(ndn_getErrorString(error));  
  if (!receiveIsReady)
    return;

  uint8_t buffer[8000];
  size_t nBytes;
  if ((error = ndn_UdpTransport_receive(transport_.get(), buffer, sizeof(buffer), &nBytes)))
    throw runtime_error(ndn_getErrorString(error));  

  ndn_BinaryXmlElementReader_onReceivedData(elementReader_.get(), buffer, nBytes);
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
  if ((error = ndn_UdpTransport_close(transport_.get())))
    throw runtime_error(ndn_getErrorString(error));  
}

UdpTransport::~UdpTransport()
{
  if (elementReader_->partialData.array)
    // Free the memory allocated in connect.
    free(elementReader_->partialData.array);
}

}
