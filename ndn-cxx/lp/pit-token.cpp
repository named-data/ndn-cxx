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

#include "ndn-cxx/lp/pit-token.hpp"
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/util/string-helper.hpp"

namespace ndn {
namespace lp {

static constexpr size_t LENGTH_MIN = 1;
static constexpr size_t LENGTH_MAX = 32;

void
PitToken::validate() const
{
  if (size() < LENGTH_MIN || size() > LENGTH_MAX) {
    NDN_THROW(ndn::tlv::Error("PitToken length must be between " +
      to_string(LENGTH_MIN) + " and " + to_string(LENGTH_MAX)));
  }
}

std::ostream&
operator<<(std::ostream& os, const PitToken& pitToken)
{
  printHex(os, pitToken, false);
  return os;
}

} // namespace lp
} // namespace ndn
