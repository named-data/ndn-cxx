/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FACE_HPP
#define NDN_FACE_HPP

#include "node.hpp"
#include "transport/transport.hpp"
#include "transport/unix-transport.hpp"

namespace ndn {

/**
 * The Face class provides the main methods for NDN communication.
 */
class Face {
public:
  /**
   * Create a new Face for communication with an NDN hub at host:port using the default TcpTransport.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub. If omitted. use 6363.
   */
  Face()
  : node_(ptr_lib::shared_ptr<UnixTransport>(new UnixTransport()))
  {
  }

  /**
   * Create a new Face for communication with an NDN hub with the given Transport object and connectionInfo.
   * @param transport A shared_ptr to a Transport object used for communication.
   * @param transport A shared_ptr to a Transport::ConnectionInfo to be used to connect to the transport.
   */
  Face(const ptr_lib::shared_ptr<Transport>& transport)
  : node_(transport)
  {
  }
  
  /**
   * Create a new Face for communication with an NDN hub at host:port using the default TcpTransport.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub. If omitted. use 6363.
   */
  // Face(const char *host, unsigned short port = 6363)
  // : node_(ptr_lib::shared_ptr<TcpTransport>(new TcpTransport(host, port)))
  // {
  // }
    
  /**
   * Send the Interest through the transport, read the entire response and call onData(interest, data).
   * @param interest A reference to the Interest.  This copies the Interest.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   * @param wireFormat A WireFormat object used to encode the message. If omitted, use WireFormat getDefaultWireFormat().
   * @return The pending interest ID which can be used with removePendingInterest.
   */
  uint64_t 
  expressInterest
    (const Interest& interest, const OnData& onData, const OnTimeout& onTimeout = OnTimeout())
  {
    return node_.expressInterest(interest, onData, onTimeout);
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
   * @param wireFormat A WireFormat object used to encode the message. If omitted, use WireFormat getDefaultWireFormat().
   * @return The pending interest ID which can be used with removePendingInterest.
   */
  uint64_t 
  expressInterest
    (const Name& name, const Interest *interestTemplate, const OnData& onData, const OnTimeout& onTimeout = OnTimeout());

  /**
   * Encode name as an Interest, using a default interest lifetime.
   * Send the interest through the transport, read the entire response and call onData(interest, data).
   * @param name A reference to a Name for the interest.  This copies the Name.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   * @param wireFormat A WireFormat object used to encode the message. If omitted, use WireFormat getDefaultWireFormat().
   * @return The pending interest ID which can be used with removePendingInterest.
   */
  uint64_t 
  expressInterest
    (const Name& name, const OnData& onData, const OnTimeout& onTimeout = OnTimeout()) 
  {
    return expressInterest(name, 0, onData, onTimeout);
  }

  /**
   * Remove the pending interest entry with the pendingInterestId from the pending interest table.
   * This does not affect another pending interest with a different pendingInterestId, even it if has the same interest name.
   * If there is no entry with the pendingInterestId, do nothing.
   * @param pendingInterestId The ID returned from expressInterest.
   */
  void
  removePendingInterest(uint64_t pendingInterestId)
  {
    node_.removePendingInterest(pendingInterestId);
  }
  
  /**
   * Register prefix with the connected NDN hub and call onInterest when a matching interest is received.
   * @param prefix A reference to a Name for the prefix to register.  This copies the Name.
   * @param onInterest A function object to call when a matching interest is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onRegisterFailed A function object to call if failed to retrieve the connected hub’s ID or failed to register the prefix.
   * This calls onRegisterFailed(prefix) where prefix is the prefix given to registerPrefix.
   * @param flags The flags for finer control of which interests are forward to the application.  If omitted, use 
   * the default flags defined by the default ForwardingFlags constructor.
   * @param wireFormat A WireFormat object used to encode the message. If omitted, use WireFormat getDefaultWireFormat().
   * @return The registered prefix ID which can be used with removeRegisteredPrefix.
   */
  uint64_t 
  setInterestFilter
    (const Name& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, const ForwardingFlags& flags = ForwardingFlags())
  {
    return node_.registerPrefix(prefix, onInterest, onRegisterFailed, flags);
  }

  /**
   * Remove the registered prefix entry with the registeredPrefixId from the pending interest table.  
   * This does not affect another registered prefix with a different registeredPrefixId, even it if has the same prefix name.
   * If there is no entry with the registeredPrefixId, do nothing.
   * @param registeredPrefixId The ID returned from registerPrefix.
   */
  void
  unsetInterestFilter(uint64_t registeredPrefixId)
  {
    node_.removeRegisteredPrefix(registeredPrefixId);
  }

  /**
   * @brief Publish data packet
   *
   * This method can be called to satisfy the incoming Interest or to put Data packet into the cache
   * of the local NDN forwarder
   */
  void
  put(const Data &data)
  {
    node_.put(data);
  }
  
  /**
   * Process any data to receive or call timeout callbacks.
   *
   * This call will block forever (default timeout == 0) to process IO on the face.
   * To exit, one expected to call face.shutdown() from one of the callback methods.
   *
   * If positive timeout is specified, then processEvents will exit after this timeout,
   * if not stopped earlier with face.shutdown() or when all active events finish.
   * The call can be called repeatedly, if desired.
   *
   * If negative timeout is specified, then processEvents will not block and process only pending
   * events.
   *
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void 
  processEvents(Milliseconds timeout = 0, bool keepThread = false)
  {
    // Just call Node's processEvents.
    node_.processEvents(timeout, keepThread);
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
