/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FACE_HPP
#define NDN_FACE_HPP

#include "node.hpp"
#include "transport/tcp-transport.hpp"

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
  Face(const ptr_lib::shared_ptr<Transport>& transport, const ptr_lib::shared_ptr<const Transport::ConnectionInfo>& connectionInfo)
  : node_(transport, connectionInfo)
  {
  }
  
  /**
   * Create a new Face for communication with an NDN hub at host:port using the default TcpTransport.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub. If omitted. use 9695.
   */
  Face(const char *host, unsigned short port = 9695)
  : node_(ptr_lib::make_shared<TcpTransport>(), 
          ptr_lib::make_shared<TcpTransport::ConnectionInfo>(host, port))
  {
  }
    
  /**
   * Send the Interest through the transport, read the entire response and call onData(interest, data).
   * @param interest A reference to the Interest.  This copies the Interest.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   */
  void 
  expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout = OnTimeout())
  {
    node_.expressInterest(interest, onData, onTimeout);
  }

  /**
   * Encode name as an Interest. If interestTemplate is not 0, use its interest selectors.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This copies the Name.
   * @param interestTemplate if not 0, copy interest selectors from the template.   This does not keep a pointer to the Interest object.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   */
  void 
  expressInterest(const Name& name, const Interest *interestTemplate, const OnData& onData, const OnTimeout& onTimeout = OnTimeout());

  /**
   * Encode name as an Interest, using a default interest lifetime.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This copies the Name.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   */
  void 
  expressInterest(const Name& name, const OnData& onData, const OnTimeout& onTimeout = OnTimeout()) 
  {
    expressInterest(name, 0, onData, onTimeout);
  }
  
  /**
   * Register prefix with the connected NDN hub and call onInterest when a matching interest is received.
   * @param prefix A reference to a Name for the prefix to register.  This copies the Name.
   * @param onInterest A function object to call when a matching interest is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onRegisterFailed A function object to call if failed to retrieve the connected hub’s ID or failed to register the prefix.
   * This calls onRegisterFailed(prefix) where prefix is the prefix given to registerPrefix.
   * @param flags The flags for finer control of which interests are forward to the application.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  registerPrefix
    (const Name& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, int flags = 0, 
     WireFormat& wireFormat = *WireFormat::getDefaultWireFormat())
  {
    node_.registerPrefix(prefix, onInterest, onRegisterFailed, flags, wireFormat);
  }
  
  /**
   * Process any data to receive or call timeout callbacks.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should repeatedly call this from an event loop, with calls to sleep as needed so that the loop doesn't use 100% of the CPU.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void 
  processEvents()
  {
    // Just call Node's processEvents.
    node_.processEvents();
  }

  /**
   * Shut down and disconnect this Face.
   */
  void 
  shutdown();
  
private:
  Node node_;
};

}

#endif
