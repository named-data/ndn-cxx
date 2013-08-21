/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_HPP
#define NDN_TRANSPORT_HPP

#include <vector>

namespace ndn {

class Node;  
class Transport {
public:
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
  
  void send(const std::vector<unsigned char> &data)
  {
    send(&data[0], data.size());
  }
  
  /**
   * Process any data to receive.  For each element received, call node.onReceivedElement.
   * This is non-blocking and will silently time out after a brief period if there is no data to receive.
   * You should repeatedly call this from an event loop.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  virtual void processEvents() = 0;

  /**
   * Close the connection.  This base class implementation does nothing, but your derived class can override.
   */
  virtual void close();
  
  virtual ~Transport();
};

}

#endif
