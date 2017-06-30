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

#include "network-configuration-detector.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace ndn {
namespace tests {

bool NetworkConfigurationDetector::m_isInitialized = false;
bool NetworkConfigurationDetector::m_hasIpv4 = false;
bool NetworkConfigurationDetector::m_hasIpv6 = false;

bool
NetworkConfigurationDetector::hasIpv4()
{
  if (!m_isInitialized) {
    detect();
  }
  return m_hasIpv4;
}

bool
NetworkConfigurationDetector::hasIpv6()
{
  if (!m_isInitialized) {
    detect();
  }
  return m_hasIpv6;
}

void
NetworkConfigurationDetector::detect()
{
  typedef boost::asio::ip::basic_resolver<boost::asio::ip::udp> BoostResolver;

  boost::asio::io_service ioService;
  BoostResolver resolver(ioService);

  // The specified hostname must contain both A and AAAA records
  BoostResolver::query query("a.root-servers.net", "");

  boost::system::error_code errorCode;
  BoostResolver::iterator begin = resolver.resolve(query, errorCode);
  if (errorCode) {
    m_isInitialized = true;
    return;
  }
  BoostResolver::iterator end;

  for (const auto& i : boost::make_iterator_range(begin, end)) {
    if (i.endpoint().address().is_v4()) {
      m_hasIpv4 = true;
    }
    else if (i.endpoint().address().is_v6()) {
      m_hasIpv6 = true;
    }
  }

  m_isInitialized = true;
}

} // namespace tests
} // namespace ndn
