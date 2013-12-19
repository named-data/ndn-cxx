/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TCPTRANSPORT_HPP
#define NDN_TCPTRANSPORT_HPP

#include <string>
#include "../common.hpp"
#include "transport.hpp"

struct ndn_TcpTransport;
struct ndn_BinaryXmlElementReader;

namespace ndn {
  
class TcpTransport : public Transport {
public:
  TcpTransport(const char *host, unsigned short port = 6363);
  
  /**
   * Connect according to the info in ConnectionInfo, and processEvents() will use elementListener.
   * @param connectionInfo A reference to a TcpTransport::ConnectionInfo.
   * @param elementListener Not a shared_ptr because we assume that it will remain valid during the life of this object.
   */
  virtual void connect(ElementListener& elementListener);
  
  /**
   * Set data to the host
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
   */
  virtual void send(const uint8_t *data, size_t dataLength);

  /**
   * Process any data to receive.  For each element received, call elementListener.onReceivedElement.
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
  
  ~TcpTransport();
  
private:
  std::string host_;
  unsigned short port_;

  bool isConnected_;
  ptr_lib::shared_ptr<struct ndn_TcpTransport> transport_;
  // TODO: This belongs in the socket listener.
  ptr_lib::shared_ptr<struct ndn_BinaryXmlElementReader> elementReader_;
};

}

#endif
