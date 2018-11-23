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

#include "time-unit-test-clock.hpp"
#include "detail/steady-timer.hpp"

#include <chrono>
#include <thread>

namespace ndn {
namespace time {

template<class BaseClock, class ClockTraits>
UnitTestClock<BaseClock, ClockTraits>::UnitTestClock(nanoseconds startTime)
  : m_currentTime(startTime)
{
}

template<class BaseClock, class ClockTraits>
std::string
UnitTestClock<BaseClock, ClockTraits>::getSince() const
{
  return " since unit test beginning";
}

template<class BaseClock, class ClockTraits>
typename BaseClock::time_point
UnitTestClock<BaseClock, ClockTraits>::getNow() const
{
  return typename BaseClock::time_point(duration_cast<typename BaseClock::duration>(m_currentTime));
}

template<class BaseClock, class ClockTraits>
typename BaseClock::duration
UnitTestClock<BaseClock, ClockTraits>::toWaitDuration(typename BaseClock::duration) const
{
  return typename BaseClock::duration(1);
}

template<class BaseClock, class ClockTraits>
void
UnitTestClock<BaseClock, ClockTraits>::advance(nanoseconds duration)
{
  m_currentTime += duration;

  // When UnitTestClock is used with Asio timers (e.g. basic_waitable_timer), and
  // an async wait operation on a timer is already in progress, Asio won't look
  // at the clock again until the earliest timer has expired, so it won't know that
  // the current time has changed.
  //
  // Therefore, in order for the clock advancement to be effective, we must sleep
  // for a period greater than wait_traits::to_wait_duration().
  //
  // See also http://blog.think-async.com/2007/08/time-travel.html - "Jumping Through Time"
  //
  std::this_thread::sleep_for(std::chrono::nanoseconds(duration_cast<nanoseconds>(
                                boost::asio::wait_traits<steady_clock>::to_wait_duration(duration) +
                                typename BaseClock::duration(1)).count()));
}

template<class BaseClock, class ClockTraits>
void
UnitTestClock<BaseClock, ClockTraits>::setNow(nanoseconds timeSinceEpoch)
{
  BOOST_ASSERT(!BaseClock::is_steady || timeSinceEpoch >= m_currentTime);

  m_currentTime = timeSinceEpoch;

  // See comment in advance()
  auto delta = timeSinceEpoch - m_currentTime;
  std::this_thread::sleep_for(std::chrono::nanoseconds(duration_cast<nanoseconds>(
                                boost::asio::wait_traits<steady_clock>::to_wait_duration(delta) +
                                typename BaseClock::duration(1)).count()));
}

template
class UnitTestClock<system_clock>;

template
class UnitTestClock<steady_clock>;

} // namespace time
} // namespace ndn
