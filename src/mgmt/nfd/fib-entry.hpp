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

#ifndef NDN_MGMT_NFD_FIB_ENTRY_HPP
#define NDN_MGMT_NFD_FIB_ENTRY_HPP

#include "../../encoding/block.hpp"
#include "../../name.hpp"

namespace ndn {
namespace nfd {

/** \ingroup management
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#FIB-Dataset
 */
class NextHopRecord
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

  NextHopRecord();

  explicit
  NextHopRecord(const Block& block);

  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  NextHopRecord&
  setFaceId(uint64_t faceId);

  uint64_t
  getCost() const
  {
    return m_cost;
  }

  NextHopRecord&
  setCost(uint64_t cost);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& block);

private:
  uint64_t m_faceId;
  uint64_t m_cost;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(NextHopRecord);

bool
operator==(const NextHopRecord& a, const NextHopRecord& b);

inline bool
operator!=(const NextHopRecord& a, const NextHopRecord& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const NextHopRecord& nh);


/** \ingroup management
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/FibMgmt#FIB-Dataset
 */
class FibEntry
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

  FibEntry();

  explicit
  FibEntry(const Block& block);

  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  FibEntry&
  setPrefix(const Name& prefix);

  const std::vector<NextHopRecord>&
  getNextHopRecords() const
  {
    return m_nextHopRecords;
  }

  template<typename InputIt>
  FibEntry&
  setNextHopRecords(InputIt first, InputIt last)
  {
    m_nextHopRecords.assign(first, last);
    m_wire.reset();
    return *this;
  }

  FibEntry&
  addNextHopRecord(const NextHopRecord& nh);

  FibEntry&
  clearNextHopRecords();

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& block);

private:
  Name m_prefix;
  std::vector<NextHopRecord> m_nextHopRecords;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(FibEntry);

bool
operator==(const FibEntry& a, const FibEntry& b);

inline bool
operator!=(const FibEntry& a, const FibEntry& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const FibEntry& entry);

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_FIB_ENTRY_HPP
