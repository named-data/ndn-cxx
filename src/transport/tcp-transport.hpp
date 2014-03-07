/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_TCP_TRANSPORT_HPP
#define NDN_TRANSPORT_TCP_TRANSPORT_HPP

#include "../common.hpp"
#include "transport.hpp"

// forward declaration
namespace boost { namespace asio { namespace ip { class tcp; } } }

namespace ndn {

// forward declaration
template<class T, class U> class StreamTransportImpl;
template<class T, class U> class StreamTransportWithResolverImpl;

class TcpTransport : public Transport
{
public:
  TcpTransport(const std::string& host, const std::string& port = "6363");
  ~TcpTransport();

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
  std::string m_host;
  std::string m_port;

  typedef StreamTransportWithResolverImpl<TcpTransport, boost::asio::ip::tcp> Impl;
  friend class StreamTransportImpl<TcpTransport, boost::asio::ip::tcp>;
  friend class StreamTransportWithResolverImpl<TcpTransport, boost::asio::ip::tcp>;
  shared_ptr< Impl > m_impl;
};

} // namespace ndn

#endif // NDN_TRANSPORT_TCP_TRANSPORT_HPP
