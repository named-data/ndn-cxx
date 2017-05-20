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

#include "util/network-monitor.hpp"

#include "boost-test.hpp"
#include <boost/asio/io_service.hpp>

namespace ndn {
namespace util {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestNetworkMonitor)

BOOST_AUTO_TEST_CASE(DestructWithoutRun)
{
#if defined(NDN_CXX_HAVE_RTNETLINK) || defined(NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H)
  boost::asio::io_service io;
  auto nm = make_unique<NetworkMonitor>(io);
  nm.reset();
#endif
  BOOST_CHECK(true); // if we got this far, the test passed
}

BOOST_AUTO_TEST_CASE(DestructWhileEnumerating)
{
#ifdef NDN_CXX_HAVE_RTNETLINK
  boost::asio::io_service io;
  auto nm = make_unique<NetworkMonitor>(io);

  nm->onInterfaceAdded.connect([&] (const shared_ptr<NetworkInterface>&) {
    io.post([&] { nm.reset(); });
  });
  nm->onEnumerationCompleted.connect([&] {
    BOOST_CHECK_EQUAL(nm->listNetworkInterfaces().size(), 0);
    // make sure the test case terminates even if we have zero interfaces
    io.post([&] { nm.reset(); });
  });

  io.run();
#endif
  BOOST_CHECK(true); // if we got this far, the test passed
}

BOOST_AUTO_TEST_SUITE_END() // TestNetworkMonitor
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
