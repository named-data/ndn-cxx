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
#include <boost/asio/time_traits.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

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
 *     system_clock::TimePoint time2 = fromUnixTimestamp(milliseconds(timestampInMilliseconds));
 * </code>
 */
class system_clock
{
public:
  typedef BOOST_SYSTEM_CLOCK_DURATION      duration;
  typedef duration::rep                    rep;
  typedef duration::period                 period;
  typedef boost::chrono::time_point<system_clock> time_point;
  static constexpr bool is_steady = false;

  typedef time_point TimePoint;
  typedef duration Duration;

  static time_point
  now() noexcept;

  static std::time_t
  to_time_t(const time_point& t) noexcept;

  static time_point
  from_time_t(std::time_t t) noexcept;
}; // class system_clock

/**
 * \brief Steady clock
 *
 * Steady clock represents a monotonic clock. The time points of this
 * clock cannot decrease as physical time moves forward. This clock is
 * not related to wall clock time, and is best suitable for measuring
 * intervals.
 */
class steady_clock
{
public:
  typedef nanoseconds      duration;
  typedef duration::rep    rep;
  typedef duration::period period;
  typedef boost::chrono::time_point<steady_clock> time_point;
  static constexpr bool is_steady = true;

  typedef time_point TimePoint;
  typedef duration Duration;

  static time_point
  now() noexcept;

private:
  /**
   * \brief Method to be used in deadline timer to select proper waiting
   *
   * Mock time implementations should return minimum value to ensure Boost.Asio
   * is not enabling any waiting on mock timers.
   *
   * @sa http://stackoverflow.com/questions/14191855/how-do-you-mock-the-time-for-boost-timers
   */
  static boost::posix_time::time_duration
  to_posix_duration(const duration& duration);

  friend struct boost::asio::time_traits<steady_clock>;
}; // class steady_clock


/**
 * \brief Get system_clock::TimePoint representing UNIX time epoch (00:00:00 on Jan 1, 1970)
 */
const system_clock::TimePoint&
getUnixEpoch();

/**
 * \brief Convert system_clock::TimePoint to UNIX timestamp
 */
milliseconds
toUnixTimestamp(const system_clock::TimePoint& point);

/**
 * \brief Convert UNIX timestamp to system_clock::TimePoint
 */
system_clock::TimePoint
fromUnixTimestamp(const milliseconds& duration);

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


////////////////////////////////////////////////////////////////////////////////

} // namespace time
} // namespace ndn

namespace boost {
namespace chrono {

template<class CharT>
struct clock_string<ndn::time::system_clock, CharT>
{
  static std::basic_string<CharT>
  since();
};

template<class CharT>
struct clock_string<ndn::time::steady_clock, CharT>
{
  static std::basic_string<CharT>
  since();
};

} // namespace chrono
} // namespace boost

#endif // NDN_TIME_HPP
