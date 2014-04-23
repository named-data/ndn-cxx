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

UnixTransport::UnixTransport(const std::string& unixSocket)
  : m_unixSocket(unixSocket)
{
}

UnixTransport::~UnixTransport()
{
}

std::string
UnixTransport::getDefaultSocketName(const ConfigFile& config)
{
  const ConfigFile::Parsed& parsed = config.getParsedConfiguration();
  try
    {
      return parsed.get<std::string>("unix_socket");
    }
  catch (boost::property_tree::ptree_bad_path& error)
    {
      // unix_socket not present, continue
    }
  catch (boost::property_tree::ptree_bad_data& error)
    {
      throw ConfigFile::Error(error.what());
    }

  // no unix_socket specified so the default socket name
  // depends on the protocol we're using
  try
    {
      const std::string protocol = parsed.get<std::string>("protocol");
      if (protocol == "ndnd-tlv-0.7")
        {
          return "/tmp/.ndnd.sock";
        }
    }
  catch (boost::property_tree::ptree_bad_path& error)
    {
      return "/var/run/nfd.sock";
    }
  catch (boost::property_tree::ptree_bad_data& error)
    {
      throw ConfigFile::Error(error.what());
    }

  // A we made here, then there's no unix_socket specified in the configuration
  // file. A protocol is present, but it's not ndnd.
  // Assume the default nfd.sock location.
  return "/var/run/nfd.sock";
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
