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

#include "net/face-uri.hpp"

#include "boost-test.hpp"
#include "collect-netifs.hpp"
#include "network-configuration-detector.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Net)
BOOST_AUTO_TEST_SUITE(TestFaceUri)

class CanonizeFixture : noncopyable
{
protected:
  void
  addTest(const std::string& request, bool shouldSucceed, const std::string& expectedUri)
  {
    ++m_nPending;
    auto tc = make_shared<CanonizeTestCase>(request, shouldSucceed, expectedUri);

    FaceUri uri(request);
    uri.canonize(bind(&CanonizeFixture::onCanonizeSuccess, this, tc, _1),
                 bind(&CanonizeFixture::onCanonizeFailure, this, tc, _1),
                 m_io, 10_s);
  }

  void
  runTests()
  {
    m_io.run();
    BOOST_CHECK_EQUAL(m_nPending, 0);
  }

private:
  class CanonizeTestCase
  {
  public:
    CanonizeTestCase(const std::string& request, bool shouldSucceed, const std::string& expectedUri)
      : m_expectedUri(expectedUri)
      , m_message(request + " should " + (shouldSucceed ? "succeed" : "fail"))
      , m_shouldSucceed(shouldSucceed)
      , m_isCompleted(false)
    {
    }

  public:
    std::string m_expectedUri;
    std::string m_message;
    bool m_shouldSucceed;
    bool m_isCompleted;
  };

  void
  onCanonizeSuccess(const shared_ptr<CanonizeTestCase>& tc, const FaceUri& canonicalUri)
  {
    BOOST_CHECK_EQUAL(tc->m_isCompleted, false);
    tc->m_isCompleted = true;
    --m_nPending;

    BOOST_CHECK_MESSAGE(tc->m_shouldSucceed, tc->m_message);
    if (tc->m_shouldSucceed) {
      BOOST_CHECK_EQUAL(canonicalUri.toString(), tc->m_expectedUri);
    }
  }

  void
  onCanonizeFailure(const shared_ptr<CanonizeTestCase>& tc, const std::string& reason)
  {
    BOOST_CHECK_EQUAL(tc->m_isCompleted, false);
    tc->m_isCompleted = true;
    --m_nPending;

    BOOST_CHECK_MESSAGE(!tc->m_shouldSucceed, tc->m_message);
  }

private:
  boost::asio::io_service m_io;
  ssize_t m_nPending = 0;
};

BOOST_AUTO_TEST_CASE(ParseInternal)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("internal://"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "internal");
  BOOST_CHECK_EQUAL(uri.getHost(), "");
  BOOST_CHECK_EQUAL(uri.getPort(), "");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK_EQUAL(uri.parse("internal:"), false);
  BOOST_CHECK_EQUAL(uri.parse("internal:/"), false);
}

BOOST_AUTO_TEST_CASE(ParseUdp)
{
  FaceUri uri("udp://hostname:6363");
  BOOST_CHECK_THROW(FaceUri("udp//hostname:6363"), FaceUri::Error);
  BOOST_CHECK_THROW(FaceUri("udp://hostname:port"), FaceUri::Error);

  BOOST_CHECK_EQUAL(uri.parse("udp//hostname:6363"), false);

  BOOST_CHECK(uri.parse("udp://hostname:80"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "udp");
  BOOST_CHECK_EQUAL(uri.getHost(), "hostname");
  BOOST_CHECK_EQUAL(uri.getPort(), "80");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK(uri.parse("udp4://192.0.2.1:20"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "udp4");
  BOOST_CHECK_EQUAL(uri.getHost(), "192.0.2.1");
  BOOST_CHECK_EQUAL(uri.getPort(), "20");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK(uri.parse("udp6://[2001:db8:3f9:0::1]:6363"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "udp6");
  BOOST_CHECK_EQUAL(uri.getHost(), "2001:db8:3f9:0::1");
  BOOST_CHECK_EQUAL(uri.getPort(), "6363");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK(uri.parse("udp6://[2001:db8:3f9:0:3025:ccc5:eeeb:86d3]:6363"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "udp6");
  BOOST_CHECK_EQUAL(uri.getHost(), "2001:db8:3f9:0:3025:ccc5:eeeb:86d3");
  BOOST_CHECK_EQUAL(uri.getPort(), "6363");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK_EQUAL(uri.parse("udp6://[2001:db8:3f9:0:3025:ccc5:eeeb:86dg]:6363"), false);

  namespace ip = boost::asio::ip;

  ip::udp::endpoint endpoint4(ip::address_v4::from_string("192.0.2.1"), 7777);
  uri = FaceUri(endpoint4);
  BOOST_CHECK_EQUAL(uri.toString(), "udp4://192.0.2.1:7777");

  ip::udp::endpoint endpoint6(ip::address_v6::from_string("2001:DB8::1"), 7777);
  uri = FaceUri(endpoint6);
  BOOST_CHECK_EQUAL(uri.toString(), "udp6://[2001:db8::1]:7777");

  BOOST_CHECK(uri.parse("udp6://[fe80::1%25eth1]:6363"));
  BOOST_CHECK_EQUAL(uri.getHost(), "fe80::1%25eth1");

  BOOST_CHECK(uri.parse("udp6://[fe80::1%eth1]:6363"));
  BOOST_CHECK_EQUAL(uri.getHost(), "fe80::1%eth1");

  BOOST_CHECK(uri.parse("udp6://[fe80::1%1]:6363"));
  BOOST_CHECK(uri.parse("udp6://[fe80::1%eth1]"));

  BOOST_CHECK(uri.parse("udp6://[ff01::114%eth#1]"));
  BOOST_CHECK(uri.parse("udp6://[ff01::114%eth.1,2]"));
  BOOST_CHECK(uri.parse("udp6://[ff01::114%a+b-c=0]"));
  BOOST_CHECK(uri.parse("udp6://[ff01::114%[foo]]"));
  BOOST_CHECK(uri.parse("udp6://[ff01::114%]]"));
  BOOST_CHECK(uri.parse("udp6://[ff01::114%%]"));
  BOOST_CHECK(!uri.parse("udp6://[ff01::114%]"));
  BOOST_CHECK(!uri.parse("udp6://[ff01::114%foo bar]"));
  BOOST_CHECK(!uri.parse("udp6://[ff01::114%foo/bar]"));
  BOOST_CHECK(!uri.parse("udp6://[ff01::114%eth0:1]"));
}

BOOST_FIXTURE_TEST_CASE(IsCanonicalUdp, CanonizeFixture)
{
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("udp"), true);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("udp4"), true);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("udp6"), true);

  BOOST_CHECK_EQUAL(FaceUri("udp4://192.0.2.1:6363").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("udp://192.0.2.1:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp4://192.0.2.1").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp4://192.0.2.1:6363/").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp6://[2001:db8::1]:6363").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("udp6://[2001:db8::01]:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp://[2001:db8::1]:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp://example.net:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp4://example.net:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp6://example.net:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp4://224.0.23.170:56363").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("udp4://[2001:db8::1]:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp6://192.0.2.1:6363").isCanonical(), false);

  const auto& networkInterfaces = ndn::net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    auto name = netif->getName();
    auto index = to_string(netif->getIndex());

    BOOST_CHECK_EQUAL(FaceUri("udp6://[fe80::1%" + name + "]:6363").isCanonical(), true);
    BOOST_CHECK_EQUAL(FaceUri("udp6://[fe80::1%" + index + "]:6363").isCanonical(), false);
    BOOST_CHECK_EQUAL(FaceUri("udp6://[fe80::1%" + name + "]").isCanonical(), false);
    BOOST_CHECK_EQUAL(FaceUri("udp6://[fe80::1068:dddb:fe26:fe3f%25en0]:6363").isCanonical(), false);
  }
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(CanonizeUdpV4, 1)
BOOST_FIXTURE_TEST_CASE(CanonizeUdpV4, CanonizeFixture)
{
  SKIP_IF_IPV4_UNAVAILABLE();

  // IPv4 unicast
  addTest("udp4://192.0.2.1:6363", true, "udp4://192.0.2.1:6363");
  addTest("udp://192.0.2.2:6363", true, "udp4://192.0.2.2:6363");
  addTest("udp4://192.0.2.3", true, "udp4://192.0.2.3:6363");
  addTest("udp4://192.0.2.4:6363/", true, "udp4://192.0.2.4:6363");
  addTest("udp4://192.0.2.5:9695", true, "udp4://192.0.2.5:9695");
  addTest("udp4://192.0.2.666:6363", false, "");
  addTest("udp4://192.0.2.7:99999", false, ""); // Bug #3897
  addTest("udp4://google-public-dns-a.google.com", true, "udp4://8.8.8.8:6363");
  addTest("udp4://google-public-dns-a.google.com:70000", false, "");
  addTest("udp4://invalid.invalid", false, "");

  // IPv4 multicast
  addTest("udp4://224.0.23.170:56363", true, "udp4://224.0.23.170:56363");
  addTest("udp4://224.0.23.170", true, "udp4://224.0.23.170:56363");
  addTest("udp4://all-routers.mcast.net:56363", true, "udp4://224.0.0.2:56363");

  // IPv6 used with udp4 protocol - not canonical
  addTest("udp4://[2001:db8::1]:6363", false, "");

  runTests();
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(CanonizeUdpV6, 1)
BOOST_FIXTURE_TEST_CASE(CanonizeUdpV6, CanonizeFixture)
{
  SKIP_IF_IPV6_UNAVAILABLE();

  // IPv6 unicast
  addTest("udp6://[2001:db8::1]:6363", true, "udp6://[2001:db8::1]:6363");
  addTest("udp6://[2001:db8::1]", true, "udp6://[2001:db8::1]:6363");
  addTest("udp://[2001:db8::1]:6363", true, "udp6://[2001:db8::1]:6363");
  addTest("udp6://[2001:db8::01]:6363", true, "udp6://[2001:db8::1]:6363");
  addTest("udp6://[2001::db8::1]:6363", false, "");
  addTest("udp6://[2001:db8::1]:99999", false, ""); // Bug #3897
  addTest("udp6://google-public-dns-a.google.com", true, "udp6://[2001:4860:4860::8888]:6363");
  addTest("udp6://google-public-dns-a.google.com:70000", false, "");
  addTest("udp6://invalid.invalid", false, "");
  addTest("udp://invalid.invalid", false, "");

  // IPv6 multicast
  addTest("udp6://[ff02::2]:56363", true, "udp6://[ff02::2]:56363");
  addTest("udp6://[ff02::2]", true, "udp6://[ff02::2]:56363");

  // IPv4 used with udp6 protocol - not canonical
  addTest("udp6://192.0.2.1:6363", false, "");

  const auto& networkInterfaces = ndn::net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    auto name = netif->getName();
    auto index = to_string(netif->getIndex());

    addTest("udp6://[fe80::1068:dddb:fe26:fe3f%25" + name + "]:6363", true,
            "udp6://[fe80::1068:dddb:fe26:fe3f%" + name + "]:6363");

    addTest("udp6://[fe80::1068:dddb:fe26:fe3f%" + index + "]:6363", true,
            "udp6://[fe80::1068:dddb:fe26:fe3f%" + name + "]:6363");
  }

  runTests();
}

BOOST_AUTO_TEST_CASE(ParseTcp)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("tcp://random.host.name"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "tcp");
  BOOST_CHECK_EQUAL(uri.getHost(), "random.host.name");
  BOOST_CHECK_EQUAL(uri.getPort(), "");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK_EQUAL(uri.parse("tcp://192.0.2.1:"), false);
  BOOST_CHECK_EQUAL(uri.parse("tcp://[::zzzz]"), false);

  namespace ip = boost::asio::ip;

  ip::tcp::endpoint endpoint4(ip::address_v4::from_string("192.0.2.1"), 7777);
  uri = FaceUri(endpoint4);
  BOOST_CHECK_EQUAL(uri.toString(), "tcp4://192.0.2.1:7777");

  uri = FaceUri(endpoint4, "wsclient");
  BOOST_CHECK_EQUAL(uri.toString(), "wsclient://192.0.2.1:7777");

  ip::tcp::endpoint endpoint6(ip::address_v6::from_string("2001:DB8::1"), 7777);
  uri = FaceUri(endpoint6);
  BOOST_CHECK_EQUAL(uri.toString(), "tcp6://[2001:db8::1]:7777");

  BOOST_CHECK(uri.parse("tcp6://[fe80::1%25eth1]:6363"));
  BOOST_CHECK_EQUAL(uri.getHost(), "fe80::1%25eth1");

  BOOST_CHECK(uri.parse("tcp6://[fe80::1%eth1]:6363"));
  BOOST_CHECK_EQUAL(uri.getHost(), "fe80::1%eth1");

  BOOST_CHECK(uri.parse("tcp6://[fe80::1%1]:6363"));
  BOOST_CHECK(uri.parse("tcp6://[fe80::1%eth1]"));
}

BOOST_FIXTURE_TEST_CASE(IsCanonicalTcp, CanonizeFixture)
{
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("tcp"), true);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("tcp4"), true);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("tcp6"), true);

  BOOST_CHECK_EQUAL(FaceUri("tcp4://192.0.2.1:6363").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("tcp://192.0.2.1:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp4://192.0.2.1").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp4://192.0.2.1:6363/").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp6://[2001:db8::1]:6363").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("tcp6://[2001:db8::01]:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp://[2001:db8::1]:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp://example.net:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp4://example.net:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp6://example.net:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp4://224.0.23.170:56363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp4://[2001:db8::1]:6363").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("tcp6://192.0.2.1:6363").isCanonical(), false);

  const auto& networkInterfaces = ndn::net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    auto name = netif->getName();
    auto index = to_string(netif->getIndex());

    BOOST_CHECK_EQUAL(FaceUri("tcp6://[fe80::1%" + name + "]:6363").isCanonical(), true);
    BOOST_CHECK_EQUAL(FaceUri("tcp6://[fe80::1%" + index + "]:6363").isCanonical(), false);
    BOOST_CHECK_EQUAL(FaceUri("tcp6://[fe80::1%" + name + "]").isCanonical(), false);
    BOOST_CHECK_EQUAL(FaceUri("tcp6://[fe80::1068:dddb:fe26:fe3f%25en0]:6363").isCanonical(), false);
  }
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(CanonizeTcpV4, 1)
BOOST_FIXTURE_TEST_CASE(CanonizeTcpV4, CanonizeFixture)
{
  SKIP_IF_IPV4_UNAVAILABLE();

  // IPv4 unicast
  addTest("tcp4://192.0.2.1:6363", true, "tcp4://192.0.2.1:6363");
  addTest("tcp://192.0.2.2:6363", true, "tcp4://192.0.2.2:6363");
  addTest("tcp4://192.0.2.3", true, "tcp4://192.0.2.3:6363");
  addTest("tcp4://192.0.2.4:6363/", true, "tcp4://192.0.2.4:6363");
  addTest("tcp4://192.0.2.5:9695", true, "tcp4://192.0.2.5:9695");
  addTest("tcp4://192.0.2.666:6363", false, "");
  addTest("tcp4://192.0.2.7:99999", false, ""); // Bug #3897
  addTest("tcp4://google-public-dns-a.google.com", true, "tcp4://8.8.8.8:6363");
  addTest("tcp4://google-public-dns-a.google.com:70000", false, "");
  addTest("tcp4://invalid.invalid", false, "");

  // IPv4 multicast
  addTest("tcp4://224.0.23.170:56363", false, "");
  addTest("tcp4://224.0.23.170", false, "");
  addTest("tcp4://all-routers.mcast.net:56363", false, "");

  // IPv6 used with tcp4 protocol - not canonical
  addTest("tcp4://[2001:db8::1]:6363", false, "");

  const auto& networkInterfaces = ndn::net::tests::collectNetworkInterfaces();
  if (!networkInterfaces.empty()) {
    const auto& netif = networkInterfaces.front();
    auto name = netif->getName();
    auto index = to_string(netif->getIndex());

    addTest("tcp6://[fe80::1068:dddb:fe26:fe3f%25" + name + "]:6363", true,
            "tcp6://[fe80::1068:dddb:fe26:fe3f%" + name + "]:6363");

    addTest("tcp6://[fe80::1068:dddb:fe26:fe3f%" + index + "]:6363", true,
            "tcp6://[fe80::1068:dddb:fe26:fe3f%" + name + "]:6363");
  }

  runTests();
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(CanonizeTcpV6, 1)
BOOST_FIXTURE_TEST_CASE(CanonizeTcpV6, CanonizeFixture)
{
  SKIP_IF_IPV6_UNAVAILABLE();

  // IPv6 unicast
  addTest("tcp6://[2001:db8::1]:6363", true, "tcp6://[2001:db8::1]:6363");
  addTest("tcp6://[2001:db8::1]", true, "tcp6://[2001:db8::1]:6363");
  addTest("tcp://[2001:db8::1]:6363", true, "tcp6://[2001:db8::1]:6363");
  addTest("tcp6://[2001:db8::01]:6363", true, "tcp6://[2001:db8::1]:6363");
  addTest("tcp6://[2001::db8::1]:6363", false, "");
  addTest("tcp6://[2001:db8::1]:99999", false, ""); // Bug #3897
  addTest("tcp6://google-public-dns-a.google.com", true, "tcp6://[2001:4860:4860::8888]:6363");
  addTest("tcp6://google-public-dns-a.google.com:70000", false, "");
  addTest("tcp6://invalid.invalid", false, "");
  addTest("tcp://invalid.invalid", false, "");

  // IPv6 multicast
  addTest("tcp6://[ff02::2]:56363", false, "");
  addTest("tcp6://[ff02::2]", false, "");

  // IPv4 used with tcp6 protocol - not canonical
  addTest("tcp6://192.0.2.1:6363", false, "");

  runTests();
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(ParseUnix, 1)
BOOST_AUTO_TEST_CASE(ParseUnix)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("unix:///var/run/example.sock"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "unix");
  BOOST_CHECK_EQUAL(uri.getHost(), "");
  BOOST_CHECK_EQUAL(uri.getPort(), "");
  BOOST_CHECK_EQUAL(uri.getPath(), "/var/run/example.sock");

  // This is not a valid unix:// URI, but the parse would treat "var" as host
  BOOST_CHECK_EQUAL(uri.parse("unix://var/run/example.sock"), false); // Bug #3896

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
  using boost::asio::local::stream_protocol;
  stream_protocol::endpoint endpoint("/var/run/example.sock");
  uri = FaceUri(endpoint);
  BOOST_CHECK_EQUAL(uri.toString(), "unix:///var/run/example.sock");
#endif // BOOST_ASIO_HAS_LOCAL_SOCKETS
}

BOOST_AUTO_TEST_CASE(ParseFd)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("fd://6"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "fd");
  BOOST_CHECK_EQUAL(uri.getHost(), "6");
  BOOST_CHECK_EQUAL(uri.getPort(), "");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  int fd = 21;
  uri = FaceUri::fromFd(fd);
  BOOST_CHECK_EQUAL(uri.toString(), "fd://21");
}

BOOST_AUTO_TEST_CASE(ParseEther)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("ether://[08:00:27:01:dd:01]"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "ether");
  BOOST_CHECK_EQUAL(uri.getHost(), "08:00:27:01:dd:01");
  BOOST_CHECK_EQUAL(uri.getPort(), "");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK_EQUAL(uri.parse("ether://[08:00:27:zz:dd:01]"), false);

  auto address = ethernet::Address::fromString("33:33:01:01:01:01");
  uri = FaceUri(address);
  BOOST_CHECK_EQUAL(uri.toString(), "ether://[33:33:01:01:01:01]");
}

BOOST_FIXTURE_TEST_CASE(CanonizeEther, CanonizeFixture)
{
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("ether"), true);

  BOOST_CHECK_EQUAL(FaceUri("ether://[08:00:27:01:01:01]").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("ether://[08:00:27:1:1:1]").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("ether://[08:00:27:01:01:01]/").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("ether://[33:33:01:01:01:01]").isCanonical(), true);

  addTest("ether://[08:00:27:01:01:01]", true, "ether://[08:00:27:01:01:01]");
  addTest("ether://[08:00:27:1:1:1]", true, "ether://[08:00:27:01:01:01]");
  addTest("ether://[08:00:27:01:01:01]/", true, "ether://[08:00:27:01:01:01]");
  addTest("ether://[33:33:01:01:01:01]", true, "ether://[33:33:01:01:01:01]");

  runTests();
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(ParseDev, 1)
BOOST_AUTO_TEST_CASE(ParseDev)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("dev://eth0"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "dev");
  BOOST_CHECK_EQUAL(uri.getHost(), "eth0");
  BOOST_CHECK_EQUAL(uri.getPort(), "");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK_EQUAL(uri.parse("dev://eth0:8888"), false); // Bug #3896

  std::string ifname = "en1";
  uri = FaceUri::fromDev(ifname);
  BOOST_CHECK_EQUAL(uri.toString(), "dev://en1");
}

BOOST_AUTO_TEST_CASE(IsCanonicalDev)
{
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("dev"), true);

  BOOST_CHECK_EQUAL(FaceUri("dev://eth0").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("dev://").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("dev://eth0:8888").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("dev://eth0/").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("dev://eth0/A").isCanonical(), false);
}

BOOST_FIXTURE_TEST_CASE(CanonizeDev, CanonizeFixture)
{
  addTest("dev://eth0", true, "dev://eth0");
  addTest("dev://", false, "");
  addTest("dev://eth0:8888", false, "");
  addTest("dev://eth0/", true, "dev://eth0");
  addTest("dev://eth0/A", false, "");

  runTests();
}

BOOST_AUTO_TEST_CASE(ParseUdpDev)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("udp4+dev://eth0:7777"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "udp4+dev");
  BOOST_CHECK_EQUAL(uri.getHost(), "eth0");
  BOOST_CHECK_EQUAL(uri.getPort(), "7777");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK(uri.parse("udp6+dev://eth1:7777"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "udp6+dev");
  BOOST_CHECK_EQUAL(uri.getHost(), "eth1");
  BOOST_CHECK_EQUAL(uri.getPort(), "7777");
  BOOST_CHECK_EQUAL(uri.getPath(), "");

  BOOST_CHECK(uri.parse("abc+efg://eth0"));
  BOOST_CHECK(!uri.parse("abc+://eth0"));
  BOOST_CHECK(!uri.parse("+abc://eth0"));

  namespace ip = boost::asio::ip;

  ip::udp::endpoint endpoint4(ip::udp::v4(), 7777);
  uri = FaceUri::fromUdpDev(endpoint4, "en1");
  BOOST_CHECK_EQUAL(uri.toString(), "udp4+dev://en1:7777");

  ip::udp::endpoint endpoint6(ip::udp::v6(), 7777);
  uri = FaceUri::fromUdpDev(endpoint6, "en2");
  BOOST_CHECK_EQUAL(uri.toString(), "udp6+dev://en2:7777");
}

BOOST_FIXTURE_TEST_CASE(CanonizeUdpDev, CanonizeFixture)
{
  BOOST_CHECK_EQUAL(FaceUri("udp4+dev://eth0:7777").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("udp6+dev://eth1:7777").isCanonical(), true);
  BOOST_CHECK_EQUAL(FaceUri("udp+dev://eth1:7777").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("udp6+dev://eth1").isCanonical(), false);

  addTest("udp4+dev://en0:7777", true, "udp4+dev://en0:7777");
  addTest("udp6+dev://en0:7777", true, "udp6+dev://en0:7777");
  addTest("udp+dev://en1:7777", false, "");
  addTest("udp6+dev://en2", false, "");
}

BOOST_AUTO_TEST_CASE(CanonizeEmptyCallback)
{
  boost::asio::io_service io;

  // unsupported scheme
  FaceUri("null://").canonize(FaceUri::CanonizeSuccessCallback(),
                              FaceUri::CanonizeFailureCallback(),
                              io, 1_ms);

  // cannot resolve
  FaceUri("udp://192.0.2.333").canonize(FaceUri::CanonizeSuccessCallback(),
                                        FaceUri::CanonizeFailureCallback(),
                                        io, 1_ms);

  // already canonical
  FaceUri("udp4://192.0.2.1:6363").canonize(FaceUri::CanonizeSuccessCallback(),
                                            FaceUri::CanonizeFailureCallback(),
                                            io, 1_ms);

  // need DNS resolution
  FaceUri("udp://192.0.2.1:6363").canonize(FaceUri::CanonizeSuccessCallback(),
                                           FaceUri::CanonizeFailureCallback(),
                                           io, 1_ms);

  io.run(); // should not crash

  // avoid "test case [...] did not check any assertions" message from Boost.Test
  BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE(CanonizeUnsupported, CanonizeFixture)
{
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("internal"), false);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("null"), false);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("unix"), false);
  BOOST_CHECK_EQUAL(FaceUri::canCanonize("fd"), false);

  BOOST_CHECK_EQUAL(FaceUri("internal://").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("null://").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("unix:///var/run/nfd.sock").isCanonical(), false);
  BOOST_CHECK_EQUAL(FaceUri("fd://0").isCanonical(), false);

  addTest("internal://", false, "");
  addTest("null://", false, "");
  addTest("unix:///var/run/nfd.sock", false, "");
  addTest("fd://0", false, "");

  runTests();
}

BOOST_AUTO_TEST_CASE(Bug1635)
{
  FaceUri uri;

  BOOST_CHECK(uri.parse("wsclient://[::ffff:76.90.11.239]:56366"));
  BOOST_CHECK_EQUAL(uri.getScheme(), "wsclient");
  BOOST_CHECK_EQUAL(uri.getHost(), "76.90.11.239");
  BOOST_CHECK_EQUAL(uri.getPort(), "56366");
  BOOST_CHECK_EQUAL(uri.getPath(), "");
  BOOST_CHECK_EQUAL(uri.toString(), "wsclient://76.90.11.239:56366");
}

BOOST_AUTO_TEST_SUITE_END() // TestFaceUri
BOOST_AUTO_TEST_SUITE_END() // Net

} // namespace tests
} // namespace ndn
