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
 *
 * @author Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#ifndef NDN_UTIL_NETWORK_MONITOR_HPP
#define NDN_UTIL_NETWORK_MONITOR_HPP

#include "signal.hpp"

#include <vector>

// forward declaration
namespace boost {
namespace asio {
class io_service;
} // namespace asio
} // namespace boost

namespace ndn {
namespace util {

class NetworkInterface;

/**
 * @brief Network interfaces monitor
 *
 * Maintains an up-to-date view of every system network interface and notifies when an interface
 * is added or removed.
 *
 * @note Implementation of this class is platform dependent and not all supported platforms
 *       are supported:
 *       - OS X: CFNotificationCenterAddObserver (incomplete)
 *       - Linux: rtnetlink notifications
 *
 * @todo macOS implementation needs to be updated to emit the new signals and keep track of
 *       interfaces (links) and addresses
 */
class NetworkMonitor : noncopyable
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

  class Impl;

  /**
   * @brief Construct instance, request enumeration of all network interfaces, and start
   *        monitoring for network state changes
   *
   * @param io io_service thread that will dispatch events
   * @throw Error when network monitoring is not supported or there is an error starting monitoring
   */
  explicit
  NetworkMonitor(boost::asio::io_service& io);

  ~NetworkMonitor();

  enum Capability : uint32_t {
    /// NetworkMonitor is not supported and is a no-op
    CAP_NONE = 0,
    /// listNetworkInterfaces() and getNetworkInterface() are supported
    CAP_ENUM = 1 << 0,
    /// NetworkMonitor onInterfaceAdded and onInterfaceRemoved signals are supported
    CAP_IF_ADD_REMOVE = 1 << 1,
    /// NetworkInterface onStateChanged signal is supported
    CAP_STATE_CHANGE = 1 << 2,
    /// NetworkInterface onMtuChanged signal is supported
    CAP_MTU_CHANGE = 1 << 3,
    /// NetworkInterface onAddressAdded and onAddressRemoved signals are supported
    CAP_ADDR_ADD_REMOVE = 1 << 4
  };

  /** \return bitwise OR'ed \p Capability supported on current platform
   */
  uint32_t
  getCapabilities() const;

  shared_ptr<NetworkInterface>
  getNetworkInterface(const std::string& ifname) const;

  std::vector<shared_ptr<NetworkInterface>>
  listNetworkInterfaces() const;

public: // signals
  /** @brief Fires when network interfaces enumeration is complete
   */
  Signal<NetworkMonitor> onEnumerationCompleted;

  /** @brief Fires when a new interface is added
   */
  Signal<NetworkMonitor, shared_ptr<NetworkInterface>> onInterfaceAdded;

  /**
   * @brief Fires when an interface is removed
   * @note The NetworkInterface object is no longer present in the network
   *       interfaces map when the signal is emitted
   */
  Signal<NetworkMonitor, shared_ptr<NetworkInterface>> onInterfaceRemoved;

  // only for backward compatibility
  Signal<NetworkMonitor> onNetworkStateChanged;

private:
  const unique_ptr<Impl> m_impl;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NETWORK_MONITOR_HPP
