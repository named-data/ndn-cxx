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

#ifndef NDN_CXX_UTIL_SEGMENTER_HPP
#define NDN_CXX_UTIL_SEGMENTER_HPP

#include "ndn-cxx/security/key-chain.hpp"
#include "ndn-cxx/util/span.hpp"

namespace ndn {

/**
 * @brief Utility class to segment an object into multiple Data packets.
 */
class Segmenter
{
public:
  /**
   * @brief Constructor.
   * @param keyChain KeyChain instance used for signing the packets.
   * @param signingInfo How to sign the packets.
   */
  Segmenter(KeyChain& keyChain, const security::SigningInfo& signingInfo);

  /**
   * @brief Splits a blob of bytes into one or more Data packets (segments).
   * @param buffer Contiguous range of bytes to divide into segments.
   * @param dataName Name prefix to use for the Data packets. A segment number will be appended to it.
   * @param maxSegmentSize Maximum size of the `Content` element (payload) of each created Data packet.
   * @param freshnessPeriod The `FreshnessPeriod` of created Data packets.
   * @param contentType The `ContentType` of created Data packets.
   * @note A minimum of one Data packet is always returned, even if @p buffer is empty.
   */
  [[nodiscard]] std::vector<std::shared_ptr<Data>>
  segment(span<const uint8_t> buffer,
          const Name& dataName,
          size_t maxSegmentSize,
          time::milliseconds freshnessPeriod,
          uint32_t contentType = tlv::ContentType_Blob);

  /**
   * @brief Creates one or more Data packets (segments) with the bytes read from an input stream.
   * @param input The input stream. Reading stops when EOF is encountered.
   * @param dataName Name prefix to use for the Data packets. A segment number will be appended to it.
   * @param maxSegmentSize Maximum size of the `Content` element (payload) of each created Data packet.
   * @param freshnessPeriod The `FreshnessPeriod` of created Data packets.
   * @param contentType The `ContentType` of created Data packets.
   * @note A minimum of one Data packet is always returned, even if @p input is empty.
   */
  [[nodiscard]] std::vector<std::shared_ptr<Data>>
  segment(std::istream& input,
          const Name& dataName,
          size_t maxSegmentSize,
          time::milliseconds freshnessPeriod,
          uint32_t contentType = tlv::ContentType_Blob);

private:
  KeyChain& m_keyChain;
  security::SigningInfo m_signingInfo;
};

namespace util {
/// \deprecated Use ndn::Segmenter
using Segmenter = ::ndn::Segmenter;
} // namespace util

} // namespace ndn

#endif // NDN_CXX_UTIL_SEGMENTER_HPP
