/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UDPTRANSPORT_HPP
#define NDN_UDPTRANSPORT_HPP

#include "../c/transport/udp-transport.h"
#include "../c/encoding/binary-xml-element-reader.h"
#include "transport.hpp"

namespace ndn {
  
class UdpTransport : public Transport {
public:
  UdpTransport() 
  : node_(0), isConnected_(false)
  {
    ndn_UdpTransport_init(&transport_);
    elementReader_.partialData.array = 0;
  }
  
  /**
   * Connect to the host specified in node.
   * @param node Not a shared_ptr because we assume that it will remain valid during the life of this Transport object.
   */
  virtual void connect(Node &node);
  
  /**
   * Set data to the host
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
   */
  virtual void send(const unsigned char *data, unsigned int dataLength);

  /**
   * Process any data to receive.  For each element received, call node.onReceivedElement.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should normally not call this directly since it is called by Face.processEvents.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  virtual void processEvents();
  
  virtual bool getIsConnected();

  /**
   * Close the connection to the host.
   */
  virtual void close();

  ~UdpTransport();
  
private:
  struct ndn_UdpTransport transport_;
  bool isConnected_;
  Node *node_;
  // TODO: This belongs in the socket listener.
  ndn_BinaryXmlElementReader elementReader_;
};

}

#endif
