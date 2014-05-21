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

#ifndef NDN_MANAGEMENT_NFD_FORWARDER_STATUS_HPP
#define NDN_MANAGEMENT_NFD_FORWARDER_STATUS_HPP

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"

#include "../util/time.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents NFD Forwarder Status
 * \sa http://redmine.named-data.net/projects/nfd/wiki/ForwarderStatus
 */
class ForwarderStatus
{
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  ForwarderStatus();

  explicit
  ForwarderStatus(const Block& payload)
  {
    this->wireDecode(payload);
  }

  /** \brief prepend ForwarderStatus as a Content block to the encoder
   *
   *  The outermost Content element isn't part of ForwardStatus structure.
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  /** \brief encode ForwarderStatus as a Content block
   *
   *  The outermost Content element isn't part of ForwardStatus structure.
   */
  const Block&
  wireEncode() const;

  /** \brief decode ForwarderStatus from a Content block
   *
   *  The outermost Content element isn't part of ForwardStatus structure.
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  int
  getNfdVersion() const
  {
    return m_nfdVersion;
  }

  ForwarderStatus&
  setNfdVersion(int nfdVersion)
  {
    m_wire.reset();
    m_nfdVersion = nfdVersion;
    return *this;
  }

  const time::system_clock::TimePoint&
  getStartTimestamp() const
  {
    return m_startTimestamp;
  }

  ForwarderStatus&
  setStartTimestamp(const time::system_clock::TimePoint& startTimestamp)
  {
    m_wire.reset();
    m_startTimestamp = startTimestamp;
    return *this;
  }

  const time::system_clock::TimePoint&
  getCurrentTimestamp() const
  {
    return m_currentTimestamp;
  }

  ForwarderStatus&
  setCurrentTimestamp(const time::system_clock::TimePoint& currentTimestamp)
  {
    m_wire.reset();
    m_currentTimestamp = currentTimestamp;
    return *this;
  }

  size_t
  getNNameTreeEntries() const
  {
    return m_nNameTreeEntries;
  }

  ForwarderStatus&
  setNNameTreeEntries(size_t nNameTreeEntries)
  {
    m_wire.reset();
    m_nNameTreeEntries = nNameTreeEntries;
    return *this;
  }

  size_t
  getNFibEntries() const
  {
    return m_nFibEntries;
  }

  ForwarderStatus&
  setNFibEntries(size_t nFibEntries)
  {
    m_wire.reset();
    m_nFibEntries = nFibEntries;
    return *this;
  }

  size_t
  getNPitEntries() const
  {
    return m_nPitEntries;
  }

  ForwarderStatus&
  setNPitEntries(size_t nPitEntries)
  {
    m_wire.reset();
    m_nPitEntries = nPitEntries;
    return *this;
  }

  size_t
  getNMeasurementsEntries() const
  {
    return m_nMeasurementsEntries;
  }

  ForwarderStatus&
  setNMeasurementsEntries(size_t nMeasurementsEntries)
  {
    m_wire.reset();
    m_nMeasurementsEntries = nMeasurementsEntries;
    return *this;
  }

  size_t
  getNCsEntries() const
  {
    return m_nCsEntries;
  }

  ForwarderStatus&
  setNCsEntries(size_t nCsEntries)
  {
    m_wire.reset();
    m_nCsEntries = nCsEntries;
    return *this;
  }

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  ForwarderStatus&
  setNInInterests(uint64_t nInInterests)
  {
    m_wire.reset();
    m_nInInterests = nInInterests;
    return *this;
  }

  uint64_t
  getNInDatas() const
  {
    return m_nInDatas;
  }

  ForwarderStatus&
  setNInDatas(uint64_t nInDatas)
  {
    m_wire.reset();
    m_nInDatas = nInDatas;
    return *this;
  }

  uint64_t
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  ForwarderStatus&
  setNOutInterests(uint64_t nOutInterests)
  {
    m_wire.reset();
    m_nOutInterests = nOutInterests;
    return *this;
  }

  uint64_t
  getNOutDatas() const
  {
    return m_nOutDatas;
  }

  ForwarderStatus&
  setNOutDatas(uint64_t nOutDatas)
  {
    m_wire.reset();
    m_nOutDatas = nOutDatas;
    return *this;
  }

private:
  int m_nfdVersion;
  time::system_clock::TimePoint m_startTimestamp;
  time::system_clock::TimePoint m_currentTimestamp;
  size_t m_nNameTreeEntries;
  size_t m_nFibEntries;
  size_t m_nPitEntries;
  size_t m_nMeasurementsEntries;
  size_t m_nCsEntries;
  uint64_t m_nInInterests;
  uint64_t m_nInDatas;
  uint64_t m_nOutInterests;
  uint64_t m_nOutDatas;

  mutable Block m_wire;
};

inline
ForwarderStatus::ForwarderStatus()
  : m_nfdVersion(0)
  , m_startTimestamp(time::system_clock::TimePoint::min())
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

template<bool T>
inline size_t
ForwarderStatus::wireEncode(EncodingImpl<T>& encoder) const
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
  totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NfdVersion,
                                                m_nfdVersion);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(Tlv::Content);
  return totalLength;
}

inline const Block&
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

inline void
ForwarderStatus::wireDecode(const Block& block)
{
  if (block.type() != Tlv::Content) {
    throw Error("expecting Content block for Status payload");
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val = m_wire.elements_begin();

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NfdVersion) {
    m_nfdVersion = static_cast<int>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NfdVersion field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::StartTimestamp) {
    m_startTimestamp = time::fromUnixTimestamp(time::milliseconds(readNonNegativeInteger(*val)));
    ++val;
  }
  else {
    throw Error("missing required StartTimestamp field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::CurrentTimestamp) {
    m_currentTimestamp = time::fromUnixTimestamp(time::milliseconds(readNonNegativeInteger(*val)));
    ++val;
  }
  else {
    throw Error("missing required CurrentTimestamp field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NNameTreeEntries) {
    m_nNameTreeEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NNameTreeEntries field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NFibEntries) {
    m_nFibEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NFibEntries field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NPitEntries) {
    m_nPitEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NPitEntries field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NMeasurementsEntries) {
    m_nMeasurementsEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NMeasurementsEntries field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NCsEntries) {
    m_nCsEntries = static_cast<size_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NCsEntries field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInInterests) {
    m_nInInterests = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NInInterests field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NInDatas) {
    m_nInDatas = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NInDatas field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutInterests) {
    m_nOutInterests = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NOutInterests field");
  }

  if (val != m_wire.elements_end() && val->type() == tlv::nfd::NOutDatas) {
    m_nOutDatas = static_cast<uint64_t>(readNonNegativeInteger(*val));
    ++val;
  }
  else {
    throw Error("missing required NOutDatas field");
  }
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FORWARDER_STATUS_HPP
