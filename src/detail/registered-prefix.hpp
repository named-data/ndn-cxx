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

#include "interest-filter-record.hpp"

namespace ndn {

namespace nfd {
class ControlParameters;
}

class RegisteredPrefix : noncopyable
{
public:
  /** \brief a callback on command success
   */
  typedef function<void(const nfd::ControlParameters&)> SuccessCallback;

  /** \brief a callback on command failure
   */
  typedef function<void(uint32_t/*code*/,const std::string&/*reason*/)> FailureCallback;

  /// @brief Function that should be called to unregister prefix
  typedef function<void(const SuccessCallback& onSuccess,
                        const FailureCallback& onFailure)> Unregistrator;

  RegisteredPrefix(const Name& prefix,
                   const Unregistrator& unregistrator)
    : m_prefix(prefix)
    , m_unregistrator(unregistrator)
  {
  }

  RegisteredPrefix(const Name& prefix,
                   const shared_ptr<InterestFilterRecord>& filter,
                   const Unregistrator& unregistrator)
    : m_prefix(prefix)
    , m_filter(filter)
    , m_unregistrator(unregistrator)
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

  void
  unregister(const SuccessCallback& onSuccess,
             const FailureCallback& onFailure)
  {
    if (static_cast<bool>(m_unregistrator)) {
      m_unregistrator(onSuccess, onFailure);
    }
  }

private:
  Name m_prefix;
  shared_ptr<InterestFilterRecord> m_filter;
  Unregistrator m_unregistrator;
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


} // namespace ndn

#endif // NDN_DETAIL_REGISTERED_PREFIX_HPP
