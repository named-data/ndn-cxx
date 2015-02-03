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

#include "signal-connection.hpp"

namespace ndn {
namespace util {
namespace signal {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Connection>));

Connection::Connection()
{
}

Connection::Connection(weak_ptr<function<void()>> disconnect)
  : m_disconnect(disconnect)
{
}

void
Connection::disconnect()
{
  shared_ptr<function<void()>> f = m_disconnect.lock();
  if (f != nullptr) {
    (*f)();
  }
}

bool
Connection::isConnected() const
{
  return !m_disconnect.expired();
}

bool
Connection::operator==(const Connection& other) const
{
  shared_ptr<function<void()>> f1 = m_disconnect.lock();
  shared_ptr<function<void()>> f2 = other.m_disconnect.lock();
  return f1 == f2;
}

bool
Connection::operator!=(const Connection& other) const
{
  return !(this->operator==(other));
}

} // namespace signal
} // namespace util
} // namespace ndn
