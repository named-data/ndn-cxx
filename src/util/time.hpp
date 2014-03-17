/**
 * Copyright (C) 2013-2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TIME_HPP
#define NDN_TIME_HPP

#include "../common.hpp"
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
inline std::string
toIsoString(const system_clock::TimePoint& timePoint)
{
  boost::posix_time::ptime ptime = boost::posix_time::from_time_t(
                                     system_clock::TimePoint::clock::to_time_t(timePoint));

  uint64_t micro = duration_cast<microseconds>(timePoint - getUnixEpoch()).count() % 1000000;
  if (micro > 0)
    {
      ptime += boost::posix_time::microseconds(micro);
      return boost::posix_time::to_iso_string(ptime);
    }
  else
    return boost::posix_time::to_iso_string(ptime);
}

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
inline system_clock::TimePoint
fromIsoString(const std::string& isoString)
{
  static boost::posix_time::ptime posixTimeEpoch = boost::posix_time::from_time_t(0);

  boost::posix_time::ptime ptime = boost::posix_time::from_iso_string(isoString);

  system_clock::TimePoint point = system_clock::from_time_t((ptime - posixTimeEpoch).total_seconds());
  point += microseconds((ptime - posixTimeEpoch).total_microseconds() % 1000000);
  return point;
}

} // namespace time
} // namespace ndn

#endif // NDN_TIME_HPP
