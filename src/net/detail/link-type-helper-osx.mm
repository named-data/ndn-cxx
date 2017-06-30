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
 */

#include "link-type-helper.hpp"

#ifndef NDN_CXX_HAVE_OSX_FRAMEWORKS
#error "This file should not be compiled ..."
#endif

#import <Foundation/Foundation.h>
#import <CoreWLAN/CoreWLAN.h>
#import <CoreWLAN/CWInterface.h>
#import <CoreWLAN/CWWiFiClient.h>

namespace ndn {
namespace net {
namespace detail {

ndn::nfd::LinkType
getLinkType(const std::string& ifName)
{
  @autoreleasepool {
    NSString* interfaceName = [NSString stringWithCString:ifName.c_str()
                                                 encoding:[NSString defaultCStringEncoding]];

    CWWiFiClient* wifiInterface = [CWWiFiClient sharedWiFiClient];
    if (wifiInterface == nullptr) {
      return nfd::LINK_TYPE_NONE;
    }

    CWInterface* airport = [wifiInterface interfaceWithName:interfaceName];
    if (airport == nullptr) {
      return nfd::LINK_TYPE_NONE;
    }

    if ([airport interfaceMode] == kCWInterfaceModeIBSS) {
      return nfd::LINK_TYPE_AD_HOC;
    }
    else {
      return nfd::LINK_TYPE_MULTI_ACCESS;
    }
  }
}

} // namespace detail
} // namespace net
} // namespace ndn
