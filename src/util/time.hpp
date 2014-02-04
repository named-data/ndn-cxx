/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TIME_HPP
#define NDN_TIME_HPP

#include "../common.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ndn {

const boost::posix_time::ptime UNIX_EPOCH_TIME =
  boost::posix_time::ptime (boost::gregorian::date (1970, boost::gregorian::Jan, 1));

/**
 * @brief Get the current time in milliseconds since 1/1/1970, including fractions of a millisecond
 */
inline MillisecondsSince1970 
getNowMilliseconds()
{
  return (boost::posix_time::microsec_clock::universal_time() - UNIX_EPOCH_TIME).total_milliseconds();
}

inline MillisecondsSince1970 
ndn_getNowMilliseconds()
{
  return getNowMilliseconds();
}


/**
 * Convert to the ISO string representation of the time.
 * @param time Milliseconds since 1/1/1970.
 * @return The ISO string.
 */
inline std::string
toIsoString(const MillisecondsSince1970& time)
{
  boost::posix_time::ptime boostTime = UNIX_EPOCH_TIME + boost::posix_time::milliseconds(time);

  /// @todo Determine whether this is necessary at all
  if ((time % 1000) == 0)
    return boost::posix_time::to_iso_string(boostTime) + ".000000"; 
  else
    return boost::posix_time::to_iso_string(boostTime);
}
  
/**
 * Convert from the ISO string representation to the internal time format.
 * @param isoString The ISO time formatted string. 
 * @return The time in milliseconds since 1/1/1970.
 */
inline MillisecondsSince1970
fromIsoString(const std::string& isoString)
{
  boost::posix_time::ptime boostTime = boost::posix_time::from_iso_string(isoString);
  
  return (boostTime-UNIX_EPOCH_TIME).total_milliseconds();
}

namespace time {

class monotonic_clock;

/** \class Duration
 *  \brief represents a time interval
 *  Time unit is nanosecond.
 */
class Duration
{
public:
  Duration()
    : m_value(0)
  {
  }

  explicit
  Duration(int64_t value)
    : m_value(value)
  {
  }
  
  operator int64_t&()
  {
    return m_value;
  }

  operator const int64_t&() const
  {
    return m_value;
  }

  Duration
  operator+(const Duration& other) const
  {
    return Duration(this->m_value + other.m_value);
  }
  
  Duration
  operator-(const Duration& other) const
  {
    return Duration(this->m_value - other.m_value);
  }

private:
  int64_t m_value;
};

/** \class Point
 *  \brief represents a point in time
 *  This uses monotonic clock.
 */
class Point
{
public:
  Point()
    : m_value(0)
  {
  }

  explicit
  Point(int64_t value)
    : m_value(value)
  {
  }
  
  operator int64_t&()
  {
    return m_value;
  }

  operator const int64_t&() const
  {
    return m_value;
  }

  Point
  operator+(const Duration& other) const
  {
    return Point(this->m_value + static_cast<int64_t>(other));
  }
  
  Duration
  operator-(const Point& other) const
  {
    return Duration(this->m_value - other.m_value);
  }

  Point
  operator-(const Duration& other) const
  {
    return Point(this->m_value  - static_cast<int64_t>(other));
  }
  
private:
  int64_t m_value;
};

inline std::ostream&
operator<<(std::ostream &os, const Duration& duration)
{
  os << static_cast<int64_t>(duration) / 1000000000.0 << " s";
  return os;
}

/**
 * \brief Get current time
 * \return{ the current time in monotonic clock }
 */
Point
now();

/**
 * \brief Get time::Duration for the specified number of seconds
 */
template<class T>
inline Duration
seconds(T value)
{
  return Duration(value * static_cast<int64_t>(1000000000));
}

/**
 * \brief Get time::Duration for the specified number of milliseconds
 */
template<class T>
inline Duration
milliseconds(T value)
{
  return Duration(value * static_cast<int64_t>(1000000));
}

/**
 * \brief Get time::Duration for the specified number of microseconds
 */
template<class T>
inline Duration
microseconds(T value)
{
  return Duration(value * static_cast<int64_t>(1000));
}

/**
 * \brief Get time::Duration for the specified number of nanoseconds
 */
inline Duration
nanoseconds(int64_t value)
{
  return Duration(value);
}


} // namespace time

} // namespace ndn

#endif // NDN_TIME_HPP
