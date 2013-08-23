/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FACE_HPP
#define NDN_FACE_HPP

#include "node.hpp"

namespace ndn {

/**
 * The Face class provides the main methods for NDN communication.
 */
class Face {
public:
  /**
   * Create a new Face for communication with an NDN hub with the given Transport object and connectionInfo.
   * @param transport A shared_ptr to a Transport object used for communication.
   * @param transport A shared_ptr to a Transport::ConnectionInfo to be used to connect to the transport.
   */
  Face(const ptr_lib::shared_ptr<Transport> &transport, const ptr_lib::shared_ptr<const Transport::ConnectionInfo> &connectionInfo)
  : node_(transport, connectionInfo)
  {
  }
  
  /**
   * Create a new Face for communication with an NDN hub at host:port using the default UdpTransport.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub.
   */
  Face(const char *host, unsigned short port)
  : node_(host, port)
  {
  }
  
  /**
   * Create a new Face for communication with an NDN hub at host with the default port 9695 and using the default UdpTransport.
   * @param host The host of the NDN hub.
   */
  Face(const char *host)
  : node_(host)
  {
  }

  /**
   * Encode name as an Interest. If interestTemplate is not 0, use its interest selectors.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This does not keep a pointer to the Name object.
   * @param interestTemplate if not 0, copy interest selectors from the template.   This does not keep a pointer to the Interest object.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   */
  void expressInterest(const Name &name, const Interest *interestTemplate, const OnData &onData, const OnTimeout &onTimeout)
  {
    node_.expressInterest(name, interestTemplate, onData, onTimeout);
  }

  /**
   * Encode name as an Interest, using a default interest lifetime.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This does not keep a pointer to the Name object.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   */
  void expressInterest(const Name &name, const OnData &onData, const OnTimeout &onTimeout) 
  {
    node_.expressInterest(name, onData, onTimeout);
  }
  
  /**
   * Encode name as an Interest. If interestTemplate is not 0, use its interest selectors.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This does not keep a pointer to the Name object.
   * @param interestTemplate if not 0, copy interest selectors from the template.   This does not keep a pointer to the Interest object.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   */
  void expressInterest(const Name &name, const Interest *interestTemplate, const OnData &onData)
  {
    node_.expressInterest(name, interestTemplate, onData);
  }
  
  /**
   * Encode name as an Interest, using a default interest lifetime.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This does not keep a pointer to the Name object.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   */
  void expressInterest(const Name &name, const OnData &onData)
  {
    node_.expressInterest(name, onData);
  }
  
  /**
   * Process any data to receive or call timeout callbacks.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should repeatedly call this from an event loop, with calls to sleep as needed so that the loop doesn't use 100% of the CPU.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void processEvents()
  {
    // Just call Node's processEvents.
    node_.processEvents();
  }

  /**
   * Shut down and disconnect this Face.
   */
  void shutdown();
  
private:
  Node node_;
};

}

#endif
