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

#include "tcp-transport.hpp"
#include "stream-transport-with-resolver-impl.hpp"
#include "util/face-uri.hpp"

namespace ndn {

TcpTransport::TcpTransport(const std::string& host, const std::string& port/* = "6363"*/)
  : m_host(host)
  , m_port(port)
{
}

TcpTransport::~TcpTransport() = default;

shared_ptr<TcpTransport>
TcpTransport::create(const std::string& uri)
{
  const auto hostAndPort(getSocketHostAndPortFromUri(uri));
  return make_shared<TcpTransport>(hostAndPort.first, hostAndPort.second);
}

std::pair<std::string, std::string>
TcpTransport::getSocketHostAndPortFromUri(const std::string& uriString)
{
  std::string host = "localhost";
  std::string port = "6363";

  if (uriString.empty()) {
    return {host, port};
  }

  try {
    const util::FaceUri uri(uriString);

    const std::string scheme = uri.getScheme();
    if (scheme != "tcp" && scheme != "tcp4" && scheme != "tcp6") {
      BOOST_THROW_EXCEPTION(Error("Cannot create TcpTransport from \"" + scheme + "\" URI"));
    }

    if (!uri.getHost().empty()) {
      host = uri.getHost();
    }

    if (!uri.getPort().empty()) {
      port = uri.getPort();
    }
  }
  catch (const util::FaceUri::Error& error) {
    BOOST_THROW_EXCEPTION(Error(error.what()));
  }

  return {host, port};
}

void
TcpTransport::connect(boost::asio::io_service& ioService,
                      const ReceiveCallback& receiveCallback)
{
  if (m_impl == nullptr) {
    Transport::connect(ioService, receiveCallback);

    m_impl = make_shared<Impl>(ref(*this), ref(ioService));
  }

  boost::asio::ip::tcp::resolver::query query(m_host, m_port);
  m_impl->connect(query);
}

void
TcpTransport::send(const Block& wire)
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->send(wire);
}

void
TcpTransport::send(const Block& header, const Block& payload)
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->send(header, payload);
}

void
TcpTransport::close()
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->close();
  m_impl.reset();
}

void
TcpTransport::pause()
{
  if (m_impl != nullptr) {
    m_impl->pause();
  }
}

void
TcpTransport::resume()
{
  BOOST_ASSERT(m_impl != nullptr);
  m_impl->resume();
}

} // namespace ndn
