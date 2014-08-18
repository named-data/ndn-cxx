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

#ifndef NDN_UTIL_SEGMENT_FETCHER_HPP
#define NDN_UTIL_SEGMENT_FETCHER_HPP

#include "../common.hpp"
#include "../face.hpp"

namespace ndn {

class OBufferStream;

namespace util {

/**
 * @brief Functor to skip validation of individual packets by SegmentFetcher
 */
class DontVerifySegment
{
public:
  bool
  operator()(const Data& data) const
  {
    return true;
  }
};

/**
 * @brief Utility class to fetch latest version of the segmented data
 *
 * SegmentFetcher assumes that the data is named /<prefix>/<version>/<segment>,
 * where:
 * - <prefix> is the specified prefix,
 * - <version> is an unknown version that needs to be discovered, and
 * - <segment> is a segment number (number of segments is unknown and is controlled
 *   by `FinalBlockId` field in at least the last Data packet
 *
 * The following logic is implemented in SegmentFetcher:
 *
 * 1. Express first interest to discover version:
 *
 *    >> Interest: /<prefix>?ChildSelector=1&MustBeFresh=yes
 *
 * 2. Infer the latest version of Data:  <version> = Data.getName().get(-2)
 *
 * 3. If segment number in the retrieved packet == 0, go to step 5.
 *
 * 4. Send Interest for segment 0:
 *
 *    >> Interest: /<prefix>/<version>/<segment=0>
 *
 * 5. Keep sending Interests for the next segment while the retrieved Data does not have
 *    FinalBlockId or FinalBlockId != Data.getName().get(-1).
 *
 *    >> Interest: /<prefix>/<version>/<segment=(N+1))>
 *
 * 6. Fire onCompletion callback with memory block that combines content part from all
 *    segmented objects.
 *
 * If an error occurs during the fetching process, an error callback is fired
 * with a proper error code.  The following errors are possible:
 *
 * - `INTEREST_TIMEOUT`: if any of the Interests times out
 * - `DATA_HAS_NO_SEGMENT`: if any of the retrieved Data packets don't have segment
 *   as a last component of the name (not counting implicit digest)
 * - `SEGMENT_VERIFICATION_FAIL`: if any retrieved segment fails user-provided validation
 *
 * In order to validate individual segments, an VerifySegment callback needs to be specified.
 * If the callback returns false, fetching process is aborted with SEGMENT_VERIFICATION_FAIL.
 * If data validation is not required, provided DontVerifySegment() functor can be used.
 *
 * Examples:
 *
 *     void
 *     onComplete(const ConstBufferPtr& data)
 *     {
 *       ...
 *     }
 *
 *     void
 *     onError(uint32_t errorCode, const std::string& errorMsg)
 *     {
 *       ...
 *     }
 *
 *     ...
 *     SegmentFetcher::fetch(face, Interest("/data/prefix", time::seconds(1000)),
 *                           DontVerifySegment(),
 *                           bind(&onComplete, this, _1),
 *                           bind(&onError, this, _1, _2));
 *
 */
class SegmentFetcher : noncopyable
{
public:
  typedef function<void (const ConstBufferPtr& data)> CompleteCallback;
  typedef function<bool (const Data& data)> VerifySegment;
  typedef function<void (uint32_t code, const std::string& msg)> ErrorCallback;

  /**
   * @brief Error codes that can be passed to ErrorCallback
   */
  enum ErrorCode {
    INTEREST_TIMEOUT = 1,
    DATA_HAS_NO_SEGMENT = 2,
    SEGMENT_VERIFICATION_FAIL = 3
  };

  /**
   * @brief Initiate segment fetching
   *
   * @param face          Reference to the Face that should be used to fetch data
   * @param baseInterest  An Interest for the initial segment of requested data.
   *                      This interest may include custom InterestLifetime and selectors that
   *                      will propagate to all subsequent Interests.  The only exception is that
   *                      the initial Interest will be forced to include "ChildSelector=1" and
   *                      "MustBeFresh=true" selectors, which will be turned off in subsequent
   *                      Interests.
   * @param verifySegment Functor to be called when Data segment is received.  If
   *                      functor return false, fetching will be aborted with
   *                      SEGMENT_VERIFICATION_FAIL error
   * @param completeCallback    Callback to be fired when all segments are fetched
   * @param errorCallback       Callback to be fired when an error occurs (@see Errors)
   */
  static
  void
  fetch(Face& face,
        const Interest& baseInterest,
        const VerifySegment& verifySegment,
        const CompleteCallback& completeCallback,
        const ErrorCallback& errorCallback);

private:
  SegmentFetcher(Face& face,
                 const VerifySegment& verifySegment,
                 const CompleteCallback& completeCallback,
                 const ErrorCallback& errorCallback);

  void
  fetchFirstSegment(const Interest& baseInterest, const shared_ptr<SegmentFetcher>& self);

  void
  fetchNextSegment(const Interest& origInterest, const Name& dataName, uint64_t segmentNo,
                   const shared_ptr<SegmentFetcher>& self);

  void
  onSegmentReceived(const Interest& origInterest,
                    const Data& data, bool isSegmentZeroExpected,
                    const shared_ptr<SegmentFetcher>& self);

private:
  Face& m_face;
  VerifySegment m_verifySegment;
  CompleteCallback m_completeCallback;
  ErrorCallback m_errorCallback;

  shared_ptr<OBufferStream> m_buffer;
};

} // util
} // ndn

#endif // NDN_UTIL_SEGMENT_FETCHER_HPP
