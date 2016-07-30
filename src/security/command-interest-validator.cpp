/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "command-interest-validator.hpp"
#include "v1/identity-certificate.hpp"
#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {

std::ostream&
operator<<(std::ostream& os, CommandInterestValidator::ErrorCode error)
{
  switch (error) {
    case CommandInterestValidator::ErrorCode::NONE:
      return os << "OK";
    case CommandInterestValidator::ErrorCode::NAME_TOO_SHORT:
      return os << "command Interest name is too short";
    case CommandInterestValidator::ErrorCode::BAD_TIMESTAMP:
      return os << "cannot parse timestamp";
    case CommandInterestValidator::ErrorCode::BAD_SIG_INFO:
      return os << "cannot parse SignatureInfo";
    case CommandInterestValidator::ErrorCode::MISSING_KEY_LOCATOR:
      return os << "KeyLocator is missing";
    case CommandInterestValidator::ErrorCode::BAD_KEY_LOCATOR_TYPE:
      return os << "KeyLocator type is not Name";
    case CommandInterestValidator::ErrorCode::BAD_CERT_NAME:
      return os << "cannot parse certificate name";
    case CommandInterestValidator::ErrorCode::TIMESTAMP_OUT_OF_GRACE:
      return os << "timestamp is out of grace period";
    case CommandInterestValidator::ErrorCode::TIMESTAMP_REORDER:
      return os << "timestamp is less than or equal to last timestamp";
  }
  return os;
}

static void
invokeReject(const OnInterestValidationFailed& reject, const Interest& interest,
             CommandInterestValidator::ErrorCode error)
{
  reject(interest.shared_from_this(), boost::lexical_cast<std::string>(error));
}

CommandInterestValidator::CommandInterestValidator(unique_ptr<Validator> inner,
                                                   const Options& options)
  : m_inner(std::move(inner))
  , m_options(options)
  , m_index(m_container.get<0>())
  , m_queue(m_container.get<1>())
{
  if (m_inner == nullptr) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("inner validator is nullptr"));
  }

  m_options.gracePeriod = std::max(m_options.gracePeriod, time::nanoseconds::zero());
}

void
CommandInterestValidator::checkPolicy(const Interest& interest, int nSteps,
                                      const OnInterestValidated& accept,
                                      const OnInterestValidationFailed& reject,
                                      std::vector<shared_ptr<ValidationRequest>>& nextSteps)
{
  BOOST_ASSERT(nSteps == 0);
  this->cleanup();

  Name keyName;
  uint64_t timestamp;
  ErrorCode res = this->parseCommandInterest(interest, keyName, timestamp);
  if (res != ErrorCode::NONE) {
    return invokeReject(reject, interest, res);
  }

  time::system_clock::TimePoint receiveTime = time::system_clock::now();

  m_inner->validate(interest,
    [=] (const shared_ptr<const Interest>& interest) {
      ErrorCode res = this->checkTimestamp(keyName, timestamp, receiveTime);
      if (res != ErrorCode::NONE) {
        return invokeReject(reject, *interest, res);
      }
      accept(interest);
    }, reject);
}

void
CommandInterestValidator::cleanup()
{
  time::steady_clock::TimePoint expiring = time::steady_clock::now() - m_options.timestampTtl;

  while ((!m_queue.empty() && m_queue.front().lastRefreshed <= expiring) ||
         (m_options.maxTimestamps >= 0 &&
          m_queue.size() > static_cast<size_t>(m_options.maxTimestamps))) {
    m_queue.pop_front();
  }
}

CommandInterestValidator::ErrorCode
CommandInterestValidator::parseCommandInterest(const Interest& interest, Name& keyName,
                                               uint64_t& timestamp) const
{
  const Name& name = interest.getName();
  if (name.size() < signed_interest::MIN_LENGTH) {
    return ErrorCode::NAME_TOO_SHORT;
  }

  const name::Component& timestampComp = name[signed_interest::POS_TIMESTAMP];
  if (!timestampComp.isNumber()) {
    return ErrorCode::BAD_TIMESTAMP;
  }
  timestamp = timestampComp.toNumber();

  SignatureInfo sig;
  try {
    sig.wireDecode(name[signed_interest::POS_SIG_INFO].blockFromValue());
  }
  catch (const tlv::Error&) {
    return ErrorCode::BAD_SIG_INFO;
  }

  if (!sig.hasKeyLocator()) {
    return ErrorCode::MISSING_KEY_LOCATOR;
  }

  const KeyLocator& keyLocator = sig.getKeyLocator();
  if (keyLocator.getType() != KeyLocator::KeyLocator_Name) {
    return ErrorCode::BAD_KEY_LOCATOR_TYPE;
  }

  try {
    keyName = v1::IdentityCertificate::certificateNameToPublicKeyName(keyLocator.getName());
  }
  catch (const v1::IdentityCertificate::Error&) {
    return ErrorCode::BAD_CERT_NAME;
  }

  return ErrorCode::NONE;
}

CommandInterestValidator::ErrorCode
CommandInterestValidator::checkTimestamp(const Name& keyName, uint64_t timestamp,
                                         time::system_clock::TimePoint receiveTime)
{
  time::steady_clock::TimePoint now = time::steady_clock::now();

  // try to insert new record
  Queue::iterator i = m_queue.end();
  bool isNew = false;
  std::tie(i, isNew) = m_queue.push_back({keyName, timestamp, now});

  if (isNew) {
    // check grace period
    time::system_clock::TimePoint sigTime = time::fromUnixTimestamp(time::milliseconds(timestamp));
    if (time::abs(sigTime - receiveTime) > m_options.gracePeriod) {
      // out of grace period, delete new record
      m_queue.erase(i);
      return ErrorCode::TIMESTAMP_OUT_OF_GRACE;
    }
  }
  else {
    BOOST_ASSERT(i->keyName == keyName);

    // compare timestamp with last timestamp
    if (timestamp <= i->timestamp) {
      return ErrorCode::TIMESTAMP_REORDER;
    }

    // set lastRefreshed field, and move to queue tail
    m_queue.erase(i);
    isNew = m_queue.push_back({keyName, timestamp, now}).second;
    BOOST_ASSERT(isNew);
  }

  return ErrorCode::NONE;
}

void
CommandInterestValidator::checkPolicy(const Data& data, int nSteps,
                                      const OnDataValidated& accept,
                                      const OnDataValidationFailed& reject,
                                      std::vector<shared_ptr<ValidationRequest>>& nextSteps)
{
  BOOST_ASSERT(nSteps == 0);
  m_inner->validate(data, accept, reject);
}

} // namespace security
} // namespace ndn
