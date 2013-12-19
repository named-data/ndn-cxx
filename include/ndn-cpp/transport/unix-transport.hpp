/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UDPTRANSPORT_HPP
#define NDN_UDPTRANSPORT_HPP

#include <string>
#include "transport.hpp"

namespace ndn {
  
class UnixTransport : public Transport {
public:
  UnixTransport(const std::string &unixSocket = "/tmp/.ndnd.sock");
  ~UnixTransport();

  /**
   * Connect according to the info in ConnectionInfo, and processEvents() will use elementListener.
   * @param connectionInfo A reference to a TcpTransport::ConnectionInfo.
   * @param elementListener Not a shared_ptr because we assume that it will remain valid during the life of this object.
   */
  virtual void 
  connect(ElementListener& elementListener);
  
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
  
private:
  std::string unixSocket_;
  bool isConnected_;

  class Impl;
  std::auto_ptr<Impl> impl_;
};

}

#endif
