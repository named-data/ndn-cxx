/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_UTIL_SIGNAL_SCOPED_CONNECTION_HPP
#define NDN_UTIL_SIGNAL_SCOPED_CONNECTION_HPP

#include "ndn-cxx/util/signal/connection.hpp"

namespace ndn {
namespace util {
namespace signal {

/** \brief Disconnects a Connection automatically upon destruction.
 */
class ScopedConnection
{
public:
  constexpr
  ScopedConnection() noexcept = default;

  ScopedConnection(const ScopedConnection&) = delete;

  ScopedConnection&
  operator=(const ScopedConnection&) = delete;

  /** \brief Move constructor.
   */
  ScopedConnection(ScopedConnection&&) noexcept;

  /** \brief Move assignment operator.
   */
  ScopedConnection&
  operator=(ScopedConnection&&) noexcept;

  /** \brief Implicit constructor from Connection.
   *  \param connection the Connection to be disconnected upon destruction
   */
  ScopedConnection(Connection connection) noexcept;

  /** \brief Assign a connection.
   *
   *  If a different connection has been assigned to this instance previously,
   *  that connection will be disconnected immediately.
   */
  ScopedConnection&
  operator=(Connection connection);

  /** \brief Destructor, automatically disconnects the connection.
   */
  ~ScopedConnection();

  /** \brief Manually disconnect the connection.
   */
  void
  disconnect();

  /** \brief Check if the connection is connected to the signal.
   *  \return false when a default-constructed connection is used, the connection is released,
   *          or the connection is disconnected
   */
  bool
  isConnected() const noexcept;

  /** \brief Release the connection so that it won't be disconnected
   *         when this ScopedConnection is destructed.
   */
  void
  release() noexcept;

private:
  Connection m_connection;
};

inline
ScopedConnection::ScopedConnection(ScopedConnection&&) noexcept = default;

inline ScopedConnection&
ScopedConnection::operator=(ScopedConnection&&) noexcept = default;

} // namespace signal
} // namespace util
} // namespace ndn

#endif // NDN_UTIL_SIGNAL_SCOPED_CONNECTION_HPP
