/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_SECURITY_INTEREST_SIGNER_HPP
#define NDN_SECURITY_INTEREST_SIGNER_HPP

#include "ndn-cxx/security/key-chain.hpp"

namespace ndn {
namespace security {

/**
 * @brief Helper class to create signed Interests
 *
 * The signer generates timestamp and nonce elements for an Interest and signs it with the KeyChain.
 */
class InterestSigner
{
public:
  explicit
  InterestSigner(KeyChain& keyChain);

  /**
   * @brief Signs a signed Interest (following Packet Specification v0.3 or newer)
   *
   * This generates a nonce and timestamp for the signed Interest.
   */
  void
  makeSignedInterest(Interest& interest, SigningInfo params = SigningInfo());

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
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_INTEREST_SIGNER_HPP
