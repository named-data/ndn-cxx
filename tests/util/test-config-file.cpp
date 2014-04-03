/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

#include "util/config-file.hpp"

#include <cstdlib>

#include "boost-test.hpp"

namespace ndn {

class ConfigFileFixture
{
public:
  ConfigFileFixture()
  {
    m_HOME = std::getenv("HOME");
  }

  ~ConfigFileFixture()
  {
    setenv("HOME", m_HOME.c_str(), 1);
    // std::cerr << "restoring home = " << m_HOME << std::endl;
  }

protected:
  std::string m_HOME;
};

BOOST_FIXTURE_TEST_SUITE(TestConfigFile, ConfigFileFixture)

BOOST_AUTO_TEST_CASE(TestParse)
{
  using namespace boost::filesystem;
  // std::cerr << "current home = " << std::getenv("HOME") << std::endl;

  setenv("HOME", "tests/util/config-file-home", 1);

  path homePath(absolute(std::getenv("HOME")));
  homePath /= ".ndn/client.conf";

  try
    {
      ConfigFile config;

      BOOST_REQUIRE_EQUAL(config.getPath(), homePath);

      const ConfigFile::Parsed& parsed = config.getParsedConfiguration();
      BOOST_CHECK_EQUAL(parsed.get<std::string>("a"), "/path/to/nowhere");
      BOOST_CHECK_EQUAL(parsed.get<std::string>("b"), "some-othervalue.01");
    }
  catch(const std::runtime_error& error)
    {
      BOOST_FAIL("Unexpected exception: " << error.what());
    }
}

BOOST_AUTO_TEST_CASE(EmptyPathParse)
{
  // std::cerr << "current home = " << std::getenv("HOME") << std::endl;

  setenv("HOME", "tests/util/does/not/exist", 1);
  try
    {
      ConfigFile config;
    }
  catch(const std::runtime_error& error)
    {
      BOOST_FAIL("Unexpected exception: " << error.what());
    }
}

BOOST_AUTO_TEST_CASE(MalformedParse)
{
  using namespace boost::filesystem;
  // std::cerr << "current home = " << std::getenv("HOME") << std::endl;

  setenv("HOME", "tests/util/config-file-malformed-home", 1);

  bool fileWasMalformed = false;
  try
    {
      ConfigFile config;
    }
  catch(const ConfigFile::Error& error)
    {
      fileWasMalformed = true;
    }

  BOOST_REQUIRE(fileWasMalformed);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
