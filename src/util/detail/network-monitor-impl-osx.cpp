/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifdef NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H

#include "network-monitor-impl-osx.hpp"

namespace ndn {
namespace util {

NetworkMonitor::Impl::Impl(NetworkMonitor& nm, boost::asio::io_service& io)
  : m_nm(nm)
  , m_scheduler(io)
  , m_cfLoopEvent(m_scheduler)
{
  scheduleCfLoop();

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
  //
  CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                  static_cast<void*>(this),
                                  &Impl::afterNotificationCenterEvent,
                                  CFSTR("com.apple.system.config.network_change"),
                                  nullptr, // object to observe
                                  CFNotificationSuspensionBehaviorDeliverImmediately);
}

NetworkMonitor::Impl::~Impl()
{
  CFNotificationCenterRemoveEveryObserver(CFNotificationCenterGetDarwinNotifyCenter(),
                                          static_cast<void*>(this));
}

void
NetworkMonitor::Impl::afterNotificationCenterEvent(CFNotificationCenterRef center,
                                                   void *observer,
                                                   CFStringRef name,
                                                   const void *object,
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

} // namespace util
} // namespace ndn

#endif // NDN_CXX_HAVE_COREFOUNDATION_COREFOUNDATION_H
