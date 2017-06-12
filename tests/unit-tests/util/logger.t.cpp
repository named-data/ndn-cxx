/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "util/logger.hpp"
#include "util/logging.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace util {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestLogger)

BOOST_AUTO_TEST_CASE(LegalAlphanumeric)
{
  Logger logger("ndnTest123");
  auto mNames = Logging::getLoggerNames();
  BOOST_CHECK_EQUAL(mNames.count("ndnTest123"), 1);
  BOOST_CHECK(logger.isLevelEnabled(LogLevel::NONE));
  Logging::get().removeLogger(logger);
}

BOOST_AUTO_TEST_CASE(AllLegalSymbols)
{
  Logger logger("ndn.~#%.test_<check>1-2-3");
  auto mNames = Logging::getLoggerNames();
  BOOST_CHECK_EQUAL(mNames.count("ndn.~#%.test_<check>1-2-3"), 1);
  BOOST_CHECK(logger.isLevelEnabled(LogLevel::NONE));
  Logging::get().removeLogger(logger);
}

BOOST_AUTO_TEST_CASE(EmptyLogger)
{
  BOOST_CHECK_THROW(Logger logger(""), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(InvalidSymbol)
{
  BOOST_CHECK_THROW(Logger logger("ndn.test.*"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(StartsWithPeriod)
{
  BOOST_CHECK_THROW(Logger logger(".ndn.test"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(EndsWithPeriod)
{
  BOOST_CHECK_THROW(Logger logger("ndn.test."), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ConsecutivePeriods)
{
  BOOST_CHECK_THROW(Logger logger("ndn..test"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestLogger
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
