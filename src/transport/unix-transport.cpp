/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "unix-transport.hpp"
#include "stream-transport.hpp"

#include "../face.hpp"

namespace ndn {

UnixTransport::UnixTransport()
{
  if (std::getenv("NFD") != 0)
      m_unixSocket = "/var/run/nfd.sock";
  else
      m_unixSocket = "/tmp/.ndnd.sock";
}

UnixTransport::UnixTransport(const std::string& unixSocket)
  : m_unixSocket(unixSocket)
{
}

UnixTransport::~UnixTransport()
{
}

void
UnixTransport::connect(boost::asio::io_service& ioService,
                       const ReceiveCallback& receiveCallback)
{
  if (!static_cast<bool>(m_impl)) {
    Transport::connect(ioService, receiveCallback);

    m_impl = make_shared<Impl> (boost::ref(*this),
                                boost::ref(ioService));
  }

  m_impl->connect(boost::asio::local::stream_protocol::endpoint(m_unixSocket));
}

void
UnixTransport::send(const Block& wire)
{
  m_impl->send(wire);
}

void
UnixTransport::send(const Block& header, const Block& payload)
{
  m_impl->send(header, payload);
}

void
UnixTransport::close()
{
  m_impl->close();
}

void
UnixTransport::pause()
{
  m_impl->pause();
}

void
UnixTransport::resume()
{
  m_impl->resume();
}

}
