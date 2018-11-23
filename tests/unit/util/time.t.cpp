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

#include "util/time.hpp"

#include "boost-test.hpp"

#include <thread>

namespace ndn {
namespace time {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestTime)

BOOST_AUTO_TEST_CASE(SystemClock)
{
  system_clock::TimePoint value = system_clock::now();
  system_clock::TimePoint referenceTime = fromUnixTimestamp(milliseconds(1390966967032LL));

  BOOST_CHECK_GT(value, referenceTime);

  BOOST_CHECK_EQUAL(toIsoString(referenceTime), "20140129T034247.032000");
  BOOST_CHECK_EQUAL(toString(referenceTime), "2014-01-29 03:42:47");
  BOOST_CHECK_EQUAL(toString(referenceTime), "2014-01-29 03:42:47");

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(toString(referenceTime, "%Y. gada %d. %B", std::locale("lv_LV.UTF-8")),
  //                   "2014. gada 29. Janvāris");

  BOOST_CHECK_EQUAL(toString(referenceTime, "%Y -- %d -- %B", std::locale("C")),
                    "2014 -- 29 -- January");

  BOOST_CHECK_EQUAL(fromIsoString("20140129T034247.032000"), referenceTime);
  BOOST_CHECK_EQUAL(fromIsoString("20140129T034247.032000Z"), referenceTime);
  BOOST_CHECK_EQUAL(fromString("2014-01-29 03:42:47"), fromUnixTimestamp(1390966967_s));

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(fromString("2014. gada 29. Janvāris", "%Y. gada %d. %B", std::locale("lv_LV.UTF-8")),
  //                   fromUnixTimestamp(1390953600_s));

  BOOST_CHECK_EQUAL(fromString("2014 -- 29 -- January", "%Y -- %d -- %B", std::locale("C")),
                    fromUnixTimestamp(1390953600_s));
}

BOOST_AUTO_TEST_CASE(SteadyClock)
{
  steady_clock::TimePoint oldValue = steady_clock::now();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  steady_clock::TimePoint newValue = steady_clock::now();
  BOOST_CHECK_GT(newValue, oldValue);
}

BOOST_AUTO_TEST_CASE(Abs)
{
  BOOST_CHECK_EQUAL(abs(nanoseconds(24422)), nanoseconds(24422));
  BOOST_CHECK_EQUAL(abs(microseconds(0)), microseconds(0));
  BOOST_CHECK_EQUAL(abs(milliseconds(-15583)), milliseconds(15583));
}

BOOST_AUTO_TEST_CASE(LargeDates)
{
  auto value = fromUnixTimestamp(1390966967032_ms);
  BOOST_CHECK_EQUAL(toIsoString(value), "20140129T034247.032000");
  BOOST_CHECK_EQUAL(fromIsoString("20140129T034247.032000"), value);
  BOOST_CHECK_EQUAL(toString(value, "%Y-%m-%d %H:%M:%S%F"), "2014-01-29 03:42:47.032000");
  BOOST_CHECK_EQUAL(fromString("2014-01-29 03:42:47.032000", "%Y-%m-%d %H:%M:%S%F"), value);

  value += 36524_days;
  BOOST_CHECK_EQUAL(toIsoString(value), "21140129T034247.032000");
  BOOST_CHECK_EQUAL(fromIsoString("21140129T034247.032000"), value);
  BOOST_CHECK_EQUAL(toString(value, "%Y-%m-%d %H:%M:%S%F"), "2114-01-29 03:42:47.032000");
  BOOST_CHECK_EQUAL(fromString("2114-01-29 03:42:47.03200", "%Y-%m-%d %H:%M:%S%F"), value);
}

BOOST_AUTO_TEST_CASE(Literals)
{
  BOOST_CHECK_EQUAL(42_s, seconds(42));

  BOOST_CHECK_EQUAL(1_day, 24_h);
  BOOST_CHECK_EQUAL(2_days, 48_h);
  BOOST_CHECK_EQUAL(0.5_day, 12_h);
  BOOST_CHECK_EQUAL(.5_days, 12_h);

  BOOST_CHECK_EQUAL(1_h, 60_min);
  BOOST_CHECK_EQUAL(0.5_h, 30_min);

  BOOST_CHECK_EQUAL(1_min, 60_s);
  BOOST_CHECK_EQUAL(0.5_min, 30_s);

  BOOST_CHECK_EQUAL(1_s, 1000_ms);
  BOOST_CHECK_EQUAL(0.5_s, 500_ms);

  BOOST_CHECK_EQUAL(1_ms, 1000_us);
  BOOST_CHECK_EQUAL(0.5_ms, 500_us);

  BOOST_CHECK_EQUAL(1_us, 1000_ns);
  BOOST_CHECK_EQUAL(0.5_us, 500_ns);

  BOOST_CHECK_EQUAL(1_ns, nanoseconds(1));
  BOOST_CHECK_EQUAL(5.5_ns, 0.0055_us);
}

BOOST_AUTO_TEST_CASE(Year2038)
{
  auto year2042 = fromIsoString("20420101T000001.042000");
  auto year2010 = fromIsoString("20100101T000001.042000");

  BOOST_CHECK_EQUAL(to_string(year2010), "1262304001042000000 nanoseconds since Jan 1, 1970");
  BOOST_CHECK_EQUAL(to_string(year2042), "2272147201042000000 nanoseconds since Jan 1, 1970");
  BOOST_CHECK_GT(year2042, year2010);
}

BOOST_AUTO_TEST_SUITE_END() // TestTime
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace time
} // namespace ndn
