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

#ifndef NDN_NET_LINUX_IF_CONSTANTS_HPP
#define NDN_NET_LINUX_IF_CONSTANTS_HPP
#ifdef __linux__

#include <cstdint>

namespace ndn {
namespace net {
namespace linux_if {

// linux/if.h and net/if.h cannot be (directly or indirectly) included in the
// same translation unit because they contain duplicate declarations, therefore
// we have to resort to this workaround when we need to include both linux/if.h
// and any other headers that pull in net/if.h (e.g. boost/asio.hpp)

// net_device_flags missing from <net/if.h>
extern const uint32_t FLAG_LOWER_UP;
extern const uint32_t FLAG_DORMANT;
extern const uint32_t FLAG_ECHO;

// RFC 2863 operational status
extern const uint8_t OPER_STATE_UNKNOWN;
extern const uint8_t OPER_STATE_NOTPRESENT;
extern const uint8_t OPER_STATE_DOWN;
extern const uint8_t OPER_STATE_LOWERLAYERDOWN;
extern const uint8_t OPER_STATE_TESTING;
extern const uint8_t OPER_STATE_DORMANT;
extern const uint8_t OPER_STATE_UP;

} // namespace linux_if
} // namespace net
} // namespace ndn

#endif // __linux__
#endif // NDN_NET_LINUX_IF_CONSTANTS_HPP
