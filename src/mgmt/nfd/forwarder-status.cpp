/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "forwarder-status.hpp"
#include "encoding/block-helpers.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-nfd.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace nfd {

BOOST_CONCEPT_ASSERT((StatusDatasetItem<ForwarderStatus>));

ForwarderStatus::ForwarderStatus()
  : m_nNameTreeEntries(0)
  , m_nFibEntries(0)
  , m_nPitEntries(0)
  , m_nMeasurementsEntries(0)
  , m_nCsEntries(0)
  , m_nInInterests(0)
  , m_nInData(0)
  , m_nInNacks(0)
  , m_nOutInterests(0)
  , m_nOutData(0)
  , m_nOutNacks(0)
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

  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutNacks, m_nOutNacks);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutData, m_nOutData);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutInterests, m_nOutInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInNacks, m_nInNacks);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInData, m_nInData);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInInterests, m_nInInterests);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NCsEntries, m_nCsEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NMeasurementsEntries, m_nMeasurementsEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NPitEntries, m_nPitEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NFibEntries, m_nFibEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NNameTreeEntries, m_nNameTreeEntries);
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::CurrentTimestamp,
                                                time::toUnixTimestamp(m_currentTimestamp).count());
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::StartTimestamp,
                                                time::toUnixTimestamp(m_startTimestamp).count());
  totalLength += prependStringBlock(encoder, tlv::nfd::NfdVersion, m_nfdVersion);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Content);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(ForwarderStatus);

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
    m_nfdVersion = readString(*val);
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
    m_nNameTreeEntries = readNonNegativeIntegerAs<size_t>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NNameTreeEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NFibEntries) {
    m_nFibEntries = readNonNegativeIntegerAs<size_t>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NFibEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NPitEntries) {
    m_nPitEntries = readNonNegativeIntegerAs<size_t>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NPitEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NMeasurementsEntries) {
    m_nMeasurementsEntries = readNonNegativeIntegerAs<size_t>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NMeasurementsEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NCsEntries) {
    m_nCsEntries = readNonNegativeIntegerAs<size_t>(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NCsEntries field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInInterests) {
    m_nInInterests = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInData) {
    m_nInData = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInData field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInNacks) {
    m_nInNacks = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NInNacks field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutInterests) {
    m_nOutInterests = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutInterests field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutData) {
    m_nOutData = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutData field"));
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutNacks) {
    m_nOutNacks = readNonNegativeInteger(*val);
    ++val;
  }
  else {
    BOOST_THROW_EXCEPTION(Error("missing required NOutNacks field"));
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
ForwarderStatus::setNInData(uint64_t nInData)
{
  m_wire.reset();
  m_nInData = nInData;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNInNacks(uint64_t nInNacks)
{
  m_wire.reset();
  m_nInNacks = nInNacks;
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
ForwarderStatus::setNOutData(uint64_t nOutData)
{
  m_wire.reset();
  m_nOutData = nOutData;
  return *this;
}

ForwarderStatus&
ForwarderStatus::setNOutNacks(uint64_t nOutNacks)
{
  m_wire.reset();
  m_nOutNacks = nOutNacks;
  return *this;
}

bool
operator==(const ForwarderStatus& a, const ForwarderStatus& b)
{
  return a.getNfdVersion() == b.getNfdVersion() &&
      a.getStartTimestamp() == b.getStartTimestamp() &&
      a.getCurrentTimestamp() == b.getCurrentTimestamp() &&
      a.getNNameTreeEntries() == b.getNNameTreeEntries() &&
      a.getNFibEntries() == b.getNFibEntries() &&
      a.getNPitEntries() == b.getNPitEntries() &&
      a.getNMeasurementsEntries() == b.getNMeasurementsEntries() &&
      a.getNCsEntries() == b.getNCsEntries() &&
      a.getNInInterests() == b.getNInInterests() &&
      a.getNInData() == b.getNInData() &&
      a.getNInNacks() == b.getNInNacks() &&
      a.getNOutInterests() == b.getNOutInterests() &&
      a.getNOutData() == b.getNOutData() &&
      a.getNOutNacks() == b.getNOutNacks();
}

std::ostream&
operator<<(std::ostream& os, const ForwarderStatus& status)
{
  os << "GeneralStatus(NfdVersion: " << status.getNfdVersion() << ",\n"
     << "              StartTimestamp: " << status.getStartTimestamp() << ",\n"
     << "              CurrentTimestamp: " << status.getCurrentTimestamp() << ",\n"
     << "              Counters: {NameTreeEntries: " << status.getNNameTreeEntries() << ",\n"
     << "                         FibEntries: " << status.getNFibEntries() << ",\n"
     << "                         PitEntries: " << status.getNPitEntries() << ",\n"
     << "                         MeasurementsEntries: " << status.getNMeasurementsEntries() << ",\n"
     << "                         CsEntries: " << status.getNCsEntries() << ",\n"
     << "                         Interests: {in: " << status.getNInInterests() << ", "
     << "out: " << status.getNOutInterests() << "},\n"
     << "                         Data: {in: " << status.getNInData() << ", "
     << "out: " << status.getNOutData() << "},\n"
     << "                         Nacks: {in: " << status.getNInNacks() << ", "
     << "out: " << status.getNOutNacks() << "}}\n"
     << "              )";

  return os;
}

} // namespace nfd
} // namespace ndn
