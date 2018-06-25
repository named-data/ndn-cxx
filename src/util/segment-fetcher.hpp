/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018, Regents of the University of California,
 *                          Colorado State University,
 *                          University Pierre & Marie Curie, Sorbonne University.
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
 *
 * @author Shuo Yang
 * @author Weiwei Liu
 * @author Chavoosh Ghasemi
 */

#ifndef NDN_UTIL_SEGMENT_FETCHER_HPP
#define NDN_UTIL_SEGMENT_FETCHER_HPP

#include "../common.hpp"
#include "../face.hpp"
#include "../security/v2/validator.hpp"
#include "rtt-estimator.hpp"
#include "scheduler.hpp"
#include "signal.hpp"

#include <queue>

namespace ndn {
namespace util {

/**
 * @brief Utility class to fetch the latest version of a segmented object.
 *
 * SegmentFetcher assumes that segments in the object are named `/<prefix>/<version>/<segment>`,
 * where:
 * - `<prefix>` is the specified prefix,
 * - `<version>` is an unknown version that needs to be discovered, and
 * - `<segment>` is a segment number (The number of segments in the object is unknown until a Data
 *   packet containing the `FinalBlockId` field is receieved.)
 *
 * The following logic is implemented in SegmentFetcher:
 *
 * 1. Express an Interest to discover the latest version of the object:
 *
 *    >> Interest: `/<prefix>?ndn.CanBePrefix=true&ndn.MustBeFresh=true`
 *
 * 2. Infer the latest version of the object: `<version> = Data.getName().get(-2)`
 *
 * 3. Keep sending Interests for future segments until an error occurs or the number of segments
 *    indicated by the FinalBlockId in a received Data packet is reached. This retrieval will start
 *    at segment 1 if segment 0 was received in response to the Interest expressed in step 2;
 *    otherwise, retrieval will start at segment 0. By default, congestion control will be used to
 *    manage the Interest window size. Interests expressed in this step will follow this Name
 *    format:
 *
 *    >> Interest: `/<prefix>/<version>/<segment=(N)>`
 *
 * 4. Signal `onComplete` with a memory block that combines the content of all segments in the
 *    object.
 *
 * If an error occurs during the fetching process, `onError` is signaled with one of the error codes
 * from `SegmentFetcher::ErrorCode`.
 *
 * A Validator instance must be specified to validate individual segments. Every time a segment has
 * been successfully validated, `afterValidationSuccess` will be signaled. If a segment fails
 * validation, `afterValidationFailure` will be signaled.
 *
 * Examples:
 *
 *     @code
 *     void
 *     afterFetchComplete(ConstBufferPtr data)
 *     {
 *       ...
 *     }
 *
 *     void
 *     afterFetchError(uint32_t errorCode, const std::string& errorMsg)
 *     {
 *       ...
 *     }
 *
 *     ...
 *     auto fetcher = SegmentFetcher::start(face, Interest("/data/prefix"), validator);
 *     fetcher->onComplete.connect(bind(&afterFetchComplete, this, _1));
 *     fetcher->onError.connect(bind(&afterFetchError, this, _1, _2));
 *     @endcode
 *
 */
class SegmentFetcher : noncopyable
{
public:
  // Deprecated: will be removed when deprecated fetch() API is removed - use start() instead
  typedef function<void (ConstBufferPtr data)> CompleteCallback;
  typedef function<void (uint32_t code, const std::string& msg)> ErrorCallback;

  /**
   * @brief Error codes passed to `onError`
   */
  enum ErrorCode {
    /// retrieval timed out because the maximum timeout between the successful receipt of segments was exceeded
    INTEREST_TIMEOUT = 1,
    /// one of the retrieved Data packets lacked a segment number in the last Name component (excl. implicit digest)
    DATA_HAS_NO_SEGMENT = 2,
    /// one of the retrieved segments failed user-provided validation
    SEGMENT_VALIDATION_FAIL = 3,
    /// an unrecoverable Nack was received during retrieval
    NACK_ERROR = 4,
    /// a received FinalBlockId did not contain a segment component
    FINALBLOCKID_NOT_SEGMENT = 5
  };

  class Options
  {
  public:
    Options()
    {
    }

    void
    validate();

  public:
    bool useConstantCwnd = false; ///< if true, window size is kept at `initCwnd`
    bool useConstantInterestTimeout = false; ///< if true, Interest timeout is kept at `maxTimeout`
    time::milliseconds maxTimeout = 60_s; ///< maximum allowed time between successful receipt of segments
    time::milliseconds interestLifetime = 4_s; ///< lifetime of sent Interests - independent of Interest timeout
    double initCwnd = 1.0; ///< initial congestion window size
    double initSsthresh = std::numeric_limits<double>::max(); ///< initial slow start threshold
    double aiStep = 1.0; ///< additive increase step (in segments)
    double mdCoef = 0.5; ///< multiplicative decrease coefficient
    bool disableCwa = false; ///< disable Conservative Window Adaptation
    bool resetCwndToInit = false; ///< reduce cwnd to initCwnd when loss event occurs
    bool ignoreCongMarks = false; ///< disable window decrease after congestion mark received
    RttEstimator::Options rttOptions; ///< options for RTT estimator
  };

  /**
   * @brief Initiates segment fetching
   *
   * @param face         Reference to the Face that should be used to fetch data
   * @param baseInterest Interest for the initial segment of requested data.
   *                     This interest may include a custom InterestLifetime and parameters that
   *                     will propagate to all subsequent Interests. The only exception is that the
   *                     initial Interest will be forced to include the "CanBePrefix=true" and
   *                     "MustBeFresh=true" parameters, which will not be included in subsequent
   *                     Interests.
   * @param validator    Reference to the Validator the fetcher will use to validate data.
   *                     The caller must ensure the validator is valid until either `onComplete` or
   *                     `onError` has been signaled.
   * @param options      Options controlling the transfer
   *
   * @return             A shared_ptr to the constructed SegmentFetcher.
   *                     This shared_ptr is kept internally for the lifetime of the transfer.
   *                     Therefore, it does not need to be saved and is provided here so that the
   *                     SegmentFetcher's signals can be connected to.
   *
   * Transfer completion, failure, and progress are indicated via signals.
   */
  static shared_ptr<SegmentFetcher>
  start(Face& face,
        const Interest& baseInterest,
        security::v2::Validator& validator,
        const Options& options = Options());

  /**
   * @brief Initiates segment fetching
   *
   * @deprecated Use @c start
   *
   * @param face             Reference to the Face that should be used to fetch data
   * @param baseInterest     An Interest for the initial segment of requested data.
   *                         This interest may include a custom InterestLifetime and parameters that
   *                         will propagate to all subsequent Interests.  The only exception is that
   *                         the initial Interest will be forced to include the parameters
   *                         "CanBePrefix=true" and "MustBeFresh=true", which will be turned off in
   *                         subsequent Interests.
   * @param validator        Reference to the Validator that should be used to validate data. Caller
   *                         must ensure validator is valid until either completeCallback or
   *                         errorCallback is invoked.
   * @param completeCallback Callback to be fired when all segments are fetched
   * @param errorCallback    Callback to be fired when an error occurs (@see Errors)
   *
   * @return                 A shared_ptr to the constructed SegmentFetcher.
   *                         This shared_ptr is kept internally for the lifetime of the transfer.
   *                         Therefore, it does not need to be saved and is provided here so that
   *                         the SegmentFetcher's signals can be connected to.
   */
  [[deprecated("use SegmentFetcher::start instead")]]
  static shared_ptr<SegmentFetcher>
  fetch(Face& face,
        const Interest& baseInterest,
        security::v2::Validator& validator,
        const CompleteCallback& completeCallback,
        const ErrorCallback& errorCallback);

  /**
   * @brief Initiate segment fetching
   *
   * @deprecated Use @c start
   *
   * @param face             Reference to the Face that should be used to fetch data
   * @param baseInterest     An Interest for the initial segment of requested data.
   *                         This interest may include a custom InterestLifetime and parameters that
   *                         will propagate to all subsequent Interests.  The only exception is that
   *                         the initial Interest will be forced to include the parameters
   *                         "CanBePrefix=true" and "MustBeFresh=true", which will both be set to
   *                         false in subsequent Interests.
   * @param validator        A shared_ptr to the Validator that should be used to validate data.
   *
   * @param completeCallback Callback to be fired when all segments are fetched
   * @param errorCallback    Callback to be fired when an error occurs (@see Errors)
   *
   * @return                 A shared_ptr to the constructed SegmentFetcher.
   *                         This shared_ptr is kept internally for the lifetime of the transfer.
   *                         Therefore, it does not need to be saved and is provided here so that
   *                         the SegmentFetcher's signals can be connected to.
   */
  [[deprecated("use SegmentFetcher::start instead")]]
  static shared_ptr<SegmentFetcher>
  fetch(Face& face,
        const Interest& baseInterest,
        shared_ptr<security::v2::Validator> validator,
        const CompleteCallback& completeCallback,
        const ErrorCallback& errorCallback);

private:
  class PendingSegment;

  SegmentFetcher(Face& face, security::v2::Validator& validator, const Options& options);

  void
  fetchFirstSegment(const Interest& baseInterest,
                    bool isRetransmission,
                    shared_ptr<SegmentFetcher> self);

  void
  fetchSegmentsInWindow(const Interest& origInterest, shared_ptr<SegmentFetcher> self);

  void
  afterSegmentReceivedCb(const Interest& origInterest,
                         const Data& data,
                         shared_ptr<SegmentFetcher> self);
  void
  afterValidationSuccess(const Data& data,
                         const Interest& origInterest,
                         std::map<uint64_t, PendingSegment>::iterator pendingSegmentIt,
                         shared_ptr<SegmentFetcher> self);

  void
  afterValidationFailure(const Data& data,
                         const security::v2::ValidationError& error,
                         shared_ptr<SegmentFetcher> self);

  void
  afterNackReceivedCb(const Interest& origInterest,
                      const lp::Nack& nack,
                      shared_ptr<SegmentFetcher> self);

  void
  afterTimeoutCb(const Interest& origInterest,
                 shared_ptr<SegmentFetcher> self);

  void
  afterNackOrTimeout(const Interest& origInterest,
                     shared_ptr<SegmentFetcher> self);

  void
  finalizeFetch(shared_ptr<SegmentFetcher> self);

  void
  windowIncrease();

  void
  windowDecrease();

  void
  signalError(uint32_t code, const std::string& msg);

  void
  updateRetransmittedSegment(uint64_t segmentNum,
                             const PendingInterestId* pendingInterest,
                             scheduler::EventId timeoutEvent);

  void
  cancelExcessInFlightSegments();

  bool
  checkAllSegmentsReceived();

  time::milliseconds
  getEstimatedRto();

public:
  /**
   * @brief Emits upon successful retrieval of the complete data
   */
  Signal<SegmentFetcher, ConstBufferPtr> onComplete;

  /**
   * @brief Emits when the retrieval could not be completed due to an error
   *
   * Handler(s) are provided with an error code and a string error message.
   */
  Signal<SegmentFetcher, uint32_t, std::string> onError;

  /**
   * @brief Emits whenever a data segment received
   */
  Signal<SegmentFetcher, Data> afterSegmentReceived;

  /**
   * @brief Emits whenever a received data segment has been successfully validated
   */
  Signal<SegmentFetcher, Data> afterSegmentValidated;

  /**
   * @brief Emits whenever an Interest for a data segment is nacked
   */
  Signal<SegmentFetcher> afterSegmentNacked;

  /**
   * @brief Emits whenever an Interest for a data segment times out
   */
  Signal<SegmentFetcher> afterSegmentTimedOut;

private:
  enum class SegmentState {
    FirstInterest, ///< the first Interest for this segment has been sent
    InRetxQueue,   ///< the segment is awaiting Interest retransmission
    Retransmitted, ///< one or more retransmitted Interests have been sent for this segment
  };

  class PendingSegment
  {
  public:
    SegmentState state;
    time::steady_clock::TimePoint sendTime;
    const PendingInterestId* id;
    scheduler::EventId timeoutEvent;
  };

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static constexpr double MIN_SSTHRESH = 2.0;

  Options m_options;
  Face& m_face;
  Scheduler m_scheduler;
  security::v2::Validator& m_validator;
  RttEstimator m_rttEstimator;
  time::milliseconds m_timeout;

  time::steady_clock::TimePoint m_timeLastSegmentReceived;
  std::queue<uint64_t> m_retxQueue;
  Name m_versionedDataName;
  uint64_t m_nextSegmentNum;
  double m_cwnd;
  double m_ssthresh;
  int64_t m_nSegmentsInFlight;
  int64_t m_nSegments;
  uint64_t m_highInterest;
  uint64_t m_highData;
  uint64_t m_recPoint;
  int64_t m_nReceived;
  int64_t m_nBytesReceived;

  std::map<uint64_t, Buffer> m_receivedSegments;
  std::map<uint64_t, PendingSegment> m_pendingSegments;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_SEGMENT_FETCHER_HPP
