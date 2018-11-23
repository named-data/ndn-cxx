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

#ifndef NDN_UTIL_DETAIL_STEADY_TIMER_HPP
#define NDN_UTIL_DETAIL_STEADY_TIMER_HPP

#include "../time.hpp"

#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/wait_traits.hpp>

namespace boost {
namespace asio {

template<>
struct wait_traits<ndn::time::steady_clock>
{
  static ndn::time::steady_clock::duration
  to_wait_duration(const ndn::time::steady_clock::duration& d)
  {
    return ndn::time::steady_clock::to_wait_duration(d);
  }
};

} // namespace asio
} // namespace boost

namespace ndn {
namespace util {
namespace detail {

class SteadyTimer : public boost::asio::basic_waitable_timer<time::steady_clock>
{
public:
  using boost::asio::basic_waitable_timer<time::steady_clock>::basic_waitable_timer;
};

} // namespace detail
} // namespace util
} // namespace ndn

#endif // NDN_UTIL_DETAIL_STEADY_TIMER_HPP
