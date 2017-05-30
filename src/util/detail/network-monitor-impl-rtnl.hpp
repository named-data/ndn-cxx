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
 * @author Davide Pesavento <davide.pesavento@lip6.fr>
 */

#ifndef NDN_UTIL_NETWORK_MONITOR_IMPL_RTNL_HPP
#define NDN_UTIL_NETWORK_MONITOR_IMPL_RTNL_HPP

#include "ndn-cxx-config.hpp"
#include "../network-monitor.hpp"

#ifndef NDN_CXX_HAVE_RTNETLINK
#error "This file should not be compiled ..."
#endif

#include <boost/asio/posix/stream_descriptor.hpp>

#include <array>
#include <map>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_addr.h>
#include <linux/if_link.h>

namespace ndn {
namespace util {

class NetworkMonitor::Impl
{
public:
  /** \brief initialize netlink socket and start enumerating interfaces
   */
  Impl(NetworkMonitor& nm, boost::asio::io_service& io);

  ~Impl();

  uint32_t
  getCapabilities() const
  {
    return NetworkMonitor::CAP_ENUM |
           NetworkMonitor::CAP_IF_ADD_REMOVE |
           NetworkMonitor::CAP_STATE_CHANGE |
           NetworkMonitor::CAP_MTU_CHANGE |
           NetworkMonitor::CAP_ADDR_ADD_REMOVE;
  }

  shared_ptr<NetworkInterface>
  getNetworkInterface(const std::string& ifname) const;

  std::vector<shared_ptr<NetworkInterface>>
  listNetworkInterfaces() const;

private:
  struct RtnlRequest
  {
    nlmsghdr nlh;
    ifinfomsg ifi;
    rtattr rta __attribute__((aligned(NLMSG_ALIGNTO))); // rtattr has to be aligned
    uint32_t rtext;                                     // space for IFLA_EXT_MASK
  };

  bool
  isEnumerating() const;

  void
  initSocket();

  void
  sendDumpRequest(uint16_t nlmsgType);

  void
  asyncRead();

  void
  handleRead(const boost::system::error_code& error, size_t nBytesReceived,
             const shared_ptr<boost::asio::posix::stream_descriptor>& socket);

  void
  parseNetlinkMessage(const nlmsghdr* nlh, size_t len);

  void
  parseLinkMessage(const nlmsghdr* nlh, const ifinfomsg* ifi);

  void
  parseAddressMessage(const nlmsghdr* nlh, const ifaddrmsg* ifa);

  void
  parseRouteMessage(const nlmsghdr* nlh, const rtmsg* rtm);

  static void
  updateInterfaceState(NetworkInterface& interface, uint8_t operState);

private:
  NetworkMonitor& m_nm;
  std::map<int /*ifindex*/, shared_ptr<NetworkInterface>> m_interfaces; ///< interface map
  std::array<uint8_t, 16384> m_buffer; ///< holds netlink messages received from the kernel
  shared_ptr<boost::asio::posix::stream_descriptor> m_socket; ///< the netlink socket
  uint32_t m_pid; ///< our port ID (unicast address for netlink sockets)
  uint32_t m_sequenceNo; ///< sequence number of the last netlink request sent to the kernel
  bool m_isEnumeratingLinks; ///< true if a dump of all links is in progress
  bool m_isEnumeratingAddresses; ///< true if a dump of all addresses is in progress
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_NETWORK_MONITOR_IMPL_RTNL_HPP
