/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
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

    m_impl = make_shared<Impl>(ref(*this), ref(ioService));
  }

  m_impl->connect(boost::asio::local::stream_protocol::endpoint(m_unixSocket));
}

void
UnixTransport::send(const Block& wire)
{
  BOOST_ASSERT(static_cast<bool>(m_impl));
  m_impl->send(wire);
}

void
UnixTransport::send(const Block& header, const Block& payload)
{
  BOOST_ASSERT(static_cast<bool>(m_impl));
  m_impl->send(header, payload);
}

void
UnixTransport::close()
{
  BOOST_ASSERT(static_cast<bool>(m_impl));
  m_impl->close();
}

void
UnixTransport::pause()
{
  BOOST_ASSERT(static_cast<bool>(m_impl));
  m_impl->pause();
}

void
UnixTransport::resume()
{
  BOOST_ASSERT(static_cast<bool>(m_impl));
  m_impl->resume();
}

}
