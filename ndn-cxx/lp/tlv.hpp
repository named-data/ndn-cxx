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

#ifndef NDN_CXX_LP_TLV_HPP
#define NDN_CXX_LP_TLV_HPP

#include <cstdint>

/**
 * @brief Contains constants related to the NDNLPv2 packet format.
 */
namespace ndn::lp::tlv {

/**
 * @brief TLV-TYPE numbers for NDNLPv2.
 * @sa https://redmine.named-data.net/projects/nfd/wiki/NDNLPv2#TLV-TYPE-Number-Assignments
 */
enum : uint32_t {
  LpPacket = 100,
  Fragment = 80,
  Sequence = 81,
  FragIndex = 82,
  FragCount = 83,
  PitToken = 98,
  Nack = 800,
  NackReason = 801,
  IncomingFaceId = 812,
  NextHopFaceId = 816,
  CachePolicy = 820,
  CachePolicyType = 821,
  CongestionMark = 832,
  Ack = 836,
  TxSequence = 840,
  NonDiscovery = 844,
  PrefixAnnouncement = 848,

  /**
   * \brief Lower bound of 1-octet header field.
   */
  HEADER1_MIN = 81,
  /**
   * \brief Upper bound of 1-octet header field.
   */
  HEADER1_MAX = 99,
  /**
   * \brief Lower bound of 3-octet header field.
   */
  HEADER3_MIN = 800,
  /**
   * \brief Upper bound of 3-octet header field.
   */
  HEADER3_MAX = 959,
};

} // namespace ndn::lp::tlv

#endif // NDN_CXX_LP_TLV_HPP
