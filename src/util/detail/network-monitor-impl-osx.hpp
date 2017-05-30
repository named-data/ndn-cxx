/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#ifndef NDN_UTIL_NETWORK_MONITOR_IMPL_OSX_HPP
#define NDN_UTIL_NETWORK_MONITOR_IMPL_OSX_HPP

#include "ndn-cxx-config.hpp"
#include "../network-monitor.hpp"

#ifndef NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H
#error "This file should not be compiled ..."
#endif

#include "../scheduler.hpp"
#include "../scheduler-scoped-event-id.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

namespace ndn {
namespace util {

class NetworkMonitor::Impl
{
public:
  Impl(NetworkMonitor& nm, boost::asio::io_service& io);

  ~Impl();

  uint32_t
  getCapabilities() const
  {
    return NetworkMonitor::CAP_NONE;
    /// \todo #3817 change to CAP_ENUM | CAP_IF_ADD_REMOVE | CAP_STATE_CHANGE | CAP_ADDR_ADD_REMOVE
  }

  shared_ptr<NetworkInterface>
  getNetworkInterface(const std::string& ifname) const;

  std::vector<shared_ptr<NetworkInterface>>
  listNetworkInterfaces() const;

  static void
  afterNotificationCenterEvent(CFNotificationCenterRef center,
                               void* observer,
                               CFStringRef name,
                               const void* object,
                               CFDictionaryRef userInfo);

private:
  void
  scheduleCfLoop();

  void
  pollCfLoop();

private:
  NetworkMonitor& m_nm;

  Scheduler m_scheduler;
  scheduler::ScopedEventId m_cfLoopEvent;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NETWORK_MONITOR_IMPL_OSX_HPP
