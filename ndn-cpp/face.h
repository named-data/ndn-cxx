/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_CXX_H
#define NDN_CXX_H

#include "ndn-cpp/common.h"
#include "ndn-cpp/fields/name.h"
#include "ndn-cpp/interest.h"
#include "ndn-cpp/data.h"

#include "trie/trie-with-policy.h"
#include "trie/policies/counting-policy.h"

#include <list>

namespace ndn {

template<class Callback>
struct CallbackTable :
    public trie::trie_with_policy< Name,
                                   trie::ptr_payload_traits< std::list<Callback> >,
                                   trie::counting_policy_traits >
{
};


class Face
{
public:
  typedef boost::function<void (Ptr<Data> incomingData, Ptr<const Interest> satisfiedInterest)> SatisfiedInterestCallback;
  typedef boost::function<void (Ptr<Interest> incomingInterest, Ptr<const Name> registeredPrefix)> ExpectedInterestCallback;

  // some internal definitions
  typedef CallbackTable< SatisfiedInterestCallback > sent_interest_container;
  typedef CallbackTable< ExpectedInterestCallback >  registered_prefix_container;

  typedef sent_interest_container::iterator sent_interest;
  typedef registered_prefix_container::iterator registered_prefix;
  
  sent_interest
  sendInterest (Ptr<const Interest> interest, const SatisfiedInterestCallback &dataCallback);

  void
  clearInterest (sent_interest interest);
  
  registered_prefix
  setInterestFilter (Ptr<const Name> prefix, const ExpectedInterestCallback &interestCallback);

  void
  clearInterestFilter (const Name &prefix);

  void
  clearInterestFilter (registered_prefix filter);
  
private:
  sent_interest_container m_sentInterests;
  registered_prefix_container m_registeredPrefixes;
};

} // ndn

#endif // NDN_CXX_H
