/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
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
  setenv("HOME", "tests/transport/test-homes/ok", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/tmp/test/nfd.sock");
}

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameMissingSocketMissingProtocol)
{
  setenv("HOME", "tests/transport/test-homes/missing-unix-socket-missing-protocol", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameMissingSocketNdndProtocol)
{
  setenv("HOME", "tests/transport/test-homes/missing-unix-socket-with-ndnd-protocol", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/tmp/.ndnd.sock");
}

BOOST_AUTO_TEST_CASE(TestGetDefaultSocketNameMissingSocketWithProtocol)
{
  setenv("HOME", "tests/transport/test-homes/missing-unix-socket-with-protocol", 1);
  ConfigFile config;
  BOOST_REQUIRE_EQUAL(UnixTransport::getDefaultSocketName(config), "/var/run/nfd.sock");
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
