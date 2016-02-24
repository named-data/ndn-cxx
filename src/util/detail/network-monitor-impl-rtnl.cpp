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

#include "ndn-cxx-config.hpp"

#ifdef NDN_CXX_HAVE_RTNETLINK

#include "network-monitor-impl-rtnl.hpp"

#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>

#include <cerrno>
#include <cstring>

namespace ndn {
namespace util {

NetworkMonitor::Impl::Impl(NetworkMonitor& nm, boost::asio::io_service& io)
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

void
NetworkMonitor::Impl::onReceiveRtNetlink(const boost::system::error_code& error, size_t nBytesReceived)
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

} // namespace util
} // namespace ndn

#endif // NDN_CXX_HAVE_RTNETLINK
