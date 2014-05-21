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

#ifndef NDN_TIME_HPP
#define NDN_TIME_HPP

#include "../common.hpp"
#include <boost/chrono.hpp>

namespace ndn {
namespace time {

using boost::chrono::duration;

typedef duration<boost::int_least32_t, boost::ratio<86400> > days;
using boost::chrono::hours;
using boost::chrono::minutes;
using boost::chrono::seconds;

using boost::chrono::milliseconds;
using boost::chrono::microseconds;
using boost::chrono::nanoseconds;

using boost::chrono::duration_cast;

/**
 * \brief System clock
 *
 * System clock represents the system-wide real time wall clock.
 *
 * It may not be monotonic: on most systems, the system time can be
 * adjusted at any moment. It is the only clock that has the ability
 * to be displayed and converted to/from UNIX timestamp.
 *
 * To get current TimePoint:
 *
 * <code>
 *     system_clock::TimePoint now = system_clock::now();
 * </code>
 *
 * To convert TimePoint to/from UNIX timestamp:
 *
 * <code>
 *     system_clock::TimePoint time = ...;
 *     uint64_t timestampInMilliseconds = toUnixTimestamp(time).count();
 *     system_clock::TimePoint time2 = fromUnixTimestamp(time::milliseconds(timestampInMilliseconds));
 * </code>
 */
class system_clock : public boost::chrono::system_clock
{
public:
  typedef time_point TimePoint;
  typedef duration Duration;

  // /// \brief Get current TimePoint
  // TimePoint
  // now();
}; // class system_clock

/**
 * \brief Steady clock
 *
 * Steady clock represents a monotonic clock. The time points of this
 * clock cannot decrease as physical time moves forward. This clock is
 * not related to wall clock time, and is best suitable for measuring
 * intervals.
 *
 * Note that on OS X platform this defaults to system clock and is not
 * truly monotonic. Refer to https://svn.boost.org/trac/boost/ticket/7719)
 */
class steady_clock : public
#ifdef __APPLE__
// steady_clock may go backwards on OS X platforms, so use system_clock
// instead
    boost::chrono::system_clock
#else
    boost::chrono::steady_clock
#endif
{
public:
  typedef time_point TimePoint;
  typedef duration Duration;

  // /// \brief Get current TimePoint
  // TimePoint
  // now();
}; // class steady_clock


/**
 * \brief Get system_clock::TimePoint representing UNIX time epoch (00:00:00 on Jan 1, 1970)
 */
inline const system_clock::TimePoint&
getUnixEpoch()
{
  static system_clock::TimePoint epoch = system_clock::from_time_t(0);
  return epoch;
}

/**
 * \brief Convert system_clock::TimePoint to UNIX timestamp
 */
inline milliseconds
toUnixTimestamp(const system_clock::TimePoint& point)
{
  return duration_cast<milliseconds>(point - getUnixEpoch());
}

/**
 * \brief Convert UNIX timestamp to system_clock::TimePoint
 */
inline system_clock::TimePoint
fromUnixTimestamp(const milliseconds& duration)
{
  return getUnixEpoch() + duration;
}

/**
 * \brief Convert to the ISO string representation of the time (YYYYMMDDTHHMMSS,fffffffff)
 *
 * If timePoint contains doesn't contain fractional seconds the
 * output format is YYYYMMDDTHHMMSS
 *
 * Examples:
 *
 *   - with fractional nanoseconds:  20020131T100001,123456789
 *   - with fractional microseconds: 20020131T100001,123456
 *   - with fractional milliseconds: 20020131T100001,123
 *   - without fractional seconds:   20020131T100001
 */
std::string
toIsoString(const system_clock::TimePoint& timePoint);

/**
 * \brief Convert from the ISO string (YYYYMMDDTHHMMSS,fffffffff) representation
 *        to the internal time format
 *
 * Examples of accepted ISO strings:
 *
 *   - with fractional nanoseconds:  20020131T100001,123456789
 *   - with fractional microseconds: 20020131T100001,123456
 *   - with fractional milliseconds: 20020131T100001,123
 *   - without fractional seconds:   20020131T100001
 *
 */
system_clock::TimePoint
fromIsoString(const std::string& isoString);

/**
 * \brief Convert time point to string with specified format
 *
 * By default, `%Y-%m-%d %H:%M:%S` is used, producing dates like
 * `2014-04-10 22:51:00`
 *
 * \param timePoint time point of system_clock
 * \param format desired output format (default: `%Y-%m-%d %H:%M:%S`)
 * \param locale desired locale (default: "C" locale)
 *
 * \sa http://www.boost.org/doc/libs/1_48_0/doc/html/date_time/date_time_io.html#date_time.format_flags
 *     described possible formatting flags
 **/
std::string
toString(const system_clock::TimePoint& timePoint,
         const std::string& format = "%Y-%m-%d %H:%M:%S",
         const std::locale& locale = std::locale("C"));

/**
 * \brief Convert from string of specified format into time point
 *
 * By default, `%Y-%m-%d %H:%M:%S` is used, accepting dates like
 * `2014-04-10 22:51:00`
 *
 * \param formattedTimePoint string representing time point
 * \param format    input output format (default: `%Y-%m-%d %H:%M:%S`)
 * \param locale    input locale (default: "C" locale)
 *
 * \sa http://www.boost.org/doc/libs/1_48_0/doc/html/date_time/date_time_io.html#date_time.format_flags
 *     described possible formatting flags
 */
system_clock::TimePoint
fromString(const std::string& formattedTimePoint,
           const std::string& format = "%Y-%m-%d %H:%M:%S",
           const std::locale& locale = std::locale("C"));

} // namespace time
} // namespace ndn

#endif // NDN_TIME_HPP
