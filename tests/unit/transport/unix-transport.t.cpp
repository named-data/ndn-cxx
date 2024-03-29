/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#include "ndn-cxx/transport/unix-transport.hpp"

#include "tests/boost-test.hpp"

namespace ndn::tests {

BOOST_AUTO_TEST_SUITE(Transport)
BOOST_AUTO_TEST_SUITE(TestUnixTransport)

using ndn::Transport;

BOOST_AUTO_TEST_CASE(GetSocketNameFromUri)
{
  BOOST_CHECK_EQUAL(UnixTransport::getSocketNameFromUri("unix:///tmp/test/nfd.sock"), "/tmp/test/nfd.sock");
#ifdef __linux__
  BOOST_CHECK_EQUAL(UnixTransport::getSocketNameFromUri(""), "/run/nfd/nfd.sock");
#else
  BOOST_CHECK_EQUAL(UnixTransport::getSocketNameFromUri(""), "/var/run/nfd/nfd.sock");
#endif // __linux__
  BOOST_CHECK_EXCEPTION(UnixTransport::getSocketNameFromUri("tcp://"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == "Cannot create UnixTransport from \"tcp\" URI"s;
                        });
  BOOST_CHECK_EXCEPTION(UnixTransport::getSocketNameFromUri("unix"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == "Malformed URI: unix"s;
                        });
}

BOOST_AUTO_TEST_SUITE_END() // TestUnixTransport
BOOST_AUTO_TEST_SUITE_END() // Transport

} // namespace ndn::tests
