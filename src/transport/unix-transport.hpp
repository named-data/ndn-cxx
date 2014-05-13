/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_UDPTRANSPORT_HPP
#define NDN_UDPTRANSPORT_HPP

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

  /**
   * Determine the default NFD unix socket
   *
   * @returns unix_socket value if present in config, else /var/run/nfd.sock
   * @throws ConfigFile::Error if fail to parse value of a present "unix_socket" field
   */
  static std::string
  getDefaultSocketName(const ConfigFile& config);

private:
  std::string m_unixSocket;

  typedef StreamTransportImpl<UnixTransport, boost::asio::local::stream_protocol> Impl;
  friend class StreamTransportImpl<UnixTransport, boost::asio::local::stream_protocol>;
  shared_ptr< Impl > m_impl;
};

}

#endif
