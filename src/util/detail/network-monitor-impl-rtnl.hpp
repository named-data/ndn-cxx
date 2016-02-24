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
 */

#ifndef NDN_UTIL_NETWORK_MONITOR_IMPL_RTNL_HPP
#define NDN_UTIL_NETWORK_MONITOR_IMPL_RTNL_HPP

#include "../network-monitor.hpp"

#include <boost/asio/posix/stream_descriptor.hpp>

namespace ndn {
namespace util {

const size_t NETLINK_BUFFER_SIZE = 4096;

class NetworkMonitor::Impl
{
public:
  Impl(NetworkMonitor& nm, boost::asio::io_service& io);

private:
  void
  onReceiveRtNetlink(const boost::system::error_code& error, size_t nBytesReceived);

private:
  NetworkMonitor& m_nm;

  uint8_t m_buffer[NETLINK_BUFFER_SIZE];
  boost::asio::posix::stream_descriptor m_socket;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NETWORK_MONITOR_IMPL_RTNL_HPP
