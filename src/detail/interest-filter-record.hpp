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

#ifndef NDN_DETAIL_INTEREST_FILTER_RECORD_HPP
#define NDN_DETAIL_INTEREST_FILTER_RECORD_HPP

#include "pending-interest.hpp"

namespace ndn {

/**
 * @brief associates an InterestFilter with Interest callback
 */
class InterestFilterRecord : noncopyable
{
public:
  /**
   * @brief Construct an Interest filter record
   *
   * @param filter an InterestFilter that represents what Interest should invoke the callback
   * @param interestCallback invoked when matching Interest is received
   */
  InterestFilterRecord(const InterestFilter& filter,
                       const InterestCallback& interestCallback)
    : m_filter(filter)
    , m_interestCallback(interestCallback)
  {
  }

  /**
   * @return the filter
   */
  const InterestFilter&
  getFilter() const
  {
    return m_filter;
  }

  /**
   * @brief Check if Interest name matches the filter
   * @param name Interest Name
   */
  bool
  doesMatch(const PendingInterest& entry) const
  {
    return (entry.getOrigin() == PendingInterestOrigin::FORWARDER || m_filter.allowsLoopback()) &&
            m_filter.doesMatch(entry.getInterest()->getName());
  }

  /**
   * @brief invokes the InterestCallback
   * @note This method does nothing if the Interest callback is empty
   */
  void
  invokeInterestCallback(const Interest& interest) const
  {
    if (m_interestCallback != nullptr) {
      m_interestCallback(m_filter, interest);
    }
  }

private:
  InterestFilter m_filter;
  InterestCallback m_interestCallback;
};

/**
 * @brief Opaque type to identify an InterestFilterRecord
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
  operator()(const shared_ptr<InterestFilterRecord>& interestFilterId) const
  {
    return reinterpret_cast<const InterestFilterId*>(interestFilterId.get()) == m_id;
  }

private:
  const InterestFilterId* m_id;
};

} // namespace ndn

#endif // NDN_DETAIL_INTEREST_FILTER_RECORD_HPP
