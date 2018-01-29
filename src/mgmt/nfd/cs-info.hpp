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

#ifndef NDN_MGMT_NFD_CS_INFO_HPP
#define NDN_MGMT_NFD_CS_INFO_HPP

#include "../../encoding/block.hpp"
#include "../../encoding/nfd-constants.hpp"

#include <bitset>

namespace ndn {
namespace nfd {

/** \ingroup management
 *  \brief represents the CS Information dataset
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/CsMgmt#CS-Information-Dataset
 */
class CsInfo
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

  CsInfo();

  explicit
  CsInfo(const Block& block);

  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

  /** \brief get CS capacity (in number of packets)
   */
  uint64_t
  getCapacity() const
  {
    return m_capacity;
  }

  CsInfo&
  setCapacity(uint64_t capacity);

  /** \brief get CS_ENABLE_ADMIT flag
   */
  bool
  getEnableAdmit() const
  {
    return m_flags.test(BIT_CS_ENABLE_ADMIT);
  }

  CsInfo&
  setEnableAdmit(bool enableAdmit);

  /** \brief get CS_ENABLE_SERVE flag
   */
  bool
  getEnableServe() const
  {
    return m_flags.test(BIT_CS_ENABLE_SERVE);
  }

  CsInfo&
  setEnableServe(bool enableServe);

  /** \brief get number of stored CS entries
   */
  uint64_t
  getNEntries() const
  {
    return m_nEntries;
  }

  CsInfo&
  setNEntries(uint64_t nEntries);

  /** \brief get number of CS lookup hits since NFD starts
   */
  uint64_t
  getNHits() const
  {
    return m_nHits;
  }

  CsInfo&
  setNHits(uint64_t nHits);

  /** \brief get number of CS lookup misses since NFD starts
   */
  uint64_t
  getNMisses() const
  {
    return m_nMisses;
  }

  CsInfo&
  setNMisses(uint64_t nMisses);

private:
  using FlagsBitSet = std::bitset<2>;

  uint64_t m_capacity;
  FlagsBitSet m_flags;
  uint64_t m_nEntries;
  uint64_t m_nHits;
  uint64_t m_nMisses;
  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(CsInfo);

bool
operator==(const CsInfo& a, const CsInfo& b);

inline bool
operator!=(const CsInfo& a, const CsInfo& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const CsInfo& csi);

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_CS_INFO_HPP
