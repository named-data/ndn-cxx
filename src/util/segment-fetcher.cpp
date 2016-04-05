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

#include "segment-fetcher.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../name-component.hpp"
#include "../lp/nack.hpp"
#include "../lp/nack-header.hpp"

namespace ndn {
namespace util {

const uint32_t SegmentFetcher::MAX_INTEREST_REEXPRESS = 3;

SegmentFetcher::SegmentFetcher(Face& face,
                               shared_ptr<Validator> validator,
                               const CompleteCallback& completeCallback,
                               const ErrorCallback& errorCallback)
  : m_face(face)
  , m_scheduler(m_face.getIoService())
  , m_validator(validator)
  , m_completeCallback(completeCallback)
  , m_errorCallback(errorCallback)
  , m_buffer(make_shared<OBufferStream>())
{
}

void
SegmentFetcher::fetch(Face& face,
                      const Interest& baseInterest,
                      Validator& validator,
                      const CompleteCallback& completeCallback,
                      const ErrorCallback& errorCallback)
{
  shared_ptr<Validator> sharedValidator = shared_ptr<Validator>(&validator, [] (Validator*) {});

  fetch(face, baseInterest, sharedValidator, completeCallback, errorCallback);
}

void
SegmentFetcher::fetch(Face& face,
                      const Interest& baseInterest,
                      shared_ptr<Validator> validator,
                      const CompleteCallback& completeCallback,
                      const ErrorCallback& errorCallback)
{
  shared_ptr<SegmentFetcher> fetcher(new SegmentFetcher(face, validator, completeCallback,
                                                        errorCallback));

  fetcher->fetchFirstSegment(baseInterest, fetcher);
}

void
SegmentFetcher::fetchFirstSegment(const Interest& baseInterest,
                                  shared_ptr<SegmentFetcher> self)
{
  Interest interest(baseInterest);
  interest.setChildSelector(1);
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&SegmentFetcher::afterSegmentReceived, this, _1, _2, true, self),
                         bind(&SegmentFetcher::afterNackReceived, this, _1, _2, 0, self),
                         bind(m_errorCallback, INTEREST_TIMEOUT, "Timeout"));
}

void
SegmentFetcher::fetchNextSegment(const Interest& origInterest, const Name& dataName,
                                 uint64_t segmentNo,
                                 shared_ptr<SegmentFetcher> self)
{
  Interest interest(origInterest); // to preserve any selectors
  interest.refreshNonce();
  interest.setChildSelector(0);
  interest.setMustBeFresh(false);
  interest.setName(dataName.getPrefix(-1).appendSegment(segmentNo));
  m_face.expressInterest(interest,
                         bind(&SegmentFetcher::afterSegmentReceived, this, _1, _2, false, self),
                         bind(&SegmentFetcher::afterNackReceived, this, _1, _2, 0, self),
                         bind(m_errorCallback, INTEREST_TIMEOUT, "Timeout"));
}

void
SegmentFetcher::afterSegmentReceived(const Interest& origInterest,
                                     const Data& data, bool isSegmentZeroExpected,
                                     shared_ptr<SegmentFetcher> self)
{
  m_validator->validate(data,
                        bind(&SegmentFetcher::afterValidationSuccess, this, _1,
                             isSegmentZeroExpected, origInterest, self),
                        bind(&SegmentFetcher::afterValidationFailure, this, _1));

}

void
SegmentFetcher::afterValidationSuccess(const shared_ptr<const Data> data,
                                       bool isSegmentZeroExpected,
                                       const Interest& origInterest,
                                       shared_ptr<SegmentFetcher> self)
{
  name::Component currentSegment = data->getName().get(-1);

  if (currentSegment.isSegment()) {
    if (isSegmentZeroExpected && currentSegment.toSegment() != 0) {
      fetchNextSegment(origInterest, data->getName(), 0, self);
    }
    else {
      m_buffer->write(reinterpret_cast<const char*>(data->getContent().value()),
                      data->getContent().value_size());

      const name::Component& finalBlockId = data->getMetaInfo().getFinalBlockId();
      if (finalBlockId.empty() || (finalBlockId > currentSegment)) {
        fetchNextSegment(origInterest, data->getName(), currentSegment.toSegment() + 1, self);
      }
      else {
        return m_completeCallback(m_buffer->buf());
      }
    }
  }
  else {
    m_errorCallback(DATA_HAS_NO_SEGMENT, "Data Name has no segment number.");
  }
}

void
SegmentFetcher::afterValidationFailure(const shared_ptr<const Data> data)
{
  return m_errorCallback(SEGMENT_VALIDATION_FAIL, "Segment validation fail");
}


void
SegmentFetcher::afterNackReceived(const Interest& origInterest, const lp::Nack& nack,
                                  uint32_t reExpressCount, shared_ptr<SegmentFetcher> self)
{
  if (reExpressCount >= MAX_INTEREST_REEXPRESS) {
    m_errorCallback(NACK_ERROR, "Nack Error");
  }
  else {
    switch (nack.getReason()) {
      case lp::NackReason::DUPLICATE:
        reExpressInterest(origInterest, reExpressCount, self);
        break;
      case lp::NackReason::CONGESTION:
        m_scheduler.scheduleEvent(time::milliseconds(static_cast<uint32_t>(pow(2, reExpressCount + 1))),
                                  bind(&SegmentFetcher::reExpressInterest, this,
                                       origInterest, reExpressCount, self));
        break;
      default:
        m_errorCallback(NACK_ERROR, "Nack Error");
        break;
    }
  }
}

void
SegmentFetcher::reExpressInterest(Interest interest, uint32_t reExpressCount,
                                  shared_ptr<SegmentFetcher> self)
{
  interest.refreshNonce();
  BOOST_ASSERT(interest.hasNonce());

  bool isSegmentZeroExpected = true;
  if (!interest.getName().empty()) {
    name::Component lastComponent = interest.getName().get(-1);
    isSegmentZeroExpected = !lastComponent.isSegment();
  }

  m_face.expressInterest(interest,
                         bind(&SegmentFetcher::afterSegmentReceived, this, _1, _2,
                              isSegmentZeroExpected, self),
                         bind(&SegmentFetcher::afterNackReceived, this, _1, _2,
                              ++reExpressCount, self),
                         bind(m_errorCallback, INTEREST_TIMEOUT, "Timeout"));
}

} // namespace util
} // namespace ndn
