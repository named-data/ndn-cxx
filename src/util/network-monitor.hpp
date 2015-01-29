/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_UTIL_NETWORK_MONITOR_HPP
#define NDN_UTIL_NETWORK_MONITOR_HPP

#include "signal.hpp"

#include <boost/asio/io_service.hpp>

namespace ndn {
namespace util {

/**
 * @brief Network state change monitor
 *
 * When network change is detected, onNetworkStateChanged signal will be fired.
 * Monitoring is run for the lifetime of the NetworkMonitor instance.
 *
 * @note Implementation of this class is platform dependent and not all supported platforms
 *       are supported:
 *       - OS X: CFNotificationCenterAddObserver
 *       - Linux: rtnetlink notifications
 *
 * Network state change detection is not guaranteed to be precise and (zero or more)
 * notifications are expected to be fired for the following events:
 * - any network interface going up or down
 * - IPv4 or IPv6 address changes on any of the interfaces
 */
class NetworkMonitor : boost::noncopyable
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  /**
   * @brief Construct instance and start monitoring for network state changes
   * @param io io_service thread that will dispatch events
   * @throw Error when network monitoring is not supported or there is an error starting monitoring
   */
  explicit
  NetworkMonitor(boost::asio::io_service& io);

  /**
   * @brief Terminate network state monitoring
   */
  ~NetworkMonitor();

  Signal<NetworkMonitor> onNetworkStateChanged;

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace util
} // namespace autoconfig

#endif // NDN_UTIL_NETWORK_MONITOR_HPP
