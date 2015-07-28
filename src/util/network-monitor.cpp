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

#include "network-monitor.hpp"
#include "scheduler.hpp"
#include "scheduler-scoped-event-id.hpp"

#if defined(NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H)

#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

namespace ndn {
namespace util {

class NetworkMonitor::Impl
{
public:
  Impl(boost::asio::io_service& io)
    : scheduler(io)
    , cfLoopEvent(scheduler)
  {
  }

  void
  scheduleCfLoop()
  {
    // poll each second for new events
    cfLoopEvent = scheduler.scheduleEvent(time::seconds(1), bind(&Impl::pollCfLoop, this));
  }

  static void
  afterNotificationCenterEvent(CFNotificationCenterRef center, void *observer, CFStringRef name,
                               const void *object, CFDictionaryRef userInfo)
  {
    static_cast<NetworkMonitor*>(observer)->onNetworkStateChanged();
  }

private:

  void
  pollCfLoop()
  {
    // this should dispatch ready events and exit
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
    scheduleCfLoop();
  }

private:
  Scheduler scheduler;
  scheduler::ScopedEventId cfLoopEvent;
};

NetworkMonitor::NetworkMonitor(boost::asio::io_service& io)
  : m_impl(new Impl(io))
{
  m_impl->scheduleCfLoop();

  // Potentially useful System Configuration regex patterns:
  //
  // State:/Network/Interface/.*/Link
  // State:/Network/Interface/.*/IPv4
  // State:/Network/Interface/.*/IPv6
  //
  // State:/Network/Global/DNS
  // State:/Network/Global/IPv4
  //
  // Potentially useful notifications from Darwin Notify Center:
  //
  // com.apple.system.config.network_change

  // network change observations
  CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                  static_cast<void*>(this),
                                  &NetworkMonitor::Impl::afterNotificationCenterEvent,
                                  CFSTR("com.apple.system.config.network_change"),
                                  nullptr, // object to observe
                                  CFNotificationSuspensionBehaviorDeliverImmediately);
}

NetworkMonitor::~NetworkMonitor()
{
  CFNotificationCenterRemoveEveryObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                          static_cast<void*>(this));
}

} // namespace util
} // namespace ndn

// done with defined(NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H)
#elif defined(NDN_CXX_HAVE_RTNETLINK)

#include <boost/asio.hpp>

#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>

#include <cerrno>
#include <cstring>

namespace ndn {
namespace util {

const size_t NETLINK_BUFFER_SIZE = 4096;

class NetworkMonitor::Impl
{
public:
  Impl(NetworkMonitor& nm, boost::asio::io_service& io)
    : m_nm(nm)
    , m_socket(io)
  {
    int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0)
      BOOST_THROW_EXCEPTION(Error(std::string("Cannot create netlink socket (") +
                                  std::strerror(errno) + ")"));

    sockaddr_nl addr{};
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK |
      RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE |
      RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;

    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
      BOOST_THROW_EXCEPTION(Error(std::string("Cannot bind on netlink socket (") +
                                  std::strerror(errno) + ")"));
    }

    m_socket.assign(fd);

    m_socket.async_read_some(boost::asio::buffer(m_buffer, NETLINK_BUFFER_SIZE),
                             bind(&Impl::onReceiveRtNetlink, this, _1, _2));
  }

private:
  void
  onReceiveRtNetlink(const boost::system::error_code& error, size_t nBytesReceived)
  {
    if (error) {
      return;
    }

    const nlmsghdr* nlh = reinterpret_cast<const nlmsghdr*>(m_buffer);
    while ((NLMSG_OK(nlh, nBytesReceived)) && (nlh->nlmsg_type != NLMSG_DONE)) {
      if (nlh->nlmsg_type == RTM_NEWADDR || nlh->nlmsg_type == RTM_DELADDR ||
          nlh->nlmsg_type == RTM_NEWLINK || nlh->nlmsg_type == RTM_DELLINK ||
          nlh->nlmsg_type == RTM_NEWROUTE || nlh->nlmsg_type == RTM_DELROUTE) {
        m_nm.onNetworkStateChanged();
        break;
      }
      nlh = NLMSG_NEXT(nlh, nBytesReceived);
    }

    m_socket.async_read_some(boost::asio::buffer(m_buffer, NETLINK_BUFFER_SIZE),
                             bind(&Impl::onReceiveRtNetlink, this, _1, _2));
  }

private:
  NetworkMonitor& m_nm;
  uint8_t m_buffer[NETLINK_BUFFER_SIZE];

  boost::asio::posix::stream_descriptor m_socket;
};



NetworkMonitor::NetworkMonitor(boost::asio::io_service& io)
  : m_impl(new Impl(*this, io))
{
}

NetworkMonitor::~NetworkMonitor()
{
}

} // namespace util
} // namespace ndn

// done with defined(NDN_CXX_HAVE_RTNETLINK)
#else // do not support network monitoring operations

namespace ndn {
namespace util {

class NetworkMonitor::Impl
{
};

NetworkMonitor::NetworkMonitor(boost::asio::io_service&)
{
  BOOST_THROW_EXCEPTION(Error("Network monitoring is not supported on this platform"));
}

NetworkMonitor::~NetworkMonitor()
{
}

} // namespace util
} // namespace ndn

#endif // do not support network monitoring operations
