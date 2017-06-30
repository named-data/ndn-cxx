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
 *
 * Parts of this implementation is based on daemondo command of MacPorts
 * (https://www.macports.org/):
 *
 *    Copyright (c) 2005-2007 James Berry <jberry@macports.org>
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of The MacPorts Project nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#include "ndn-cxx-config.hpp"

#include "network-monitor-impl-osx.hpp"
#include "../../name.hpp"
#include "../../util/logger.hpp"
#include "../network-address.hpp"

#include <ifaddrs.h>       // for getifaddrs()
#include <arpa/inet.h>     // for inet_ntop()
#include <netinet/in.h>    // for struct sockaddr_in{,6}
#include <net/if_dl.h>     // for struct sockaddr_dl
#include <net/if_types.h>  // for IFT_* constants

#include <boost/asio.hpp>

namespace ndn {
namespace net {

NDN_LOG_INIT(ndn.NetworkMonitor);

NetworkMonitor::Impl::Impl(NetworkMonitor& nm, boost::asio::io_service& io)
  : m_nm(nm)
  , m_scheduler(io)
  , m_cfLoopEvent(m_scheduler)
  , m_context{0, this, nullptr, nullptr, nullptr}
  , m_scStore(SCDynamicStoreCreate(nullptr, CFSTR("net.named-data.ndn-cxx.NetworkMonitor"),
                                   &Impl::onConfigChanged, &m_context))
  , m_loopSource(SCDynamicStoreCreateRunLoopSource(nullptr, m_scStore.get(), 0))
  , m_nullUdpSocket(io, boost::asio::ip::udp::v4())

{
  scheduleCfLoop();

  // Notifications from Darwin Notify Center:
  //
  // com.apple.system.config.network_change
  //
  CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                  static_cast<void*>(this),
                                  &Impl::afterNotificationCenterEvent,
                                  CFSTR("com.apple.system.config.network_change"),
                                  nullptr, // object to observe
                                  CFNotificationSuspensionBehaviorDeliverImmediately);

  io.post([this] { enumerateInterfaces(); });

  CFRunLoopAddSource(CFRunLoopGetCurrent(), m_loopSource.get(), kCFRunLoopDefaultMode);

  // Notifications from SystemConfiguration:
  //
  // State:/Network/Interface/.*/Link
  // State:/Network/Interface/.*/IPv4
  // State:/Network/Interface/.*/IPv6
  // State:/Network/Global/DNS
  // State:/Network/Global/IPv4
  //
  auto patterns = CFArrayCreateMutable(nullptr, 0, &kCFTypeArrayCallBacks);
  CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/.*/Link"));
  CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/.*/IPv4"));
  CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/.*/IPv6"));
  // CFArrayAppendValue(patterns, CFSTR("State:/Network/Global/DNS"));
  // CFArrayAppendValue(patterns, CFSTR("State:/Network/Global/IPv4"));

  SCDynamicStoreSetNotificationKeys(m_scStore.get(), nullptr, patterns);
}

NetworkMonitor::Impl::~Impl()
{
  CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m_loopSource.get(), kCFRunLoopDefaultMode);

  CFNotificationCenterRemoveEveryObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                          static_cast<void*>(this));
}

shared_ptr<NetworkInterface>
NetworkMonitor::Impl::getNetworkInterface(const std::string& ifname) const
{
  auto it = m_interfaces.find(ifname);
  if (it != m_interfaces.end()) {
    return it->second;
  }
  else {
    return nullptr;
  }
}

std::vector<shared_ptr<NetworkInterface>>
NetworkMonitor::Impl::listNetworkInterfaces() const
{
  std::vector<shared_ptr<NetworkInterface>> v;
  v.reserve(m_interfaces.size());

  for (const auto& e : m_interfaces) {
    v.push_back(e.second);
  }
  return v;
}

void
NetworkMonitor::Impl::afterNotificationCenterEvent(CFNotificationCenterRef center,
                                                   void* observer,
                                                   CFStringRef name,
                                                   const void* object,
                                                   CFDictionaryRef userInfo)
{
  static_cast<Impl*>(observer)->m_nm.onNetworkStateChanged();
}

void
NetworkMonitor::Impl::scheduleCfLoop()
{
  // poll each second for new events
  m_cfLoopEvent = m_scheduler.scheduleEvent(time::seconds(1), bind(&Impl::pollCfLoop, this));
}

void
NetworkMonitor::Impl::pollCfLoop()
{
  // this should dispatch ready events and exit
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);

  scheduleCfLoop();
}

void
NetworkMonitor::Impl::addNewInterface(const std::string& ifName)
{
  shared_ptr<NetworkInterface> interface(new NetworkInterface);

  interface->setName(ifName);
  interface->setState(getInterfaceState(interface->getName()));
  updateInterfaceInfo(*interface);
  if (interface->getType() == InterfaceType::UNKNOWN) {
    NDN_LOG_DEBUG("ignoring " << ifName << " because it has unhandled interface type");
    return;
  }

  NDN_LOG_DEBUG("adding interface " << interface->getName());
  m_interfaces.insert(make_pair(interface->getName(), interface));
  m_nm.onInterfaceAdded(interface);
}

void
NetworkMonitor::Impl::enumerateInterfaces()
{
  for (const auto& ifName : getInterfaceNames()) {
    addNewInterface(ifName);
  }
  m_nm.onEnumerationCompleted();
}

static std::string
convertToStdString(CFStringRef cfString)
{
  const char* cStr = CFStringGetCStringPtr(cfString, kCFStringEncodingASCII);
  if (cStr != nullptr) {
    return cStr;
  }

  size_t stringSize =  CFStringGetLength(cfString);
  char* buffer = new char[stringSize + 1];
  CFStringGetCString(cfString, buffer, sizeof(buffer), kCFStringEncodingASCII);
  std::string retval = buffer;
  delete [] buffer;
  return retval;
}

std::set<std::string>
NetworkMonitor::Impl::getInterfaceNames()
{
  CFReleaser<CFDictionaryRef> dict = (CFDictionaryRef)SCDynamicStoreCopyValue(m_scStore.get(), CFSTR("State:/Network/Interface"));
  CFArrayRef interfaces = (CFArrayRef)CFDictionaryGetValue(dict.get(), CFSTR("Interfaces"));

  std::set<std::string> ifNames;
  size_t count = CFArrayGetCount(interfaces);
  for (size_t i = 0; i != count; ++i) {
    auto ifName = (CFStringRef)CFArrayGetValueAtIndex(interfaces, i);
    ifNames.insert(convertToStdString(ifName));
  }
  return ifNames;
}

InterfaceState
NetworkMonitor::Impl::getInterfaceState(const std::string& ifName)
{
  CFReleaser<CFStringRef> linkName = CFStringCreateWithCString(nullptr,
                                                               ("State:/Network/Interface/" + ifName + "/Link").c_str(),
                                                               kCFStringEncodingASCII);

  CFReleaser<CFDictionaryRef> dict = (CFDictionaryRef)SCDynamicStoreCopyValue(m_scStore.get(), linkName.get());
  if (dict.get() == nullptr) {
    return InterfaceState::UNKNOWN;
  }

  CFBooleanRef isActive = (CFBooleanRef)CFDictionaryGetValue(dict.get(), CFSTR("Active"));
  if (isActive == nullptr) {
    return InterfaceState::UNKNOWN;
  }

  return CFBooleanGetValue(isActive) ? InterfaceState::RUNNING : InterfaceState::DOWN;
}

void
NetworkMonitor::Impl::updateInterfaceInfo(NetworkInterface& netif)
{
  ifaddrs* ifa_list = nullptr;
  if (::getifaddrs(&ifa_list) < 0) {
    BOOST_THROW_EXCEPTION(Error(std::string("getifaddrs() failed: ") + strerror(errno)));
  }

  for (ifaddrs* ifa = ifa_list; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_name != netif.getName()) {
      continue;
    }

    netif.setFlags(ifa->ifa_flags);
    netif.setMtu(getInterfaceMtu(netif.getName()));

    if (ifa->ifa_addr == nullptr)
      continue;

    NetworkAddress address;

    switch (ifa->ifa_addr->sa_family) {
      case AF_INET: {
        address.m_family = AddressFamily::V4;

        const sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(ifa->ifa_addr);
        boost::asio::ip::address_v4::bytes_type bytes;
        std::copy_n(reinterpret_cast<const unsigned char*>(&sin->sin_addr), bytes.size(), bytes.begin());
        address.m_ip = boost::asio::ip::address_v4(bytes);

        const sockaddr_in* sinMask = reinterpret_cast<sockaddr_in*>(ifa->ifa_netmask);
        std::copy_n(reinterpret_cast<const unsigned char*>(&sinMask->sin_addr), bytes.size(), bytes.begin());
        uint8_t mask = 0;
        for (auto byte : bytes) {
          while (byte != 0) {
            ++mask;
            byte <<= 1;
          }
        }
        address.m_prefixLength = mask;
        break;
      }

      case AF_INET6: {
        address.m_family = AddressFamily::V6;

        const sockaddr_in6* sin6 = reinterpret_cast<sockaddr_in6*>(ifa->ifa_addr);
        boost::asio::ip::address_v6::bytes_type bytes;
        std::copy_n(reinterpret_cast<const unsigned char*>(&sin6->sin6_addr), bytes.size(), bytes.begin());
        address.m_ip = boost::asio::ip::address_v6(bytes);

        const sockaddr_in6* sinMask = reinterpret_cast<sockaddr_in6*>(ifa->ifa_netmask);
        std::copy_n(reinterpret_cast<const unsigned char*>(&sinMask->sin6_addr), bytes.size(), bytes.begin());
        uint8_t mask = 0;
        for (auto byte : bytes) {
          while (byte != 0) {
            ++mask;
            byte <<= 1;
          }
        }
        address.m_prefixLength = mask;
        break;
      }

      case AF_LINK: {
        const sockaddr_dl* sdl = reinterpret_cast<sockaddr_dl*>(ifa->ifa_addr);
        netif.setIndex(sdl->sdl_index);
        if (sdl->sdl_type == IFT_ETHER && sdl->sdl_alen == ethernet::ADDR_LEN) {
          netif.setType(InterfaceType::ETHERNET);
          netif.setEthernetAddress(ethernet::Address(reinterpret_cast<uint8_t*>(LLADDR(sdl))));
          NDN_LOG_TRACE(netif.getName() << ": set Ethernet address " << netif.getEthernetAddress());
        }
        else if (sdl->sdl_type == IFT_LOOP) {
          netif.setType(InterfaceType::LOOPBACK);
        }
        else {
          netif.setType(InterfaceType::UNKNOWN);
        }
        break;
      }

      default:
        continue;
    }

    if (netif.canBroadcast() && ifa->ifa_broadaddr != nullptr) {
      const sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(ifa->ifa_broadaddr);
      boost::asio::ip::address_v4::bytes_type bytes;
      std::copy_n(reinterpret_cast<const unsigned char*>(&sin->sin_addr), bytes.size(), bytes.begin());
      address.m_broadcast = boost::asio::ip::address_v4(bytes);
      NDN_LOG_TRACE(netif.getName() << ": set IPv4 broadcast address " << address.m_broadcast);
    }

    if (netif.canBroadcast()) {
      netif.setEthernetBroadcastAddress(ethernet::getBroadcastAddress());
    }

    netif.addNetworkAddress(address);
  }

  ::freeifaddrs(ifa_list);
}

size_t
NetworkMonitor::Impl::getInterfaceMtu(const std::string& ifName)
{
  ifreq ifr{};
  std::strncpy(ifr.ifr_name, ifName.c_str(), sizeof(ifr.ifr_name) - 1);

  if (::ioctl(m_nullUdpSocket.native_handle(), SIOCGIFMTU, &ifr) == 0) {
    return static_cast<size_t>(ifr.ifr_mtu);
  }

  NDN_LOG_WARN("Failed to get interface MTU: " << std::strerror(errno));
  return ethernet::MAX_DATA_LEN;
}

void
NetworkMonitor::Impl::onConfigChanged(SCDynamicStoreRef m_scStore, CFArrayRef changedKeys, void* context)
{
  static_cast<Impl*>(context)->onConfigChanged(changedKeys);
}

void
NetworkMonitor::Impl::onConfigChanged(CFArrayRef changedKeys)
{
  size_t count = CFArrayGetCount(changedKeys);
  for (size_t i = 0; i != count; ++i) {
    std::string keyName = convertToStdString((CFStringRef)CFArrayGetValueAtIndex(changedKeys, i));
    Name key(keyName);
    std::string ifName = key.at(-2).toUri();

    auto ifIt = m_interfaces.find(ifName);
    if (ifIt == m_interfaces.end()) {
      addNewInterface(ifName);
      return;
    }

    NetworkInterface& netif = *ifIt->second;

    auto removeInterface = [&] {
      NDN_LOG_DEBUG("removing interface " << ifName);
      shared_ptr<NetworkInterface> removedInterface = ifIt->second;
      m_interfaces.erase(ifIt);
      m_nm.onInterfaceRemoved(removedInterface);
    };

    if (key.at(-1).toUri() == "Link") {
      auto newState = getInterfaceState(ifName);

      if (newState == InterfaceState::UNKNOWN) {
        // check if it is really unknown or interface removed
        if (getInterfaceNames().count(ifName) == 0) {
          // newState = InterfaceState::DOWN;
          removeInterface();
          return;
        }
      }

      NDN_LOG_TRACE("Status of " << ifName << " changed from " << netif.getState() << " to " << newState);
      netif.setState(newState);
    }

    if (key.at(-1).toUri() == "IPv4" || key.at(-1).toUri() == "IPv6") {
      NetworkInterface updatedInterface;
      updatedInterface.setName(ifName);
      updateInterfaceInfo(updatedInterface);
      if (updatedInterface.getType() == InterfaceType::UNKNOWN) {
        // somehow, type of interface changed to unknown
        NDN_LOG_DEBUG("Removing " << ifName << " because it changed to unhandled interface type");
        removeInterface();
        return;
      }

      const auto& newAddrs = updatedInterface.getNetworkAddresses();
      const auto& oldAddrs = netif.getNetworkAddresses();

      std::set<NetworkAddress> added;
      std::set<NetworkAddress> removed;

      std::set_difference(newAddrs.begin(), newAddrs.end(),
                          oldAddrs.begin(), oldAddrs.end(), std::inserter(added, added.end()));

      std::set_difference(oldAddrs.begin(), oldAddrs.end(),
                          newAddrs.begin(), newAddrs.end(), std::inserter(removed, removed.end()));

      for (const auto& addr : removed) {
        netif.removeNetworkAddress(addr);
      }

      for (const auto& addr : added) {
        netif.addNetworkAddress(addr);
      }
    }
  }
}

} // namespace net
} // namespace ndn
