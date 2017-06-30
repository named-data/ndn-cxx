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

#ifndef NDN_TESTS_NET_NETWORK_CONFIGURATION_DETECTOR_HPP
#define NDN_TESTS_NET_NETWORK_CONFIGURATION_DETECTOR_HPP

#define SKIP_IF_IPV4_UNAVAILABLE() \
  do { \
    if (!::ndn::tests::NetworkConfigurationDetector::hasIpv4()) { \
      BOOST_WARN_MESSAGE(false, "skipping assertions that require IPv4 support"); \
      return; \
    } \
  } while (false)

#define SKIP_IF_IPV6_UNAVAILABLE() \
  do { \
    if (!::ndn::tests::NetworkConfigurationDetector::hasIpv6()) { \
      BOOST_WARN_MESSAGE(false, "skipping assertions that require IPv6 support"); \
      return; \
    } \
  } while (false)

#define SKIP_IF_IP_UNAVAILABLE() \
  do { \
    if (!::ndn::tests::NetworkConfigurationDetector::hasIpv4() && \
        !::ndn::tests::NetworkConfigurationDetector::hasIpv6()) { \
      BOOST_WARN_MESSAGE(false, "skipping assertions that require either IPv4 or IPv6 support"); \
      return; \
    } \
  } while (false)

namespace ndn {
namespace tests {

class NetworkConfigurationDetector
{
public:
  static bool
  hasIpv4();

  static bool
  hasIpv6();

private:
  static void
  detect();

private:
  static bool m_isInitialized;
  static bool m_hasIpv4;
  static bool m_hasIpv6;
};

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_NET_NETWORK_CONFIGURATION_DETECTOR_HPP
