/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_INTEREST_SIGNER_HPP
#define NDN_CXX_SECURITY_INTEREST_SIGNER_HPP

#include "ndn-cxx/security/key-chain.hpp"

namespace ndn {
namespace security {

/**
 * @brief Helper class to create signed Interests
 *
 * The signer generates signature elements for an Interest and signs it with the KeyChain.
 */
class InterestSigner
{
public:
  /**
   * @brief Flags to indicate which elements to include in Interest signatures created with
   *        makeSignedInterest.
   * @sa https://named-data.net/doc/NDN-packet-spec/0.3/signature.html#interest-signature
   */
  enum SigningFlags : uint32_t {
    WantNonce = 1 << 0,
    WantTime = 1 << 1,
    WantSeqNum = 1 << 2,
  };

public:
  explicit
  InterestSigner(KeyChain& keyChain);

  /**
   * @brief Signs an Interest (following Packet Specification v0.3 or newer)
   * @param interest Interest to sign
   * @param params SigningInfo that provides parameters on how to sign the Interest.
   * @param signingFlags Indicates which elements to include in the signature. At least one element
   *                     must be specified for inclusion.
   * @throw std::invalid_argument No signature elements were specified for inclusion.
   *
   */
  void
  makeSignedInterest(Interest& interest,
                     SigningInfo params = SigningInfo(),
                     uint32_t signingFlags = WantNonce | WantTime);

  /**
   * @brief Creates and signs a command Interest
   * @deprecated Use the new signed Interest format instead of command Interests. These can be
   *             created with makeSignedInterest.
   *
   * This generates a nonce and timestamp for the command Interest.
   */
  Interest
  makeCommandInterest(Name name, const SigningInfo& params = SigningInfo());

private:
  /**
   * @brief Get current timestamp, but ensure it is unique by increasing by 1 ms if already used
   */
  time::system_clock::TimePoint
  getFreshTimestamp();

private:
  KeyChain& m_keyChain;
  time::system_clock::TimePoint m_lastUsedTimestamp;
  uint64_t m_lastUsedSeqNum;
};

} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_INTEREST_SIGNER_HPP
