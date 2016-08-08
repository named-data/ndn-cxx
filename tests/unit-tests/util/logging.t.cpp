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

#include "util/logging.hpp"
#include "util/logger.hpp"

#include "boost-test.hpp"
#include <boost/test/output_test_stream.hpp>
#include "../unit-test-time-fixture.hpp"

namespace ndn {
namespace util {
namespace tests {

using namespace ndn::tests;
using boost::test_tools::output_test_stream;

void
logFromModule1();

void
logFromModule2();

void
logFromNewLogger(const std::string& moduleName)
{
  // clang complains -Wreturn-stack-address on OSX-10.9 if Logger is allocated on stack
  auto loggerPtr = make_unique<Logger>(moduleName);
  Logger& logger = *loggerPtr;

  auto getNdnCxxLogger = [&logger] () -> Logger& { return logger; };
  NDN_LOG_TRACE("trace" << moduleName);
  NDN_LOG_DEBUG("debug" << moduleName);
  NDN_LOG_INFO("info" << moduleName);
  NDN_LOG_WARN("warn" << moduleName);
  NDN_LOG_ERROR("error" << moduleName);
  NDN_LOG_FATAL("fatal" << moduleName);

  BOOST_CHECK(Logging::get().removeLogger(logger));
}

const time::system_clock::Duration LOG_SYSTIME = time::microseconds(1468108800311239LL);
const std::string LOG_SYSTIME_STR = "1468108800.311239";

class LoggingFixture : public UnitTestTimeFixture
{
protected:
  explicit
  LoggingFixture()
    : m_oldLevels(Logging::get().getLevels())
    , m_oldDestination(Logging::get().getDestination())
  {
    this->systemClock->setNow(LOG_SYSTIME);
    Logging::get().resetLevels();
    Logging::setDestination(os);
  }

  ~LoggingFixture()
  {
    Logging::setLevel(m_oldLevels);
    Logging::setDestination(m_oldDestination);
  }

protected:
  output_test_stream os;

private:
  std::string m_oldLevels;
  shared_ptr<std::ostream> m_oldDestination;
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestLogging, LoggingFixture)

BOOST_AUTO_TEST_SUITE(Severity)

BOOST_AUTO_TEST_CASE(None)
{
  Logging::setLevel("Module1", LogLevel::NONE);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_CASE(Error)
{
  Logging::setLevel("Module1", LogLevel::ERROR);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_CASE(Warn)
{
  Logging::setLevel("Module1", LogLevel::WARN);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_CASE(Info)
{
  Logging::setLevel("Module1", LogLevel::INFO);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " INFO: [Module1] info1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_CASE(Debug)
{
  Logging::setLevel("Module1", LogLevel::DEBUG);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " DEBUG: [Module1] debug1\n" +
    LOG_SYSTIME_STR + " INFO: [Module1] info1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_CASE(Trace)
{
  Logging::setLevel("Module1", LogLevel::TRACE);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " TRACE: [Module1] trace1\n" +
    LOG_SYSTIME_STR + " DEBUG: [Module1] debug1\n" +
    LOG_SYSTIME_STR + " INFO: [Module1] info1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_CASE(All)
{
  Logging::setLevel("Module1", LogLevel::ALL);
  logFromModule1();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " TRACE: [Module1] trace1\n" +
    LOG_SYSTIME_STR + " DEBUG: [Module1] debug1\n" +
    LOG_SYSTIME_STR + " INFO: [Module1] info1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
}

BOOST_AUTO_TEST_SUITE_END() // Severity

BOOST_AUTO_TEST_CASE(SameNameLoggers)
{
  Logging::setLevel("Module1", LogLevel::WARN);
  logFromModule1();
  logFromNewLogger("Module1");

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module1] warnModule1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] errorModule1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatalModule1\n"
    ));
}

BOOST_AUTO_TEST_CASE(LateRegistration)
{
  BOOST_CHECK_NO_THROW(Logging::setLevel("Module3", LogLevel::DEBUG));
  logFromNewLogger("Module3");

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " DEBUG: [Module3] debugModule3\n" +
    LOG_SYSTIME_STR + " INFO: [Module3] infoModule3\n" +
    LOG_SYSTIME_STR + " WARNING: [Module3] warnModule3\n" +
    LOG_SYSTIME_STR + " ERROR: [Module3] errorModule3\n" +
    LOG_SYSTIME_STR + " FATAL: [Module3] fatalModule3\n"
    ));
}

BOOST_AUTO_TEST_SUITE(DefaultSeverity)

BOOST_AUTO_TEST_CASE(Unset)
{
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(NoOverride)
{
  Logging::setLevel("*", LogLevel::WARN);
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module2] warn2\n" +
    LOG_SYSTIME_STR + " ERROR: [Module2] error2\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(Override)
{
  Logging::setLevel("*", LogLevel::WARN);
  Logging::setLevel("Module2", LogLevel::DEBUG);
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " DEBUG: [Module2] debug2\n" +
    LOG_SYSTIME_STR + " INFO: [Module2] info2\n" +
    LOG_SYSTIME_STR + " WARNING: [Module2] warn2\n" +
    LOG_SYSTIME_STR + " ERROR: [Module2] error2\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_SUITE_END() // DefaultSeverity

BOOST_AUTO_TEST_SUITE(SeverityConfig)

BOOST_AUTO_TEST_CASE(SetEmpty)
{
  Logging::setLevel("");
  BOOST_CHECK_EQUAL(Logging::get().getLevels(), "");
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(SetDefault)
{
  Logging::setLevel("*=WARN");
  BOOST_CHECK_EQUAL(Logging::get().getLevels(), "*=WARN");
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " WARNING: [Module2] warn2\n" +
    LOG_SYSTIME_STR + " ERROR: [Module2] error2\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(SetModule)
{
  Logging::setLevel("Module1=ERROR");
  BOOST_CHECK_EQUAL(Logging::get().getLevels(), "Module1=ERROR");
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(SetOverride)
{
  Logging::setLevel("*=WARN:Module2=DEBUG");
  BOOST_CHECK_EQUAL(Logging::get().getLevels(), "*=WARN:Module2=DEBUG");
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " DEBUG: [Module2] debug2\n" +
    LOG_SYSTIME_STR + " INFO: [Module2] info2\n" +
    LOG_SYSTIME_STR + " WARNING: [Module2] warn2\n" +
    LOG_SYSTIME_STR + " ERROR: [Module2] error2\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(SetTwice)
{
  Logging::setLevel("*=WARN");
  Logging::setLevel("Module2=DEBUG");
  BOOST_CHECK_EQUAL(Logging::get().getLevels(), "*=WARN:Module2=DEBUG");
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " WARNING: [Module1] warn1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " DEBUG: [Module2] debug2\n" +
    LOG_SYSTIME_STR + " INFO: [Module2] info2\n" +
    LOG_SYSTIME_STR + " WARNING: [Module2] warn2\n" +
    LOG_SYSTIME_STR + " ERROR: [Module2] error2\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(Reset)
{
  Logging::setLevel("Module2=DEBUG");
  Logging::setLevel("*=ERROR");
  BOOST_CHECK_EQUAL(Logging::get().getLevels(), "*=ERROR");
  logFromModule1();
  logFromModule2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " ERROR: [Module1] error1\n" +
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " ERROR: [Module2] error2\n" +
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));
}

BOOST_AUTO_TEST_CASE(Malformed)
{
  BOOST_CHECK_THROW(Logging::setLevel("Module1=INVALID-LEVEL"), std::invalid_argument);
  BOOST_CHECK_THROW(Logging::setLevel("Module1-MISSING-EQUAL-SIGN"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // SeverityConfig

BOOST_AUTO_TEST_CASE(ChangeDestination)
{
  logFromModule1();

  auto os2 = make_shared<output_test_stream>();
  Logging::setDestination(os2);
  weak_ptr<output_test_stream> os2weak(os2);
  os2.reset();

  logFromModule2();

  Logging::flush();
  os2 = os2weak.lock();
  BOOST_REQUIRE(os2 != nullptr);

  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n"
    ));
  BOOST_CHECK(os2->is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module2] fatal2\n"
    ));

  os2.reset();
  Logging::setDestination(os);
  BOOST_CHECK(os2weak.expired());
}

BOOST_AUTO_TEST_SUITE_END() // TestLogging
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
