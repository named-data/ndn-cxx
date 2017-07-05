/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "scoped-connection.hpp"

namespace ndn {
namespace util {
namespace signal {

#if NDN_CXX_HAVE_IS_NOTHROW_MOVE_CONSTRUCTIBLE
static_assert(std::is_nothrow_move_constructible<ScopedConnection>::value,
              "ScopedConnection must be MoveConstructible with noexcept");
#endif // NDN_CXX_HAVE_IS_NOTHROW_MOVE_CONSTRUCTIBLE

ScopedConnection::ScopedConnection()
{
}

ScopedConnection::ScopedConnection(const Connection& connection)
  : m_connection(connection)
{
}

ScopedConnection::ScopedConnection(ScopedConnection&& other) noexcept
  : m_connection(other.m_connection)
{
  other.release();
}

ScopedConnection&
ScopedConnection::operator=(const Connection& connection)
{
  if (m_connection != connection) {
    m_connection.disconnect();
    m_connection = connection;
  }
  return *this;
}

ScopedConnection::~ScopedConnection() noexcept
{
  m_connection.disconnect();
}

void
ScopedConnection::disconnect()
{
  m_connection.disconnect();
}

bool
ScopedConnection::isConnected() const
{
  return m_connection.isConnected();
}

void
ScopedConnection::release()
{
  m_connection = {};
}

} // namespace signal
} // namespace util
} // namespace ndn
