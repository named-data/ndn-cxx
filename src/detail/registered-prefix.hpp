/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_DETAIL_REGISTERED_PREFIX_HPP
#define NDN_DETAIL_REGISTERED_PREFIX_HPP

#include "../common.hpp"
#include "../name.hpp"
#include "../interest.hpp"

#include "interest-filter-record.hpp"
#include "management/nfd-command-options.hpp"
#include "management/nfd-control-parameters.hpp"

namespace ndn {

class RegisteredPrefix : noncopyable
{
public:
  /** \brief a callback on command success
   */
  typedef function<void(const nfd::ControlParameters&)> SuccessCallback;

  /** \brief a callback on command failure
   */
  typedef function<void(uint32_t/*code*/,const std::string&/*reason*/)> FailureCallback;

  RegisteredPrefix(const Name& prefix,
                   const shared_ptr<InterestFilterRecord>& filter,
                   const nfd::CommandOptions& options)
    : m_prefix(prefix)
    , m_filter(filter)
    , m_options(options)
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

  const nfd::CommandOptions&
  getCommandOptions() const
  {
    return m_options;
  }

private:
  Name m_prefix;
  shared_ptr<InterestFilterRecord> m_filter;
  nfd::CommandOptions m_options;
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
