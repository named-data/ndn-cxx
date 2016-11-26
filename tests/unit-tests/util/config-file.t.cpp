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

#include "util/config-file.hpp"
#include "../test-home-env-saver.hpp"

#include "boost-test.hpp"

#include <cstdlib>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestConfigFile, TestHomeEnvSaver)

BOOST_AUTO_TEST_CASE(Parse)
{
  namespace fs = boost::filesystem;

  setenv("TEST_HOME", "tests/unit-tests/util/config-file-home", 1);

  fs::path homePath(fs::absolute(std::getenv("TEST_HOME")));
  homePath /= ".ndn/client.conf";

  ConfigFile config;
  BOOST_REQUIRE_EQUAL(config.getPath(), homePath);

  const ConfigFile::Parsed& parsed = config.getParsedConfiguration();
  BOOST_CHECK_EQUAL(parsed.get<std::string>("a"), "/path/to/nowhere");
  BOOST_CHECK_EQUAL(parsed.get<std::string>("b"), "some-othervalue.01");
}

BOOST_AUTO_TEST_CASE(ParseEmptyPath)
{
  setenv("TEST_HOME", "tests/unit-tests/util/does/not/exist", 1);

  BOOST_CHECK_NO_THROW(ConfigFile config);
}

BOOST_AUTO_TEST_CASE(ParseMalformed)
{
  setenv("TEST_HOME", "tests/unit-tests/util/config-file-malformed-home", 1);

  BOOST_CHECK_THROW(ConfigFile config, ConfigFile::Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestConfigFile
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace ndn
