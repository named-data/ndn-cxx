/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2018 Regents of the University of California,
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

#include "notification-subscriber.hpp"
#include "random.hpp"

#include <cmath>

namespace ndn {
namespace util {

NotificationSubscriberBase::NotificationSubscriberBase(Face& face, const Name& prefix,
                                                       time::milliseconds interestLifetime)
  : m_face(face)
  , m_prefix(prefix)
  , m_isRunning(false)
  , m_lastSequenceNo(std::numeric_limits<uint64_t>::max())
  , m_lastNackSequenceNo(std::numeric_limits<uint64_t>::max())
  , m_attempts(1)
  , m_scheduler(face.getIoService())
  , m_nackEvent(m_scheduler)
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

  this->sendInitialInterest();
}

void
NotificationSubscriberBase::stop()
{
  if (!m_isRunning) // not running
    return;
  m_isRunning = false;

  if (m_lastInterestId != 0)
    m_face.removePendingInterest(m_lastInterestId);
  m_lastInterestId = 0;
}

void
NotificationSubscriberBase::sendInitialInterest()
{
  if (this->shouldStop())
    return;

  auto interest = make_shared<Interest>(m_prefix);
  interest->setMustBeFresh(true);
  interest->setChildSelector(1);
  interest->setInterestLifetime(getInterestLifetime());

  m_lastInterestId = m_face.expressInterest(*interest,
                       bind(&NotificationSubscriberBase::afterReceiveData, this, _2),
                       bind(&NotificationSubscriberBase::afterReceiveNack, this, _2),
                       bind(&NotificationSubscriberBase::afterTimeout, this));
}

void
NotificationSubscriberBase::sendNextInterest()
{
  if (this->shouldStop())
    return;

  BOOST_ASSERT(m_lastSequenceNo != std::numeric_limits<uint64_t>::max()); // overflow or missing initial reply

  Name nextName = m_prefix;
  nextName.appendSequenceNumber(m_lastSequenceNo + 1);

  auto interest = make_shared<Interest>(nextName);
  interest->setInterestLifetime(getInterestLifetime());

  m_lastInterestId = m_face.expressInterest(*interest,
                       bind(&NotificationSubscriberBase::afterReceiveData, this, _2),
                       bind(&NotificationSubscriberBase::afterReceiveNack, this, _2),
                       bind(&NotificationSubscriberBase::afterTimeout, this));
}

bool
NotificationSubscriberBase::shouldStop()
{
  if (!m_isRunning)
    return true;
  if (!this->hasSubscriber() && onNack.isEmpty()) {
    this->stop();
    return true;
  }
  return false;
}

void
NotificationSubscriberBase::afterReceiveData(const Data& data)
{
  if (this->shouldStop())
    return;

  try {
    m_lastSequenceNo = data.getName().get(-1).toSequenceNumber();
  }
  catch (const tlv::Error&) {
    this->onDecodeError(data);
    this->sendInitialInterest();
    return;
  }

  if (!this->decodeAndDeliver(data)) {
    this->onDecodeError(data);
    this->sendInitialInterest();
    return;
  }

  this->sendNextInterest();
}

void
NotificationSubscriberBase::afterReceiveNack(const lp::Nack& nack)
{
  if (this->shouldStop())
    return;

  this->onNack(nack);

  time::milliseconds delay = exponentialBackoff(nack);
  m_nackEvent = m_scheduler.scheduleEvent(delay, [this] {this->sendInitialInterest();});
}

void
NotificationSubscriberBase::afterTimeout()
{
  if (this->shouldStop())
    return;

  this->onTimeout();

  this->sendInitialInterest();
}

time::milliseconds
NotificationSubscriberBase::exponentialBackoff(lp::Nack nack)
{
  uint64_t nackSequenceNo;

  try {
    nackSequenceNo = nack.getInterest().getName().get(-1).toSequenceNumber();
  }
  catch (const tlv::Error&) {
    nackSequenceNo = 0;
  }

  if (m_lastNackSequenceNo == nackSequenceNo) {
    ++m_attempts;
  }
  else {
    m_attempts = 1;
  }

  m_lastNackSequenceNo = nackSequenceNo;

  return time::milliseconds(static_cast<time::milliseconds::rep>(std::pow(2, m_attempts) * 100 +
                                                                 random::generateWord32() % 100));
}

} // namespace util
} // namespace ndn
