/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#ifndef NDN_CXX_NET_FACE_URI_HPP
#define NDN_CXX_NET_FACE_URI_HPP

#include "ndn-cxx/detail/asio-fwd.hpp"
#include "ndn-cxx/net/ethernet.hpp"
#include "ndn-cxx/util/time.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/stream_protocol.hpp>

namespace ndn {

/**
 * \brief The underlying protocol and address used by a Face.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#FaceUri
 */
class FaceUri
{
public:
  class Error : public std::invalid_argument
  {
  public:
    using std::invalid_argument::invalid_argument;
  };

  /// Construct an empty FaceUri.
  FaceUri();

  /**
   * \brief Construct by parsing from a string.
   * \param uri `scheme://host[:port]/path`
   * \throw Error URI cannot be parsed
   */
  explicit
  FaceUri(const std::string& uri);

  /**
   * \brief Construct by parsing from a null-terminated string.
   * \param uri `scheme://host[:port]/path`
   * \throw Error URI cannot be parsed
   * \note This overload is needed so that calls with a string literal won't be
   *       resolved to the boost::asio::local::stream_protocol::endpoint overload.
   */
  explicit
  FaceUri(const char* uri);

  /// Exception-safe parsing.
  [[nodiscard]] bool
  parse(std::string_view uri);

public: // scheme-specific construction
  /// Construct a udp4 or udp6 canonical FaceUri.
  explicit
  FaceUri(const boost::asio::ip::udp::endpoint& endpoint);

  /// Construct a tcp4 or tcp6 canonical FaceUri.
  explicit
  FaceUri(const boost::asio::ip::tcp::endpoint& endpoint);

  /// Construct a tcp canonical FaceUri with custom scheme.
  FaceUri(const boost::asio::ip::tcp::endpoint& endpoint, std::string_view scheme);

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
  /// Construct a unix canonical FaceUri.
  explicit
  FaceUri(const boost::asio::local::stream_protocol::endpoint& endpoint);
#endif // BOOST_ASIO_HAS_LOCAL_SOCKETS

  /// Construct an ether canonical FaceUri.
  explicit
  FaceUri(const ethernet::Address& address);

  /// Construct an fd FaceUri from a file descriptor.
  static FaceUri
  fromFd(int fd);

  /// Construct a dev FaceUri from a network device name.
  static FaceUri
  fromDev(std::string_view ifname);

  /// Construct a udp4 or udp6 NIC-associated FaceUri from endpoint and network device name.
  static FaceUri
  fromUdpDev(const boost::asio::ip::udp::endpoint& endpoint, std::string_view ifname);

public: // getters
  /// Get scheme (protocol)
  const std::string&
  getScheme() const
  {
    return m_scheme;
  }

  /// Get host (domain or address)
  const std::string&
  getHost() const
  {
    return m_host;
  }

  /// Get port
  const std::string&
  getPort() const
  {
    return m_port;
  }

  /// Get path
  const std::string&
  getPath() const
  {
    return m_path;
  }

  /// Return string representation
  std::string
  toString() const;

public: // canonical FaceUri
  /**
   * \brief Return whether a FaceUri of the specified scheme can be canonized.
   */
  static bool
  canCanonize(const std::string& scheme);

  /** \brief Determine whether this FaceUri is in canonical form.
   *  \return true if this FaceUri is in canonical form,
   *          false if this FaceUri is not in canonical form or
   *          or it's undetermined whether this FaceUri is in canonical form
   */
  bool
  isCanonical() const;

  using CanonizeSuccessCallback = std::function<void(const FaceUri&)>;
  using CanonizeFailureCallback = std::function<void(const std::string& reason)>;

  /** \brief Asynchronously convert this FaceUri to canonical form.
   *  \note A new FaceUri in canonical form will be created; this FaceUri is unchanged.
   *
   *  \param onSuccess function to call after this FaceUri is converted to canonical form
   *  \param onFailure function to call if this FaceUri cannot be converted to canonical form
   *  \param io        reference to `boost::asio::io_service` instance
   *  \param timeout   maximum allowable duration of the operations.
   *                   It's intentional not to provide a default value: the caller should set
   *                   a reasonable value in balance between network delay and user experience.
   */
  void
  canonize(const CanonizeSuccessCallback& onSuccess,
           const CanonizeFailureCallback& onFailure,
           boost::asio::io_service& io,
           time::nanoseconds timeout) const;

private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const FaceUri& lhs, const FaceUri& rhs)
  {
    return !(lhs != rhs);
  }

  friend bool
  operator!=(const FaceUri& lhs, const FaceUri& rhs)
  {
    return lhs.m_isV6 != rhs.m_isV6 ||
           lhs.m_scheme != rhs.m_scheme ||
           lhs.m_host != rhs.m_host ||
           lhs.m_port != rhs.m_port ||
           lhs.m_path != rhs.m_path;
  }

private:
  std::string m_scheme;
  std::string m_host;
  std::string m_port;
  std::string m_path;
  bool m_isV6 = false; ///< whether to add [] around host when converting to string representation

  friend std::ostream& operator<<(std::ostream& os, const FaceUri& uri);
};

std::ostream&
operator<<(std::ostream& os, const FaceUri& uri);

} // namespace ndn

#endif // NDN_CXX_NET_FACE_URI_HPP
