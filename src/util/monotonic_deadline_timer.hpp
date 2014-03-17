/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

/**
 * This code is based on https://svn.boost.org/trac/boost/attachment/ticket/3504/MonotonicDeadlineTimer.h
 */

#ifndef NDN_UTIL_MONOTONIC_DEADLINE_TIMER_HPP
#define NDN_UTIL_MONOTONIC_DEADLINE_TIMER_HPP

#include <boost/asio.hpp>
#include "time.hpp"

namespace boost {
namespace asio {

template <>
struct time_traits<ndn::time::steady_clock::TimePoint::clock>
{
  typedef ndn::time::steady_clock::TimePoint time_type;
  typedef ndn::time::steady_clock::TimePoint::clock::duration duration_type;

  static time_type
  now()
  {
    return ndn::time::steady_clock::now();
  }

  static time_type
  add(const time_type& time, const duration_type& duration)
  {
    return time + duration;
  }

  static duration_type
  subtract(const time_type& timeLhs, const time_type& timeRhs)
  {
    return timeLhs - timeRhs;
  }

  static bool
  less_than(const time_type& timeLhs, const time_type& timeRhs)
  {
    return timeLhs < timeRhs;
  }

  static boost::posix_time::time_duration
  to_posix_duration(const duration_type& duration)
  {
    return
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
      boost::posix_time::nanoseconds(
        ndn::time::duration_cast<ndn::time::nanoseconds>(duration).count())
#else
      boost::posix_time::microseconds(
        ndn::time::duration_cast<ndn::time::microseconds>(duration).count())
#endif
      ;
  }
};

} // namespace asio
} // namespace boost

namespace ndn {

typedef boost::asio::basic_deadline_timer<time::steady_clock::TimePoint::clock> monotonic_deadline_timer;

} // namespace ndn

#endif // NDN_UTIL_MONOTONIC_DEADLINE_TIMER_HPP
