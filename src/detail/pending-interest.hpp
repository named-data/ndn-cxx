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
  typedef function<void(const shared_ptr<const Interest>&, const shared_ptr<Data>&)> OnData;
  typedef function<void(const shared_ptr<const Interest>&)> OnTimeout;

  /**
   * Create a new PitEntry and set the timeoutTime_ based on the current time and the interest lifetime.
   * @param interest A shared_ptr for the interest.
   * @param onData A function object to call when a matching data packet is received.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   */
  PendingInterest(const shared_ptr<const Interest>& interest, const OnData& onData, 
                  const OnTimeout& onTimeout)
    : interest_(interest),
      onData_(onData), onTimeout_(onTimeout)
  {
    // Set up timeoutTime_.
    if (interest_->getInterestLifetime() >= 0)
      timeoutTimeMilliseconds_ = getNowMilliseconds() + interest_->getInterestLifetime();
    else
      // No timeout.
      /**
       * @todo Set more meaningful default timeout.  This timeout MUST exist.
       */
      timeoutTimeMilliseconds_ = getNowMilliseconds() + 4000;
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
   * @param nowMilliseconds The current time in milliseconds from getNowMilliseconds.
   * @return true if this interest timed out, otherwise false.
   */
  bool 
  isTimedOut(MillisecondsSince1970 nowMilliseconds)
  {
    return timeoutTimeMilliseconds_ >= 0.0 && nowMilliseconds >= timeoutTimeMilliseconds_;
  }

  /**
   * Call onTimeout_ (if defined).  This ignores exceptions from the onTimeout_.
   */
  void 
  callTimeout()
  {
    if (onTimeout_) {
      onTimeout_(interest_);
    }
  }
    
private:
  shared_ptr<const Interest> interest_;
  const OnData onData_;
  const OnTimeout onTimeout_;

  /** The time when the interest times out in milliseconds according to getNowMilliseconds, or -1 for no timeout. */
  MillisecondsSince1970 timeoutTimeMilliseconds_;
};


struct PendingInterestId;

/**
 * @brief Functor to match pending interests against PendingInterestId
 */
struct MatchPendingInterestId
{
  MatchPendingInterestId(const PendingInterestId *pendingInterestId)
    : id_(pendingInterestId)
  {
  }

  bool
  operator()(const shared_ptr<const PendingInterest> &pendingInterest) const
  {
    return (reinterpret_cast<const PendingInterestId *>(pendingInterest.get()) == id_);
  }
private:
  const PendingInterestId *id_;
};


} // namespace ndn

#endif // NDN_DETAIL_PENDING_INTEREST_HPP
