/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "../common.hpp"
#include "../interest.hpp"
#include "../data.hpp"
#include "../util/time.hpp"
#include "../util/scheduler.hpp"
#include "../util/scheduler-scoped-event-id.hpp"

namespace ndn {

class PendingInterest : noncopyable
{
public:
  typedef function<void(const Interest&, Data&)> OnData;
  typedef function<void(const Interest&)> OnTimeout;

  /**
   * @brief Create a new PitEntry and set the timeout based on the current time and
   *        the interest lifetime.
   *
   * @param interest A shared_ptr for the interest
   * @param onData A function object to call when a matching data packet is received.
   * @param onTimeout A function object to call if the interest times out.
   *                  If onTimeout is an empty OnTimeout(), this does not use it.
   * @param scheduler Scheduler instance to use to schedule a timeout event.  The scheduled
   *                  event will be automatically cancelled when pending interest is destroyed.
   */
  PendingInterest(shared_ptr<const Interest> interest, const OnData& onData,
                  const OnTimeout& onTimeout, Scheduler& scheduler)
    : m_interest(interest)
    , m_onData(onData)
    , m_onTimeout(onTimeout)
    , m_timeoutEvent(scheduler)
  {
    m_timeoutEvent =
      scheduler.scheduleEvent(m_interest->getInterestLifetime() > time::milliseconds::zero() ?
                              m_interest->getInterestLifetime() :
                              DEFAULT_INTEREST_LIFETIME,
                              bind(&PendingInterest::invokeTimeoutCallback, this));
  }

  /**
   * @return the Interest
   */
  const Interest&
  getInterest() const
  {
    return *m_interest;
  }

  /**
   * @brief invokes the DataCallback
   * @note If the DataCallback is an empty function, this method does nothing.
   */
  void
  invokeDataCallback(Data& data)
  {
    m_onData(*m_interest, data);
  }

  /**
   * @brief Set cleanup function to be called after interest times out
   */
  void
  setDeleter(const std::function<void()>& deleter)
  {
    m_deleter = deleter;
  }

private:
  /**
   * @brief invokes the TimeoutCallback
   * @note If the TimeoutCallback is an empty function, this method does nothing.
   */
  void
  invokeTimeoutCallback()
  {
    if (m_onTimeout) {
      m_onTimeout(*m_interest);
    }

    BOOST_ASSERT(m_deleter);
    m_deleter();
  }

private:
  shared_ptr<const Interest> m_interest;
  const OnData m_onData;
  const OnTimeout m_onTimeout;
  util::scheduler::ScopedEventId m_timeoutEvent;
  std::function<void()> m_deleter;
};


class PendingInterestId;

/**
 * @brief Functor to match pending interests against PendingInterestId
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
    return (reinterpret_cast<const PendingInterestId*>(
              &pendingInterest->getInterest()) == m_id);
  }
private:
  const PendingInterestId* m_id;
};


} // namespace ndn

#endif // NDN_DETAIL_PENDING_INTEREST_HPP
