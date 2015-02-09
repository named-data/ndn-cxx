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

#ifndef NDN_MANAGEMENT_NFD_FORWARDER_STATUS_HPP
#define NDN_MANAGEMENT_NFD_FORWARDER_STATUS_HPP

#include "../encoding/block.hpp"
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
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  ForwarderStatus();

  explicit
  ForwarderStatus(const Block& payload);

  /** \brief prepend ForwarderStatus as a Content block to the encoder
   *
   *  The outermost Content element isn't part of ForwardStatus structure.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

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
  const std::string&
  getNfdVersion() const
  {
    return m_nfdVersion;
  }

  ForwarderStatus&
  setNfdVersion(const std::string& nfdVersion);

  const time::system_clock::TimePoint&
  getStartTimestamp() const
  {
    return m_startTimestamp;
  }

  ForwarderStatus&
  setStartTimestamp(const time::system_clock::TimePoint& startTimestamp);

  const time::system_clock::TimePoint&
  getCurrentTimestamp() const
  {
    return m_currentTimestamp;
  }

  ForwarderStatus&
  setCurrentTimestamp(const time::system_clock::TimePoint& currentTimestamp);

  size_t
  getNNameTreeEntries() const
  {
    return m_nNameTreeEntries;
  }

  ForwarderStatus&
  setNNameTreeEntries(size_t nNameTreeEntries);

  size_t
  getNFibEntries() const
  {
    return m_nFibEntries;
  }

  ForwarderStatus&
  setNFibEntries(size_t nFibEntries);

  size_t
  getNPitEntries() const
  {
    return m_nPitEntries;
  }

  ForwarderStatus&
  setNPitEntries(size_t nPitEntries);

  size_t
  getNMeasurementsEntries() const
  {
    return m_nMeasurementsEntries;
  }

  ForwarderStatus&
  setNMeasurementsEntries(size_t nMeasurementsEntries);

  size_t
  getNCsEntries() const
  {
    return m_nCsEntries;
  }

  ForwarderStatus&
  setNCsEntries(size_t nCsEntries);

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  ForwarderStatus&
  setNInInterests(uint64_t nInInterests);

  uint64_t
  getNInDatas() const
  {
    return m_nInDatas;
  }

  ForwarderStatus&
  setNInDatas(uint64_t nInDatas);

  uint64_t
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  ForwarderStatus&
  setNOutInterests(uint64_t nOutInterests);

  uint64_t
  getNOutDatas() const
  {
    return m_nOutDatas;
  }

  ForwarderStatus&
  setNOutDatas(uint64_t nOutDatas);

private:
  std::string m_nfdVersion;
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

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FORWARDER_STATUS_HPP
