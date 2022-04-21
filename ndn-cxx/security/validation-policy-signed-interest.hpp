/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_VALIDATION_POLICY_SIGNED_INTEREST_HPP
#define NDN_CXX_SECURITY_VALIDATION_POLICY_SIGNED_INTEREST_HPP

#include "ndn-cxx/security/validation-policy.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

namespace ndn {
namespace security {
inline namespace v2 {

/** \brief Validation policy for signed Interests
 *
 *  This policy checks the timestamp, sequence number, and nonce fields of signed Interests.
 */
class ValidationPolicySignedInterest : public ValidationPolicy
{
private:
  using SigNonce = std::vector<uint8_t>;
  struct NonceSet {};
  struct NonceList {};

public:
  class Options
  {
  public:
    Options()
    {
    }

  public:
    /** \brief Whether to validate timestamps in signed Interests by ensuring they are not
     *         reordered for a given public key and are within a given grace period of the current
     *         time
     *
     *  If set to false, timestamps checks will be skipped when validating signed Interests.
     *
     *  The grace period is controlled by #timestampGracePeriod.
     */
    bool shouldValidateTimestamps = true;

    /** \brief Tolerance of timestamp differences from the current time
     *
     *  A signed Interest is considered "initial" if the validator does not currently store a
     *  record for its associated public key -- entries may be erased due to age or storage
     *  limitations. For such "initial" signed Interests, their timestamp will be compared with the
     *  current system clock, and a signed Interest will be rejected if the absolute difference of
     *  its timestamp from the clock time is greater than this grace interval.
     *
     *  This value should be positive. Setting this option to 0 or to a negative value causes the
     *  validator to require exactly the same timestamp as the system clock, which will most likely
     *  reject all signed Interests due to network delay and clock skew. Therefore, it is not
     *  recommended to set this value to zero or less or to a very small interval.
     */
    time::nanoseconds timestampGracePeriod = 2_min;

    /** \brief Whether to validate sequence numbers in signed Interests by ensuring they are present
     *         and are strictly increasing for a given public key
     *
     *  If set to false, sequence numbers checks will be skipped when validating signed Interests.
     */
    bool shouldValidateSeqNums = false;

    /** \brief Whether to validate nonces by ensuring that they are present and do not overlap with
     *         one of the last n nonces for a given public key
     *
     *  If set to false, nonce checks will be skipped when validating signed Interests.
     *
     *  The number of previous nonces to check for uniqueness against is controlled by
     *  #maxNonceRecordCount.
     */
    bool shouldValidateNonces = true;

    /** \brief Number of previous nonces to track for each public key
     *
     *  If nonce checks are enabled, incoming Interests will be dropped if their nonce matches one
     *  of the last n nonces for their associated public key, where n is the value of this option.
     *
     *  Setting this option to -1 allows an unlimited number of nonces to be tracked for each
     *  public key.
     *  Setting this option to 0 will cause last-n nonce matching to not be performed. However, if
     *  #shouldValidateNonces is set to true, signed Interests will still fail validation if they do
     *  not contain a nonce.
     */
    ssize_t maxNonceRecordCount = 1000;

    /** \brief Max number of distinct public keys to track
     *
     *  The validator records a "last" timestamp and sequence number, along with the last n nonces,
     *  for every public key. For subsequent signed Interest associated with the same public key,
     *  depending upon the other validator options, their timestamps, sequence numbers, and/or
     *  nonces will be compared to the last timestamp, last sequence number, and last n nonces
     *  observed from signed Interests using that public key. Depending upon the enabled checks, a
     *  signed Interest will be rejected if its timestamp or sequence number is less than or equal
     *  to the respective last value for the associated public key, or if its nonce matches of the
     *  last n nonces observed for the associated public key.
     *
     *  This option limits the number of distinct public keys that can be tracked. If this limit is
     *  exceeded, the records will be deleted until the number of records is less than or
     *  equal to this limit in LRU order (by the time the record was last refreshed).
     *
     *  Setting this option to -1 allows an unlimited number of public keys to be tracked.
     *  Setting this option to 0 disables last timestamp, sequence number, and nonce records and
     *  will cause every signed Interest to be treated as being associated with a
     *  previously-unobserved public key -- this is not recommended for obvious security reasons.
     */
    ssize_t maxRecordCount = 1000;
  };

  /** \brief Constructor
   *  \param inner Validator for signed Interest and Data validation. This must not be nullptr.
   *  \param options Signed Interest validation options
   *  \throw std::invalid_argument Inner policy is nullptr
   */
  explicit
  ValidationPolicySignedInterest(unique_ptr<ValidationPolicy> inner, const Options& options = {});

protected:
  void
  checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) override;

  void
  checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
              const ValidationContinuation& continueValidation) override;

private:
  bool
  checkIncomingInterest(const shared_ptr<ValidationState>& state, const Interest& interest);

  void
  insertRecord(const Name& keyName,
               optional<time::system_clock::TimePoint> timestamp,
               optional<uint64_t> seqNum,
               optional<SigNonce> nonce);

private:
  Options m_options;

  using NonceContainer = boost::multi_index_container<
    SigNonce,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<NonceSet>,
        boost::multi_index::identity<SigNonce>
      >,
      boost::multi_index::sequenced<
        boost::multi_index::tag<NonceList>
      >
    >
  >;

  struct LastInterestRecord
  {
    LastInterestRecord(const Name& keyName,
                       optional<time::system_clock::TimePoint> timestamp,
                       optional<uint64_t> seqNum)
      : keyName(keyName)
      , timestamp(timestamp)
      , seqNum(seqNum)
      , lastRefreshed(time::steady_clock::now())
    {
    }

    Name keyName;
    optional<time::system_clock::TimePoint> timestamp;
    optional<uint64_t> seqNum;
    NonceContainer observedNonces;
    time::steady_clock::TimePoint lastRefreshed;
  };

  using Container = boost::multi_index_container<
    LastInterestRecord,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::member<LastInterestRecord, Name, &LastInterestRecord::keyName>
      >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::member<LastInterestRecord, time::steady_clock::TimePoint,
                                   &LastInterestRecord::lastRefreshed>
      >
    >
  >;

  Container m_container;
  Container::nth_index<0>::type& m_byKeyName;
  Container::nth_index<1>::type& m_byLastRefreshed;
};

} // inline namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_VALIDATION_POLICY_SIGNED_INTEREST_HPP
