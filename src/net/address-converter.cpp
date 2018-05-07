/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California,
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

#include "address-converter.hpp"

#include <net/if.h> // for if_indextoname()

namespace ndn {
namespace ip {

optional<std::string>
scopeNameFromId(unsigned int scopeId)
{
  char buffer[IFNAMSIZ];
  auto scopeName = if_indextoname(scopeId, buffer);
  if (scopeName != nullptr) {
    return std::string(scopeName);
  }
  return nullopt;
}

boost::asio::ip::address
addressFromString(const std::string& address, boost::system::error_code& ec)
{
  return boost::asio::ip::address::from_string(address, ec);
}

boost::asio::ip::address
addressFromString(const std::string& address)
{
  boost::system::error_code ec;
  auto addr = addressFromString(address, ec);
  if (ec) {
    BOOST_THROW_EXCEPTION(boost::system::system_error(ec));
  }
  return addr;
}

boost::asio::ip::address_v6
addressV6FromString(const std::string& address, boost::system::error_code& ec)
{
  auto addr = addressFromString(address, ec);
  if (ec || addr.is_v4()) {
    ec = boost::asio::error::invalid_argument;
    return {};
  }
  return addr.to_v6();
}

boost::asio::ip::address_v6
addressV6FromString(const std::string& address)
{
  boost::system::error_code ec;
  auto addr = addressV6FromString(address, ec);
  if (ec) {
    BOOST_THROW_EXCEPTION(boost::system::system_error(ec));
  }
  return addr;
}

} // namespace ip
} // namespace ndn
