/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/util/segmenter.hpp"

#include <boost/iostreams/read.hpp>

namespace ndn {

Segmenter::Segmenter(KeyChain& keyChain, const security::SigningInfo& signingInfo)
  : m_keyChain(keyChain)
  , m_signingInfo(signingInfo)
{
}

std::vector<std::shared_ptr<Data>>
Segmenter::segment(span<const uint8_t> buffer, const Name& dataName, size_t maxSegmentSize,
                   time::milliseconds freshnessPeriod, uint32_t contentType)
{
  if (maxSegmentSize == 0) {
    NDN_THROW(std::invalid_argument("maxSegmentSize must be greater than 0"));
  }

  // minimum of one (possibly empty) segment
  const uint64_t numSegments = 1 + (buffer.size() - !buffer.empty()) / maxSegmentSize;
  const auto finalBlockId = name::Component::fromSegment(numSegments - 1);

  std::vector<std::shared_ptr<Data>> segments;
  segments.reserve(numSegments);

  do {
    auto segLen = std::min(buffer.size(), maxSegmentSize);

    auto data = std::make_shared<Data>();
    data->setName(Name(dataName).appendSegment(segments.size()));
    data->setContentType(contentType);
    data->setFreshnessPeriod(freshnessPeriod);
    data->setFinalBlock(finalBlockId);
    data->setContent(buffer.first(segLen));

    m_keyChain.sign(*data, m_signingInfo);
    segments.push_back(std::move(data));

    buffer = buffer.subspan(segLen);
  } while (!buffer.empty());

  BOOST_ASSERT(segments.size() == numSegments);
  return segments;
}

std::vector<std::shared_ptr<Data>>
Segmenter::segment(std::istream& input, const Name& dataName, size_t maxSegmentSize,
                   time::milliseconds freshnessPeriod, uint32_t contentType)
{
  if (maxSegmentSize == 0) {
    NDN_THROW(std::invalid_argument("maxSegmentSize must be greater than 0"));
  }

  std::vector<std::shared_ptr<Data>> segments;

  while (true) {
    auto buffer = std::make_shared<Buffer>(maxSegmentSize);
    auto n = boost::iostreams::read(input, buffer->get<char>(), buffer->size());
    if (n < 0) { // EOF
      break;
    }
    buffer->resize(n);

    auto data = std::make_shared<Data>();
    data->setName(Name(dataName).appendSegment(segments.size()));
    data->setContentType(contentType);
    data->setFreshnessPeriod(freshnessPeriod);
    data->setContent(std::move(buffer));

    segments.push_back(std::move(data));
  }

  // ensure we return at least one (empty) segment
  if (segments.empty()) {
    auto data = std::make_shared<Data>();
    data->setName(Name(dataName).appendSegment(0));
    data->setContentType(contentType);
    data->setFreshnessPeriod(freshnessPeriod);
    segments.push_back(std::move(data));
  }

  // add the FinalBlockId to each packet and sign
  const auto finalBlockId = name::Component::fromSegment(segments.size() - 1);
  for (const auto& data : segments) {
    data->setFinalBlock(finalBlockId);
    m_keyChain.sign(*data, m_signingInfo);
  }

  return segments;
}

} // namespace ndn
