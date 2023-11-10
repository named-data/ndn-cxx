/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "tests/unit/net/network-configuration-detector.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/udp.hpp>

namespace ndn::tests {

bool
NetworkConfigurationDetector::hasIpv4()
{
  if (!s_isInitialized) {
    detect();
  }
  return s_hasIpv4;
}

bool
NetworkConfigurationDetector::hasIpv6()
{
  if (!s_isInitialized) {
    detect();
  }
  return s_hasIpv6;
}

void
NetworkConfigurationDetector::detect()
{
  boost::asio::io_context io;
  boost::asio::ip::udp::resolver resolver(io);

  boost::system::error_code ec;
  // The specified hostname must have both A and AAAA records
  auto results = resolver.resolve("a.root-servers.net", "", ec);

  if (!ec) {
    for (const auto& i : results) {
      if (i.endpoint().address().is_v4()) {
        s_hasIpv4 = true;
      }
      else if (i.endpoint().address().is_v6()) {
        s_hasIpv6 = true;
      }
    }
  }
  s_isInitialized = true;
}

} // namespace ndn::tests
