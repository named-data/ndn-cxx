/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "util/dns.hpp"

#include "boost-test.hpp"
#include "../network-configuration-detector.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace util {
namespace tests {

using boost::asio::ip::address_v4;
using boost::asio::ip::address_v6;

using ndn::tests::NetworkConfigurationDetector;

class DnsFixture
{
public:
  DnsFixture()
    : m_nFailures(0)
    , m_nSuccesses(0)
  {
  }

  void
  onSuccess(const dns::IpAddress& resolvedAddress,
            const dns::IpAddress& expectedAddress,
            bool isValid,
            bool shouldCheckAddress = false)
  {
    BOOST_TEST_MESSAGE("Resolved to: " << resolvedAddress);

    ++m_nSuccesses;

    if (!isValid)
      {
        BOOST_FAIL("Resolved to " + boost::lexical_cast<std::string>(resolvedAddress)
                   + ", but should have failed");
      }

    BOOST_CHECK_EQUAL(resolvedAddress.is_v4(), expectedAddress.is_v4());

    // checking address is not deterministic and should be enabled only
    // if only one IP address will be returned by resolution
    if (shouldCheckAddress)
      {
        BOOST_CHECK_EQUAL(resolvedAddress, expectedAddress);
      }
  }

  void
  onFailure(bool isValid)
  {
    ++m_nFailures;

    if (!isValid)
      {
        BOOST_FAIL("Resolution should not have failed");
      }

    BOOST_CHECK_MESSAGE(true, "Resolution failed as expected");
  }

public:
  uint32_t m_nFailures;
  uint32_t m_nSuccesses;

  boost::asio::io_service m_ioService;
};

BOOST_FIXTURE_TEST_SUITE(UtilDns, DnsFixture)

BOOST_AUTO_TEST_CASE(Asynchronous)
{
  if (!NetworkConfigurationDetector::hasIpv4() && !NetworkConfigurationDetector::hasIpv6()) {
    BOOST_TEST_MESSAGE("Platform does not support both IPv4 and IPv6, skipping test case");
    return;
  }

  dns::asyncResolve("nothost.nothost.nothost.arpa",
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v4()), false, false),
                    bind(&DnsFixture::onFailure, this, true),
                    m_ioService); // should fail
  m_ioService.run();

  BOOST_CHECK_EQUAL(m_nFailures, 1);
  BOOST_CHECK_EQUAL(m_nSuccesses, 0);
}

BOOST_AUTO_TEST_CASE(AsynchronousV4)
{
  if (!NetworkConfigurationDetector::hasIpv4()) {
    BOOST_TEST_MESSAGE("Platform does not support IPv4, skipping the test case");
    return;
  }

  dns::asyncResolve("192.0.2.1",
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v4::from_string("192.0.2.1")),
                         true, true),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService);
  m_ioService.run();

  BOOST_CHECK_EQUAL(m_nFailures, 0);
  BOOST_CHECK_EQUAL(m_nSuccesses, 1);
}

BOOST_AUTO_TEST_CASE(AsynchronousV6)
{
  if (!NetworkConfigurationDetector::hasIpv6()) {
    BOOST_TEST_MESSAGE("Platform does not support IPv6, skipping the test case");
    return;
  }

  dns::asyncResolve("ipv6.google.com", // only IPv6 address should be available
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v6()), true, false),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService);

  dns::asyncResolve("2001:db8:3f9:0:3025:ccc5:eeeb:86d3",
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v6::
                                      from_string("2001:db8:3f9:0:3025:ccc5:eeeb:86d3")),
                         true, true),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService);
  m_ioService.run();

  BOOST_CHECK_EQUAL(m_nFailures, 0);
  BOOST_CHECK_EQUAL(m_nSuccesses, 2);
}

BOOST_AUTO_TEST_CASE(AsynchronousV4AndV6)
{
  if (!NetworkConfigurationDetector::hasIpv4() || !NetworkConfigurationDetector::hasIpv6()) {
    BOOST_TEST_MESSAGE("Platform does not support either IPv4 or IPv6, skipping test case");
    return;
  }

  dns::asyncResolve("www.named-data.net",
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v4()), true, false),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService,
                    dns::Ipv4Only());

  dns::asyncResolve("a.root-servers.net",
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v4()), true, false),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService,
                    dns::Ipv4Only()); // request IPv4 address

  dns::asyncResolve("a.root-servers.net",
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v6()), true, false),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService,
                    dns::Ipv6Only()); // request IPv6 address

  dns::asyncResolve("ipv6.google.com", // only IPv6 address should be available
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v6()), true, false),
                    bind(&DnsFixture::onFailure, this, false),
                    m_ioService,
                    dns::Ipv6Only());

  dns::asyncResolve("ipv6.google.com", // only IPv6 address should be available
                    bind(&DnsFixture::onSuccess, this, _1,
                         dns::IpAddress(address_v6()), false, false),
                    bind(&DnsFixture::onFailure, this, true), // should fail
                    m_ioService,
                    dns::Ipv4Only());
  m_ioService.run();

  BOOST_CHECK_EQUAL(m_nFailures, 1);
  BOOST_CHECK_EQUAL(m_nSuccesses, 4);
}

BOOST_AUTO_TEST_CASE(Synchronous)
{
  if (!NetworkConfigurationDetector::hasIpv4() && !NetworkConfigurationDetector::hasIpv6()) {
    BOOST_TEST_MESSAGE("Platform does not support both IPv4 and IPv6, skipping test case");
    return;
  }
  dns::IpAddress address;
  BOOST_CHECK_NO_THROW(address = dns::syncResolve("www.named-data.net", m_ioService));

  BOOST_CHECK(address.is_v4() || address.is_v6());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace util
} // namespace ndn
