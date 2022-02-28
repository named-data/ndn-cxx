/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/mgmt/status-dataset-context.hpp"

namespace ndn {
namespace mgmt {

const size_t MAX_PAYLOAD_LENGTH = MAX_NDN_PACKET_SIZE - 800;

StatusDatasetContext::StatusDatasetContext(const Interest& interest,
                                           DataSender dataSender, NackSender nackSender)
  : m_interest(interest)
  , m_dataSender(std::move(dataSender))
  , m_nackSender(std::move(nackSender))
{
  setPrefix(interest.getName());
  m_buffer.reserve(MAX_PAYLOAD_LENGTH);
}

StatusDatasetContext&
StatusDatasetContext::setPrefix(const Name& prefix)
{
  if (m_state != State::INITIAL) {
    NDN_THROW(std::logic_error("cannot call setPrefix() after append/end/reject"));
  }

  if (!m_interest.getName().isPrefixOf(prefix)) {
    NDN_THROW(std::invalid_argument("prefix must start with the Interest's name"));
  }

  if (prefix.at(-1).isSegment()) {
    NDN_THROW(std::invalid_argument("prefix must not contain a segment component"));
  }

  m_prefix = prefix;
  if (!m_prefix.at(-1).isVersion()) {
    m_prefix.appendVersion();
  }

  return *this;
}

void
StatusDatasetContext::append(span<const uint8_t> bytes)
{
  if (m_state == State::FINALIZED) {
    NDN_THROW(std::logic_error("cannot call append() on a finalized context"));
  }

  m_state = State::RESPONDED;

  while (!bytes.empty()) {
    if (m_buffer.size() == MAX_PAYLOAD_LENGTH) {
      m_dataSender(Name(m_prefix).appendSegment(m_segmentNo++),
                   makeBinaryBlock(tlv::Content, m_buffer), false);
      m_buffer.clear();
    }

    auto chunk = bytes.first(std::min(bytes.size(), MAX_PAYLOAD_LENGTH - m_buffer.size()));
    m_buffer.insert(m_buffer.end(), chunk.begin(), chunk.end());
    bytes = bytes.subspan(chunk.size());
  }
}

void
StatusDatasetContext::end()
{
  if (m_state == State::FINALIZED) {
    NDN_THROW(std::logic_error("cannot call end() on a finalized context"));
  }

  m_state = State::FINALIZED;

  BOOST_ASSERT(m_buffer.size() <= MAX_PAYLOAD_LENGTH);
  m_dataSender(Name(m_prefix).appendSegment(m_segmentNo),
               makeBinaryBlock(tlv::Content, m_buffer), true);
}

void
StatusDatasetContext::reject(const ControlResponse& resp)
{
  if (m_state != State::INITIAL) {
    NDN_THROW(std::logic_error("cannot call reject() after append/end"));
  }

  m_state = State::FINALIZED;
  m_nackSender(resp);
}

} // namespace mgmt
} // namespace ndn
