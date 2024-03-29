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

#ifndef NDN_CXX_UTIL_TIME_CUSTOM_CLOCK_HPP
#define NDN_CXX_UTIL_TIME_CUSTOM_CLOCK_HPP

#include "ndn-cxx/util/time.hpp"

#include <memory>

namespace ndn::time {

/**
 * \brief Class implementing custom system or steady clock behavior.
 *
 * Instance of specialization of this class may be passed to setCustomClocks() free function
 * in order to change global behavior of system or steady clock.
 */
template<typename BaseClock>
class CustomClock
{
public:
  virtual
  ~CustomClock() = default;

  virtual typename BaseClock::time_point
  getNow() const = 0;

  virtual std::string
  getSince() const = 0;

  virtual typename BaseClock::duration
  toWaitDuration(typename BaseClock::duration d) const = 0;
};

using CustomSystemClock = CustomClock<system_clock>;
using CustomSteadyClock = CustomClock<steady_clock>;

/**
 * \brief Set custom system and steady clocks.
 *
 * When \p steadyClock or \p systemClock set to nullptr, the default implementation
 * of the corresponding clock will be used
 */
void
setCustomClocks(std::shared_ptr<CustomSteadyClock> steadyClock = nullptr,
                std::shared_ptr<CustomSystemClock> systemClock = nullptr);

} // namespace ndn::time

#endif // NDN_CXX_UTIL_TIME_CUSTOM_CLOCK_HPP
