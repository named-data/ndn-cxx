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
 *
 * @author Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#ifndef NDN_NET_NETWORK_MONITOR_HPP
#define NDN_NET_NETWORK_MONITOR_HPP

#include "asio-fwd.hpp"
#include "network-interface.hpp"

#include <vector>

namespace ndn {
namespace net {

class NetworkMonitorImpl;

/**
 * @brief Network interface monitor
 *
 * Maintains an up-to-date view of every system network interface and notifies when an interface
 * is added or removed.
 *
 * @note The implementation of this class is highly platform dependent, and not all platform
 *       backends provide all the features. On macOS, @e SystemConfiguration and
 *       @e CFNotificationCenterAddObserver are used (notification of MTU change is not supported).
 *       On Linux, @e rtnetlink notifications from the kernel are used. See getCapabilities() for
 *       the detailed set of capabilities supported by the platform backend currently in use.
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

  /**
   * @brief Construct instance, request enumeration of all network interfaces, and start
   *        monitoring for network state changes
   *
   * @param io io_service instance that will dispatch events
   * @throw Error error starting monitoring
   */
  explicit
  NetworkMonitor(boost::asio::io_service& io);

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

  /// Returns a bitwise OR'ed set of @ref Capability flags supported on the current platform
  uint32_t
  getCapabilities() const;

  /// Returns the NetworkInterface with the given name, or @c nullptr if it does not exist
  shared_ptr<const NetworkInterface>
  getNetworkInterface(const std::string& ifname) const;

  /**
   * @brief Lists all network interfaces currently available on the system
   * @warning May return incomplete results if called before the
   *          #onEnumerationCompleted signal has been emitted.
   */
  std::vector<shared_ptr<const NetworkInterface>>
  listNetworkInterfaces() const;

protected:
  explicit
  NetworkMonitor(unique_ptr<NetworkMonitorImpl> impl);

  NetworkMonitorImpl&
  getImpl()
  {
    return *m_impl;
  }

private:
  const unique_ptr<NetworkMonitorImpl> m_impl;
  // Intentional violation of code-style rule 1.4: m_impl must be assigned before its signals can
  // be assigned to references below.

public: // signals
  /// Fires when network interfaces enumeration is complete
  util::Signal<NetworkMonitorImpl>& onEnumerationCompleted;

  /// Fires when a new interface is added
  util::Signal<NetworkMonitorImpl, shared_ptr<const NetworkInterface>>& onInterfaceAdded;

  /**
   * @brief Fires when an interface is removed
   * @note The NetworkInterface object is no longer present in the internal list of
   *       network interfaces when this signal is emitted.
   */
  util::Signal<NetworkMonitorImpl, shared_ptr<const NetworkInterface>>& onInterfaceRemoved;

  /// @deprecated Only for backward compatibility
  util::Signal<NetworkMonitorImpl>& onNetworkStateChanged;
};

class NetworkMonitorImpl : noncopyable
{
public:
  virtual
  ~NetworkMonitorImpl() = default;

  virtual uint32_t
  getCapabilities() const = 0;

  virtual shared_ptr<const NetworkInterface>
  getNetworkInterface(const std::string&) const = 0;

  virtual std::vector<shared_ptr<const NetworkInterface>>
  listNetworkInterfaces() const = 0;

protected:
  static shared_ptr<NetworkInterface>
  makeNetworkInterface();

public:
  util::Signal<NetworkMonitorImpl> onEnumerationCompleted;
  util::Signal<NetworkMonitorImpl, shared_ptr<const NetworkInterface>> onInterfaceAdded;
  util::Signal<NetworkMonitorImpl, shared_ptr<const NetworkInterface>> onInterfaceRemoved;
  util::Signal<NetworkMonitorImpl> onNetworkStateChanged;

protected:
  DECLARE_SIGNAL_EMIT(onEnumerationCompleted)
  DECLARE_SIGNAL_EMIT(onInterfaceAdded)
  DECLARE_SIGNAL_EMIT(onInterfaceRemoved)
  DECLARE_SIGNAL_EMIT(onNetworkStateChanged)
};

} // namespace net
} // namespace ndn

#endif // NDN_NET_NETWORK_MONITOR_HPP
