/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_DETAIL_PENDING_INTEREST_HPP
#define NDN_DETAIL_PENDING_INTEREST_HPP

#include "../common.hpp"
#include "../interest.hpp"
#include "../data.hpp"
#include "../util/time.hpp"

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
   */
  PendingInterest(const shared_ptr<const Interest>& interest, const OnData& onData,
                  const OnTimeout& onTimeout)
    : m_interest(interest)
    , m_onData(onData)
    , m_onTimeout(onTimeout)
  {
    if (m_interest->getInterestLifetime() >= time::milliseconds::zero())
      m_timeout = time::steady_clock::now() + m_interest->getInterestLifetime();
    else
      m_timeout = time::steady_clock::now() + DEFAULT_INTEREST_LIFETIME;
  }

  const shared_ptr<const Interest>&
  getInterest()
  {
    return m_interest;
  }

  const OnData&
  getOnData()
  {
    return m_onData;
  }

  /**
   * Check if this interest is timed out.
   * @return true if this interest timed out, otherwise false.
   */
  bool
  isTimedOut(const time::steady_clock::TimePoint& now)
  {
    return now >= m_timeout;
  }

  /**
   * Call m_onTimeout (if defined).  This ignores exceptions from the m_onTimeout.
   */
  void
  callTimeout()
  {
    if (m_onTimeout) {
      m_onTimeout(*m_interest);
    }
  }

private:
  shared_ptr<const Interest> m_interest;
  const OnData m_onData;
  const OnTimeout m_onTimeout;
  time::steady_clock::TimePoint m_timeout;
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
    return (reinterpret_cast<const PendingInterestId*>(pendingInterest.get()) == m_id);
  }
private:
  const PendingInterestId* m_id;
};


} // namespace ndn

#endif // NDN_DETAIL_PENDING_INTEREST_HPP
