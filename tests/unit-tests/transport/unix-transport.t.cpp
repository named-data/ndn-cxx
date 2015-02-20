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

#include "transport/unix-transport.hpp"
#include "transport-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(TransportUnixTransport, TransportFixture)

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameOk)
{
  initializeConfig("tests/unit-tests/transport/test-homes/unix-transport/ok");

  BOOST_CHECK_EQUAL(UnixTransport::getDefaultSocketName(*m_config), "/tmp/test/nfd.sock");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameOkOmittedSocketOmittedProtocol)
{
  initializeConfig("tests/unit-tests/transport/test-homes/unix-transport/"
                   "ok-omitted-unix-socket-omitted-protocol");

  BOOST_CHECK_EQUAL(UnixTransport::getDefaultSocketName(*m_config), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameOkOmittedSocketWithProtocol)
{
  initializeConfig("tests/unit-tests/transport/test-homes/unix-transport/"
                   "ok-omitted-unix-socket-with-protocol");

  BOOST_CHECK_EQUAL(UnixTransport::getDefaultSocketName(*m_config), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameBadWrongTransport)
{
  initializeConfig("tests/unit-tests/transport/test-homes/unix-transport/"
                   "bad-wrong-transport");

  BOOST_CHECK_EXCEPTION(UnixTransport::getDefaultSocketName(*m_config),
                        Transport::Error,
                        [] (const Transport::Error& error) {
                          return error.what() == std::string("Cannot create UnixTransport "
                                                             "from \"tcp\" URI");
                        });
}

BOOST_AUTO_TEST_CASE(GetDefaultSocketNameBadMalformedUri)
{
  initializeConfig("tests/unit-tests/transport/test-homes/unix-transport/"
                   "bad-malformed-uri");

  BOOST_CHECK_EXCEPTION(UnixTransport::getDefaultSocketName(*m_config),
                        ConfigFile::Error,
                        [] (const ConfigFile::Error& error) {
                          return error.what() == std::string("Malformed URI: unix");
                        });
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
