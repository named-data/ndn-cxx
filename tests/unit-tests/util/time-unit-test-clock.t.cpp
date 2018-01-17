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

#include "util/time-unit-test-clock.hpp"
#include "util/scheduler.hpp"

#include "boost-test.hpp"
#include "../unit-test-time-fixture.hpp"

#include <boost/lexical_cast.hpp>
#include <thread>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_FIXTURE_TEST_SUITE(TestTimeUnitTestClock, UnitTestTimeFixture)

BOOST_AUTO_TEST_CASE(SystemClock)
{
  BOOST_CHECK_EQUAL(time::system_clock::now().time_since_epoch(),
                    time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  BOOST_CHECK_EQUAL(time::system_clock::now().time_since_epoch(),
                    time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());

  steadyClock->advance(1_day);
  BOOST_CHECK_EQUAL(time::system_clock::now().time_since_epoch(),
                    time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());

  systemClock->advance(1_day);
  BOOST_CHECK_GT(time::system_clock::now().time_since_epoch(),
                 time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());

  time::system_clock::TimePoint referenceTime =
    time::fromUnixTimestamp(time::milliseconds(1390966967032LL));
  BOOST_CHECK_GT(time::system_clock::now(), referenceTime);

  systemClock->setNow(referenceTime.time_since_epoch());
  BOOST_CHECK_EQUAL(time::system_clock::now(), referenceTime);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(time::system_clock::now()),
                    "1390966967032000000 nanoseconds since unit test beginning");

  BOOST_CHECK_EQUAL(time::toIsoString(referenceTime), "20140129T034247.032000");
  BOOST_CHECK_EQUAL(time::toString(referenceTime), "2014-01-29 03:42:47");
  BOOST_CHECK_EQUAL(time::toString(referenceTime), "2014-01-29 03:42:47");

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(time::toString(referenceTime, "%Y. gada %d. %B",
  //                                  std::locale("lv_LV.UTF-8")),
  //                   "2014. gada 29. Janvāris");

  BOOST_CHECK_EQUAL(time::toString(referenceTime, "%Y -- %d -- %B",
                                   std::locale("C")),
                    "2014 -- 29 -- January");

  BOOST_CHECK_EQUAL(time::fromIsoString("20140129T034247.032000"), referenceTime);
  BOOST_CHECK_EQUAL(time::fromIsoString("20140129T034247.032000Z"), referenceTime);
  BOOST_CHECK_EQUAL(time::fromString("2014-01-29 03:42:47"),
                    time::fromUnixTimestamp(1390966967_s));

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(time::fromString("2014. gada 29. Janvāris", "%Y. gada %d. %B",
  //                                    std::locale("lv_LV.UTF-8")),
  //                   time::fromUnixTimestamp(1390953600_s));

  BOOST_CHECK_EQUAL(time::fromString("2014 -- 29 -- January", "%Y -- %d -- %B",
                                     std::locale("C")),
                    time::fromUnixTimestamp(1390953600_s));
}

BOOST_AUTO_TEST_CASE(SteadyClock)
{
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(),
                    time::steady_clock::duration::zero());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(),
                    time::steady_clock::duration::zero());

  systemClock->advance(36500_days);
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(),
                    time::steady_clock::duration::zero());

  steadyClock->advance(100_ns);
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(), 100_ns);

  steadyClock->advance(100_us);
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(), 100100_ns);

  steadyClock->setNow(1_ms);
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(), 1000000_ns);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(time::steady_clock::now()),
                    "1000000 nanoseconds since unit test beginning");
}

BOOST_AUTO_TEST_CASE(Scheduler)
{
  ndn::Scheduler scheduler(io);

  bool hasFired = false;
  scheduler.scheduleEvent(100_s, [&] { hasFired = true; });

  io.poll();
  BOOST_CHECK_EQUAL(hasFired, false);

  steadyClock->advance(100_s);

  io.poll();
  BOOST_CHECK_EQUAL(hasFired, true);
}

BOOST_AUTO_TEST_SUITE_END() // TestTimeUnitTestClock
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace ndn
