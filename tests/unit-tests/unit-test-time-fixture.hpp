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

#ifndef NDN_TESTS_UNIT_TESTS_UNIT_TEST_TIME_FIXTURE_HPP
#define NDN_TESTS_UNIT_TESTS_UNIT_TEST_TIME_FIXTURE_HPP

#include "util/time-unit-test-clock.hpp"

#include <boost/asio/io_service.hpp>

namespace ndn {
namespace tests {

/** \brief a test fixture that overrides steady clock and system clock
 */
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

  /** \brief advance steady and system clocks
   *
   *  Clocks are advanced in increments of \p tick for \p nTicks ticks.
   *  After each tick, io_service is polled to process pending I/O events.
   *
   *  Exceptions thrown during I/O events are propagated to the caller.
   *  Clock advancing would stop in case of an exception.
   */
  void
  advanceClocks(const time::nanoseconds& tick, size_t nTicks = 1)
  {
    this->advanceClocks(tick, tick * nTicks);
  }

  /** \brief advance steady and system clocks
   *
   *  Clocks are advanced in increments of \p tick for \p total time.
   *  The last increment might be shorter than \p tick.
   *  After each tick, io_service is polled to process pending I/O events.
   *
   *  Exceptions thrown during I/O events are propagated to the caller.
   *  Clock advancing would stop in case of an exception.
   */
  void
  advanceClocks(const time::nanoseconds& tick, const time::nanoseconds& total)
  {
    BOOST_ASSERT(tick > time::nanoseconds::zero());
    BOOST_ASSERT(total >= time::nanoseconds::zero());

    time::nanoseconds remaining = total;
    while (remaining > time::nanoseconds::zero()) {
      if (remaining >= tick) {
        steadyClock->advance(tick);
        systemClock->advance(tick);
        remaining -= tick;
      }
      else {
        steadyClock->advance(remaining);
        systemClock->advance(remaining);
        remaining = time::nanoseconds::zero();
      }

      if (io.stopped())
        io.reset();
      io.poll();
    }
  }

public:
  shared_ptr<time::UnitTestSteadyClock> steadyClock;
  shared_ptr<time::UnitTestSystemClock> systemClock;
  boost::asio::io_service io;
};

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_UNIT_TESTS_UNIT_TEST_TIME_FIXTURE_HPP
