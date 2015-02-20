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

#include "util/time-unit-test-clock.hpp"
#include "util/scheduler.hpp"

#include "boost-test.hpp"
#include <boost/lexical_cast.hpp>
#include <thread>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(UtilTimeUnitTestClock)

class UnitTestTimeFixture
{
public:
  UnitTestTimeFixture()
    : steadyClock(make_shared<time::UnitTestSteadyClock>())
    , systemClock(make_shared<time::UnitTestSystemClock>())
  {
    time::setCustomClocks(steadyClock, systemClock);
  }

  ~UnitTestTimeFixture()
  {
    time::setCustomClocks(nullptr, nullptr);
  }

public:
  shared_ptr<time::UnitTestSteadyClock> steadyClock;
  shared_ptr<time::UnitTestSystemClock> systemClock;
};

BOOST_FIXTURE_TEST_CASE(SystemClock, UnitTestTimeFixture)
{
  BOOST_CHECK_EQUAL(time::system_clock::now().time_since_epoch(),
                    time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  BOOST_CHECK_EQUAL(time::system_clock::now().time_since_epoch(),
                    time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());

  steadyClock->advance(time::days(1));
  BOOST_CHECK_EQUAL(time::system_clock::now().time_since_epoch(),
                    time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());

  systemClock->advance(time::days(1));
  BOOST_CHECK_GT(time::system_clock::now().time_since_epoch(),
                 time::UnitTestClockTraits<time::system_clock>::getDefaultStartTime());

  time::system_clock::TimePoint referenceTime =
    time::fromUnixTimestamp(time::milliseconds(1390966967032LL));
  BOOST_CHECK_GT(time::system_clock::now(), referenceTime);

  systemClock->setNow(referenceTime.time_since_epoch());
  BOOST_CHECK_EQUAL(time::system_clock::now(), referenceTime);

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(time::system_clock::now()),
                    "1390966967032000000 nanoseconds since unit test clock advancements");

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
                    time::fromUnixTimestamp(time::seconds(1390966967)));

  // Unfortunately, not all systems has lv_LV locale installed :(
  // BOOST_CHECK_EQUAL(time::fromString("2014. gada 29. Janvāris", "%Y. gada %d. %B",
  //                                    std::locale("lv_LV.UTF-8")),
  //                   time::fromUnixTimestamp(time::seconds(1390953600)));

  BOOST_CHECK_EQUAL(time::fromString("2014 -- 29 -- January", "%Y -- %d -- %B",
                                     std::locale("C")),
                    time::fromUnixTimestamp(time::seconds(1390953600)));
}

BOOST_FIXTURE_TEST_CASE(SteadyClock, UnitTestTimeFixture)
{
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(),
                    time::steady_clock::duration::zero());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(),
                    time::steady_clock::duration::zero());

  systemClock->advance(time::days(36500));
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(),
                    time::steady_clock::duration::zero());

  steadyClock->advance(time::nanoseconds(100));
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(), time::nanoseconds(100));

  steadyClock->advance(time::microseconds(100));
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(), time::nanoseconds(100100));

  steadyClock->setNow(time::nanoseconds(100));
  BOOST_CHECK_EQUAL(time::steady_clock::now().time_since_epoch(), time::nanoseconds(100));

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(time::steady_clock::now()),
                    "100 nanoseconds since unit test clock advancements");
}

BOOST_FIXTURE_TEST_CASE(Scheduler, UnitTestTimeFixture)
{
  boost::asio::io_service io;
  ndn::Scheduler scheduler(io);

  bool hasFired = false;
  scheduler.scheduleEvent(time::seconds(100), [&] { hasFired = true; });

  io.poll();
  BOOST_CHECK_EQUAL(hasFired, false);

  steadyClock->advance(time::seconds(100));

  io.poll();
  BOOST_CHECK_EQUAL(hasFired, true);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
