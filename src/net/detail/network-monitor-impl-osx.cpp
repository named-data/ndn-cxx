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

#include "network-monitor-impl-osx.hpp"

#include "../network-address.hpp"
#include "../../name.hpp"
#include "../../util/logger.hpp"

#include <ifaddrs.h>      // for getifaddrs()
#include <net/if.h>       // for if_nametoindex()
#include <net/if_dl.h>    // for struct sockaddr_dl
#include <net/if_types.h> // for IFT_* constants
#include <netinet/in.h>   // for struct sockaddr_in{,6}

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ndn {
namespace net {

using util::CFReleaser;

NDN_LOG_INIT(ndn.NetworkMonitor);

class IfAddrs : noncopyable
{
public:
  IfAddrs()
  {
    if (::getifaddrs(&m_ifaList) < 0) {
      BOOST_THROW_EXCEPTION(NetworkMonitorImplOsx::Error(std::string("getifaddrs() failed: ") +
                                                         strerror(errno)));
    }
  }

  ~IfAddrs()
  {
    if (m_ifaList != nullptr) {
      ::freeifaddrs(m_ifaList);
    }
  }

  ifaddrs*
  get() const noexcept
  {
    return m_ifaList;
  }

private:
  ifaddrs* m_ifaList = nullptr;
};

NetworkMonitorImplOsx::NetworkMonitorImplOsx(boost::asio::io_service& io)
  : m_scheduler(io)
  , m_cfLoopEvent(m_scheduler)
  , m_context{0, this, nullptr, nullptr, nullptr}
  , m_scStore(SCDynamicStoreCreate(nullptr, CFSTR("net.named-data.ndn-cxx.NetworkMonitor"),
                                   &NetworkMonitorImplOsx::onConfigChanged, &m_context))
  , m_loopSource(SCDynamicStoreCreateRunLoopSource(nullptr, m_scStore.get(), 0))
  , m_ioctlSocket(io, boost::asio::ip::udp::v4())
{
  scheduleCfLoop();

  // Notifications from Darwin Notify Center:
  //
  // com.apple.system.config.network_change
  //
  CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                  static_cast<void*>(this),
                                  &NetworkMonitorImplOsx::afterNotificationCenterEvent,
                                  CFSTR("com.apple.system.config.network_change"),
                                  nullptr, // object to observe
                                  CFNotificationSuspensionBehaviorDeliverImmediately);

  CFRunLoopAddSource(CFRunLoopGetCurrent(), m_loopSource.get(), kCFRunLoopDefaultMode);

  // Notifications from SystemConfiguration:
  //
  // State:/Network/Interface/.*/Link
  // State:/Network/Interface/.*/IPv4
  // State:/Network/Interface/.*/IPv6
  // State:/Network/Interface/.*/AirPort (not used)
  //
  // https://developer.apple.com/library/content/documentation/Networking/Conceptual/SystemConfigFrameworks/SC_UnderstandSchema/SC_UnderstandSchema.html
  //
  auto patterns = CFArrayCreateMutable(nullptr, 0, &kCFTypeArrayCallBacks);
  CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/.*/Link"));
  CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/.*/IPv4"));
  CFArrayAppendValue(patterns, CFSTR("State:/Network/Interface/.*/IPv6"));

  if (!SCDynamicStoreSetNotificationKeys(m_scStore.get(), nullptr, patterns)) {
    BOOST_THROW_EXCEPTION(Error("SCDynamicStoreSetNotificationKeys failed"));
  }

  io.post([this] { enumerateInterfaces(); });
}

NetworkMonitorImplOsx::~NetworkMonitorImplOsx()
{
  CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m_loopSource.get(), kCFRunLoopDefaultMode);

  CFNotificationCenterRemoveEveryObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                          static_cast<void*>(this));
}

shared_ptr<const NetworkInterface>
NetworkMonitorImplOsx::getNetworkInterface(const std::string& ifname) const
{
  auto it = m_interfaces.find(ifname);
  return it == m_interfaces.end() ? nullptr : it->second;
}

std::vector<shared_ptr<const NetworkInterface>>
NetworkMonitorImplOsx::listNetworkInterfaces() const
{
  std::vector<shared_ptr<const NetworkInterface>> v;
  v.reserve(m_interfaces.size());

  for (const auto& e : m_interfaces) {
    v.push_back(e.second);
  }
  return v;
}

void
NetworkMonitorImplOsx::afterNotificationCenterEvent(CFNotificationCenterRef center,
                                                    void* observer,
                                                    CFStringRef name,
                                                    const void* object,
                                                    CFDictionaryRef userInfo)
{
  static_cast<NetworkMonitorImplOsx*>(observer)->emitSignal(onNetworkStateChanged);
}

void
NetworkMonitorImplOsx::scheduleCfLoop()
{
  // poll each second for new events
  m_cfLoopEvent = m_scheduler.scheduleEvent(1_s, [this] {
    // this should dispatch ready events and exit
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
    scheduleCfLoop();
  });
}

void
NetworkMonitorImplOsx::enumerateInterfaces()
{
  IfAddrs ifaList;
  for (const auto& ifName : getInterfaceNames()) {
    addNewInterface(ifName, ifaList);
  }
  this->emitSignal(onEnumerationCompleted);
}

static std::string
convertToStdString(CFStringRef cfStr)
{
  const char* cStr = CFStringGetCStringPtr(cfStr, kCFStringEncodingASCII);
  if (cStr != nullptr) {
    // fast path
    return cStr;
  }

  // reserve space for the string + null terminator
  std::string str(CFStringGetLength(cfStr) + 1, '\0');
  if (!CFStringGetCString(cfStr, &str.front(), str.size(), kCFStringEncodingASCII)) {
    BOOST_THROW_EXCEPTION(NetworkMonitorImplOsx::Error("CFString conversion failed"));
  }

  // drop the null terminator, std::string doesn't need it
  str.pop_back();
  return str;
}

std::set<std::string>
NetworkMonitorImplOsx::getInterfaceNames() const
{
  CFReleaser<CFDictionaryRef> dict =
    (CFDictionaryRef)SCDynamicStoreCopyValue(m_scStore.get(), CFSTR("State:/Network/Interface"));
  if (dict.get() == nullptr) {
    return {};
  }

  CFArrayRef interfaces = (CFArrayRef)CFDictionaryGetValue(dict.get(), CFSTR("Interfaces"));
  if (interfaces == nullptr) {
    return {};
  }

  std::set<std::string> ifNames;
  size_t count = CFArrayGetCount(interfaces);
  for (size_t i = 0; i != count; ++i) {
    auto ifName = (CFStringRef)CFArrayGetValueAtIndex(interfaces, i);
    ifNames.insert(convertToStdString(ifName));
  }
  return ifNames;
}

void
NetworkMonitorImplOsx::addNewInterface(const std::string& ifName, const IfAddrs& ifaList)
{
  shared_ptr<NetworkInterface> interface = makeNetworkInterface();
  interface->setName(ifName);
  interface->setState(getInterfaceState(*interface));
  updateInterfaceInfo(*interface, ifaList);

  if (interface->getType() == InterfaceType::UNKNOWN) {
    NDN_LOG_DEBUG("ignoring " << ifName << " due to unhandled interface type");
    return;
  }

  NDN_LOG_DEBUG("adding interface " << interface->getName());
  m_interfaces[interface->getName()] = interface;
  this->emitSignal(onInterfaceAdded, interface);
}

InterfaceState
NetworkMonitorImplOsx::getInterfaceState(const NetworkInterface& netif) const
{
  CFReleaser<CFStringRef> linkName =
    CFStringCreateWithCString(kCFAllocatorDefault,
                              ("State:/Network/Interface/" + netif.getName() + "/Link").data(),
                              kCFStringEncodingASCII);

  CFReleaser<CFDictionaryRef> dict =
    (CFDictionaryRef)SCDynamicStoreCopyValue(m_scStore.get(), linkName.get());
  if (dict.get() == nullptr) {
    return InterfaceState::UNKNOWN;
  }

  CFBooleanRef isActive = (CFBooleanRef)CFDictionaryGetValue(dict.get(), CFSTR("Active"));
  if (isActive == nullptr) {
    return InterfaceState::UNKNOWN;
  }

  return CFBooleanGetValue(isActive) ? InterfaceState::RUNNING : InterfaceState::DOWN;
}

size_t
NetworkMonitorImplOsx::getInterfaceMtu(const NetworkInterface& netif)
{
  ifreq ifr{};
  std::strncpy(ifr.ifr_name, netif.getName().data(), sizeof(ifr.ifr_name) - 1);

  if (::ioctl(m_ioctlSocket.native_handle(), SIOCGIFMTU, &ifr) == 0) {
    return static_cast<size_t>(ifr.ifr_mtu);
  }

  NDN_LOG_WARN("failed to get MTU of " << netif.getName() << ": " << std::strerror(errno));
  return ethernet::MAX_DATA_LEN;
}

template<typename AddressBytes>
static uint8_t
computePrefixLength(const AddressBytes& mask)
{
  uint8_t prefixLength = 0;
  for (auto byte : mask) {
    while (byte != 0) {
      ++prefixLength;
      byte <<= 1;
    }
  }
  return prefixLength;
}

void
NetworkMonitorImplOsx::updateInterfaceInfo(NetworkInterface& netif, const IfAddrs& ifaList)
{
  BOOST_ASSERT(!netif.getName().empty());

  netif.setMtu(getInterfaceMtu(netif));

  for (ifaddrs* ifa = ifaList.get(); ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_name != netif.getName())
      continue;

    netif.setFlags(ifa->ifa_flags);

    if (ifa->ifa_addr == nullptr)
      continue;

    namespace ip = boost::asio::ip;
    AddressFamily addrFamily = AddressFamily::UNSPECIFIED;
    ip::address ipAddr, broadcastAddr;
    uint8_t prefixLength = 0;

    switch (ifa->ifa_addr->sa_family) {
      case AF_INET: {
        addrFamily = AddressFamily::V4;

        const sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(ifa->ifa_addr);
        ip::address_v4::bytes_type bytes;
        std::copy_n(reinterpret_cast<const unsigned char*>(&sin->sin_addr), bytes.size(), bytes.begin());
        ipAddr = ip::address_v4(bytes);

        const sockaddr_in* sinMask = reinterpret_cast<sockaddr_in*>(ifa->ifa_netmask);
        std::copy_n(reinterpret_cast<const unsigned char*>(&sinMask->sin_addr), bytes.size(), bytes.begin());
        prefixLength = computePrefixLength(bytes);
        break;
      }

      case AF_INET6: {
        addrFamily = AddressFamily::V6;

        const sockaddr_in6* sin6 = reinterpret_cast<sockaddr_in6*>(ifa->ifa_addr);
        ip::address_v6::bytes_type bytes;
        std::copy_n(reinterpret_cast<const unsigned char*>(&sin6->sin6_addr), bytes.size(), bytes.begin());
        ip::address_v6 v6Addr(bytes);
        if (v6Addr.is_link_local())
          v6Addr.scope_id(if_nametoindex(netif.getName().data()));
        ipAddr = v6Addr;

        const sockaddr_in6* sinMask = reinterpret_cast<sockaddr_in6*>(ifa->ifa_netmask);
        std::copy_n(reinterpret_cast<const unsigned char*>(&sinMask->sin6_addr), bytes.size(), bytes.begin());
        prefixLength = computePrefixLength(bytes);
        break;
      }

      case AF_LINK: {
        const sockaddr_dl* sdl = reinterpret_cast<sockaddr_dl*>(ifa->ifa_addr);
        netif.setIndex(sdl->sdl_index);

        if (sdl->sdl_type == IFT_ETHER && sdl->sdl_alen == ethernet::ADDR_LEN) {
          netif.setType(InterfaceType::ETHERNET);
          netif.setEthernetAddress(ethernet::Address(reinterpret_cast<uint8_t*>(LLADDR(sdl))));
          NDN_LOG_TRACE(netif.getName() << " has Ethernet address " << netif.getEthernetAddress());
        }
        else if (sdl->sdl_type == IFT_LOOP) {
          netif.setType(InterfaceType::LOOPBACK);
        }
        else {
          netif.setType(InterfaceType::UNKNOWN);
        }
        break;
      }
    }

    if (netif.canBroadcast()) {
      netif.setEthernetBroadcastAddress(ethernet::getBroadcastAddress());

      if (addrFamily == AddressFamily::V4 && ifa->ifa_broadaddr != nullptr) {
        const sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(ifa->ifa_broadaddr);
        ip::address_v4::bytes_type bytes;
        std::copy_n(reinterpret_cast<const unsigned char*>(&sin->sin_addr), bytes.size(), bytes.begin());
        broadcastAddr = ip::address_v4(bytes);
      }
    }

    if (addrFamily == AddressFamily::UNSPECIFIED)
      continue;

    AddressScope scope = AddressScope::GLOBAL;
    if (ipAddr.is_loopback()) {
      scope = AddressScope::HOST;
    }
    else if ((ipAddr.is_v4() && (ipAddr.to_v4().to_ulong() & 0xFFFF0000) == 0xA9FE0000) ||
             (ipAddr.is_v6() && ipAddr.to_v6().is_link_local())) {
      scope = AddressScope::LINK;
    }

    netif.addNetworkAddress(NetworkAddress(addrFamily, ipAddr, broadcastAddr, prefixLength, scope, 0));
  }
}

void
NetworkMonitorImplOsx::onConfigChanged(SCDynamicStoreRef m_scStore, CFArrayRef changedKeys, void* context)
{
  static_cast<NetworkMonitorImplOsx*>(context)->onConfigChanged(changedKeys);
}

void
NetworkMonitorImplOsx::onConfigChanged(CFArrayRef changedKeys)
{
  IfAddrs ifaList;

  size_t count = CFArrayGetCount(changedKeys);
  for (size_t i = 0; i != count; ++i) {
    Name key(convertToStdString((CFStringRef)CFArrayGetValueAtIndex(changedKeys, i)));
    std::string ifName = key.at(-2).toUri();

    auto ifIt = m_interfaces.find(ifName);
    if (ifIt == m_interfaces.end()) {
      addNewInterface(ifName, ifaList);
      return;
    }

    auto removeInterface = [&] {
      NDN_LOG_DEBUG("removing interface " << ifName);
      shared_ptr<NetworkInterface> removedNetif = ifIt->second;
      m_interfaces.erase(ifIt);
      this->emitSignal(onInterfaceRemoved, removedNetif);
    };

    NetworkInterface& netif = *ifIt->second;
    std::string changedItem = key.at(-1).toUri();
    if (changedItem == "Link") {
      auto newState = getInterfaceState(netif);
      if (newState == InterfaceState::UNKNOWN) {
        // check if it is really unknown or interface removed
        if (getInterfaceNames().count(ifName) == 0) {
          removeInterface();
          return;
        }
      }
      NDN_LOG_TRACE(ifName << " status changed from " << netif.getState() << " to " << newState);
      netif.setState(newState);
    }
    else if (changedItem == "IPv4" || changedItem == "IPv6") {
      auto updatedNetif = makeNetworkInterface();
      updatedNetif->setName(ifName);
      updateInterfaceInfo(*updatedNetif, ifaList);
      if (updatedNetif->getType() == InterfaceType::UNKNOWN) {
        NDN_LOG_DEBUG(ifName << " type changed to unknown");
        removeInterface();
        return;
      }

      const auto& newAddrs = updatedNetif->getNetworkAddresses();
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
