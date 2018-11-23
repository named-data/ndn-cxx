/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_SECURITY_V2_VALIDATION_POLICY_COMMAND_INTEREST_HPP
#define NDN_SECURITY_V2_VALIDATION_POLICY_COMMAND_INTEREST_HPP

#include "validation-policy.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/key_extractors.hpp>

namespace ndn {
namespace security {
namespace v2 {

/** \brief Validation policy for stop-and-wait command Interests
 *  \sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
 *
 *  This policy checks the timestamp field of a stop-and-wait command Interest.
 *  Signed Interest validation and Data validation requests are delegated to an inner policy.
 */
class ValidationPolicyCommandInterest : public ValidationPolicy
{
public:
  class Options
  {
  public:
    Options()
    {
    }

  public:
    /** \brief tolerance of initial timestamp
     *
     *  A stop-and-wait command Interest is considered "initial" if the validator
     *  has not recorded the last timestamp from the same public key, or when
     *  such knowledge has been erased.
     *  For an initial command Interest, its timestamp is compared to the current
     *  system clock, and the command Interest is rejected if the absolute difference
     *  is greater than the grace interval.
     *
     *  This should be positive.
     *  Setting this option to 0 or negative causes the validator to require exactly same
     *  timestamp as the system clock, which most likely rejects all command Interests.
     */
    time::nanoseconds gracePeriod = 2_min;

    /** \brief max number of distinct public keys of which to record the last timestamp
     *
     *  The validator records last timestamps for every public key.
     *  For a subsequent command Interest using the same public key,
     *  its timestamp is compared to the last timestamp from that public key,
     *  and the command Interest is rejected if its timestamp is
     *  less than or equal to the recorded timestamp.
     *
     *  This option limits the number of distinct public keys being tracked.
     *  If the limit is exceeded, the oldest record is deleted.
     *
     *  Setting this option to -1 allows tracking unlimited public keys.
     *  Setting this option to 0 disables last timestamp records and causes
     *  every command Interest to be processed as initial.
     */
    ssize_t maxRecords = 1000;

    /** \brief max lifetime of a last timestamp record
     *
     *  A last timestamp record expires and can be deleted if it has not been refreshed
     *  within this duration.
     *  Setting this option to 0 or negative makes last timestamp records expire immediately
     *  and causes every command Interest to be processed as initial.
     */
    time::nanoseconds recordLifetime = 1_h;
  };

  /** \brief constructor
   *  \param inner a Validator for signed Interest signature validation and Data validation;
   *               this must not be nullptr
   *  \param options stop-and-wait command Interest validation options
   *  \throw std::invalid_argument inner policy is nullptr
   */
  explicit
  ValidationPolicyCommandInterest(unique_ptr<ValidationPolicy> inner,
                                  const Options& options = {});

protected:
  void
  checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) override;

  void
  checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) override;

private:
  void
  cleanup();

  std::tuple<bool, Name, uint64_t>
  parseCommandInterest(const Interest& interest, const shared_ptr<ValidationState>& state) const;

  bool
  checkTimestamp(const shared_ptr<ValidationState>& state,
                 const Name& keyName, uint64_t timestamp);

  void
  insertNewRecord(const Name& keyName, uint64_t timestamp);

private:
  Options m_options;

  struct LastTimestampRecord
  {
    Name keyName;
    uint64_t timestamp;
    time::steady_clock::TimePoint lastRefreshed;
  };

  using Container = boost::multi_index_container<
    LastTimestampRecord,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::member<LastTimestampRecord, Name, &LastTimestampRecord::keyName>
      >,
      boost::multi_index::sequenced<>
    >
  >;
  using Index = Container::nth_index<0>::type;
  using Queue = Container::nth_index<1>::type;

  Container m_container;
  Index& m_index;
  Queue& m_queue;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_VALIDATION_POLICY_COMMAND_INTEREST_HPP
