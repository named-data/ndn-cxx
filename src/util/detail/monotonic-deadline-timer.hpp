/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

/**
 * This code is based on https://svn.boost.org/trac/boost/attachment/ticket/3504/MonotonicDeadlineTimer.h
 */

#ifndef NDN_UTIL_DETAIL_MONOTONIC_DEADLINE_TIMER_HPP
#define NDN_UTIL_DETAIL_MONOTONIC_DEADLINE_TIMER_HPP

#include "../time.hpp"
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/io_service.hpp>

namespace boost {
namespace asio {

template<>
struct time_traits<ndn::time::steady_clock>
{
  using time_type     = ndn::time::steady_clock::TimePoint;
  using duration_type = ndn::time::steady_clock::Duration;

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
    return ndn::time::steady_clock::to_posix_duration(duration);
  }
};

} // namespace asio
} // namespace boost

namespace ndn {
namespace util {
namespace detail {

class MonotonicDeadlineTimer : public boost::asio::basic_deadline_timer<time::steady_clock>
{
public:
  MonotonicDeadlineTimer(boost::asio::io_service& ioService)
    : boost::asio::basic_deadline_timer<time::steady_clock>(ioService)
  {
  }
};

} // namespace detail
} // namespace util
} // namespace ndn

#endif // NDN_UTIL_DETAIL_MONOTONIC_DEADLINE_TIMER_HPP
