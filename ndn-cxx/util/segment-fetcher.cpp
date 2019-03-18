/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University.
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

#include "ndn-cxx/util/segment-fetcher.hpp"
#include "ndn-cxx/name-component.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/lp/nack.hpp"
#include "ndn-cxx/lp/nack-header.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/map.hpp>

#include <cmath>

namespace ndn {
namespace util {

constexpr double SegmentFetcher::MIN_SSTHRESH;

void
SegmentFetcher::Options::validate()
{
  if (maxTimeout < 1_ms) {
    NDN_THROW(std::invalid_argument("maxTimeout must be greater than or equal to 1 millisecond"));
  }

  if (initCwnd < 1.0) {
    NDN_THROW(std::invalid_argument("initCwnd must be greater than or equal to 1"));
  }

  if (aiStep < 0.0) {
    NDN_THROW(std::invalid_argument("aiStep must be greater than or equal to 0"));
  }

  if (mdCoef < 0.0 || mdCoef > 1.0) {
    NDN_THROW(std::invalid_argument("mdCoef must be in range [0, 1]"));
  }
}

SegmentFetcher::SegmentFetcher(Face& face,
                               security::v2::Validator& validator,
                               const SegmentFetcher::Options& options)
  : m_options(options)
  , m_face(face)
  , m_scheduler(m_face.getIoService())
  , m_validator(validator)
  , m_rttEstimator(options.rttOptions)
  , m_timeLastSegmentReceived(time::steady_clock::now())
  , m_nextSegmentNum(0)
  , m_cwnd(options.initCwnd)
  , m_ssthresh(options.initSsthresh)
  , m_nSegmentsInFlight(0)
  , m_nSegments(0)
  , m_highInterest(0)
  , m_highData(0)
  , m_recPoint(0)
  , m_nReceived(0)
  , m_nBytesReceived(0)
{
  m_options.validate();
}

shared_ptr<SegmentFetcher>
SegmentFetcher::start(Face& face,
                      const Interest& baseInterest,
                      security::v2::Validator& validator,
                      const SegmentFetcher::Options& options)
{
  shared_ptr<SegmentFetcher> fetcher(new SegmentFetcher(face, validator, options));
  fetcher->m_this = fetcher;
  fetcher->fetchFirstSegment(baseInterest, false);
  return fetcher;
}

void
SegmentFetcher::stop()
{
  if (!m_this) {
    return;
  }

  m_pendingSegments.clear(); // cancels pending Interests and timeout events
  m_face.getIoService().post([self = std::move(m_this)] {});
}

bool
SegmentFetcher::shouldStop(const weak_ptr<SegmentFetcher>& weakSelf)
{
  auto self = weakSelf.lock();
  return self == nullptr || self->m_this == nullptr;
}

void
SegmentFetcher::fetchFirstSegment(const Interest& baseInterest, bool isRetransmission)
{
  Interest interest(baseInterest);
  interest.setCanBePrefix(true);
  interest.setMustBeFresh(true);
  interest.setInterestLifetime(m_options.interestLifetime);
  if (isRetransmission) {
    interest.refreshNonce();
  }

  sendInterest(0, interest, isRetransmission);
}

void
SegmentFetcher::fetchSegmentsInWindow(const Interest& origInterest)
{
  if (checkAllSegmentsReceived()) {
    // All segments have been retrieved
    return finalizeFetch();
  }

  int64_t availableWindowSize = static_cast<int64_t>(m_cwnd) - m_nSegmentsInFlight;
  std::vector<std::pair<uint64_t, bool>> segmentsToRequest; // The boolean indicates whether a retx or not

  while (availableWindowSize > 0) {
    if (!m_retxQueue.empty()) {
      auto pendingSegmentIt = m_pendingSegments.find(m_retxQueue.front());
      m_retxQueue.pop();
      if (pendingSegmentIt == m_pendingSegments.end()) {
        // Skip re-requesting this segment, since it was received after RTO timeout
        continue;
      }
      BOOST_ASSERT(pendingSegmentIt->second.state == SegmentState::InRetxQueue);
      segmentsToRequest.emplace_back(pendingSegmentIt->first, true);
    }
    else if (m_nSegments == 0 || m_nextSegmentNum < static_cast<uint64_t>(m_nSegments)) {
      if (m_receivedSegments.count(m_nextSegmentNum) > 0) {
        // Don't request a segment a second time if received in response to first "discovery" Interest
        m_nextSegmentNum++;
        continue;
      }
      segmentsToRequest.emplace_back(m_nextSegmentNum++, false);
    }
    else {
      break;
    }
    availableWindowSize--;
  }

  for (const auto& segment : segmentsToRequest) {
    Interest interest(origInterest); // to preserve Interest elements
    interest.setName(Name(m_versionedDataName).appendSegment(segment.first));
    interest.setCanBePrefix(false);
    interest.setMustBeFresh(false);
    interest.setInterestLifetime(m_options.interestLifetime);
    interest.refreshNonce();
    sendInterest(segment.first, interest, segment.second);
  }
}

void
SegmentFetcher::sendInterest(uint64_t segNum, const Interest& interest, bool isRetransmission)
{
  weak_ptr<SegmentFetcher> weakSelf = m_this;

  ++m_nSegmentsInFlight;
  auto pendingInterest = m_face.expressInterest(interest,
    [this, weakSelf] (const Interest& interest, const Data& data) {
      afterSegmentReceivedCb(interest, data, weakSelf);
    },
    [this, weakSelf] (const Interest& interest, const lp::Nack& nack) {
      afterNackReceivedCb(interest, nack, weakSelf);
    },
    nullptr);

  auto timeout = m_options.useConstantInterestTimeout ? m_options.maxTimeout : getEstimatedRto();
  auto timeoutEvent = m_scheduler.schedule(timeout, [this, interest, weakSelf] {
    afterTimeoutCb(interest, weakSelf);
  });

  if (isRetransmission) {
    updateRetransmittedSegment(segNum, pendingInterest, timeoutEvent);
    return;
  }

  PendingSegment pendingSegment{SegmentState::FirstInterest, time::steady_clock::now(),
                                pendingInterest, timeoutEvent};
  bool isNew = m_pendingSegments.emplace(segNum, std::move(pendingSegment)).second;
  BOOST_VERIFY(isNew);
  m_highInterest = segNum;
}

void
SegmentFetcher::afterSegmentReceivedCb(const Interest& origInterest, const Data& data,
                                       const weak_ptr<SegmentFetcher>& weakSelf)
{
  if (shouldStop(weakSelf))
    return;

  BOOST_ASSERT(m_nSegmentsInFlight > 0);
  m_nSegmentsInFlight--;

  name::Component currentSegmentComponent = data.getName().get(-1);
  if (!currentSegmentComponent.isSegment()) {
    return signalError(DATA_HAS_NO_SEGMENT, "Data Name has no segment number");
  }

  uint64_t currentSegment = currentSegmentComponent.toSegment();

  // The first received Interest could have any segment ID
  std::map<uint64_t, PendingSegment>::iterator pendingSegmentIt;
  if (m_receivedSegments.size() > 0) {
    pendingSegmentIt = m_pendingSegments.find(currentSegment);
  }
  else {
    pendingSegmentIt = m_pendingSegments.begin();
  }

  if (pendingSegmentIt == m_pendingSegments.end()) {
    return;
  }

  pendingSegmentIt->second.timeoutEvent.cancel();

  afterSegmentReceived(data);

  m_validator.validate(data,
                       bind(&SegmentFetcher::afterValidationSuccess, this, _1, origInterest,
                            pendingSegmentIt, weakSelf),
                       bind(&SegmentFetcher::afterValidationFailure, this, _1, _2, weakSelf));
}

void
SegmentFetcher::afterValidationSuccess(const Data& data, const Interest& origInterest,
                                       std::map<uint64_t, PendingSegment>::iterator pendingSegmentIt,
                                       const weak_ptr<SegmentFetcher>& weakSelf)
{
  if (shouldStop(weakSelf))
    return;

  // We update the last receive time here instead of in the segment received callback so that the
  // transfer will not fail to terminate if we only received invalid Data packets.
  m_timeLastSegmentReceived = time::steady_clock::now();

  m_nReceived++;

  // It was verified in afterSegmentReceivedCb that the last Data name component is a segment number
  uint64_t currentSegment = data.getName().get(-1).toSegment();
  // Add measurement to RTO estimator (if not retransmission)
  if (pendingSegmentIt->second.state == SegmentState::FirstInterest) {
    m_rttEstimator.addMeasurement(m_timeLastSegmentReceived - pendingSegmentIt->second.sendTime,
                                  std::max<int64_t>(m_nSegmentsInFlight + 1, 1));
  }

  // Remove from pending segments map
  m_pendingSegments.erase(pendingSegmentIt);

  // Copy data in segment to temporary buffer
  auto receivedSegmentIt = m_receivedSegments.emplace(std::piecewise_construct,
                                                      std::forward_as_tuple(currentSegment),
                                                      std::forward_as_tuple(data.getContent().value_size()));
  std::copy(data.getContent().value_begin(), data.getContent().value_end(),
            receivedSegmentIt.first->second.begin());
  m_nBytesReceived += data.getContent().value_size();
  afterSegmentValidated(data);

  if (data.getFinalBlock()) {
    if (!data.getFinalBlock()->isSegment()) {
      return signalError(FINALBLOCKID_NOT_SEGMENT,
                         "Received FinalBlockId did not contain a segment component");
    }

    if (data.getFinalBlock()->toSegment() + 1 != static_cast<uint64_t>(m_nSegments)) {
      m_nSegments = data.getFinalBlock()->toSegment() + 1;
      cancelExcessInFlightSegments();
    }
  }

  if (m_receivedSegments.size() == 1) {
    m_versionedDataName = data.getName().getPrefix(-1);
    if (currentSegment == 0) {
      // We received the first segment in response, so we can increment the next segment number
      m_nextSegmentNum++;
    }
  }

  if (m_highData < currentSegment) {
    m_highData = currentSegment;
  }

  if (data.getCongestionMark() > 0 && !m_options.ignoreCongMarks) {
    windowDecrease();
  }
  else {
    windowIncrease();
  }

  fetchSegmentsInWindow(origInterest);
}

void
SegmentFetcher::afterValidationFailure(const Data& data,
                                       const security::v2::ValidationError& error,
                                       const weak_ptr<SegmentFetcher>& weakSelf)
{
  if (shouldStop(weakSelf))
    return;

  signalError(SEGMENT_VALIDATION_FAIL, "Segment validation failed: " + boost::lexical_cast<std::string>(error));
}

void
SegmentFetcher::afterNackReceivedCb(const Interest& origInterest, const lp::Nack& nack,
                                    const weak_ptr<SegmentFetcher>& weakSelf)
{
  if (shouldStop(weakSelf))
    return;

  afterSegmentNacked();

  BOOST_ASSERT(m_nSegmentsInFlight > 0);
  m_nSegmentsInFlight--;

  switch (nack.getReason()) {
    case lp::NackReason::DUPLICATE:
    case lp::NackReason::CONGESTION:
      afterNackOrTimeout(origInterest);
      break;
    default:
      signalError(NACK_ERROR, "Nack Error");
      break;
  }
}

void
SegmentFetcher::afterTimeoutCb(const Interest& origInterest,
                               const weak_ptr<SegmentFetcher>& weakSelf)
{
  if (shouldStop(weakSelf))
    return;

  afterSegmentTimedOut();

  BOOST_ASSERT(m_nSegmentsInFlight > 0);
  m_nSegmentsInFlight--;
  afterNackOrTimeout(origInterest);
}

void
SegmentFetcher::afterNackOrTimeout(const Interest& origInterest)
{
  if (time::steady_clock::now() >= m_timeLastSegmentReceived + m_options.maxTimeout) {
    // Fail transfer due to exceeding the maximum timeout between the successful receipt of segments
    return signalError(INTEREST_TIMEOUT, "Timeout exceeded");
  }

  name::Component lastNameComponent = origInterest.getName().get(-1);
  std::map<uint64_t, PendingSegment>::iterator pendingSegmentIt;
  BOOST_ASSERT(m_pendingSegments.size() > 0);
  if (lastNameComponent.isSegment()) {
    BOOST_ASSERT(m_pendingSegments.count(lastNameComponent.toSegment()) > 0);
    pendingSegmentIt = m_pendingSegments.find(lastNameComponent.toSegment());
  }
  else { // First Interest
    BOOST_ASSERT(m_pendingSegments.size() > 0);
    pendingSegmentIt = m_pendingSegments.begin();
  }

  // Cancel timeout event and set status to InRetxQueue
  pendingSegmentIt->second.timeoutEvent.cancel();
  pendingSegmentIt->second.state = SegmentState::InRetxQueue;

  m_rttEstimator.backoffRto();

  if (m_receivedSegments.size() == 0) {
    // Resend first Interest (until maximum receive timeout exceeded)
    fetchFirstSegment(origInterest, true);
  }
  else {
    windowDecrease();
    m_retxQueue.push(pendingSegmentIt->first);
    fetchSegmentsInWindow(origInterest);
  }
}

void
SegmentFetcher::finalizeFetch()
{
  // Combine segments into final buffer
  OBufferStream buf;
  // We may have received more segments than exist in the object.
  BOOST_ASSERT(m_receivedSegments.size() >= static_cast<uint64_t>(m_nSegments));

  for (int64_t i = 0; i < m_nSegments; i++) {
    buf.write(m_receivedSegments[i].get<const char>(), m_receivedSegments[i].size());
  }

  onComplete(buf.buf());
  stop();
}

void
SegmentFetcher::windowIncrease()
{
  if (m_options.useConstantCwnd) {
    BOOST_ASSERT(m_cwnd == m_options.initCwnd);
    return;
  }

  if (m_cwnd < m_ssthresh) {
    m_cwnd += m_options.aiStep; // additive increase
  }
  else {
    m_cwnd += m_options.aiStep / std::floor(m_cwnd); // congestion avoidance
  }
}

void
SegmentFetcher::windowDecrease()
{
  if (m_options.disableCwa || m_highData > m_recPoint) {
    m_recPoint = m_highInterest;

    if (m_options.useConstantCwnd) {
      BOOST_ASSERT(m_cwnd == m_options.initCwnd);
      return;
    }

    // Refer to RFC 5681, Section 3.1 for the rationale behind the code below
    m_ssthresh = std::max(MIN_SSTHRESH, m_cwnd * m_options.mdCoef); // multiplicative decrease
    m_cwnd = m_options.resetCwndToInit ? m_options.initCwnd : m_ssthresh;
  }
}

void
SegmentFetcher::signalError(uint32_t code, const std::string& msg)
{
  onError(code, msg);
  stop();
}

void
SegmentFetcher::updateRetransmittedSegment(uint64_t segmentNum,
                                           const PendingInterestHandle& pendingInterest,
                                           scheduler::EventId timeoutEvent)
{
  auto pendingSegmentIt = m_pendingSegments.find(segmentNum);
  BOOST_ASSERT(pendingSegmentIt != m_pendingSegments.end());
  BOOST_ASSERT(pendingSegmentIt->second.state == SegmentState::InRetxQueue);
  pendingSegmentIt->second.state = SegmentState::Retransmitted;
  pendingSegmentIt->second.hdl = pendingInterest; // cancels previous pending Interest via scoped handle
  pendingSegmentIt->second.timeoutEvent = timeoutEvent;
}

void
SegmentFetcher::cancelExcessInFlightSegments()
{
  for (auto it = m_pendingSegments.begin(); it != m_pendingSegments.end();) {
    if (it->first >= static_cast<uint64_t>(m_nSegments)) {
      it = m_pendingSegments.erase(it); // cancels pending Interest and timeout event
      BOOST_ASSERT(m_nSegmentsInFlight > 0);
      m_nSegmentsInFlight--;
    }
    else {
      ++it;
    }
  }
}

bool
SegmentFetcher::checkAllSegmentsReceived()
{
  bool haveReceivedAllSegments = false;

  if (m_nSegments != 0 && m_nReceived >= m_nSegments) {
    haveReceivedAllSegments = true;
    // Verify that all segments in window have been received. If not, send Interests for missing segments.
    for (uint64_t i = 0; i < static_cast<uint64_t>(m_nSegments); i++) {
      if (m_receivedSegments.count(i) == 0) {
        m_retxQueue.push(i);
        haveReceivedAllSegments = false;
      }
    }
  }

  return haveReceivedAllSegments;
}

time::milliseconds
SegmentFetcher::getEstimatedRto()
{
  // We don't want an Interest timeout greater than the maximum allowed timeout between the
  // succesful receipt of segments
  return std::min(m_options.maxTimeout,
                  time::duration_cast<time::milliseconds>(m_rttEstimator.getEstimatedRto()));
}

} // namespace util
} // namespace ndn
