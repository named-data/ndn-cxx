/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/transport/unix-transport.hpp"
#include "ndn-cxx/transport/detail/stream-transport-impl.hpp"

#include "ndn-cxx/face.hpp"
#include "ndn-cxx/net/face-uri.hpp"
#include "ndn-cxx/util/logger.hpp"

NDN_LOG_INIT(ndn.UnixTransport);
// DEBUG level: connect, close, pause, resume.

namespace ndn {

UnixTransport::UnixTransport(const std::string& unixSocket)
  : m_unixSocket(unixSocket)
{
}

UnixTransport::~UnixTransport() = default;

std::string
UnixTransport::getSocketNameFromUri(const std::string& uriString)
{
  // Assume the default nfd.sock location.
  std::string path = "/var/run/nfd.sock";

  if (uriString.empty()) {
    return path;
  }

  try {
    const FaceUri uri(uriString);

    if (uri.getScheme() != "unix") {
      NDN_THROW(Error("Cannot create UnixTransport from \"" + uri.getScheme() + "\" URI"));
    }

    if (!uri.getPath().empty()) {
      path = uri.getPath();
    }
  }
  catch (const FaceUri::Error& error) {
    NDN_THROW_NESTED(Error(error.what()));
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
  NDN_LOG_DEBUG("connect path=" << m_unixSocket);

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
  NDN_LOG_DEBUG("close");
  m_impl->close();
  m_impl.reset();
}

void
UnixTransport::pause()
{
  if (m_impl != nullptr) {
    NDN_LOG_DEBUG("pause");
    m_impl->pause();
  }
}

void
UnixTransport::resume()
{
  BOOST_ASSERT(m_impl != nullptr);
  NDN_LOG_DEBUG("resume");
  m_impl->resume();
}

} // namespace ndn
