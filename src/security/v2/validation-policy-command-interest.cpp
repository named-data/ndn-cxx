/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "validation-policy-command-interest.hpp"

namespace ndn {
namespace security {
namespace v2 {

ValidationPolicyCommandInterest::ValidationPolicyCommandInterest(unique_ptr<ValidationPolicy> inner,
                                                                 const Options& options)
  : m_options(options)
  , m_index(m_container.get<0>())
  , m_queue(m_container.get<1>())
{
  if (inner == nullptr) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("inner policy is missing"));
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
  bool isOk = false;
  Name keyName;
  uint64_t timestamp = 0;
  std::tie(isOk, keyName, timestamp) = parseCommandInterest(interest, state);
  if (!isOk) {
    return;
  }

  if (!checkTimestamp(state, keyName, timestamp)) {
    return;
  }
  getInnerPolicy().checkPolicy(interest, state, std::bind(continueValidation, _1, _2));
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

std::tuple<bool, Name, uint64_t>
ValidationPolicyCommandInterest::parseCommandInterest(const Interest& interest,
                                                      const shared_ptr<ValidationState>& state) const
{
  const Name& name = interest.getName();
  if (name.size() < command_interest::MIN_SIZE) {
    state->fail({ValidationError::POLICY_ERROR, "Command interest name `" +
                 interest.getName().toUri() + "` is too short"});
    return std::make_tuple(false, Name(), 0);
  }

  const name::Component& timestampComp = name.at(command_interest::POS_TIMESTAMP);
  if (!timestampComp.isNumber()) {
    state->fail({ValidationError::POLICY_ERROR, "Command interest `" +
                 interest.getName().toUri() + "` doesn't include timestamp component"});
    return std::make_tuple(false, Name(), 0);
  }

  Name klName = getKeyLocatorName(interest, *state);
  if (!state->getOutcome()) { // already failed
    return std::make_tuple(false, Name(), 0);
  }

  return std::make_tuple(true, klName, timestampComp.toNumber());
}

bool
ValidationPolicyCommandInterest::checkTimestamp(const shared_ptr<ValidationState>& state,
                                                const Name& keyName, uint64_t timestamp)
{
  this->cleanup();

  auto now = time::system_clock::now();
  auto timestampPoint = time::fromUnixTimestamp(time::milliseconds(timestamp));
  if (timestampPoint < now - m_options.gracePeriod || timestampPoint > now + m_options.gracePeriod) {
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
ValidationPolicyCommandInterest::insertNewRecord(const Name& keyName, uint64_t timestamp)
{
  // try to insert new record
  auto now = time::steady_clock::now();
  auto i = m_queue.end();
  bool isNew = false;
  LastTimestampRecord newRecord{keyName, timestamp, now};
  std::tie(i, isNew) = m_queue.push_back(newRecord);

  if (!isNew) {
    BOOST_ASSERT(i->keyName == keyName);

    // set lastRefreshed field, and move to queue tail
    m_queue.erase(i);
    isNew = m_queue.push_back(newRecord).second;
    BOOST_VERIFY(isNew);
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
