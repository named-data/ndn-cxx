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

#include "network-monitor.hpp"
#include "ndn-cxx-config.hpp"

#if defined(NDN_CXX_HAVE_OSX_FRAMEWORKS)
#include "detail/network-monitor-impl-osx.hpp"
#define NETWORK_MONITOR_IMPL_TYPE NetworkMonitorImplOsx
#elif defined(NDN_CXX_HAVE_RTNETLINK)
#include "detail/network-monitor-impl-rtnl.hpp"
#define NETWORK_MONITOR_IMPL_TYPE NetworkMonitorImplRtnl
#else
#include "detail/network-monitor-impl-noop.hpp"
#define NETWORK_MONITOR_IMPL_TYPE NetworkMonitorImplNoop
#endif

namespace ndn {
namespace net {

NetworkMonitor::NetworkMonitor(boost::asio::io_service& io)
  : NetworkMonitor(make_unique<NETWORK_MONITOR_IMPL_TYPE>(io))
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
