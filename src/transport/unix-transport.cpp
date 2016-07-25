/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "unix-transport.hpp"
#include "stream-transport-impl.hpp"

#include "../face.hpp"
#include "util/face-uri.hpp"

namespace ndn {

UnixTransport::UnixTransport(const std::string& unixSocket)
  : m_unixSocket(unixSocket)
{
}

UnixTransport::~UnixTransport()
{
}

std::string
UnixTransport::getSocketNameFromUri(const std::string& uriString)
{
  // Assume the default nfd.sock location.
  std::string path = "/var/run/nfd.sock";

  if (uriString.empty()) {
    return path;
  }

  try {
    const util::FaceUri uri(uriString);

    if (uri.getScheme() != "unix") {
      BOOST_THROW_EXCEPTION(Error("Cannot create UnixTransport from \"" +
                                  uri.getScheme() + "\" URI"));
    }

    if (!uri.getPath().empty()) {
      path = uri.getPath();
    }
  }
  catch (const util::FaceUri::Error& error) {
    BOOST_THROW_EXCEPTION(Error(error.what()));
  }

  return path;
}

shared_ptr<UnixTransport>
UnixTransport::create(const std::string& uri)
{
  return make_shared<UnixTransport>(getSocketNameFromUri(uri));
}

void
UnixTransport::connect(boost::asio::io_service& ioService,
                       const ReceiveCallback& receiveCallback)
{
  if (m_impl == nullptr) {
    Transport::connect(ioService, receiveCallback);

    m_impl = make_shared<Impl>(ref(*this), ref(ioService));
  }

  m_impl->connect(boost::asio::local::stream_protocol::endpoint(m_unixSocket));
}

void
UnixTransport::send(const Block& wire)
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->send(wire);
}

void
UnixTransport::send(const Block& header, const Block& payload)
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->send(header, payload);
}

void
UnixTransport::close()
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->close();
  m_impl.reset();
}

void
UnixTransport::pause()
{
  if (m_impl != nullptr) {
    m_impl->pause();
  }
}

void
UnixTransport::resume()
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->resume();
}

} // namespace ndn
