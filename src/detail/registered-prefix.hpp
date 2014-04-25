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

#ifndef NDN_DETAIL_REGISTERED_PREFIX_HPP
#define NDN_DETAIL_REGISTERED_PREFIX_HPP

#include "../common.hpp"
#include "../name.hpp"
#include "../interest.hpp"

namespace ndn {

class RegisteredPrefix
{
public:
  typedef function<void (const Name&, const Interest&)> OnInterest;

  /**
   * Create a new PrefixEntry.
   * @param prefix A shared_ptr for the prefix.
   * @param onInterest A function object to call when a matching data packet is received.
   */
  RegisteredPrefix(const Name& prefix, const OnInterest& onInterest)
    : m_prefix(new Name(prefix))
    , m_onInterest(onInterest)
  {
  }

  const Name&
  getPrefix() const
  {
    return* m_prefix;
  }

  const OnInterest&
  getOnInterest() const
  {
    return m_onInterest;
  }

private:
  shared_ptr<Name> m_prefix;
  const OnInterest m_onInterest;
};


struct RegisteredPrefixId;

/**
 * @brief Functor to match pending interests against PendingInterestId
 */
struct MatchRegisteredPrefixId
{
  MatchRegisteredPrefixId(const RegisteredPrefixId* registeredPrefixId)
    : m_id(registeredPrefixId)
  {
  }

  bool
  operator()(const shared_ptr<RegisteredPrefix>& registeredPrefix) const
  {
    return (reinterpret_cast<const RegisteredPrefixId*>(registeredPrefix.get()) == m_id);
  }
private:
  const RegisteredPrefixId* m_id;
};

} // namespace ndn

#endif // NDN_DETAIL_REGISTERED_PREFIX_HPP
