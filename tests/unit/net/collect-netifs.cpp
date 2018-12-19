/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#include "tests/unit/net/collect-netifs.hpp"
#include "ndn-cxx/net/network-monitor.hpp"

#include <boost/asio/io_service.hpp>

namespace ndn {
namespace net {
namespace tests {

std::vector<shared_ptr<const NetworkInterface>>
collectNetworkInterfaces(bool allowCached)
{
  static optional<std::vector<shared_ptr<const NetworkInterface>>> cached;

  if (!allowCached || !cached) {
    boost::asio::io_service io;
    NetworkMonitor netmon(io);

    if (netmon.getCapabilities() & NetworkMonitor::CAP_ENUM) {
      netmon.onEnumerationCompleted.connect([&io] { io.stop(); });
      io.run();
      io.reset();
    }
    cached = netmon.listNetworkInterfaces();
  }

  return *cached;
}

} // namespace tests
} // namespace net
} // namespace ndn
