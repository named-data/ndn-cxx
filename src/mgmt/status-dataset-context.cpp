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

#include "status-dataset-context.hpp"

namespace ndn {
namespace mgmt {

const time::milliseconds DEFAULT_STATUS_DATASET_FRESHNESS_PERIOD = 1_s;

const Name&
StatusDatasetContext::getPrefix() const
{
  return m_prefix;
}

StatusDatasetContext&
StatusDatasetContext::setPrefix(const Name& prefix)
{
  if (!m_interest.getName().isPrefixOf(prefix)) {
    BOOST_THROW_EXCEPTION(std::invalid_argument("prefix does not start with Interest Name"));
  }

  if (m_state != State::INITIAL) {
    BOOST_THROW_EXCEPTION(std::domain_error("state is not in INITIAL"));
  }

  m_prefix = prefix;

  if (!m_prefix[-1].isVersion()) {
    m_prefix.appendVersion();
  }

  return *this;
}

const time::milliseconds&
StatusDatasetContext::getExpiry() const
{
  return m_expiry;
}

StatusDatasetContext&
StatusDatasetContext::setExpiry(const time::milliseconds& expiry)
{
  m_expiry = expiry;
  return *this;
}

void
StatusDatasetContext::append(const Block& block)
{
  if (m_state == State::FINALIZED) {
    BOOST_THROW_EXCEPTION(std::domain_error("state is in FINALIZED"));
  }

  m_state = State::RESPONDED;

  size_t nBytesLeft = block.size();
  while (nBytesLeft > 0) {
    size_t nBytesAppend = std::min(nBytesLeft,
                                   (ndn::MAX_NDN_PACKET_SIZE >> 1) - m_buffer->size());
    m_buffer->appendByteArray(block.wire() + (block.size() - nBytesLeft), nBytesAppend);
    nBytesLeft -= nBytesAppend;

    if (nBytesLeft > 0) {
      m_dataSender(Name(m_prefix).appendSegment(m_segmentNo++),
                   makeBinaryBlock(tlv::Content, m_buffer->buf(), m_buffer->size()),
                   m_expiry, false);

      m_buffer = std::make_shared<EncodingBuffer>();
    }
  }
}

void
StatusDatasetContext::end()
{
  if (m_state == State::FINALIZED) {
    BOOST_THROW_EXCEPTION(std::domain_error("state is in FINALIZED"));
  }

  m_state = State::FINALIZED;

  m_dataSender(Name(m_prefix).appendSegment(m_segmentNo),
               makeBinaryBlock(tlv::Content, m_buffer->buf(), m_buffer->size()),
               m_expiry, true);
}

void
StatusDatasetContext::reject(const ControlResponse& resp /*= a ControlResponse with 400*/)
{
  if (m_state != State::INITIAL) {
    BOOST_THROW_EXCEPTION(std::domain_error("state is in REPONSED or FINALIZED"));
  }

  m_state = State::FINALIZED;
  m_nackSender(resp);
}

StatusDatasetContext::StatusDatasetContext(const Interest& interest,
                                           const DataSender& dataSender,
                                           const NackSender& nackSender)
  : m_interest(interest)
  , m_dataSender(dataSender)
  , m_nackSender(nackSender)
  , m_expiry(DEFAULT_STATUS_DATASET_FRESHNESS_PERIOD)
  , m_buffer(make_shared<EncodingBuffer>())
  , m_segmentNo(0)
  , m_state(State::INITIAL)
{
  setPrefix(interest.getName());
}

} // namespace mgmt
} // namespace ndn
