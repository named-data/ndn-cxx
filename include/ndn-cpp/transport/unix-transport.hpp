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
  
class UnixTransport : public Transport
{
public:
  UnixTransport(const std::string &unixSocket = "/tmp/.ndnd.sock");
  ~UnixTransport();

  // from Transport
  virtual void 
  connect(boost::asio::io_service &ioService,
          const ReceiveCallback &receiveCallback);
  
  virtual void 
  close();

  virtual void 
  send(const Block &wire);
  
private:
  std::string unixSocket_;

  class Impl;
  ptr_lib::shared_ptr<Impl> impl_;
};

}

#endif
