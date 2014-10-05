/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014,  Regents of the University of California,
 *                      Arizona Board of Regents,
 *                      Colorado State University,
 *                      University Pierre & Marie Curie, Sorbonne University,
 *                      Washington University in St. Louis,
 *                      Beijing Institute of Technology,
 *                      The University of Memphis
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

#include "face-uri.hpp"

#include <boost/concept_check.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace util {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<FaceUri>));

FaceUri::FaceUri()
  : m_isV6(false)
{
}

FaceUri::FaceUri(const std::string& uri)
{
  if (!parse(uri)) {
    throw Error("Malformed URI: " + uri);
  }
}

FaceUri::FaceUri(const char* uri)
{
  if (!parse(uri)) {
    throw Error("Malformed URI: " + std::string(uri));
  }
}

bool
FaceUri::parse(const std::string& uri)
{
  m_scheme.clear();
  m_host.clear();
  m_isV6 = false;
  m_port.clear();
  m_path.clear();

  static const boost::regex protocolExp("(\\w+\\d?)://([^/]*)(\\/[^?]*)?");
  boost::smatch protocolMatch;
  if (!boost::regex_match(uri, protocolMatch, protocolExp)) {
    return false;
  }
  m_scheme = protocolMatch[1];
  const std::string& authority = protocolMatch[2];
  m_path = protocolMatch[3];

  // pattern for IPv6 address enclosed in [ ], with optional port number
  static const boost::regex v6Exp("^\\[([a-fA-F0-9:]+)\\](?:\\:(\\d+))?$");
  // pattern for Ethernet address in standard hex-digits-and-colons notation
  static const boost::regex etherExp("^\\[((?:[a-fA-F0-9]{1,2}\\:){5}(?:[a-fA-F0-9]{1,2}))\\]$");
  // pattern for IPv4-mapped IPv6 address, with optional port number
  static const boost::regex v4MappedV6Exp("^\\[::ffff:(\\d+(?:\\.\\d+){3})\\](?:\\:(\\d+))?$");
  // pattern for IPv4/hostname/fd/ifname, with optional port number
  static const boost::regex v4HostExp("^([^:]+)(?:\\:(\\d+))?$");

  if (authority.empty()) {
    // UNIX, internal
  }
  else {
    boost::smatch match;
    m_isV6 = boost::regex_match(authority, match, v6Exp);
    if (m_isV6 ||
        boost::regex_match(authority, match, etherExp) ||
        boost::regex_match(authority, match, v4MappedV6Exp) ||
        boost::regex_match(authority, match, v4HostExp)) {
      m_host = match[1];
      m_port = match[2];
    }
    else {
      return false;
    }
  }

  return true;
}

FaceUri::FaceUri(const boost::asio::ip::udp::endpoint& endpoint)
{
  m_isV6 = endpoint.address().is_v6();
  m_scheme = m_isV6 ? "udp6" : "udp4";
  m_host = endpoint.address().to_string();
  m_port = boost::lexical_cast<std::string>(endpoint.port());
}

FaceUri::FaceUri(const boost::asio::ip::tcp::endpoint& endpoint)
{
  m_isV6 = endpoint.address().is_v6();
  m_scheme = m_isV6 ? "tcp6" : "tcp4";
  m_host = endpoint.address().to_string();
  m_port = boost::lexical_cast<std::string>(endpoint.port());
}

FaceUri::FaceUri(const boost::asio::ip::tcp::endpoint& endpoint, const std::string& scheme)
  : m_scheme(scheme)
{
  m_isV6 = endpoint.address().is_v6();
  m_host = endpoint.address().to_string();
  m_port = boost::lexical_cast<std::string>(endpoint.port());
}

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
FaceUri::FaceUri(const boost::asio::local::stream_protocol::endpoint& endpoint)
  : m_isV6(false)
{
  m_scheme = "unix";
  m_path = endpoint.path();
}
#endif // BOOST_ASIO_HAS_LOCAL_SOCKETS

FaceUri
FaceUri::fromFd(int fd)
{
  FaceUri uri;
  uri.m_scheme = "fd";
  uri.m_host = boost::lexical_cast<std::string>(fd);
  return uri;
}

FaceUri::FaceUri(const ethernet::Address& address)
  : m_isV6(true)
{
  m_scheme = "ether";
  m_host = address.toString();
}

FaceUri
FaceUri::fromDev(const std::string& ifname)
{
  FaceUri uri;
  uri.m_scheme = "dev";
  uri.m_host = ifname;
  return uri;
}

bool
FaceUri::operator==(const FaceUri& rhs) const
{
  return (m_scheme == rhs.m_scheme &&
          m_host == rhs.m_host &&
          m_isV6 == rhs.m_isV6 &&
          m_port == rhs.m_port &&
          m_path == rhs.m_path);
}

bool
FaceUri::operator!=(const FaceUri& rhs) const
{
  return !(*this == rhs);
}

std::string
FaceUri::toString() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

std::ostream&
operator<<(std::ostream& os, const FaceUri& uri)
{
  os << uri.m_scheme << "://";
  if (uri.m_isV6) {
    os << "[" << uri.m_host << "]";
  }
  else {
    os << uri.m_host;
  }
  if (!uri.m_port.empty()) {
    os << ":" << uri.m_port;
  }
  os << uri.m_path;
  return os;
}

} // namespace util
} // namespace ndn
