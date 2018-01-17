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

#ifndef NDN_UTIL_TIME_HPP
#define NDN_UTIL_TIME_HPP

#include "../common.hpp"

#include <boost/asio/wait_traits.hpp>
#include <boost/chrono.hpp>

namespace ndn {
namespace time {

using boost::chrono::duration;
using boost::chrono::duration_cast;

using days = duration<int_fast32_t, boost::ratio<86400>>;
using boost::chrono::hours;
using boost::chrono::minutes;
using boost::chrono::seconds;
using boost::chrono::milliseconds;
using boost::chrono::microseconds;
using boost::chrono::nanoseconds;

/** \return the absolute value of the duration d
 *  \note The function does not participate in the overload resolution
 *        unless std::numeric_limits<Rep>::is_signed is true.
 */
template<typename Rep, typename Period,
         typename = typename std::enable_if<std::numeric_limits<Rep>::is_signed>::type>
constexpr duration<Rep, Period>
abs(duration<Rep, Period> d)
{
  return d >= d.zero() ? d : -d;
}

} // namespace time

inline namespace literals {
inline namespace time_literals {

constexpr time::days
operator "" _day(unsigned long long days)
{
  return time::days{days};
}

constexpr time::duration<long double, time::days::period>
operator "" _day(long double days)
{
  return time::duration<long double, time::days::period>{days};
}

constexpr time::days
operator "" _days(unsigned long long days)
{
  return time::days{days};
}

constexpr time::duration<long double, time::days::period>
operator "" _days(long double days)
{
  return time::duration<long double, time::days::period>{days};
}

constexpr time::hours
operator "" _h(unsigned long long hrs)
{
  return time::hours{hrs};
}

constexpr time::duration<long double, time::hours::period>
operator "" _h(long double hrs)
{
  return time::duration<long double, time::hours::period>{hrs};
}

constexpr time::minutes
operator "" _min(unsigned long long mins)
{
  return time::minutes{mins};
}

constexpr time::duration<long double, time::minutes::period>
operator "" _min(long double mins)
{
  return time::duration<long double, time::minutes::period>{mins};
}

constexpr time::seconds
operator "" _s(unsigned long long secs)
{
  return time::seconds{secs};
}

constexpr time::duration<long double, time::seconds::period>
operator "" _s(long double secs)
{
  return time::duration<long double, time::seconds::period>{secs};
}

constexpr time::milliseconds
operator "" _ms(unsigned long long msecs)
{
  return time::milliseconds{msecs};
}

constexpr time::duration<long double, time::milliseconds::period>
operator "" _ms(long double msecs)
{
  return time::duration<long double, time::milliseconds::period>{msecs};
}

constexpr time::microseconds
operator "" _us(unsigned long long usecs)
{
  return time::microseconds{usecs};
}

constexpr time::duration<long double, time::microseconds::period>
operator "" _us(long double usecs)
{
  return time::duration<long double, time::microseconds::period>{usecs};
}

constexpr time::nanoseconds
operator "" _ns(unsigned long long nsecs)
{
  return time::nanoseconds{nsecs};
}

constexpr time::duration<long double, time::nanoseconds::period>
operator "" _ns(long double nsecs)
{
  return time::duration<long double, time::nanoseconds::period>{nsecs};
}

} // inline namespace time_literals
} // inline namespace literals

namespace time {

using namespace literals::time_literals;

/**
 * \brief System clock
 *
 * System clock represents the system-wide real time wall clock.
 *
 * It may not be monotonic: on most systems, the system time can be
 * adjusted at any moment. It is the only clock that has the ability
 * to be displayed and converted to/from UNIX timestamp.
 *
 * To get the current time:
 *
 * <code>
 *     system_clock::TimePoint now = system_clock::now();
 * </code>
 *
 * To convert a TimePoint to/from UNIX timestamp:
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
  using duration   = boost::chrono::system_clock::duration;
  using rep        = duration::rep;
  using period     = duration::period;
  using time_point = boost::chrono::time_point<system_clock>;
  static constexpr bool is_steady = boost::chrono::system_clock::is_steady;

  typedef time_point TimePoint;
  typedef duration Duration;

  static time_point
  now() noexcept;

  static std::time_t
  to_time_t(const time_point& t) noexcept;

  static time_point
  from_time_t(std::time_t t) noexcept;
};

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
  using duration   = boost::chrono::steady_clock::duration;
  using rep        = duration::rep;
  using period     = duration::period;
  using time_point = boost::chrono::time_point<steady_clock>;
  static constexpr bool is_steady = true;

  typedef time_point TimePoint;
  typedef duration Duration;

  static time_point
  now() noexcept;

private:
  /**
   * \brief Trait function used in detail::SteadyTimer to select proper waiting time
   *
   * Mock time implementations should return the minimum value to ensure
   * that Boost.Asio doesn't perform any waiting on mock timers.
   *
   * @sa http://blog.think-async.com/2007/08/time-travel.html
   */
  static duration
  to_wait_duration(duration d);

  friend struct boost::asio::wait_traits<steady_clock>; // see steady-timer.hpp
};

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
fromUnixTimestamp(milliseconds duration);

/**
 * \brief Convert to the ISO string representation of the time (YYYYMMDDTHHMMSS,fffffffff)
 *
 * If \p timePoint contains doesn't contain fractional seconds,
 * the output format is YYYYMMDDTHHMMSS
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
 * \sa http://www.boost.org/doc/libs/1_54_0/doc/html/date_time/date_time_io.html#date_time.format_flags
 *     describes possible formatting flags
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
 * \param timePointStr string representing time point
 * \param format input output format (default: `%Y-%m-%d %H:%M:%S`)
 * \param locale input locale (default: "C" locale)
 *
 * \sa http://www.boost.org/doc/libs/1_54_0/doc/html/date_time/date_time_io.html#date_time.format_flags
 *     describes possible formatting flags
 */
system_clock::TimePoint
fromString(const std::string& timePointStr,
           const std::string& format = "%Y-%m-%d %H:%M:%S",
           const std::locale& locale = std::locale("C"));

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

extern template struct clock_string<ndn::time::system_clock, char>;
extern template struct clock_string<ndn::time::steady_clock, char>;

} // namespace chrono
} // namespace boost

#endif // NDN_UTIL_TIME_HPP
