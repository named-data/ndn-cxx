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

#include "net/address-converter.hpp"

#include "boost-test.hpp"
#include "collect-netifs.hpp"

namespace ndn {
namespace ip {
namespace tests {

BOOST_AUTO_TEST_SUITE(Net)
BOOST_AUTO_TEST_SUITE(TestAddressConverter)

#define CHECK_IPV6_ADDRESS(address, string, scope) do {                 \
    auto addrV6 = boost::asio::ip::address_v6::from_string(string);     \
    addrV6.scope_id(scope);                                             \
    BOOST_CHECK_EQUAL(address, addrV6);                                 \
} while (false)

BOOST_AUTO_TEST_CASE(ScopeNameFromId)
{
  const auto& networkInterfaces = net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    auto index = netif->getIndex();
    auto name = netif->getName();

    BOOST_CHECK_EQUAL(scopeNameFromId(index).value(), name);
  }

  BOOST_CHECK(!scopeNameFromId(std::numeric_limits<unsigned int>::max()));
}

BOOST_AUTO_TEST_CASE(AddressFromString)
{
  boost::asio::ip::address addr;
  boost::system::error_code ec;

  // empty string
  BOOST_CHECK_THROW(addressFromString(""), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressFromString("", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

  // IPv4 address
  BOOST_CHECK_EQUAL(addressFromString("192.168.0.1", ec),
                    boost::asio::ip::address::from_string("192.168.0.1"));
  BOOST_CHECK_EQUAL(ec, boost::system::errc::success);

  BOOST_CHECK_THROW(addressFromString("192.168.0"), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressFromString("192.168.0", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

  BOOST_CHECK_THROW(addressFromString("192.168.0.1%"), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressFromString("192.168.0.1%", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

  // regular IPv6 address
  BOOST_CHECK_EQUAL(addressFromString("2001:db8::1", ec),
                    boost::asio::ip::address::from_string("2001:db8::1"));
  BOOST_CHECK_EQUAL(ec, boost::system::errc::success);

  BOOST_CHECK_THROW(addressFromString("2001:db8:::"), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressFromString("2001:db8:::", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

  // link-local IPv6 address
  const auto& networkInterfaces = net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    CHECK_IPV6_ADDRESS(addressFromString("fe80::1%" + netif->getName(), ec).to_v6(),
                       "fe80::1", netif->getIndex());
    BOOST_CHECK_EQUAL(ec, boost::system::errc::success);
  }
}

BOOST_AUTO_TEST_CASE(AddressV6FromString)
{
  boost::asio::ip::address_v6 addr;
  boost::system::error_code ec;

  // empty string
  BOOST_CHECK_THROW(addressV6FromString(""), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressV6FromString("", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

  // IPv4 address
  BOOST_CHECK_THROW(addressV6FromString("192.168.0.1"), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressV6FromString("192.168.0.1", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);

  // regular IPv6 addresses
  BOOST_CHECK_EQUAL(addressV6FromString("2001:db8::1", ec),
                    boost::asio::ip::address_v6::from_string("2001:db8::1", ec));
  BOOST_CHECK_EQUAL(ec, boost::system::errc::success);

  BOOST_CHECK_THROW(addressV6FromString("2001:db8:::"), boost::system::system_error);
  BOOST_CHECK_EQUAL(addressV6FromString("2001:db8:::", ec), addr);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::invalid_argument);


  const auto& networkInterfaces = net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    auto index = netif->getIndex();

    CHECK_IPV6_ADDRESS(addressV6FromString("fe80::1%" + netif->getName(), ec), "fe80::1", index);
    BOOST_CHECK_EQUAL(ec, boost::system::errc::success);

    CHECK_IPV6_ADDRESS(addressV6FromString("fe80::1%" + to_string(index), ec), "fe80::1", index);
    BOOST_CHECK_EQUAL(ec, boost::system::errc::success);
  }

  int invalidIndex = 0;
  for (const auto& netif : networkInterfaces) {
    invalidIndex += netif->getIndex();
  }

  // an invalid interface name will lead to a default scope id (i.e. 0) which means no scope
  CHECK_IPV6_ADDRESS(addressV6FromString("fe80::1%NotAnInterface", ec), "fe80::1", 0);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::success);

  // supplying an interface index in the string won't trigger any checks on its validity
  CHECK_IPV6_ADDRESS(addressV6FromString("fe80::1%" + to_string(invalidIndex), ec),
                     "fe80::1", invalidIndex);
  BOOST_CHECK_EQUAL(ec, boost::system::errc::success);
}

BOOST_AUTO_TEST_SUITE_END() // TestAddressConverter
BOOST_AUTO_TEST_SUITE_END() // Net

} // namespace tests
} // namespace ip
} // namespace ndn
