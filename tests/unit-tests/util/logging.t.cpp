/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "../unit-test-time-fixture.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace util {
namespace tests {

NDN_LOG_INIT(ndn.util.tests.Logging);

void
logFromModule1();

void
logFromModule2();

void
logFromFilterModule();

static void
logFromNewLogger(const char* moduleName)
{
  Logger logger(moduleName);
  auto ndn_cxx_getLogger = [&logger] () -> Logger& { return logger; };

  NDN_LOG_TRACE("trace" << moduleName);
  NDN_LOG_DEBUG("debug" << moduleName);
  NDN_LOG_INFO("info" << moduleName);
  NDN_LOG_WARN("warn" << moduleName);
  NDN_LOG_ERROR("error" << moduleName);
  NDN_LOG_FATAL("fatal" << moduleName);

  BOOST_CHECK(Logging::get().removeLogger(logger));
}

namespace ns1 {

NDN_LOG_INIT(ndn.util.tests.ns1);

static void
logFromNamespace1()
{
  NDN_LOG_INFO("hello world from ns1");
}

} // namespace ns1

namespace ns2 {

NDN_LOG_INIT(ndn.util.tests.ns2);

static void
logFromNamespace2()
{
  NDN_LOG_INFO("hi there from ns2");
}

} // namespace ns2

class ClassWithLogger
{
public:
  void
  logFromConstMemberFunction() const
  {
    NDN_LOG_INFO("const member function");
  }

  static void
  logFromStaticMemberFunction()
  {
    NDN_LOG_INFO("static member function");
  }

private:
  NDN_LOG_MEMBER_DECL();
};

NDN_LOG_MEMBER_INIT(ClassWithLogger, ndn.util.tests.ClassWithLogger);

template<class T, class U>
class ClassTemplateWithLogger
{
public:
  void
  logFromMemberFunction()
  {
    NDN_LOG_INFO("class template non-static member function");
  }

  static void
  logFromStaticMemberFunction()
  {
    NDN_LOG_INFO("class template static member function");
  }

private:
  NDN_LOG_MEMBER_DECL();
};

// Technically this declaration is not necessary in this case,
// but we want to test that the macro expands to well-formed code
NDN_LOG_MEMBER_DECL_SPECIALIZED((ClassTemplateWithLogger<int, double>));

NDN_LOG_MEMBER_INIT_SPECIALIZED((ClassTemplateWithLogger<int, double>), ndn.util.tests.Specialized1);
NDN_LOG_MEMBER_INIT_SPECIALIZED((ClassTemplateWithLogger<int, std::string>), ndn.util.tests.Specialized2);

const time::microseconds LOG_SYSTIME(1468108800311239LL);
const std::string LOG_SYSTIME_STR("1468108800.311239");

class LoggingFixture : public ndn::tests::UnitTestTimeFixture
{
protected:
  LoggingFixture()
    : m_oldEnabledLevel(Logging::get().getLevels())
    , m_oldDestination(Logging::get().getDestination())
  {
    this->systemClock->setNow(LOG_SYSTIME);
    Logging::get().resetLevels();
    Logging::setDestination(os);
  }

  ~LoggingFixture()
  {
    Logging::get().setLevelImpl(m_oldEnabledLevel);
    Logging::setDestination(m_oldDestination);
  }

protected:
  boost::test_tools::output_test_stream os;

private:
  std::unordered_map<std::string, LogLevel> m_oldEnabledLevel;
  shared_ptr<std::ostream> m_oldDestination;
};

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestLogging, LoggingFixture)

BOOST_AUTO_TEST_CASE(GetLoggerNames)
{
  // check that all names are registered from the start even if
  // logger instances are lazily created on first use
  auto n = Logging::getLoggerNames().size();
  NDN_LOG_TRACE("GetLoggerNames");
  auto names = Logging::getLoggerNames();
  BOOST_CHECK_EQUAL(names.size(), n);
  BOOST_CHECK_EQUAL(names.count("ndn.util.tests.Logging"), 1);
}

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

BOOST_AUTO_TEST_CASE(NamespaceLogger)
{
  Logging::setLevel("ndn.util.tests.ns1", LogLevel::INFO);
  Logging::setLevel("ndn.util.tests.ns2", LogLevel::DEBUG);

  const auto& levels = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(levels.size(), 2);
  BOOST_CHECK_EQUAL(levels.at("ndn.util.tests.ns1"), LogLevel::INFO);
  BOOST_CHECK_EQUAL(levels.at("ndn.util.tests.ns2"), LogLevel::DEBUG);

  const auto& names = Logging::getLoggerNames();
  BOOST_CHECK_EQUAL(names.count("ndn.util.tests.ns1"), 1);
  BOOST_CHECK_EQUAL(names.count("ndn.util.tests.ns2"), 1);

  ns1::logFromNamespace1();
  ns2::logFromNamespace2();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " INFO: [ndn.util.tests.ns1] hello world from ns1\n" +
    LOG_SYSTIME_STR + " INFO: [ndn.util.tests.ns2] hi there from ns2\n"
    ));
}

BOOST_AUTO_TEST_CASE(MemberLogger)
{
  Logging::setLevel("ndn.util.tests.ClassWithLogger", LogLevel::INFO);
  Logging::setLevel("ndn.util.tests.Specialized1", LogLevel::INFO);
  // ndn.util.tests.Specialized2 is not enabled

  const auto& names = Logging::getLoggerNames();
  BOOST_CHECK_EQUAL(names.count("ndn.util.tests.ClassWithLogger"), 1);
  BOOST_CHECK_EQUAL(names.count("ndn.util.tests.Specialized1"), 1);
  BOOST_CHECK_EQUAL(names.count("ndn.util.tests.Specialized2"), 1);

  ClassWithLogger::logFromStaticMemberFunction();
  ClassWithLogger{}.logFromConstMemberFunction();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " INFO: [ndn.util.tests.ClassWithLogger] static member function\n" +
    LOG_SYSTIME_STR + " INFO: [ndn.util.tests.ClassWithLogger] const member function\n"
    ));

  ClassTemplateWithLogger<int, double>::logFromStaticMemberFunction();
  ClassTemplateWithLogger<int, double>{}.logFromMemberFunction();
  ClassTemplateWithLogger<int, std::string>::logFromStaticMemberFunction();
  ClassTemplateWithLogger<int, std::string>{}.logFromMemberFunction();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " INFO: [ndn.util.tests.Specialized1] class template static member function\n" +
    LOG_SYSTIME_STR + " INFO: [ndn.util.tests.Specialized1] class template non-static member function\n"
    ));
}

BOOST_AUTO_TEST_CASE(SameNameLoggers)
{
  Logging::setLevel("Module1", LogLevel::WARN);
  logFromModule1();
  logFromNewLogger("Module1");

  BOOST_CHECK_EQUAL(Logging::getLoggerNames().count("Module1"), 1);

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
  Logging::setLevel("Module3", LogLevel::DEBUG);
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
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 0);
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
  const auto& prefixMap = Logging::get().getLevels();
  // "*" is treated as "" internally
  BOOST_CHECK_EQUAL(prefixMap.size(), 1);
  BOOST_CHECK_EQUAL(prefixMap.at(""), LogLevel::WARN);
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
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 1);
  BOOST_CHECK_EQUAL(prefixMap.at("Module1"), LogLevel::ERROR);
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
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 2);
  // "*" is treated as "" internally
  BOOST_CHECK_EQUAL(prefixMap.at(""), LogLevel::WARN);
  BOOST_CHECK_EQUAL(prefixMap.at("Module2"), LogLevel::DEBUG);
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
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 2);
  // "*" is treated as "" internally
  BOOST_CHECK_EQUAL(prefixMap.at(""), LogLevel::WARN);
  BOOST_CHECK_EQUAL(prefixMap.at("Module2"), LogLevel::DEBUG);
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
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 1);
  // "*" is treated as "" internally
  BOOST_CHECK_EQUAL(prefixMap.at(""), LogLevel::ERROR);
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

BOOST_AUTO_TEST_CASE(SetFilter)
{
  Logging::setLevel("*=FATAL:fm.*=DEBUG");
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 2);
  // "*" is treated as "" internally
  BOOST_CHECK_EQUAL(prefixMap.at(""), LogLevel::FATAL);
  // "name.*" is treated as "name." internally
  BOOST_CHECK_EQUAL(prefixMap.at("fm."), LogLevel::DEBUG);
  logFromModule1();
  logFromFilterModule();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " DEBUG: [fm.FilterModule] debugFM\n" +
    LOG_SYSTIME_STR + " INFO: [fm.FilterModule] infoFM\n" +
    LOG_SYSTIME_STR + " WARNING: [fm.FilterModule] warnFM\n" +
    LOG_SYSTIME_STR + " ERROR: [fm.FilterModule] errorFM\n" +
    LOG_SYSTIME_STR + " FATAL: [fm.FilterModule] fatalFM\n"
    ));
}

BOOST_AUTO_TEST_CASE(SetOverrideFilter)
{
  Logging::setLevel("*=FATAL:fm.FilterModule=DEBUG");
  Logging::setLevel("fm.*", LogLevel::INFO);
  const auto& prefixMap = Logging::get().getLevels();
  BOOST_CHECK_EQUAL(prefixMap.size(), 2);
  // "*" is treated as "" internally
  BOOST_CHECK_EQUAL(prefixMap.at(""), LogLevel::FATAL);
  // "name.*" is treated as "name." internally
  BOOST_CHECK_EQUAL(prefixMap.at("fm."), LogLevel::INFO);
  logFromModule1();
  logFromFilterModule();

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " FATAL: [Module1] fatal1\n" +
    LOG_SYSTIME_STR + " INFO: [fm.FilterModule] infoFM\n" +
    LOG_SYSTIME_STR + " WARNING: [fm.FilterModule] warnFM\n" +
    LOG_SYSTIME_STR + " ERROR: [fm.FilterModule] errorFM\n" +
    LOG_SYSTIME_STR + " FATAL: [fm.FilterModule] fatalFM\n"
    ));
}

BOOST_AUTO_TEST_CASE(FindPrefixRule)
{
  Logging::setLevel("*=FATAL:fm.a.*=ERROR:fm.a.b=INFO");
  logFromNewLogger("fm.a.b");
  logFromNewLogger("fm.a.b.c");
  logFromNewLogger("fm.a.b.d");
  logFromNewLogger("fm.b");

  Logging::flush();
  BOOST_CHECK(os.is_equal(
    LOG_SYSTIME_STR + " INFO: [fm.a.b] infofm.a.b\n" +
    LOG_SYSTIME_STR + " WARNING: [fm.a.b] warnfm.a.b\n" +
    LOG_SYSTIME_STR + " ERROR: [fm.a.b] errorfm.a.b\n" +
    LOG_SYSTIME_STR + " FATAL: [fm.a.b] fatalfm.a.b\n" +
    LOG_SYSTIME_STR + " ERROR: [fm.a.b.c] errorfm.a.b.c\n" +
    LOG_SYSTIME_STR + " FATAL: [fm.a.b.c] fatalfm.a.b.c\n" +
    LOG_SYSTIME_STR + " ERROR: [fm.a.b.d] errorfm.a.b.d\n" +
    LOG_SYSTIME_STR + " FATAL: [fm.a.b.d] fatalfm.a.b.d\n" +
    LOG_SYSTIME_STR + " FATAL: [fm.b] fatalfm.b\n"
    ));
}

BOOST_AUTO_TEST_SUITE_END() // SeverityConfig

BOOST_AUTO_TEST_CASE(ChangeDestination)
{
  using boost::test_tools::output_test_stream;

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
