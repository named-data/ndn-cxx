/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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
   * Connect according to the info in ConnectionInfo, and processEvents() will use elementListener.
   * @param connectionInfo A reference to an object of a subclass of ConnectionInfo.
   * @param elementListener Not a shared_ptr because we assume that it will remain valid during the life of this object.
   */
  virtual void 
  connect(ElementListener& elementListener) = 0;
  
  /**
   * Set data to the host
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
   */
  virtual void 
  send(const uint8_t *data, size_t dataLength) = 0;
  
  inline void 
  send(const std::vector<uint8_t>& data)
  {
    send(&data[0], data.size());
  }
  
  /**
   * Process any data to receive.  For each element received, call elementListener.onReceivedElement.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should normally not call this directly since it is called by Face.processEvents.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  virtual void 
  processEvents() = 0;

  virtual bool 
  getIsConnected() = 0;
  
  /**
   * Close the connection.  This base class implementation does nothing, but your derived class can override.
   */
  virtual void 
  close();
  
  virtual ~Transport();
};

}

#endif
