/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California,
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

#if BOOST_VERSION < 105800
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#endif // BOOST_VERSION < 105800

#include <net/if.h> // for if_nametoindex and if_indextoname

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

#if BOOST_VERSION < 105800
static unsigned int
scopeIdFromString(const std::string& scope)
{
  auto id = if_nametoindex(scope.c_str());
  if (id != 0) {
    return id;
  }

  // cannot find a corresponding index, assume it's not a name but an interface index
  try {
    return boost::lexical_cast<unsigned int>(scope);
  }
  catch (const boost::bad_lexical_cast&) {
    return 0;
  }
}

struct ParsedAddress
{
  boost::asio::ip::address addr;
  std::string scope;
};

static ParsedAddress
parseAddressFromString(const std::string& address, boost::system::error_code& ec)
{
  std::vector<std::string> parseResult;
  boost::algorithm::split(parseResult, address, boost::is_any_of("%"));
  auto addr = boost::asio::ip::address::from_string(parseResult[0], ec);

  switch (parseResult.size()) {
  case 1:
    // regular address
    return {addr, ""};
  case 2:
    // the presence of % in either an IPv4 address or a regular IPv6 address is invalid
    if (!ec && addr.is_v6() && addr.to_v6().is_link_local()) {
      return {addr, parseResult[1]};
    }
    NDN_CXX_FALLTHROUGH;
  default:
    ec = boost::asio::error::invalid_argument;
    return {};
  }
}
#endif // BOOST_VERSION < 105800

boost::asio::ip::address
addressFromString(const std::string& address, boost::system::error_code& ec)
{
  // boost < 1.58 cannot recognize scope-id in link-local IPv6 address
#if BOOST_VERSION < 105800
  auto parsedAddress = parseAddressFromString(address, ec);
  if (ec || parsedAddress.addr.is_v4()) {
    return parsedAddress.addr;
  }
  auto addr = parsedAddress.addr.to_v6();
  addr.scope_id(scopeIdFromString(parsedAddress.scope));
  return addr;
#else
  return boost::asio::ip::address::from_string(address, ec);
#endif // BOOST_VERSION < 105800
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
