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

#ifndef NDN_TRANSPORT_UNIX_TRANSPORT_HPP
#define NDN_TRANSPORT_UNIX_TRANSPORT_HPP

#include "../common.hpp"
#include "transport.hpp"
#include "../util/config-file.hpp"

// forward declaration
namespace boost { namespace asio { namespace local { class stream_protocol; } } }

namespace ndn {

// forward declaration
template<class T, class U>
class StreamTransportImpl;

class UnixTransport : public Transport
{
public:

  /**
   * Create Unix transport based on the socket specified
   * in a well-known configuration file or fallback to /var/run/nfd.sock
   *
   * @throws Throws UnixTransport::Error on failure to parse a discovered configuration file
   */
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

  static shared_ptr<UnixTransport>
  create(const ConfigFile& config);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * Determine the default NFD unix socket
   *
   * @returns transport value if present in config, else /var/run/nfd.sock
   * @throws ConfigFile::Error if fail to parse value of a present "transport" field
   */
  static std::string
  getDefaultSocketName(const ConfigFile& config);

private:
  std::string m_unixSocket;

  typedef StreamTransportImpl<UnixTransport, boost::asio::local::stream_protocol> Impl;
  friend class StreamTransportImpl<UnixTransport, boost::asio::local::stream_protocol>;
  shared_ptr< Impl > m_impl;
};

} // namespace ndn

#endif // NDN_TRANSPORT_UNIX_TRANSPORT_HPP
