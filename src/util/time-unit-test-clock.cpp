/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
#include "monotonic_deadline_timer.hpp"
#include <thread>

namespace ndn {
namespace time {

const std::chrono::microseconds SLEEP_AFTER_TIME_CHANGE(2);

template<class BaseClock>
UnitTestClock<BaseClock>::UnitTestClock(const nanoseconds& startTime)
  : m_currentTime(startTime)
{
}

template<class BaseClock>
std::string
UnitTestClock<BaseClock>::getSince() const
{
  return " since unit test clock advancements";
}

template<class BaseClock>
typename BaseClock::time_point
UnitTestClock<BaseClock>::getNow() const
{
  return typename BaseClock::time_point(duration_cast<typename BaseClock::duration>(m_currentTime));
}

template<class BaseClock>
boost::posix_time::time_duration
UnitTestClock<BaseClock>::toPosixDuration(const typename BaseClock::duration& duration) const
{
  return
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
    boost::posix_time::nanoseconds(1)
#else
    boost::posix_time::microseconds(1)
#endif
    ;
}


template<class BaseClock>
void
UnitTestClock<BaseClock>::advance(const nanoseconds& duration)
{
  m_currentTime += duration;

  // On some platforms, boost::asio::io_service for deadline_timer (e.g., the one used in
  // Scheduler) will call time_traits<>::now() and will "sleep" for
  // time_traits<>::to_posix_time(duration) period before calling time_traits<>::now()
  // again. (Note that such "sleep" will occur even if there is no actual waiting and
  // program is calling io_service.poll().)
  //
  // As a result, in order for the clock advancement to be effective, we must sleep for a
  // period greater than time_traits<>::to_posix_time().
  //
  // See also http://blog.think-async.com/2007/08/time-travel.html
  BOOST_ASSERT(boost::posix_time::microseconds(SLEEP_AFTER_TIME_CHANGE.count()) >
               boost::asio::time_traits<steady_clock>::to_posix_duration(duration));
  std::this_thread::sleep_for(SLEEP_AFTER_TIME_CHANGE);
}

template<class BaseClock>
void
UnitTestClock<BaseClock>::setNow(const nanoseconds& timeSinceEpoch)
{
  BOOST_ASSERT(boost::posix_time::microseconds(SLEEP_AFTER_TIME_CHANGE.count()) >
               boost::asio::time_traits<steady_clock>::to_posix_duration(timeSinceEpoch -
                                                                         m_currentTime));
  m_currentTime = timeSinceEpoch;
  std::this_thread::sleep_for(SLEEP_AFTER_TIME_CHANGE);
}

template
class UnitTestClock<system_clock>;

template
class UnitTestClock<steady_clock>;

} // namespace time
} // namespace ndn
