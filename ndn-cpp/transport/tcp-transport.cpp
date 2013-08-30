/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "../node.hpp"
#include "../c/util/ndn_realloc.h"
#include "tcp-transport.hpp"

using namespace std;

namespace ndn {

TcpTransport::ConnectionInfo::~ConnectionInfo()
{  
}

void TcpTransport::connect(const Transport::ConnectionInfo& connectionInfo, ElementListener& elementListener)
{
  const TcpTransport::ConnectionInfo& tcpConnectionInfo = dynamic_cast<const TcpTransport::ConnectionInfo&>(connectionInfo);
  
  ndn_Error error;
  if ((error = ndn_TcpTransport_connect(&transport_, (char *)tcpConnectionInfo.getHost().c_str(), tcpConnectionInfo.getPort())))
    throw std::runtime_error(ndn_getErrorString(error)); 

  // TODO: This belongs in the socket listener.
  const unsigned int initialLength = 1000;
  // Automatically cast elementReader_ to (struct ndn_ElementListener *)
  ndn_BinaryXmlElementReader_initialize
    (&elementReader_, &elementListener, (unsigned char *)malloc(initialLength), initialLength, ndn_realloc);
  
  isConnected_ = true;
  elementListener_ = &elementListener;
}

void TcpTransport::send(const unsigned char *data, unsigned int dataLength)
{
  ndn_Error error;
  if ((error = ndn_TcpTransport_send(&transport_, (unsigned char *)data, dataLength)))
    throw std::runtime_error(ndn_getErrorString(error));  
}

void TcpTransport::processEvents()
{
  int receiveIsReady;
  ndn_Error error;
  if ((error = ndn_TcpTransport_receiveIsReady(&transport_, &receiveIsReady)))
    throw std::runtime_error(ndn_getErrorString(error));  
  if (!receiveIsReady)
    return;

  unsigned char buffer[8000];
  unsigned int nBytes;
  if ((error = ndn_TcpTransport_receive(&transport_, buffer, sizeof(buffer), &nBytes)))
    throw std::runtime_error(ndn_getErrorString(error));  

  ndn_BinaryXmlElementReader_onReceivedData(&elementReader_, buffer, nBytes);
}

bool TcpTransport::getIsConnected()
{
  return isConnected_;
}

void TcpTransport::close()
{
  ndn_Error error;
  if ((error = ndn_TcpTransport_close(&transport_)))
    throw std::runtime_error(ndn_getErrorString(error));  
}

TcpTransport::~TcpTransport()
{
  if (elementReader_.partialData.array)
    // Free the memory allocated in connect.
    free(elementReader_.partialData.array);
}

}
