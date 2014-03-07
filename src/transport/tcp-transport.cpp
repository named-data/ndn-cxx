/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "tcp-transport.hpp"
#include "stream-transport.hpp"

namespace ndn {

TcpTransport::TcpTransport(const std::string& host, const std::string& port/* = "6363"*/)
  : m_host(host)
  , m_port(port)
{
}

TcpTransport::~TcpTransport()
{
}

void
TcpTransport::connect(boost::asio::io_service& ioService,
                      const ReceiveCallback& receiveCallback)
{
  if (!static_cast<bool>(m_impl)) {
    Transport::connect(ioService, receiveCallback);

    m_impl = make_shared<Impl> (boost::ref(*this),
                                boost::ref(ioService));
  }

  boost::asio::ip::tcp::resolver::query query(m_host, m_port);
  m_impl->connect(query);
}

void
TcpTransport::send(const Block& wire)
{
  m_impl->send(wire);
}

void
TcpTransport::send(const Block& header, const Block& payload)
{
  m_impl->send(header, payload);
}

void
TcpTransport::close()
{
  m_impl->close();
}

void
TcpTransport::pause()
{
  m_impl->pause();
}

void
TcpTransport::resume()
{
  m_impl->resume();
}

} // namespace ndn
