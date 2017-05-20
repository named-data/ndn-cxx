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

#if defined(NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H)
#include "detail/network-monitor-impl-osx.hpp"
#elif defined(NDN_CXX_HAVE_RTNETLINK)
#include "detail/network-monitor-impl-rtnl.hpp"
#else

namespace ndn {
namespace util {

class NetworkMonitor::Impl
{
public:
  Impl(NetworkMonitor&, boost::asio::io_service&)
  {
    BOOST_THROW_EXCEPTION(Error("Network monitoring is not supported on this platform"));
  }

  shared_ptr<NetworkInterface>
  getNetworkInterface(const std::string&) const
  {
    return {};
  }

  std::vector<shared_ptr<NetworkInterface>>
  listNetworkInterfaces() const
  {
    return {};
  }
};

} // namespace util
} // namespace ndn

#endif

namespace ndn {
namespace util {

NetworkMonitor::NetworkMonitor(boost::asio::io_service& io)
  : m_impl(make_unique<Impl>(*this, io))
{
}

NetworkMonitor::~NetworkMonitor() = default;

shared_ptr<NetworkInterface>
NetworkMonitor::getNetworkInterface(const std::string& ifname) const
{
  return m_impl->getNetworkInterface(ifname);
}

std::vector<shared_ptr<NetworkInterface>>
NetworkMonitor::listNetworkInterfaces() const
{
  return m_impl->listNetworkInterfaces();
}

} // namespace util
} // namespace ndn
