/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_DETAIL_PENDING_INTEREST_HPP
#define NDN_DETAIL_PENDING_INTEREST_HPP

#include "../data.hpp"
#include "../interest.hpp"
#include "../lp/nack.hpp"
#include "../util/scheduler-scoped-event-id.hpp"

namespace ndn {

/**
 * @brief Indicates where a pending Interest came from
 */
enum class PendingInterestOrigin
{
  APP, ///< Interest was received from this app via Face::expressInterest API
  FORWARDER ///< Interest was received from the forwarder via Transport
};

std::ostream&
operator<<(std::ostream& os, PendingInterestOrigin origin)
{
  switch (origin) {
    case PendingInterestOrigin::APP:
      return os << "app";
    case PendingInterestOrigin::FORWARDER:
      return os << "forwarder";
  }
  BOOST_ASSERT(false);
  return os;
}

/**
 * @brief Stores a pending Interest and associated callbacks
 */
class PendingInterest : noncopyable
{
public:
  /**
   * @brief Construct a pending Interest record for an Interest from Face::expressInterest
   *
   * The timeout is set based on the current time and InterestLifetime.
   * This class will invoke the timeout callback unless the record is deleted before timeout.
   *
   * @param interest the Interest
   * @param dataCallback invoked when matching Data packet is received
   * @param nackCallback invoked when Nack matching Interest is received
   * @param timeoutCallback invoked when Interest times out
   * @param scheduler Scheduler for scheduling the timeout event
   */
  PendingInterest(shared_ptr<const Interest> interest,
                  const DataCallback& dataCallback,
                  const NackCallback& nackCallback,
                  const TimeoutCallback& timeoutCallback,
                  Scheduler& scheduler)
    : m_interest(std::move(interest))
    , m_origin(PendingInterestOrigin::APP)
    , m_dataCallback(dataCallback)
    , m_nackCallback(nackCallback)
    , m_timeoutCallback(timeoutCallback)
    , m_timeoutEvent(scheduler)
    , m_nNotNacked(0)
  {
    scheduleTimeoutEvent(scheduler);
  }

  /**
   * @brief Construct a pending Interest record for an Interest from NFD
   *
   * @param interest the Interest
   * @param scheduler Scheduler for scheduling the timeout event
   */
  PendingInterest(shared_ptr<const Interest> interest, Scheduler& scheduler)
    : m_interest(std::move(interest))
    , m_origin(PendingInterestOrigin::FORWARDER)
    , m_timeoutEvent(scheduler)
    , m_nNotNacked(0)
  {
    scheduleTimeoutEvent(scheduler);
  }

  /**
   * @brief Get the Interest
   */
  shared_ptr<const Interest>
  getInterest() const
  {
    return m_interest;
  }

  PendingInterestOrigin
  getOrigin() const
  {
    return m_origin;
  }

  /**
   * @brief Record that the Interest has been forwarded to one destination
   *
   * A "destination" could be either a local InterestFilter or the forwarder.
   */
  void
  recordForwarding()
  {
    ++m_nNotNacked;
  }

  /**
   * @brief Record an incoming Nack against a forwarded Interest
   * @return least severe Nack if all destinations where the Interest was forwarded have Nacked;
   *          otherwise, nullopt
   */
  optional<lp::Nack>
  recordNack(const lp::Nack& nack)
  {
    --m_nNotNacked;
    BOOST_ASSERT(m_nNotNacked >= 0);

    if (!m_leastSevereNack || lp::isLessSevere(nack.getReason(), m_leastSevereNack->getReason())) {
      m_leastSevereNack = nack;
    }

    return m_nNotNacked > 0 ? nullopt : m_leastSevereNack;
  }

  /**
   * @brief Invoke the Data callback
   * @note This method does nothing if the Data callback is empty
   */
  void
  invokeDataCallback(const Data& data)
  {
    if (m_dataCallback != nullptr) {
      m_dataCallback(*m_interest, data);
    }
  }

  /**
   * @brief Invoke the Nack callback
   * @note This method does nothing if the Nack callback is empty
   */
  void
  invokeNackCallback(const lp::Nack& nack)
  {
    if (m_nackCallback != nullptr) {
      m_nackCallback(*m_interest, nack);
    }
  }

  /**
   * @brief Set cleanup function to be invoked when Interest times out
   */
  void
  setDeleter(const std::function<void()>& deleter)
  {
    m_deleter = deleter;
  }

private:
  void
  scheduleTimeoutEvent(Scheduler& scheduler)
  {
    m_timeoutEvent = scheduler.scheduleEvent(m_interest->getInterestLifetime(),
                                             [=] { this->invokeTimeoutCallback(); });
  }

  /**
   * @brief Invoke the timeout callback (if non-empty) and the deleter
   */
  void
  invokeTimeoutCallback()
  {
    if (m_timeoutCallback) {
      m_timeoutCallback(*m_interest);
    }

    BOOST_ASSERT(m_deleter);
    m_deleter();
  }

private:
  shared_ptr<const Interest> m_interest;
  PendingInterestOrigin m_origin;
  DataCallback m_dataCallback;
  NackCallback m_nackCallback;
  TimeoutCallback m_timeoutCallback;
  util::scheduler::ScopedEventId m_timeoutEvent;
  int m_nNotNacked; ///< number of Interest destinations that have not Nacked
  optional<lp::Nack> m_leastSevereNack;
  std::function<void()> m_deleter;
};

/**
 * @brief Opaque type to identify a PendingInterest
 */
class PendingInterestId;

/**
 * @brief Functor to match PendingInterestId
 */
class MatchPendingInterestId
{
public:
  explicit
  MatchPendingInterestId(const PendingInterestId* pendingInterestId)
    : m_id(pendingInterestId)
  {
  }

  bool
  operator()(const shared_ptr<const PendingInterest>& pendingInterest) const
  {
    return reinterpret_cast<const PendingInterestId*>(pendingInterest->getInterest().get()) == m_id;
  }

private:
  const PendingInterestId* m_id;
};

} // namespace ndn

#endif // NDN_DETAIL_PENDING_INTEREST_HPP
