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

#include "segment-fetcher.hpp"

#include "../encoding/buffer-stream.hpp"

namespace ndn {
namespace util {

SegmentFetcher::SegmentFetcher(Face& face,
                               const VerifySegment& verifySegment,
                               const CompleteCallback& completeCallback,
                               const ErrorCallback& errorCallback)
  : m_face(face)
  , m_verifySegment(verifySegment)
  , m_completeCallback(completeCallback)
  , m_errorCallback(errorCallback)
  , m_buffer(make_shared<OBufferStream>())
{
}

void
SegmentFetcher::fetch(Face& face,
                      const Interest& baseInterest,
                      const VerifySegment& verifySegment,
                      const CompleteCallback& completeCallback,
                      const ErrorCallback& errorCallback)
{
  shared_ptr<SegmentFetcher> fetcher =
    shared_ptr<SegmentFetcher>(new SegmentFetcher(face, verifySegment,
                                                  completeCallback, errorCallback));

  fetcher->fetchFirstSegment(baseInterest, fetcher);
}

void
SegmentFetcher::fetchFirstSegment(const Interest& baseInterest,
                                  const shared_ptr<SegmentFetcher>& self)
{
  Interest interest(baseInterest);
  interest.setChildSelector(1);
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&SegmentFetcher::onSegmentReceived, this, _1, _2, true, self),
                         bind(m_errorCallback, INTEREST_TIMEOUT, "Timeout"));
}

void
SegmentFetcher::fetchNextSegment(const Interest& origInterest, const Name& dataName,
                                 uint64_t segmentNo,
                                 const shared_ptr<SegmentFetcher>& self)
{
  Interest interest(origInterest); // to preserve any special selectors
  interest.refreshNonce();
  interest.setChildSelector(0);
  interest.setMustBeFresh(false);
  interest.setName(dataName.getPrefix(-1).appendSegment(segmentNo));
  m_face.expressInterest(interest,
                         bind(&SegmentFetcher::onSegmentReceived, this, _1, _2, false, self),
                         bind(m_errorCallback, INTEREST_TIMEOUT, "Timeout"));
}

void
SegmentFetcher::onSegmentReceived(const Interest& origInterest,
                                  const Data& data, bool isSegmentZeroExpected,
                                  const shared_ptr<SegmentFetcher>& self)
{
  if (!m_verifySegment(data)) {
    return m_errorCallback(SEGMENT_VERIFICATION_FAIL, "Segment validation fail");
  }

  try {
    uint64_t currentSegment = data.getName().get(-1).toSegment();

    if (isSegmentZeroExpected && currentSegment != 0) {
      fetchNextSegment(origInterest, data.getName(), 0, self);
    }
    else {
      m_buffer->write(reinterpret_cast<const char*>(data.getContent().value()),
                      data.getContent().value_size());

      const name::Component& finalBlockId = data.getMetaInfo().getFinalBlockId();
      if (finalBlockId.empty() ||
          finalBlockId.toSegment() > currentSegment)
        {
          fetchNextSegment(origInterest, data.getName(), currentSegment + 1, self);
        }
      else {
        return m_completeCallback(m_buffer->buf());
      }
    }
  }
  catch (const tlv::Error& e) {
    m_errorCallback(DATA_HAS_NO_SEGMENT, std::string("Error while decoding segment: ") + e.what());
  }
}

} // util
} // ndn
