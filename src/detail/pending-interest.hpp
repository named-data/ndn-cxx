/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DETAIL_PENDING_INTEREST_HPP
#define NDN_DETAIL_PENDING_INTEREST_HPP

#include "../common.hpp"
#include "../interest.hpp"
#include "../data.hpp"
#include "../util/time.hpp"

namespace ndn {

class PendingInterest {
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
    : interest_(interest)
    , onData_(onData)
    , m_onTimeout(onTimeout)
  {
    if (interest_->getInterestLifetime() >= time::milliseconds::zero())
      m_timeout = time::steady_clock::now() + interest_->getInterestLifetime();
    else
      m_timeout = time::steady_clock::now() + DEFAULT_INTEREST_LIFETIME;
  }

  const shared_ptr<const Interest>&
  getInterest()
  {
    return interest_;
  }

  const OnData&
  getOnData()
  {
    return onData_;
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
      m_onTimeout(*interest_);
    }
  }

private:
  shared_ptr<const Interest> interest_;
  const OnData onData_;
  const OnTimeout m_onTimeout;
  time::steady_clock::TimePoint m_timeout;
};


struct PendingInterestId;

/**
 * @brief Functor to match pending interests against PendingInterestId
 */
struct MatchPendingInterestId
{
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
