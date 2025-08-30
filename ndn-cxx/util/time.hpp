/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_UTIL_TIME_HPP
#define NDN_CXX_UTIL_TIME_HPP

#include <boost/asio/wait_traits.hpp>
#include <boost/chrono.hpp>

#include <cstdint>
#include <limits>
#include <locale>
#include <string>
#include <type_traits>

namespace ndn {
namespace time {

template<typename Rep, typename Period>
using duration = ::boost::chrono::duration<Rep, Period>;

using ::boost::chrono::duration_cast;

// C++20
using days   = duration<int_fast32_t, boost::ratio<86400>>;
using weeks  = duration<int_fast32_t, boost::ratio<604800>>;
using months = duration<int_fast32_t, boost::ratio<2629746>>;
using years  = duration<int_fast32_t, boost::ratio<31556952>>;

// C++11
using hours        = ::boost::chrono::hours;
using minutes      = ::boost::chrono::minutes;
using seconds      = ::boost::chrono::seconds;
using milliseconds = ::boost::chrono::milliseconds;
using microseconds = ::boost::chrono::microseconds;
using nanoseconds  = ::boost::chrono::nanoseconds;

/**
 * \brief Returns the absolute value of the duration \p d.
 * \note This function does not participate in overload resolution
 *       unless \c std::numeric_limits<Rep>::is_signed is true.
 */
template<typename Rep, typename Period, typename = std::enable_if_t<std::numeric_limits<Rep>::is_signed>>
constexpr duration<Rep, Period>
abs(duration<Rep, Period> d)
{
  return d >= d.zero() ? d : -d;
}

} // namespace time

inline namespace literals {
inline namespace time_literals {

constexpr time::days
operator ""_day(unsigned long long days)
{
  return time::days{days};
}

constexpr time::duration<long double, time::days::period>
operator ""_day(long double days)
{
  return time::duration<long double, time::days::period>{days};
}

constexpr time::days
operator ""_days(unsigned long long days)
{
  return time::days{days};
}

constexpr time::duration<long double, time::days::period>
operator ""_days(long double days)
{
  return time::duration<long double, time::days::period>{days};
}

constexpr time::hours
operator ""_h(unsigned long long hrs)
{
  return time::hours{hrs};
}

constexpr time::duration<long double, time::hours::period>
operator ""_h(long double hrs)
{
  return time::duration<long double, time::hours::period>{hrs};
}

constexpr time::minutes
operator ""_min(unsigned long long mins)
{
  return time::minutes{mins};
}

constexpr time::duration<long double, time::minutes::period>
operator ""_min(long double mins)
{
  return time::duration<long double, time::minutes::period>{mins};
}

constexpr time::seconds
operator ""_s(unsigned long long secs)
{
  return time::seconds{secs};
}

constexpr time::duration<long double, time::seconds::period>
operator ""_s(long double secs)
{
  return time::duration<long double, time::seconds::period>{secs};
}

constexpr time::milliseconds
operator ""_ms(unsigned long long msecs)
{
  return time::milliseconds{msecs};
}

constexpr time::duration<long double, time::milliseconds::period>
operator ""_ms(long double msecs)
{
  return time::duration<long double, time::milliseconds::period>{msecs};
}

constexpr time::microseconds
operator ""_us(unsigned long long usecs)
{
  return time::microseconds{usecs};
}

constexpr time::duration<long double, time::microseconds::period>
operator ""_us(long double usecs)
{
  return time::duration<long double, time::microseconds::period>{usecs};
}

constexpr time::nanoseconds
operator ""_ns(unsigned long long nsecs)
{
  return time::nanoseconds{nsecs};
}

constexpr time::duration<long double, time::nanoseconds::period>
operator ""_ns(long double nsecs)
{
  return time::duration<long double, time::nanoseconds::period>{nsecs};
}

} // inline namespace time_literals
} // inline namespace literals

namespace time {

using namespace ::ndn::literals::time_literals;

/**
 * \brief System clock.
 *
 * System clock represents the system-wide real time wall clock.
 *
 * It may not be monotonic: on most systems, the system time can be
 * adjusted at any moment. It is the only clock that has the ability
 * to be displayed and converted to/from UNIX timestamp.
 *
 * To get the current time:
 *
 * \code
 * const auto now = ndn::time::system_clock::now();
 * \endcode
 *
 * To convert a time_point to/from UNIX timestamp:
 *
 * \code
 * time::system_clock::time_point t1 = ...;
 * auto timeInMilliseconds = time::toUnixTimestamp(t1).count();
 * time::system_clock::time_point t2 = time::fromUnixTimestamp(time::milliseconds(timeInMilliseconds));
 * \endcode
 */
class system_clock
{
public:
  using duration   = ::boost::chrono::system_clock::duration;
  using rep        = duration::rep;
  using period     = duration::period;
  using time_point = ::boost::chrono::time_point<system_clock>;
  static constexpr bool is_steady = ::boost::chrono::system_clock::is_steady;

  static time_point
  now() noexcept;

  static std::time_t
  to_time_t(const time_point& t) noexcept;

  static time_point
  from_time_t(std::time_t t) noexcept;
};

/**
 * \brief Steady clock.
 *
 * Steady clock represents a monotonic clock. The time points of this
 * clock cannot decrease as physical time moves forward. This clock is
 * not related to wall clock time, and is best suitable for measuring
 * intervals.
 */
class steady_clock
{
public:
  using duration   = ::boost::chrono::steady_clock::duration;
  using rep        = duration::rep;
  using period     = duration::period;
  using time_point = ::boost::chrono::time_point<steady_clock>;
  static constexpr bool is_steady = true;

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
 * \brief Return a system_clock::time_point representing the UNIX time epoch,
 *        i.e., 00:00:00 UTC on 1 January 1970.
 */
const system_clock::time_point&
getUnixEpoch();

/**
 * \brief Convert system_clock::time_point to UNIX timestamp.
 */
template<typename Duration = milliseconds>
constexpr Duration
toUnixTimestamp(const system_clock::time_point& tp)
{
  return duration_cast<Duration>(tp.time_since_epoch());
}

/**
 * \brief Convert UNIX timestamp to system_clock::time_point.
 */
constexpr system_clock::time_point
fromUnixTimestamp(system_clock::duration d)
{
  return system_clock::time_point{d};
}

/**
 * \brief Convert to the ISO 8601 string representation, basic format (`YYYYMMDDTHHMMSS,fffffffff`).
 *
 * If \p timePoint does not contain any fractional seconds, the output format is `YYYYMMDDTHHMMSS`.
 *
 * Examples:
 *
 *   - with fractional nanoseconds:  20020131T100001,123456789
 *   - with fractional microseconds: 20020131T100001,123456
 *   - with fractional milliseconds: 20020131T100001,123
 *   - without fractional seconds:   20020131T100001
 */
std::string
toIsoString(const system_clock::time_point& timePoint);

/**
 * \brief Convert from the ISO 8601 basic string format (`YYYYMMDDTHHMMSS,fffffffff`)
 *        to the internal time format.
 *
 * Examples of accepted strings:
 *
 *   - with fractional nanoseconds:  20020131T100001,123456789
 *   - with fractional microseconds: 20020131T100001,123456
 *   - with fractional milliseconds: 20020131T100001,123
 *   - without fractional seconds:   20020131T100001
 */
system_clock::time_point
fromIsoString(const std::string& isoString);

/**
 * \brief Convert to the ISO 8601 string representation, extended format (`YYYY-MM-DDTHH:MM:SS,fffffffff`).
 */
std::string
toIsoExtendedString(const system_clock::time_point& timePoint);

/**
 * \brief Convert from the ISO 8601 extended string format (`YYYY-MM-DDTHH:MM:SS,fffffffff`)
 *        to the internal time format.
 */
system_clock::time_point
fromIsoExtendedString(const std::string& isoString);

/**
 * \brief Convert time point to string with specified format.
 *
 * By default, `%Y-%m-%d %H:%M:%S` is used, producing dates like
 * `2014-04-10 22:51:00`
 *
 * \param timePoint time point of system_clock
 * \param format desired output format (default: `%Y-%m-%d %H:%M:%S`)
 * \param locale desired locale (default: "C" locale)
 *
 * \sa https://www.boost.org/doc/libs/1_74_0/doc/html/date_time/date_time_io.html#date_time.format_flags
 *     describes possible formatting flags
 **/
std::string
toString(const system_clock::time_point& timePoint,
         const std::string& format = "%Y-%m-%d %H:%M:%S",
         const std::locale& locale = std::locale("C"));

/**
 * \brief Convert from string of specified format into time point.
 *
 * By default, `%Y-%m-%d %H:%M:%S` is used, accepting dates like
 * `2014-04-10 22:51:00`
 *
 * \param timePointStr string representing time point
 * \param format input output format (default: `%Y-%m-%d %H:%M:%S`)
 * \param locale input locale (default: "C" locale)
 *
 * \sa https://www.boost.org/doc/libs/1_74_0/doc/html/date_time/date_time_io.html#date_time.format_flags
 *     describes possible formatting flags
 */
system_clock::time_point
fromString(const std::string& timePointStr,
           const std::string& format = "%Y-%m-%d %H:%M:%S",
           const std::locale& locale = std::locale("C"));

} // namespace time
} // namespace ndn

namespace boost::chrono {

template<typename CharT>
struct clock_string<ndn::time::system_clock, CharT>
{
  static std::basic_string<CharT>
  since();
};

template<typename CharT>
struct clock_string<ndn::time::steady_clock, CharT>
{
  static std::basic_string<CharT>
  since();
};

extern template struct clock_string<ndn::time::system_clock, char>;
extern template struct clock_string<ndn::time::steady_clock, char>;

} // namespace boost::chrono

#endif // NDN_CXX_UTIL_TIME_HPP
