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

namespace ndn {
namespace time {

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
}

template<class BaseClock>
void
UnitTestClock<BaseClock>::setNow(const nanoseconds& timeSinceEpoch)
{
  m_currentTime = timeSinceEpoch;
}

template
class UnitTestClock<system_clock>;

template
class UnitTestClock<steady_clock>;

} // namespace time
} // namespace ndn
