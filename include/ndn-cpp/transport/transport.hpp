/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_HPP
#define NDN_TRANSPORT_HPP

#include <ndn-cpp/common.hpp>

#include <vector>
#include <boost/asio.hpp>

namespace ndn {

class Transport {
public:
  typedef ptr_lib::function<void (const Block &wire)> ReceiveCallback;
  
  inline
  Transport();
  
  inline virtual
  ~Transport();

  /**
   * Connect according to the info in ConnectionInfo, and processEvents() will use elementListener.
   * @param connectionInfo A reference to an object of a subclass of ConnectionInfo.
   */
  inline virtual void 
  connect(boost::asio::io_service &io_service, const ReceiveCallback &receiveCallback);
  
  /**
   * Close the connection.
   */
  virtual void 
  close() = 0;

  /**
   * Set data to the host
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
   */
  virtual void 
  send(const Block &wire) = 0;

  inline bool 
  isConnected();

protected:
  inline void
  receive(const Block &wire);
  
protected:
  boost::asio::io_service *ioService_;
  bool isConnected_;
  ReceiveCallback receiveCallback_;
};

inline
Transport::Transport()
  : ioService_(0)
  , isConnected_(false)
{
}

inline
Transport::~Transport()
{
}

inline void 
Transport::connect(boost::asio::io_service &ioService, const ReceiveCallback &receiveCallback)
{
  ioService_ = &ioService;
  receiveCallback_ = receiveCallback;
}

inline bool 
Transport::isConnected()
{
  return isConnected_;
}

inline void
Transport::receive(const Block &wire)
{
  receiveCallback_(wire);
}

}

#endif
