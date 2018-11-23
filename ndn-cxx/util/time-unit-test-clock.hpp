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

#ifndef NDN_UTIL_TIME_UNIT_TEST_CLOCK_HPP
#define NDN_UTIL_TIME_UNIT_TEST_CLOCK_HPP

#include "time-custom-clock.hpp"

namespace ndn {
namespace time {

/**
 * @brief Traits for UnitTestClock, defining default behavior for different clocks
 *
 * The only behavior that is currently controlled by the traits is default start
 * time.  The generic implementation assumes start time to be zero.
 */
template<class BaseClock>
class UnitTestClockTraits
{
public:
  static nanoseconds
  getDefaultStartTime()
  {
    return nanoseconds::zero();
  }
};

/**
 * @brief Specialization of UnitTestClockTraits for system_clock
 *
 * This specialization sets the default start time to 1415684132 seconds
 * (equivalent to Tue 11 Nov 2014 05:35:32 UTC, if Unix epoch is assumed).
 */
template<>
class UnitTestClockTraits<system_clock>
{
public:
  static nanoseconds
  getDefaultStartTime()
  {
    return seconds(1415684132);
  }
};

/**
 * @brief Clock that can be used in unit tests for time-dependent tests independent of wall clock
 *
 * This clock should be explicitly advanced with UnitTestClock<BaseClock>::advance() or set
 * with UnitTestClock<BaseClock>::setNow() methods.
 *
 * @note Default start time is determined by UnitTestClockTraits
 */
template<class BaseClock, class ClockTraits = UnitTestClockTraits<BaseClock>>
class UnitTestClock : public CustomClock<BaseClock>
{
public:
  explicit
  UnitTestClock(nanoseconds startTime = ClockTraits::getDefaultStartTime());

  /**
   * @brief Advance unit test clock by @p duration
   */
  void
  advance(nanoseconds duration);

  /**
   * @brief Explicitly set clock to @p timeSinceEpoch
   */
  void
  setNow(nanoseconds timeSinceEpoch);

public:
  std::string
  getSince() const override;

  typename BaseClock::time_point
  getNow() const override;

  typename BaseClock::duration
  toWaitDuration(typename BaseClock::duration d) const override;

private:
  nanoseconds m_currentTime;
};

extern template class UnitTestClock<system_clock>;
extern template class UnitTestClock<steady_clock>;

using UnitTestSystemClock = UnitTestClock<system_clock>;
using UnitTestSteadyClock = UnitTestClock<steady_clock>;

} // namespace time
} // namespace ndn

#endif // NDN_UTIL_TIME_UNIT_TEST_CLOCK_HPP
