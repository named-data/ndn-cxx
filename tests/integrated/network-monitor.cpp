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

#define BOOST_TEST_MAIN 1
#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE ndn-cxx Integrated Tests (Network Monitor)

#include "util/network-monitor.hpp"

#include "boost-test.hpp"
#include <cstdlib>
#include <iostream>
#include "util/time.hpp"

namespace ndn {
namespace util {

BOOST_AUTO_TEST_SUITE(UtilNetworkMonitor)

BOOST_AUTO_TEST_CASE(Basic)
{
  boost::asio::io_service io;
  BOOST_REQUIRE_NO_THROW((NetworkMonitor(io)));

  NetworkMonitor monitor(io);

  monitor.onNetworkStateChanged.connect([] {
      std::cout << time::toString(time::system_clock::now())
                << "\tReceived network state change event" << std::endl;
    });

  io.run();
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace util
} // namespace ndn
