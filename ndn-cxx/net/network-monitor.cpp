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
 *
 * @author Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#include "ndn-cxx/net/network-monitor.hpp"
#include "ndn-cxx/util/logger.hpp"

#include "ndn-cxx/detail/config.hpp"
#include "ndn-cxx/net/impl/network-monitor-impl-noop.hpp"
#if defined(NDN_CXX_HAVE_OSX_FRAMEWORKS)
#include "ndn-cxx/net/impl/network-monitor-impl-osx.hpp"
#define NETWORK_MONITOR_IMPL_TYPE NetworkMonitorImplOsx
#elif defined(NDN_CXX_HAVE_NETLINK)
#include "ndn-cxx/net/impl/network-monitor-impl-netlink.hpp"
#define NETWORK_MONITOR_IMPL_TYPE NetworkMonitorImplNetlink
#else
#define NETWORK_MONITOR_IMPL_TYPE NetworkMonitorImplNoop
#endif

NDN_LOG_INIT(ndn.NetworkMonitor);

namespace ndn {
namespace net {

static unique_ptr<NetworkMonitorImpl>
makeNetworkMonitorImpl(boost::asio::io_service& io)
{
  try {
    return make_unique<NETWORK_MONITOR_IMPL_TYPE>(io);
  }
  catch (const std::runtime_error& e) {
    NDN_LOG_WARN("failed to initialize " BOOST_STRINGIZE(NETWORK_MONITOR_IMPL_TYPE) ": " << e.what());
    // fallback to dummy implementation
    return make_unique<NetworkMonitorImplNoop>(io);
  }
}

NetworkMonitor::NetworkMonitor(boost::asio::io_service& io)
  : NetworkMonitor(makeNetworkMonitorImpl(io))
{
}

NetworkMonitor::NetworkMonitor(unique_ptr<NetworkMonitorImpl> impl)
  : m_impl(std::move(impl))
  , onEnumerationCompleted(m_impl->onEnumerationCompleted)
  , onInterfaceAdded(m_impl->onInterfaceAdded)
  , onInterfaceRemoved(m_impl->onInterfaceRemoved)
  , onNetworkStateChanged(m_impl->onNetworkStateChanged)
{
}

uint32_t
NetworkMonitor::getCapabilities() const
{
  return m_impl->getCapabilities();
}

shared_ptr<const NetworkInterface>
NetworkMonitor::getNetworkInterface(const std::string& ifname) const
{
  return m_impl->getNetworkInterface(ifname);
}

std::vector<shared_ptr<const NetworkInterface>>
NetworkMonitor::listNetworkInterfaces() const
{
  return m_impl->listNetworkInterfaces();
}

shared_ptr<NetworkInterface>
NetworkMonitorImpl::makeNetworkInterface()
{
  // cannot use make_shared because NetworkInterface constructor is private
  return shared_ptr<NetworkInterface>(new NetworkInterface);
}

} // namespace net
} // namespace ndn
