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

#ifndef NDN_UTIL_SIGNAL_SCOPED_CONNECTION_HPP
#define NDN_UTIL_SIGNAL_SCOPED_CONNECTION_HPP

#include "signal-connection.hpp"

namespace ndn {
namespace util {
namespace signal {

/** \brief disconnects a Connection automatically upon destruction
 */
class ScopedConnection : noncopyable
{
public:
  ScopedConnection();

  /** \brief implicit constructor from Connection
   *  \param connection the Connection to be disconnected upon destruction
   */
  ScopedConnection(const Connection& connection);

  /** \brief move constructor
   */
  ScopedConnection(ScopedConnection&& other) noexcept;

  /** \brief assigns a connection
   *
   *  If a different connection has been assigned to this instance previously,
   *  that connection will be disconnected immediately.
   */
  ScopedConnection&
  operator=(const Connection& connection);

  /** \brief disconnects the connection
   */
  ~ScopedConnection();

  /** \brief disconnects the connection manually
   */
  void
  disconnect();

  /** \brief check if the connection is connected to the signal
   *  \return false when a default-constructed connection is used, the connection is released,
   *          or the connection is disconnected
   */
  bool
  isConnected() const;

  /** \brief releases the connection so that it won't be disconnected
   *         when this ScopedConnection is destructed
   */
  void
  release();

private:
  Connection m_connection;
};

} // namespace signal
} // namespace util
} // namespace ndn

#endif // NDN_UTIL_SIGNAL_SCOPED_CONNECTION_HPP
