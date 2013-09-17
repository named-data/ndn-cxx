/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_HPP
#define NDN_TRANSPORT_HPP

#include <vector>

namespace ndn {

class ElementListener;

class Transport {
public:
  /**
   * A Transport::ConnectionInfo is a base class for connection information used by subclasses of Transport.
   */
  class ConnectionInfo { 
  public:
    virtual ~ConnectionInfo();
  };
  
  /**
   * Connect according to the info in ConnectionInfo, and processEvents() will use elementListener.
   * @param connectionInfo A reference to an object of a subclass of ConnectionInfo.
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
  send(const unsigned char *data, unsigned int dataLength);
  
  void 
  send(const std::vector<unsigned char>& data)
  {
    send(&data[0], data.size());
  }
  
  /**
   * Process any data to receive.  For each element received, call elementListener.onReceivedElement.
   * This is non-blocking and will silently time out after a brief period if there is no data to receive.
   * You should repeatedly call this from an event loop.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  virtual void 
  processEvents() = 0;

  virtual bool 
  getIsConnected();
  
  /**
   * Close the connection.  This base class implementation does nothing, but your derived class can override.
   */
  virtual void 
  close();
  
  virtual ~Transport();
};

}

#endif
