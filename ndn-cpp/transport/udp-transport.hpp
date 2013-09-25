/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UDPTRANSPORT_HPP
#define NDN_UDPTRANSPORT_HPP

#include <string>
#include "../c/transport/udp-transport.h"
#include "../c/encoding/binary-xml-element-reader.h"
#include "transport.hpp"

namespace ndn {
  
class UdpTransport : public Transport {
public:
  /**
   * A UdpTransport::ConnectionInfo extends Transport::ConnectionInfo to hold the host and port info for the UDP connection.
   */
  class ConnectionInfo : public Transport::ConnectionInfo {
  public:
    /**
     * Create a ConnectionInfo with the given host and port.
     * @param host The host for the connection.
     * @param port The port number for the connection. If omitted, use 9695.
     */
    ConnectionInfo(const char *host, unsigned short port = 9695)
    : host_(host), port_(port)
    {
    }

    /**
     * Get the host given to the constructor.
     * @return A string reference for the host.
     */
    const std::string& 
    getHost() const { return host_; }
    
    /**
     * Get the port given to the constructor.
     * @return The port number.
     */
    unsigned short 
    getPort() const { return port_; }
    
    virtual 
    ~ConnectionInfo();

  private:
    std::string host_;
    unsigned short port_;
  };

  UdpTransport() 
  : elementListener_(0), isConnected_(false)
  {
    ndn_UdpTransport_initialize(&transport_);
    elementReader_.partialData.array = 0;
  }
  
  /**
   * Connect according to the info in ConnectionInfo, and processEvents() will use elementListener.
   * @param connectionInfo A reference to a TcpTransport::ConnectionInfo.
   * @param elementListener Not a shared_ptr because we assume that it will remain valid during the life of this object.
   */
  virtual void 
  connect(const Transport::ConnectionInfo& connectionInfo, ElementListener& elementListener);
  
  /**
   * Set data to the host
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
   */
  virtual void 
  send(const uint8_t *data, size_t dataLength);

  /**
   * Process any data to receive.  For each element received, call elementListener.onReceivedElement.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should normally not call this directly since it is called by Face.processEvents.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  virtual void 
  processEvents();
  
  virtual bool 
  getIsConnected();

  /**
   * Close the connection to the host.
   */
  virtual void 
  close();

  ~UdpTransport();
  
private:
  struct ndn_UdpTransport transport_;
  bool isConnected_;
  ElementListener *elementListener_;
  // TODO: This belongs in the socket listener.
  ndn_BinaryXmlElementReader elementReader_;
};

}

#endif
