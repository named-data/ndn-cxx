/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "nfd-forwarder-status.hpp"
#include "encoding/tlv-nfd.hpp"
#include "encoding/block-helpers.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

//BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ForwarderStatus>));
BOOST_CONCEPT_ASSERT((WireEncodable<ForwarderStatus>));
BOOST_CONCEPT_ASSERT((WireDecodable<ForwarderStatus>));
static_assert(std::is_base_of<tlv::Error, ForwarderStatus::Error>::value,
              "ForwarderStatus::Error must inherit from tlv::Error");

ForwarderStatus::ForwarderStatus()
  : m_startTimestamp(time::system_clock::TimePoint::min())
  , m_currentTimestamp(time::system_clock::TimePoint::min())
  , m_nNameTreeEntries(0)
  , m_nFibEntries(0)
  , m_nPitEntries(0)
  , m_nMeasurementsEntries(0)
  , m_nCsEntries(0)
  , m_nInInterests(0)
  , m_nInDatas(0)
  , m_nOutInterests(0)
  , m_nOutDatas(0)
{
}

ForwarderStatus::ForwarderStatus(const Block& payload)
{
  this->wireDecode(payload);
}

template<encoding::Tag TAG>
size_t
ForwarderStatus::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutDatas,
                                                m_nOutDatas);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutInterests,
                                                m_nOutInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInDatas,
                                                m_nInDatas);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInInterests,
                                                m_nInInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NCsEntries,
                                                m_nCsEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NMeasurementsEntries,
                                                m_nMeasurementsEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NPitEntries,
                                                m_nPitEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NFibEntries,
                                                m_nFibEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NNameTreeEntries,
                                                m_nNameTreeEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::CurrentTimestamp,
                                                time::toUnixTimestamp(m_currentTimestamp).count());
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::StartTimestamp,
                                                time::toUnixTimestamp(m_startTimestamp).count());
  totalLength += encoder.prependByteArrayBlock(tlv::nfd::NfdVersion,
                                       reinterpret_cast<const uint8_t*>(m_nfdVersion.c_str()),
                                       m_nfdVersion.size());

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Content);
  return totalLength;
}

template size_t
ForwarderStatus::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>&) const;

template size_t
ForwarderStatus::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>&) const;

const Block&
ForwarderStatus::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
ForwarderStatus::wireDecode(const Block& block)
{
  if (block.type() != tlv::Content) {
    BOOST_THROW_EXCEPTION(Error("expecting Content block for Status payload"));
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NfdVersion) {
    m_nfdVersion.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NfdVersion field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::StartTimestamp) {
    m_startTimestamp = time::fromUnixTimestamp(time::milliseconds(readNonNegativeInteger(*val)));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required StartTimestamp field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::CurrentTimestamp) {
    m_currentTimestamp = time::fromUnixTimestamp(time::milliseconds(readNonNegativeInteger(*val)));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required CurrentTimestamp field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NNameTreeEntries) {
    m_nNameTreeEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NNameTreeEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NFibEntries) {
    m_nFibEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NFibEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NPitEntries) {
    m_nPitEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NPitEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NMeasurementsEntries) {
    m_nMeasurementsEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NMeasurementsEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NCsEntries) {
    m_nCsEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NCsEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInInterests) {
    m_nInInterests = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInDatas) {
    m_nInDatas = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInDatas field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutInterests) {
    m_nOutInterests = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutDatas) {
    m_nOutDatas = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutDatas field"));
  }
}

ForwarderStatus&
ForwarderStatus::setNfdVersion(const std::string& nfdVersion)
{
  m_wire.reset();
  m_nfdVersion = nfdVersion;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setStartTimestamp(const time::system_clock::TimePoint& startTimestamp)
{
  m_wire.reset();
  m_startTimestamp = startTimestamp;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setCurrentTimestamp(const time::system_clock::TimePoint& currentTimestamp)
{
  m_wire.reset();
  m_currentTimestamp = currentTimestamp;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNNameTreeEntries(size_t nNameTreeEntries)
{
  m_wire.reset();
  m_nNameTreeEntries = nNameTreeEntries;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNFibEntries(size_t nFibEntries)
{
  m_wire.reset();
  m_nFibEntries = nFibEntries;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNPitEntries(size_t nPitEntries)
{
  m_wire.reset();
  m_nPitEntries = nPitEntries;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNMeasurementsEntries(size_t nMeasurementsEntries)
{
  m_wire.reset();
  m_nMeasurementsEntries = nMeasurementsEntries;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNCsEntries(size_t nCsEntries)
{
  m_wire.reset();
  m_nCsEntries = nCsEntries;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNInInterests(uint64_t nInInterests)
{
  m_wire.reset();
  m_nInInterests = nInInterests;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNInDatas(uint64_t nInDatas)
{
  m_wire.reset();
  m_nInDatas = nInDatas;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNOutInterests(uint64_t nOutInterests)
{
  m_wire.reset();
  m_nOutInterests = nOutInterests;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNOutDatas(uint64_t nOutDatas)
{
  m_wire.reset();
  m_nOutDatas = nOutDatas;
  return *this;
}

} // namespace nfd
} // namespace ndn
