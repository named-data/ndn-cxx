/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_UNIT_CLOCK_FIXTURE_HPP
#define NDN_CXX_TESTS_UNIT_CLOCK_FIXTURE_HPP

#include "ndn-cxx/util/time-unit-test-clock.hpp"

namespace ndn {
namespace tests {

/** \brief A test fixture that overrides steady clock and system clock.
 */
class ClockFixture
{
public:
  virtual
  ~ClockFixture();

  /** \brief Advance steady and system clocks.
   *
   *  Clocks are advanced in increments of \p tick for \p nTicks ticks.
   *  afterTick() is called after each tick.
   *
   *  Exceptions thrown during I/O events are propagated to the caller.
   *  Clock advancement will stop in the event of an exception.
   */
  void
  advanceClocks(time::nanoseconds tick, size_t nTicks = 1)
  {
    advanceClocks(tick, tick * nTicks);
  }

  /** \brief Advance steady and system clocks.
   *
   *  Clocks are advanced in increments of \p tick for \p total time.
   *  The last increment might be shorter than \p tick.
   *  afterTick() is called after each tick.
   *
   *  Exceptions thrown during I/O events are propagated to the caller.
   *  Clock advancement will stop in the event of an exception.
   */
  void
  advanceClocks(time::nanoseconds tick, time::nanoseconds total);

protected:
  ClockFixture();

private:
  /** \brief Called by advanceClocks() after each clock advancement (tick).
   *
   *  The base class implementation is a no-op.
   */
  virtual void
  afterTick()
  {
  }

protected:
  shared_ptr<time::UnitTestSteadyClock> m_steadyClock;
  shared_ptr<time::UnitTestSystemClock> m_systemClock;
};

} // namespace tests
} // namespace ndn

#endif // NDN_CXX_TESTS_UNIT_CLOCK_FIXTURE_HPP
