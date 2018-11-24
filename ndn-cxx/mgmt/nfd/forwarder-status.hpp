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

#ifndef NDN_MGMT_NFD_FORWARDER_STATUS_HPP
#define NDN_MGMT_NFD_FORWARDER_STATUS_HPP

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/util/time.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents NFD General Status dataset
 * \sa https://redmine.named-data.net/projects/nfd/wiki/ForwarderStatus#General-Status-Dataset
 */
class ForwarderStatus
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  ForwarderStatus();

  explicit
  ForwarderStatus(const Block& payload);

  /** \brief prepend ForwarderStatus as a Content block to the encoder
   *
   *  The outermost Content element isn't part of ForwarderStatus structure.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** \brief encode ForwarderStatus as a Content block
   *
   *  The outermost Content element isn't part of ForwarderStatus structure.
   */
  const Block&
  wireEncode() const;

  /** \brief decode ForwarderStatus from a Content block
   *
   *  The outermost Content element isn't part of ForwarderStatus structure.
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

  uint64_t
  getNNameTreeEntries() const
  {
    return m_nNameTreeEntries;
  }

  ForwarderStatus&
  setNNameTreeEntries(uint64_t nNameTreeEntries);

  uint64_t
  getNFibEntries() const
  {
    return m_nFibEntries;
  }

  ForwarderStatus&
  setNFibEntries(uint64_t nFibEntries);

  uint64_t
  getNPitEntries() const
  {
    return m_nPitEntries;
  }

  ForwarderStatus&
  setNPitEntries(uint64_t nPitEntries);

  uint64_t
  getNMeasurementsEntries() const
  {
    return m_nMeasurementsEntries;
  }

  ForwarderStatus&
  setNMeasurementsEntries(uint64_t nMeasurementsEntries);

  uint64_t
  getNCsEntries() const
  {
    return m_nCsEntries;
  }

  ForwarderStatus&
  setNCsEntries(uint64_t nCsEntries);

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  ForwarderStatus&
  setNInInterests(uint64_t nInInterests);

  uint64_t
  getNInData() const
  {
    return m_nInData;
  }

  ForwarderStatus&
  setNInData(uint64_t nInData);

  uint64_t
  getNInNacks() const
  {
    return m_nInNacks;
  }

  ForwarderStatus&
  setNInNacks(uint64_t nInNacks);

  uint64_t
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  ForwarderStatus&
  setNOutInterests(uint64_t nOutInterests);

  uint64_t
  getNOutData() const
  {
    return m_nOutData;
  }

  ForwarderStatus&
  setNOutData(uint64_t nOutData);

  uint64_t
  getNOutNacks() const
  {
    return m_nOutNacks;
  }

  ForwarderStatus&
  setNOutNacks(uint64_t nOutNacks);

  uint64_t
  getNSatisfiedInterests() const
  {
    return m_nSatisfiedInterests;
  }

  ForwarderStatus&
  setNSatisfiedInterests(uint64_t nSatisfiedInterests);

  uint64_t
  getNUnsatisfiedInterests() const
  {
    return m_nUnsatisfiedInterests;
  }

  ForwarderStatus&
  setNUnsatisfiedInterests(uint64_t nUnsatisfiedInterests);

private:
  std::string m_nfdVersion;
  time::system_clock::TimePoint m_startTimestamp;
  time::system_clock::TimePoint m_currentTimestamp;
  uint64_t m_nNameTreeEntries;
  uint64_t m_nFibEntries;
  uint64_t m_nPitEntries;
  uint64_t m_nMeasurementsEntries;
  uint64_t m_nCsEntries;
  uint64_t m_nInInterests;
  uint64_t m_nInData;
  uint64_t m_nInNacks;
  uint64_t m_nOutInterests;
  uint64_t m_nOutData;
  uint64_t m_nOutNacks;
  uint64_t m_nSatisfiedInterests;
  uint64_t m_nUnsatisfiedInterests;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(ForwarderStatus);

bool
operator==(const ForwarderStatus& a, const ForwarderStatus& b);

inline bool
operator!=(const ForwarderStatus& a, const ForwarderStatus& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const ForwarderStatus& status);

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_FORWARDER_STATUS_HPP
