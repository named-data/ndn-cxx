/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/interest-signer.hpp"
#include "ndn-cxx/util/random.hpp"

namespace ndn {
namespace security {

InterestSigner::InterestSigner(KeyChain& keyChain)
  : m_keyChain(keyChain)
  , m_lastUsedSeqNum(-1) // Will wrap around to 0 on next Interest
{
}

void
InterestSigner::makeSignedInterest(Interest& interest, SigningInfo params, uint32_t signingFlags)
{
  SignatureInfo info = params.getSignatureInfo();

  if ((signingFlags & (WantNonce | WantTime | WantSeqNum)) == 0) {
    NDN_THROW(std::invalid_argument("No signature elements specified"));
  }

  if (signingFlags & WantNonce) {
    std::vector<uint8_t> nonce(8);
    random::generateSecureBytes(nonce);
    info.setNonce(nonce);
  }

  if (signingFlags & WantTime) {
    info.setTime(getFreshTimestamp());
  }

  if (signingFlags & WantSeqNum) {
    info.setSeqNum(++m_lastUsedSeqNum);
  }

  params.setSignatureInfo(info);
  params.setSignedInterestFormat(SignedInterestFormat::V03);
  m_keyChain.sign(interest, params);
}

Interest
InterestSigner::makeCommandInterest(Name name, const SigningInfo& params)
{
  Interest interest;
  time::milliseconds timestamp = time::toUnixTimestamp(getFreshTimestamp());
  name
    .append(name::Component::fromNumber(timestamp.count()))
    .append(name::Component::fromNumber(random::generateWord64())) // nonce
    ;
  interest.setName(name);
  m_keyChain.sign(interest, params);
  return interest;
}

time::system_clock::TimePoint
InterestSigner::getFreshTimestamp()
{
  auto timestamp = time::system_clock::now();
  if (time::duration_cast<time::milliseconds>(timestamp - m_lastUsedTimestamp) > 0_ms) {
    m_lastUsedTimestamp = timestamp;
  }
  else {
    m_lastUsedTimestamp = m_lastUsedTimestamp + 1_ms;
    timestamp = m_lastUsedTimestamp;
  }
  return timestamp;
}

} // namespace security
} // namespace ndn
