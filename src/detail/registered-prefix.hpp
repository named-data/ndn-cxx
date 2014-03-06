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

class InterestFilterRecord
{
public:
  typedef function<void (const InterestFilter&, const Interest&)> OnInterest;

  InterestFilterRecord(const InterestFilter& filter, const OnInterest& onInterest)
    : m_filter(filter)
    , m_onInterest(onInterest)
  {
  }

  /**
   * @brief Check if Interest name matches the filter
   * @param name Interest Name
   */
  bool
  doesMatch(const Name& name) const
  {
    return m_filter.doesMatch(name);
  }

  void
  operator()(const Interest& interest) const
  {
    m_onInterest(m_filter, interest);
  }

  const InterestFilter&
  getFilter() const
  {
    return m_filter;
  }

private:
  InterestFilter m_filter;
  OnInterest m_onInterest;
};


class RegisteredPrefix
{
public:
  explicit
  RegisteredPrefix(const Name& prefix)
    : m_prefix(prefix)
  {
  }

  RegisteredPrefix(const Name& prefix, shared_ptr<InterestFilterRecord> filter)
    : m_prefix(prefix)
    , m_filter(filter)
  {
  }

  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  const shared_ptr<InterestFilterRecord>&
  getFilter() const
  {
    return m_filter;
  }

private:
  Name m_prefix;

  // to support old interface of combined (un)setInterestFilter
  shared_ptr<InterestFilterRecord> m_filter;
};

/**
 * @brief Opaque class representing ID of the registered prefix
 */
class RegisteredPrefixId;

/**
 * @brief Functor to match RegisteredPrefixId
 */
class MatchRegisteredPrefixId
{
public:
  explicit
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


/**
 * @brief Opaque class representing ID of the Interest filter
 */
class InterestFilterId;

/**
 * @brief Functor to match InterestFilterId
 */
class MatchInterestFilterId
{
public:
  explicit
  MatchInterestFilterId(const InterestFilterId* interestFilterId)
    : m_id(interestFilterId)
  {
  }

  bool
  operator()(const shared_ptr<InterestFilter>& interestFilterId) const
  {
    return (reinterpret_cast<const InterestFilterId*>(interestFilterId.get()) == m_id);
  }
private:
  const InterestFilterId* m_id;
};

} // namespace ndn

#endif // NDN_DETAIL_REGISTERED_PREFIX_HPP
