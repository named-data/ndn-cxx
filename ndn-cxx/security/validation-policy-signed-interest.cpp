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

#include "ndn-cxx/security/validation-policy-signed-interest.hpp"

namespace ndn {
namespace security {
inline namespace v2 {

ValidationPolicySignedInterest::ValidationPolicySignedInterest(unique_ptr<ValidationPolicy> inner,
                                                               const Options& options)
  : m_options(options)
  , m_byKeyName(m_container.get<0>())
  , m_byLastRefreshed(m_container.get<1>())
{
  if (inner == nullptr) {
    NDN_THROW(std::invalid_argument("Inner policy is missing"));
  }
  setInnerPolicy(std::move(inner));

  m_options.timestampGracePeriod = std::max(m_options.timestampGracePeriod, 0_ns);
}

void
ValidationPolicySignedInterest::checkPolicy(const Data& data,
                                            const shared_ptr<ValidationState>& state,
                                            const ValidationContinuation& continueValidation)
{
  getInnerPolicy().checkPolicy(data, state, continueValidation);
}

void
ValidationPolicySignedInterest::checkPolicy(const Interest& interest,
                                            const shared_ptr<ValidationState>& state,
                                            const ValidationContinuation& continueValidation)
{
  auto fmt = state->getTag<SignedInterestFormatTag>();
  BOOST_ASSERT(fmt);

  if (!state->getOutcome()) {
    return;
  }

  if (*fmt == SignedInterestFormat::V03 && !checkIncomingInterest(state, interest)) {
    return;
  }

  getInnerPolicy().checkPolicy(interest, state, std::bind(continueValidation, _1, _2));
}

bool
ValidationPolicySignedInterest::checkIncomingInterest(const shared_ptr<ValidationState>& state,
                                                      const Interest& interest)
{
  // Extract information from Interest
  BOOST_ASSERT(interest.getSignatureInfo());
  Name keyName = getKeyLocatorName(interest, *state);
  auto timestamp = interest.getSignatureInfo()->getTime();
  auto seqNum = interest.getSignatureInfo()->getSeqNum();
  auto nonce = interest.getSignatureInfo()->getNonce();

  auto record = m_byKeyName.find(keyName);

  if (m_options.shouldValidateTimestamps) {
    if (!timestamp.has_value()) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Timestamp is required by policy but is not present"});
      return false;
    }

    auto now = time::system_clock::now();
    if (time::abs(now - *timestamp) > m_options.timestampGracePeriod) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Timestamp is outside the grace period for key " + keyName.toUri()});
      return false;
    }

    if (record != m_byKeyName.end() && record->timestamp.has_value() && timestamp <= record->timestamp) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Timestamp is reordered for key " + keyName.toUri()});
      return false;
    }
  }

  if (m_options.shouldValidateSeqNums) {
    if (!seqNum.has_value()) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Sequence number is required by policy but is not present"});
      return false;
    }

    if (record != m_byKeyName.end() && record->seqNum.has_value() && seqNum <= record->seqNum) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Sequence number is reordered for key " + keyName.toUri()});
      return false;
    }
  }

  if (m_options.shouldValidateNonces) {
    if (!nonce.has_value()) {
      state->fail({ValidationError::POLICY_ERROR, "Nonce is required by policy but is not present"});
      return false;
    }

    if (record != m_byKeyName.end() && record->observedNonces.get<NonceSet>().count(*nonce) > 0) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Nonce matches previously-seen nonce for key " + keyName.toUri()});
      return false;
    }
  }

  if (m_options.maxRecordCount != 0) {
    auto interestState = dynamic_pointer_cast<InterestValidationState>(state);
    BOOST_ASSERT(interestState != nullptr);
    interestState->afterSuccess.connect([=] (const Interest&) {
      insertRecord(keyName, timestamp, seqNum, nonce);
    });
  }
  return true;
}

void
ValidationPolicySignedInterest::insertRecord(const Name& keyName,
                                             optional<time::system_clock::TimePoint> timestamp,
                                             optional<uint64_t> seqNum,
                                             optional<SigNonce> nonce)
{
  // If key record exists, update last refreshed time. Otherwise, create new record.
  Container::nth_index<0>::type::iterator it;
  bool isOk;
  std::tie(it, isOk) = m_byKeyName.emplace(keyName, timestamp, seqNum);
  if (!isOk) {
    // There was already a record for this key, we just need to update it
    isOk = m_byKeyName.modify(it, [&] (LastInterestRecord& record) {
      record.lastRefreshed = time::steady_clock::now();
      if (timestamp.has_value()) {
        record.timestamp = timestamp;
      }
      if (seqNum.has_value()) {
        record.seqNum = seqNum;
      }
    });
    BOOST_VERIFY(isOk);
  }

  // If has nonce and max nonce list size > 0 (or unlimited), append to observed nonce list
  if (m_options.shouldValidateNonces && m_options.maxNonceRecordCount != 0 && nonce.has_value()) {
    isOk = m_byKeyName.modify(it, [this, &nonce] (LastInterestRecord& record) {
      auto& sigNonceList = record.observedNonces.get<NonceList>();
      sigNonceList.push_back(*nonce);
      // Ensure observed nonce list is at or below max nonce list size
      if (m_options.maxNonceRecordCount >= 0 &&
          sigNonceList.size() > static_cast<size_t>(m_options.maxNonceRecordCount)) {
        BOOST_ASSERT(sigNonceList.size() == static_cast<size_t>(m_options.maxNonceRecordCount) + 1);
        sigNonceList.pop_front();
      }
    });
    BOOST_VERIFY(isOk);
  }

  // Ensure record count is at or below max
  if (m_options.maxRecordCount >= 0 &&
      m_byLastRefreshed.size() > static_cast<size_t>(m_options.maxRecordCount)) {
    BOOST_ASSERT(m_byLastRefreshed.size() == static_cast<size_t>(m_options.maxRecordCount) + 1);
    m_byLastRefreshed.erase(m_byLastRefreshed.begin());
  }
}

} // inline namespace v2
} // namespace security
} // namespace ndn
