/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UDPTRANSPORT_HPP
#define NDN_UDPTRANSPORT_HPP

#include "../common.hpp"
#include "transport.hpp"

// forward declaration
namespace boost { namespace asio { namespace local { class stream_protocol; } } }

namespace ndn {

// forward declaration
template<class T, class U>
class StreamTransportImpl;

class UnixTransport : public Transport
{
public:
  UnixTransport();

  UnixTransport(const std::string& unixSocket);
  ~UnixTransport();

  // from Transport
  virtual void
  connect(boost::asio::io_service& ioService,
          const ReceiveCallback& receiveCallback);

  virtual void
  close();

  virtual void
  pause();

  virtual void
  resume();

  virtual void
  send(const Block& wire);

  virtual void
  send(const Block& header, const Block& payload);
  
private:
  std::string m_unixSocket;

  typedef StreamTransportImpl<UnixTransport, boost::asio::local::stream_protocol> Impl;
  friend class StreamTransportImpl<UnixTransport, boost::asio::local::stream_protocol>;
  ptr_lib::shared_ptr< Impl > m_impl;
};

}

#endif
