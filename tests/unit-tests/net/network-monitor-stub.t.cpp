/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "net/network-monitor-stub.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace net {
namespace tests {

BOOST_AUTO_TEST_SUITE(Net)
BOOST_AUTO_TEST_SUITE(TestNetworkMonitorStub)

BOOST_AUTO_TEST_CASE(Capabilities)
{
  NetworkMonitorStub stub(NetworkMonitor::CAP_ENUM | NetworkMonitor::CAP_IF_ADD_REMOVE);
  BOOST_CHECK_EQUAL(stub.getCapabilities(),
                    NetworkMonitor::CAP_ENUM | NetworkMonitor::CAP_IF_ADD_REMOVE);
}

class StubFixture
{
public:
  StubFixture()
    : stub(~0)
  {
    stub.onEnumerationCompleted.connect([this] { signals.push_back("EnumerationCompleted"); });
    stub.onInterfaceAdded.connect([this] (shared_ptr<const NetworkInterface> netif) {
      signals.push_back("InterfaceAdded " + netif->getName()); });
    stub.onInterfaceRemoved.connect([this] (shared_ptr<const NetworkInterface> netif) {
      signals.push_back("InterfaceRemoved " + netif->getName()); });
  }

public:
  NetworkMonitorStub stub;
  std::vector<std::string> signals;
};

BOOST_FIXTURE_TEST_CASE(AddInterface, StubFixture)
{
  BOOST_CHECK_EQUAL(stub.listNetworkInterfaces().size(), 0);
  BOOST_CHECK(stub.getNetworkInterface("eth1") == nullptr);

  shared_ptr<NetworkInterface> if1 = stub.makeNetworkInterface();
  if1->setIndex(13697);
  if1->setName("eth1");
  stub.addInterface(if1);
  if1.reset();
  BOOST_REQUIRE(stub.getNetworkInterface("eth1") != nullptr);
  BOOST_CHECK_EQUAL(stub.getNetworkInterface("eth1")->getIndex(), 13697);
  BOOST_CHECK_EQUAL(stub.listNetworkInterfaces().at(0)->getIndex(), 13697);
  BOOST_REQUIRE_EQUAL(signals.size(), 1);
  BOOST_CHECK_EQUAL(signals.back(), "InterfaceAdded eth1");

  shared_ptr<NetworkInterface> if1b = stub.makeNetworkInterface();
  if1b->setIndex(3280);
  if1b->setName("eth1");
  BOOST_CHECK_THROW(stub.addInterface(if1b), std::invalid_argument);
  BOOST_CHECK(stub.getNetworkInterface("eth1") != nullptr);
  BOOST_CHECK_EQUAL(stub.getNetworkInterface("eth1")->getIndex(), 13697);
  BOOST_CHECK_EQUAL(stub.listNetworkInterfaces().at(0)->getIndex(), 13697);
  BOOST_CHECK_EQUAL(signals.size(), 1);

  stub.emitEnumerationCompleted();
  BOOST_REQUIRE_EQUAL(signals.size(), 2);
  BOOST_CHECK_EQUAL(signals.back(), "EnumerationCompleted");

  shared_ptr<NetworkInterface> if2 = stub.makeNetworkInterface();
  if2->setIndex(19243);
  if2->setName("eth2");
  stub.addInterface(if2);
  if2.reset();
  BOOST_REQUIRE(stub.getNetworkInterface("eth2") != nullptr);
  BOOST_CHECK_EQUAL(stub.getNetworkInterface("eth2")->getIndex(), 19243);
  BOOST_CHECK_EQUAL(stub.listNetworkInterfaces().size(), 2);
  BOOST_REQUIRE_EQUAL(signals.size(), 3);
  BOOST_CHECK_EQUAL(signals.back(), "InterfaceAdded eth2");
}

BOOST_FIXTURE_TEST_CASE(RemoveInterface, StubFixture)
{
  shared_ptr<NetworkInterface> if1 = stub.makeNetworkInterface();
  if1->setIndex(13697);
  if1->setName("eth1");
  stub.addInterface(if1);

  stub.emitEnumerationCompleted();
  BOOST_REQUIRE_EQUAL(signals.size(), 2);
  BOOST_CHECK_EQUAL(signals.back(), "EnumerationCompleted");

  stub.removeInterface("eth1");
  BOOST_CHECK(stub.getNetworkInterface("eth1") == nullptr);
  BOOST_CHECK_EQUAL(stub.listNetworkInterfaces().size(), 0);
  BOOST_REQUIRE_EQUAL(signals.size(), 3);
  BOOST_CHECK_EQUAL(signals.back(), "InterfaceRemoved eth1");

  stub.removeInterface("eth2"); // non-existent
  BOOST_CHECK_EQUAL(signals.size(), 3);
}

BOOST_AUTO_TEST_SUITE_END() // TestNetworkMonitorStub
BOOST_AUTO_TEST_SUITE_END() // Net

} // namespace tests
} // namespace net
} // namespace ndn
