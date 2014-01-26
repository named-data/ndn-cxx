/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TCP_TRANSPORT_HPP
#define NDN_TCP_TRANSPORT_HPP

#include <string>
#include "transport.hpp"

namespace ndn {
  
class TcpTransport : public Transport
{
public:
  TcpTransport(const std::string& host, const std::string& port = "6363");
  ~TcpTransport();

  // from Transport
  virtual void 
  connect(boost::asio::io_service &ioService,
          const ReceiveCallback &receiveCallback);
  
  virtual void 
  close();

  virtual void 
  send(const Block &wire);
  
private:
  std::string host_;
  std::string port_;

  class Impl;
  ptr_lib::shared_ptr<Impl> impl_;
};

}

#endif // NDN_TCP_TRANSPORT_HPP
