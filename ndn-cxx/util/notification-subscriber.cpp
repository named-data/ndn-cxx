/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2019 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#include "ndn-cxx/util/notification-subscriber.hpp"
#include "ndn-cxx/util/random.hpp"

#include <cmath>

namespace ndn {
namespace util {

NotificationSubscriberBase::NotificationSubscriberBase(Face& face, const Name& prefix,
                                                       time::milliseconds interestLifetime)
  : m_face(face)
  , m_prefix(prefix)
  , m_isRunning(false)
  , m_lastSequenceNum(std::numeric_limits<uint64_t>::max())
  , m_lastNackSequenceNum(std::numeric_limits<uint64_t>::max())
  , m_attempts(1)
  , m_scheduler(face.getIoService())
  , m_lastInterestId(nullptr)
  , m_interestLifetime(interestLifetime)
{
}

NotificationSubscriberBase::~NotificationSubscriberBase() = default;

void
NotificationSubscriberBase::start()
{
  if (m_isRunning) // already running
    return;
  m_isRunning = true;

  sendInitialInterest();
}

void
NotificationSubscriberBase::stop()
{
  if (!m_isRunning) // not running
    return;
  m_isRunning = false;

  if (m_lastInterestId != nullptr)
    m_face.removePendingInterest(m_lastInterestId);
  m_lastInterestId = nullptr;
}

void
NotificationSubscriberBase::sendInitialInterest()
{
  if (shouldStop())
    return;

  auto interest = make_shared<Interest>(m_prefix);
  interest->setCanBePrefix(true);
  interest->setMustBeFresh(true);
  interest->setInterestLifetime(m_interestLifetime);
  sendInterest(*interest);
}

void
NotificationSubscriberBase::sendNextInterest()
{
  if (shouldStop())
    return;

  Name nextName = m_prefix;
  nextName.appendSequenceNumber(m_lastSequenceNum + 1);

  auto interest = make_shared<Interest>(nextName);
  interest->setCanBePrefix(false);
  interest->setInterestLifetime(m_interestLifetime);
  sendInterest(*interest);
}

void
NotificationSubscriberBase::sendInterest(const Interest& interest)
{
  m_lastInterestId = m_face.expressInterest(interest,
                                            [this] (const auto&, const auto& d) { this->afterReceiveData(d); },
                                            [this] (const auto&, const auto& n) { this->afterReceiveNack(n); },
                                            [this] (const auto&) { this->afterTimeout(); });
}

bool
NotificationSubscriberBase::shouldStop()
{
  if (!m_isRunning)
    return true;

  if (!hasSubscriber() && onNack.isEmpty()) {
    stop();
    return true;
  }
  return false;
}

void
NotificationSubscriberBase::afterReceiveData(const Data& data)
{
  if (shouldStop())
    return;

  try {
    m_lastSequenceNum = data.getName().get(-1).toSequenceNumber();
  }
  catch (const tlv::Error&) {
    onDecodeError(data);
    sendInitialInterest();
    return;
  }

  if (!decodeAndDeliver(data)) {
    onDecodeError(data);
    sendInitialInterest();
    return;
  }

  sendNextInterest();
}

void
NotificationSubscriberBase::afterReceiveNack(const lp::Nack& nack)
{
  if (shouldStop())
    return;

  onNack(nack);

  time::milliseconds delay = exponentialBackoff(nack);
  m_nackEvent = m_scheduler.scheduleEvent(delay, [this] { sendInitialInterest(); });
}

void
NotificationSubscriberBase::afterTimeout()
{
  if (shouldStop())
    return;

  onTimeout();

  sendInitialInterest();
}

time::milliseconds
NotificationSubscriberBase::exponentialBackoff(lp::Nack nack)
{
  uint64_t nackSequenceNum;
  try {
    nackSequenceNum = nack.getInterest().getName().get(-1).toSequenceNumber();
  }
  catch (const tlv::Error&) {
    nackSequenceNum = 0;
  }

  if (m_lastNackSequenceNum == nackSequenceNum) {
    ++m_attempts;
  }
  else {
    m_attempts = 1;
  }

  m_lastNackSequenceNum = nackSequenceNum;

  return time::milliseconds(static_cast<time::milliseconds::rep>(std::pow(2, m_attempts) * 100 +
                                                                 random::generateWord32() % 100));
}

} // namespace util
} // namespace ndn
