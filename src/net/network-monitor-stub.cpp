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

#include "network-monitor-stub.hpp"
#include <unordered_map>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace ndn {
namespace net {

class NetworkMonitorImplStub : public NetworkMonitorImpl
{
public:
  explicit
  NetworkMonitorImplStub(uint32_t capabilities)
    : m_capabilities(capabilities)
  {
  }

  uint32_t
  getCapabilities() const final
  {
    return m_capabilities;
  }

  shared_ptr<const NetworkInterface>
  getNetworkInterface(const std::string& ifname) const final
  {
    auto i = m_interfaces.find(ifname);
    return i == m_interfaces.end() ? nullptr : i->second;
  }

  std::vector<shared_ptr<const NetworkInterface>>
  listNetworkInterfaces() const final
  {
    std::vector<shared_ptr<const NetworkInterface>> v;
    boost::copy(m_interfaces | boost::adaptors::map_values, std::back_inserter(v));
    return v;
  }

public: // internal
  using NetworkMonitorImpl::makeNetworkInterface;

  void
  addInterface(shared_ptr<NetworkInterface> netif)
  {
    BOOST_ASSERT(netif != nullptr);
    bool isNew = m_interfaces.emplace(netif->getName(), netif).second;
    if (!isNew) {
      BOOST_THROW_EXCEPTION(std::invalid_argument("duplicate ifname"));
    }
    this->emitSignal(onInterfaceAdded, netif);
  }

  void
  removeInterface(const std::string& ifname)
  {
    auto i = m_interfaces.find(ifname);
    if (i == m_interfaces.end()) {
      return;
    }
    shared_ptr<NetworkInterface> netif = i->second;
    m_interfaces.erase(i);
    this->emitSignal(onInterfaceRemoved, netif);
  }

  void
  emitEnumerationCompleted()
  {
    this->emitSignal(onEnumerationCompleted);
  }

private:
  uint32_t m_capabilities;
  std::unordered_map<std::string, shared_ptr<NetworkInterface>> m_interfaces;
};

NetworkMonitorStub::NetworkMonitorStub(uint32_t capabilities)
  : NetworkMonitor(make_unique<NetworkMonitorImplStub>(capabilities))
{
}

NetworkMonitorImplStub&
NetworkMonitorStub::getImpl()
{
  return static_cast<NetworkMonitorImplStub&>(this->NetworkMonitor::getImpl());
}

shared_ptr<NetworkInterface>
NetworkMonitorStub::makeNetworkInterface()
{
  return NetworkMonitorImplStub::makeNetworkInterface();
}

void
NetworkMonitorStub::addInterface(shared_ptr<NetworkInterface> netif)
{
  this->getImpl().addInterface(std::move(netif));
}

void
NetworkMonitorStub::removeInterface(const std::string& ifname)
{
  this->getImpl().removeInterface(ifname);
}

void
NetworkMonitorStub::emitEnumerationCompleted()
{
  this->getImpl().emitEnumerationCompleted();
}

} // namespace net
} // namespace ndn
