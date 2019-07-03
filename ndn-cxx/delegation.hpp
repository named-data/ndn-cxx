/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_DELEGATION_HPP
#define NDN_DELEGATION_HPP

#include "ndn-cxx/name.hpp"

namespace ndn {

/** \brief Represents a Delegation.
 *  \sa https://named-data.net/doc/NDN-packet-spec/current/link.html
 */
class Delegation
{
private: // non-member operators
  // NOTE: the following "hidden friend" operators are available via
  //       argument-dependent lookup only and must be defined inline.

  friend bool
  operator==(const Delegation& lhs, const Delegation& rhs)
  {
    return !(lhs != rhs);
  }

  friend bool
  operator!=(const Delegation& lhs, const Delegation& rhs)
  {
    return lhs.preference != rhs.preference ||
           lhs.name != rhs.name;
  }

  friend bool
  operator<(const Delegation& lhs, const Delegation& rhs)
  {
    return std::tie(lhs.preference, lhs.name) <
           std::tie(rhs.preference, rhs.name);
  }

  friend bool
  operator<=(const Delegation& lhs, const Delegation& rhs)
  {
    return !(rhs < lhs);
  }

  friend bool
  operator>(const Delegation& lhs, const Delegation& rhs)
  {
    return rhs < lhs;
  }

  friend bool
  operator>=(const Delegation& lhs, const Delegation& rhs)
  {
    return !(lhs < rhs);
  }

  friend std::ostream&
  operator<<(std::ostream& os, const Delegation& d)
  {
    return os << d.name << '(' << d.preference << ')';
  }

public:
  uint64_t preference;
  Name name;
};

} // namespace ndn

#endif // NDN_DELEGATION_HPP
