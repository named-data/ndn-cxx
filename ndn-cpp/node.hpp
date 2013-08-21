/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NODE_HPP
#define NDN_NODE_HPP

#include "interest.hpp"
#include "closure.hpp"
#include "transport/udp-transport.hpp"
#include "encoding/binary-xml-element-reader.hpp"

namespace ndn {

class Face;
  
class Node : public ElementListener {
public:
  /**
   * Create a new Node for communication with an NDN hub at host:port with the given Transport object.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub.
   * @param transport A pointer to a Transport object used for communication.
   */
  Node(const char *host, unsigned short port, const ptr_lib::shared_ptr<Transport> &transport)
  : host_(host), port_(port), transport_(transport), tempClosure_(0)
  {
  }
  
  /**
   * Create a new Node for communication with an NDN hub at host:port using the default UdpTransport.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub.
   */
  Node(const char *host, unsigned short port)
  : host_(host), port_(port), transport_(new UdpTransport()), tempClosure_(0)
  {
  }
  
  /**
   * Create a new Node for communication with an NDN hub at host with the default port 9695 and using the default UdpTransport.
   * @param host The host of the NDN hub.
   */
  Node(const char *host)
  : host_(host), port_(9695), transport_(new UdpTransport()), tempClosure_(0)
  {
  }

  /**
   * Encode name as an Interest. If interestTemplate is not 0, use its interest selectors.
   * Send the interest through the transport, read the entire response and call
   * closure->upcall(UPCALL_DATA (or UPCALL_DATA_UNVERIFIED),
   *                 UpcallInfo(this, interest, 0, data)).
   * @param name reference to a Name for the interest.  This does not keep a pointer to the Name object.
   * @param closure a pointer for the Closure.  The caller must manage the memory for the Closure.  This will not try to delete it.
   * @param interestTemplate if not 0, copy interest selectors from the template.   This does not keep a pointer to the Interest object.
   */
  void expressInterest(const Name &name, Closure *closure, const Interest *interestTemplate);
  
  void expressInterest(const Name &name, Closure *closure)
  {
    expressInterest(name, closure, 0);
  }

  /**
   * Process any data to receive.  For each element received, call onReceivedElement.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should repeatedly call this from an event loop, with calls to sleep as needed so that the loop doesn't use 100% of the CPU.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void processEvents();
  
  const char *getHost() const { return host_.c_str(); }
  
  unsigned short getPort() const { return port_; }
  
  const ptr_lib::shared_ptr<Transport> &getTransport() { return transport_; }
  
  virtual void onReceivedElement(unsigned char *element, unsigned int elementLength);
  
  void shutdown();

private:
  ptr_lib::shared_ptr<Transport> transport_;
  std::string host_;
  unsigned short port_;
  Closure *tempClosure_;
};

}

#endif
