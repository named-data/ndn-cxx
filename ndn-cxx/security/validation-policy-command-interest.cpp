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

#include "ndn-cxx/security/validation-policy-command-interest.hpp"

namespace ndn::security {

ValidationPolicyCommandInterest::ValidationPolicyCommandInterest(unique_ptr<ValidationPolicy> inner,
                                                                 const Options& options)
  : m_options(options)
  , m_index(m_container.get<0>())
  , m_queue(m_container.get<1>())
{
  if (inner == nullptr) {
    NDN_THROW(std::invalid_argument("inner policy is missing"));
  }
  setInnerPolicy(std::move(inner));

  m_options.gracePeriod = std::max(m_options.gracePeriod, 0_ns);
}

void
ValidationPolicyCommandInterest::checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  getInnerPolicy().checkPolicy(data, state, continueValidation);
}

void
ValidationPolicyCommandInterest::checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  auto [isOk, keyName, timestamp] = parseCommandInterest(interest, state);
  if (!isOk) {
    return;
  }

  if (!checkTimestamp(state, keyName, timestamp)) {
    return;
  }

  getInnerPolicy().checkPolicy(interest, state, continueValidation);
}

std::tuple<bool, Name, time::system_clock::time_point>
ValidationPolicyCommandInterest::parseCommandInterest(const Interest& interest,
                                                      const shared_ptr<ValidationState>& state)
{
  auto sigInfo = getSignatureInfo(interest, *state);
  if (!state->getOutcome()) { // already failed
    return {false, {}, {}};
  }

  time::system_clock::time_point timestamp;

  auto fmt = state->getTag<SignedInterestFormatTag>();
  BOOST_ASSERT(fmt);
  if (*fmt == SignedInterestFormat::V03) {
    // SignatureTime is a hard requirement of this policy
    // New apps should use the more flexible ValidationPolicySignedInterest (v0.3 only)
    auto optionalTimestamp = sigInfo.getTime();
    if (!optionalTimestamp) {
      state->fail({ValidationError::POLICY_ERROR, "Signed Interest `" +
                   interest.getName().toUri() + "` lacks required SignatureTime element"});
      return {false, {}, {}};
    }

    timestamp = *optionalTimestamp;
  }
  else {
    const Name& name = interest.getName();
    if (name.size() < command_interest::MIN_SIZE) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Command Interest name too short `" + interest.getName().toUri() + "`"});
      return {false, {}, {}};
    }

    const auto& timestampComp = name.at(command_interest::POS_TIMESTAMP);
    if (!timestampComp.isNumber()) {
      state->fail({ValidationError::POLICY_ERROR, "Command Interest `" +
                   interest.getName().toUri() + "` lacks required timestamp component"});
      return {false, {}, {}};
    }

    timestamp = time::fromUnixTimestamp(time::milliseconds(timestampComp.toNumber()));
  }

  Name klName = getKeyLocatorName(sigInfo, *state);
  if (!state->getOutcome()) { // already failed
    return {false, {}, {}};
  }

  return {true, klName, timestamp};
}

void
ValidationPolicyCommandInterest::cleanup()
{
  auto expiring = time::steady_clock::now() - m_options.recordLifetime;

  while ((!m_queue.empty() && m_queue.front().lastRefreshed <= expiring) ||
         (m_options.maxRecords >= 0 &&
          m_queue.size() > static_cast<size_t>(m_options.maxRecords))) {
    m_queue.pop_front();
  }
}

bool
ValidationPolicyCommandInterest::checkTimestamp(const shared_ptr<ValidationState>& state,
                                                const Name& keyName, time::system_clock::time_point timestamp)
{
  this->cleanup();

  auto now = time::system_clock::now();

  if (timestamp < now - m_options.gracePeriod || timestamp > now + m_options.gracePeriod) {
    state->fail({ValidationError::POLICY_ERROR,
                 "Timestamp is outside the grace period for key " + keyName.toUri()});
    return false;
  }

  auto it = m_index.find(keyName);
  if (it != m_index.end()) {
    if (timestamp <= it->timestamp) {
      state->fail({ValidationError::POLICY_ERROR,
                   "Timestamp is reordered for key " + keyName.toUri()});
      return false;
    }
  }

  auto interestState = dynamic_pointer_cast<InterestValidationState>(state);
  BOOST_ASSERT(interestState != nullptr);
  interestState->afterSuccess.connect([=] (const Interest&) { insertNewRecord(keyName, timestamp); });
  return true;
}

void
ValidationPolicyCommandInterest::insertNewRecord(const Name& keyName, time::system_clock::time_point timestamp)
{
  // try to insert new record
  LastTimestampRecord newRecord{keyName, timestamp, time::steady_clock::now()};
  auto [i, isNew] = m_queue.push_back(newRecord);

  if (!isNew) {
    BOOST_ASSERT(i->keyName == keyName);

    // set lastRefreshed field, and move to queue tail
    m_queue.erase(i);
    isNew = m_queue.push_back(newRecord).second;
    BOOST_VERIFY(isNew);
  }
}

} // namespace ndn::security
