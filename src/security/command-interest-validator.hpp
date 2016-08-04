/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_SECURITY_COMMAND_INTEREST_VALIDATOR_HPP
#define NDN_SECURITY_COMMAND_INTEREST_VALIDATOR_HPP

#include "validator.hpp"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/key_extractors.hpp>

namespace ndn {
namespace security {

/** \brief a validator for stop-and-wait command Interests
 *  \sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
 *
 *  This validator checks timestamp field of a stop-and-wait command Interest.
 *  Signed Interest validation and Data validation requests are delegated to an inner validator.
 */
class CommandInterestValidator : public Validator
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
    time::nanoseconds gracePeriod = time::seconds(120);

    /** \brief max number of distinct public keys to record last timestamp
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
    ssize_t maxTimestamps = 1000;

    /** \brief max lifetime of a last timestamp record
     *
     *  A last timestamp record expires and can be deleted if it has not been refreshed
     *  within this duration.
     *  Setting this option to 0 or negative makes last timestamp records expire immediately
     *  and causes every command Interest to be processed as initial.
     */
    time::nanoseconds timestampTtl = time::hours(1);

  };

  /** \brief error codes
   *  \todo #1872 assign numeric codes to these errors
   */
  enum class ErrorCode {
    NONE = 0,
    NAME_TOO_SHORT,
    BAD_TIMESTAMP,
    BAD_SIG_INFO,
    MISSING_KEY_LOCATOR,
    BAD_KEY_LOCATOR_TYPE,
    BAD_CERT_NAME,
    TIMESTAMP_OUT_OF_GRACE,
    TIMESTAMP_REORDER
  };

  /** \brief constructor
   *  \param inner a Validator for signed Interest signature validation and Data validation;
   *               this must not be nullptr
   *  \param options stop-and-wait command Interest validation options
   *  \throw std::invalid inner is nullptr
   */
  explicit
  CommandInterestValidator(unique_ptr<Validator> inner,
                           const Options& options = Options());

protected:
  /** \brief validate command Interest
   *
   *  This function executes the following validation procedure:
   *
   *  1. parse the Interest as a command Interest, and extract the public key name
   *  2. invoke inner validation to verify the signed Interest
   *  3. classify the command Interest as either initial or subsequent,
   *     and check the timestamp accordingly
   *  4. record the timestamp as last timestamp of the public key name
   *
   *  The validation request is rejected if any step in this procedure fails.
   */
  virtual void
  checkPolicy(const Interest& interest, int nSteps,
              const OnInterestValidated& accept,
              const OnInterestValidationFailed& reject,
              std::vector<shared_ptr<ValidationRequest>>& nextSteps) override;

  /** \brief validate Data
   *
   *  The validation request is redirected to the inner validator.
   */
  virtual void
  checkPolicy(const Data& data, int nSteps,
              const OnDataValidated& accept,
              const OnDataValidationFailed& reject,
              std::vector<shared_ptr<ValidationRequest>>& nextSteps) override;

private:
  void
  cleanup();

  ErrorCode
  parseCommandInterest(const Interest& interest, Name& keyName, uint64_t& timestamp) const;

  ErrorCode
  checkTimestamp(const Name& keyName, uint64_t timestamp,
                 time::system_clock::TimePoint receiveTime);

private:
  unique_ptr<Validator> m_inner;
  Options m_options;

  struct LastTimestampRecord
  {
    Name keyName;
    uint64_t timestamp;
    time::steady_clock::TimePoint lastRefreshed;
  };

  typedef boost::multi_index_container<
    LastTimestampRecord,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::member<LastTimestampRecord, Name, &LastTimestampRecord::keyName>
      >,
      boost::multi_index::sequenced<>
    >
  > Container;
  typedef Container::nth_index<0>::type Index;
  typedef Container::nth_index<1>::type Queue;

  Container m_container;
  Index& m_index;
  Queue& m_queue;
};

std::ostream&
operator<<(std::ostream& os, CommandInterestValidator::ErrorCode error);

} // namespace security
} // namespace ndn


#endif // NDN_SECURITY_COMMAND_INTEREST_VALIDATOR_HPP
