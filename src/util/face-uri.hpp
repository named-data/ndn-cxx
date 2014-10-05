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

#ifndef NDN_UTIL_FACE_URI_HPP
#define NDN_UTIL_FACE_URI_HPP

#include "common.hpp"
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include "ethernet.hpp"

namespace ndn {
namespace util {

/** \brief represents the underlying protocol and address used by a Face
 *  \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#FaceUri
 */
class FaceUri
{
public:
  class Error : public std::invalid_argument
  {
  public:
    explicit
    Error(const std::string& what)
      : std::invalid_argument(what)
    {
    }
  };

  FaceUri();

  /** \brief construct by parsing
   *
   *  \param uri scheme://host[:port]/path
   *  \throw FaceUri::Error if URI cannot be parsed
   */
  explicit
  FaceUri(const std::string& uri);

  // This overload is needed so that calls with string literal won't be
  // resolved to boost::asio::local::stream_protocol::endpoint overload.
  explicit
  FaceUri(const char* uri);

  /// exception-safe parsing
  bool
  parse(const std::string& uri);

public: // scheme-specific construction
  /// construct udp4 or udp6 canonical FaceUri
  explicit
  FaceUri(const boost::asio::ip::udp::endpoint& endpoint);

  /// construct tcp4 or tcp6 canonical FaceUri
  explicit
  FaceUri(const boost::asio::ip::tcp::endpoint& endpoint);

  /// construct tcp canonical FaceUri with customized scheme
  FaceUri(const boost::asio::ip::tcp::endpoint& endpoint, const std::string& scheme);

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
  /// construct unix canonical FaceUri
  explicit
  FaceUri(const boost::asio::local::stream_protocol::endpoint& endpoint);
#endif // BOOST_ASIO_HAS_LOCAL_SOCKETS

  /// create fd FaceUri from file descriptor
  static FaceUri
  fromFd(int fd);

  /// construct ether canonical FaceUri
  explicit
  FaceUri(const ethernet::Address& address);

  /// create dev FaceUri from network device name
  static FaceUri
  fromDev(const std::string& ifname);

public: // getters
  /// get scheme (protocol)
  const std::string&
  getScheme() const
  {
    return m_scheme;
  }

  /// get host (domain)
  const std::string&
  getHost() const
  {
    return m_host;
  }

  /// get port
  const std::string&
  getPort() const
  {
    return m_port;
  }

  /// get path
  const std::string&
  getPath() const
  {
    return m_path;
  }

  /// write as a string
  std::string
  toString() const;

public: // EqualityComparable concept
  bool
  operator==(const FaceUri& rhs) const;

  bool
  operator!=(const FaceUri& rhs) const;

private:
  std::string m_scheme;
  std::string m_host;
  /// whether to add [] around host when writing string
  bool m_isV6;
  std::string m_port;
  std::string m_path;

  friend std::ostream& operator<<(std::ostream& os, const FaceUri& uri);
};

std::ostream&
operator<<(std::ostream& os, const FaceUri& uri);

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_FACE_URI_HPP
