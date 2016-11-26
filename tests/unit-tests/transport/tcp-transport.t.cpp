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

#include "transport/tcp-transport.hpp"
#include "transport-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Transport)
BOOST_FIXTURE_TEST_SUITE(TestTcpTransport, TransportFixture)

using ndn::Transport;

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameOk)
{
  const auto got = TcpTransport::getSocketHostAndPortFromUri("tcp://127.0.0.1:6000");

  BOOST_CHECK_EQUAL(got.first, "127.0.0.1");
  BOOST_CHECK_EQUAL(got.second, "6000");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketHostAndPortBadMissingHost)
{
  BOOST_CHECK_EXCEPTION(TcpTransport::getSocketHostAndPortFromUri("tcp://:6000"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == std::string("Malformed URI: tcp://:6000");
                        });
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketHostAndPortOkOmittedPort)
{
  const auto got = TcpTransport::getSocketHostAndPortFromUri("tcp://127.0.0.1");

  BOOST_CHECK_EQUAL(got.first, "127.0.0.1");
  BOOST_CHECK_EQUAL(got.second, "6363");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketHostAndPortNameOkOmittedHostOmittedPort)
{
  const auto got = TcpTransport::getSocketHostAndPortFromUri("tcp://");

  BOOST_CHECK_EQUAL(got.first, "localhost");
  BOOST_CHECK_EQUAL(got.second, "6363");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketHostAndPortBadWrongTransport)
{
  BOOST_CHECK_EXCEPTION(TcpTransport::getSocketHostAndPortFromUri("unix://"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == std::string("Cannot create TcpTransport "
                                                             "from \"unix\" URI");
                        });
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketHostAndPortBadMalformedUri)
{
  BOOST_CHECK_EXCEPTION(TcpTransport::getSocketHostAndPortFromUri("tcp"),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == std::string("Malformed URI: tcp");
                        });
}

BOOST_AUTO_TEST_SUITE_END() // TestTcpTransport
BOOST_AUTO_TEST_SUITE_END() // Transport

} // namespace tests
} // namespace ndn
