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

#ifndef NDN_DETAIL_INTEREST_FILTER_RECORD_HPP
#define NDN_DETAIL_INTEREST_FILTER_RECORD_HPP

#include "../common.hpp"
#include "../name.hpp"
#include "../interest.hpp"

namespace ndn {

class InterestFilterRecord : noncopyable
{
public:
  typedef function<void (const InterestFilter&, const Interest&)> InterestCallback;

  InterestFilterRecord(const InterestFilter& filter, const InterestCallback& afterInterest)
    : m_filter(filter)
    , m_afterInterest(afterInterest)
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

  /**
   * @brief invokes the InterestCallback
   * @note If the DataCallback is an empty function, this method does nothing.
   */
  void
  invokeInterestCallback(const Interest& interest) const
  {
    m_afterInterest(m_filter, interest);
  }

  const InterestFilter&
  getFilter() const
  {
    return m_filter;
  }

private:
  InterestFilter m_filter;
  InterestCallback m_afterInterest;
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
  operator()(const shared_ptr<InterestFilterRecord>& interestFilterId) const
  {
    return (reinterpret_cast<const InterestFilterId*>(interestFilterId.get()) == m_id);
  }
private:
  const InterestFilterId* m_id;
};

} // namespace ndn

#endif // NDN_DETAIL_INTEREST_FILTER_RECORD_HPP
