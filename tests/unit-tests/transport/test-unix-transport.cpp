/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
#include "util/config-file.hpp"

#include "boost-test.hpp"

namespace ndn {

class UnixTransportFixture
{
public:
  UnixTransportFixture()
  {
    m_HOME = std::getenv("HOME");
  }

  ~UnixTransportFixture()
  {
    setenv("HOME", m_HOME.c_str(), 1);
    // std::cerr << "restoring home = " << m_HOME << std::endl;
  }

protected:
  std::string m_HOME;
};

BOOST_FIXTURE_TEST_SUITE(TransportTestUnixTransport, UnixTransportFixture)

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameOk)
{
  setenv("HOME", "tests/unit-tests/transport/test-homes/ok", 1);

  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/tmp/test/nfd.sock");
}

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameMissingSocketMissingProtocol)
{
  setenv("HOME", "tests/unit-tests/transport/test-homes/missing-unix-socket-missing-protocol", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameMissingSocketNdndProtocol)
{
  setenv("HOME", "tests/unit-tests/transport/test-homes/missing-unix-socket-with-ndnd-protocol", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/tmp/.ndnd.sock");
}

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameMissingSocketWithProtocol)
{
  setenv("HOME", "tests/unit-tests/transport/test-homes/missing-unix-socket-with-protocol", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
