/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/detail/cf-string-osx.hpp"

namespace ndn {
namespace detail {
namespace cfstring {

CFReleaser<CFStringRef>
fromBuffer(const uint8_t* buf, size_t buflen)
{
  CFStringRef cfStr = CFStringCreateWithBytes(kCFAllocatorDefault, buf, buflen, kCFStringEncodingUTF8, false);
  if (cfStr == nullptr) {
    NDN_THROW(std::runtime_error("Failed to create CFString from buffer"));
  }
  return cfStr;
}

CFReleaser<CFStringRef>
fromStdString(const std::string& str)
{
  CFStringRef cfStr = CFStringCreateWithCString(kCFAllocatorDefault, str.data(), kCFStringEncodingUTF8);
  if (cfStr == nullptr) {
    NDN_THROW(std::runtime_error("Failed to create CFString from std::string"));
  }
  return cfStr;
}

std::string
toStdString(CFStringRef cfStr)
{
  const char* cStr = CFStringGetCStringPtr(cfStr, kCFStringEncodingUTF8);
  if (cStr != nullptr) {
    // fast path
    return cStr;
  }

  // reserve space for the string + null terminator
  std::string str(CFStringGetLength(cfStr) + 1, '\0');
  // copy the CFString into the std::string buffer
  if (!CFStringGetCString(cfStr, &str.front(), str.size(), kCFStringEncodingUTF8)) {
    NDN_THROW(std::runtime_error("CFString to std::string conversion failed"));
  }
  // drop the null terminator, std::string doesn't need it
  str.pop_back();

  return str;
}

} // namespace cfstring
} // namespace detail
} // namespace ndn
