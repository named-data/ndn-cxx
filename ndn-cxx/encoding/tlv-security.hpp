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

#ifndef NDN_CXX_ENCODING_TLV_SECURITY_HPP
#define NDN_CXX_ENCODING_TLV_SECURITY_HPP

#include <cstdint>

namespace ndn::tlv::security {

/**
 * @brief TLV-TYPE numbers for SafeBag and related elements.
 * @sa <a href="../specs/safe-bag.html">SafeBag Format</a>
 */
enum : uint32_t {
  SafeBag = 128,
  EncryptedKey = 129,
};

} // namespace ndn::tlv::security

#endif // NDN_CXX_ENCODING_TLV_SECURITY_HPP
