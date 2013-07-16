/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../NDN.hpp"
#include "../c/encoding/BinaryXMLElementReader.h"
#include "TcpTransport.hpp"

using namespace std;

namespace ndn {

void TcpTransport::connect(NDN &ndn)
{
  ndn_Error error;
  if (error = ndn_TcpTransport_connect(&transport_, (char *)ndn.getHost(), ndn.getPort()))
    throw std::runtime_error(ndn_getErrorString(error)); 
  
  // TODO: Properly indicate connected status.
  ndn_ = &ndn;
}

void TcpTransport::send(unsigned char *data, unsigned int dataLength)
{
  ndn_Error error;
  if (error = ndn_TcpTransport_send(&transport_, data, dataLength))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void TcpTransport::tempReceive()
{
  if (!ndn_)
    // TODO: Properly check if connected.
    return;
  ndn_BinaryXMLElementReader elementReader;
  // Automaticall cast ndn_ to (struct ndn_ElementListener *)
  ndn_BinaryXMLElementReader_init(&elementReader, ndn_);
    
  unsigned char buffer[8000];
  ndn_Error error;
  unsigned int nBytes;
  if (error = ndn_TcpTransport_receive(&transport_, buffer, sizeof(buffer), &nBytes))
    throw std::runtime_error(ndn_getErrorString(error));  
  ndn_BinaryXMLElementReader_onReceivedData(&elementReader, buffer, nBytes);      
}

}
