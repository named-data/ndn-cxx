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

#include "util/config-file.hpp"
#include "../util/test-home-environment-fixture.hpp"

#include <cstdlib>

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(UtilConfigFile, util::TestHomeEnvironmentFixture)

BOOST_AUTO_TEST_CASE(TestParse)
{
  using namespace boost::filesystem;
  // std::cerr << "current home = " << std::getenv("TEST_HOME") << std::endl;

  setenv("TEST_HOME", "tests/unit-tests/util/config-file-home", 1);

  path homePath(absolute(std::getenv("TEST_HOME")));
  homePath /= ".ndn/client.conf";

  try
    {
      ConfigFile config;

      BOOST_REQUIRE_EQUAL(config.getPath(), homePath);

      const ConfigFile::Parsed& parsed = config.getParsedConfiguration();
      BOOST_CHECK_EQUAL(parsed.get<std::string>("a"), "/path/to/nowhere");
      BOOST_CHECK_EQUAL(parsed.get<std::string>("b"), "some-othervalue.01");
    }
  catch (const std::runtime_error& error)
    {
      BOOST_FAIL("Unexpected exception: " << error.what());
    }
}

BOOST_AUTO_TEST_CASE(EmptyPathParse)
{
  // std::cerr << "current home = " << std::getenv("TEST_HOME") << std::endl;

  setenv("TEST_HOME", "tests/unit-tests/util/does/not/exist", 1);
  try
    {
      ConfigFile config;
    }
  catch (const std::runtime_error& error)
    {
      BOOST_FAIL("Unexpected exception: " << error.what());
    }
}

BOOST_AUTO_TEST_CASE(MalformedParse)
{
  using namespace boost::filesystem;
  // std::cerr << "current home = " << std::getenv("TEST_HOME") << std::endl;

  setenv("TEST_HOME", "tests/unit-tests/util/config-file-malformed-home", 1);

  bool fileWasMalformed = false;
  try
    {
      ConfigFile config;
    }
  catch (const ConfigFile::Error& error)
    {
      fileWasMalformed = true;
    }

  BOOST_REQUIRE(fileWasMalformed);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
