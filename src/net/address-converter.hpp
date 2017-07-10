/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California,
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

#ifndef NDN_NET_ADDRESS_CONVERTER_HPP
#define NDN_NET_ADDRESS_CONVERTER_HPP

#include "../common.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/system/error_code.hpp>

namespace ndn {
namespace ip {

/**
 * \brief Convert scope ID of IPv6 address into interface name
 *
 * \return interface name, or ndn::nullopt if \p scopeId cannot be converted
 */
optional<std::string>
scopeNameFromId(unsigned int scopeId);

/**
 * \brief parse and convert the input string into an IP address
 *
 * \param str the string to parse
 *
 * \return the converted IP address
 * \throw boost::system::system_error in case of failure
 */
boost::asio::ip::address
addressFromString(const std::string& str);

/**
 * \brief parse and convert the input string into an IP address
 *
 * \param str the string to parse
 * \param ec the error code of failure in conversion
 *
 * \return the converted IP address, or a default-constructed
 *         `boost::asio::ip::address` in case of failure
 */
boost::asio::ip::address
addressFromString(const std::string& str, boost::system::error_code& ec);

/**
 * \brief parse and convert the input string into an IPv6 address
 *
 * \param str the string to parse
 *
 * \return the converted IPv6 address
 * \throw boost::system::system_error in case of failure
 */
boost::asio::ip::address_v6
addressV6FromString(const std::string& str);

/**
 * \brief parse and convert the input string into an IPv6 address
 *
 * \param str the string to parse
 * \param ec the error code of failure in conversion
 *
 * \return the converted IPv6 address, or a default-constructed
 *         `boost::asio::ip::address_v6` in case of failure
 */
boost::asio::ip::address_v6
addressV6FromString(const std::string& str, boost::system::error_code& ec);

} // namespace ip
} // namespace ndn

#endif // NDN_NET_ADDRESS_CONVERTER_HPP
