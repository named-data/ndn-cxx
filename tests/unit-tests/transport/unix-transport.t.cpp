/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "transport/unix-transport.hpp"
#include "transport-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Transport)
BOOST_FIXTURE_TEST_SUITE(TestUnixTransport, TransportFixture)

using ndn::Transport;

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameOk)
{
  BOOST_CHECK_EQUAL(UnixTransport::getSocketNameFromUri("unix:///tmp/test/nfd.sock"), "/tmp/test/nfd.sock");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameOkOmittedSocketOmittedProtocol)
{
  BOOST_CHECK_EQUAL(UnixTransport::getSocketNameFromUri(""), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameBadWrongTransport)
{
  BOOST_CHECK_EXCEPTION(UnixTransport::getSocketNameFromUri("tcp://"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == std::string("Cannot create UnixTransport "
                                                             "from \"tcp\" URI");
                        });
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameBadMalformedUri)
{
  BOOST_CHECK_EXCEPTION(UnixTransport::getSocketNameFromUri("unix"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == std::string("Malformed URI: unix");
                        });
}

BOOST_AUTO_TEST_SUITE_END() // TestUnixTransport
BOOST_AUTO_TEST_SUITE_END() // Transport

} // namespace tests
} // namespace ndn
