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

#ifndef NDN_CXX_MGMT_NFD_FACE_STATUS_HPP
#define NDN_CXX_MGMT_NFD_FACE_STATUS_HPP

#include "ndn-cxx/mgmt/nfd/face-traits.hpp"
#include "ndn-cxx/util/time.hpp"

#include <optional>

namespace ndn::nfd {

/**
 * \ingroup management
 * \brief Represents an item in NFD %Face dataset.
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceStatus : public FaceTraits<FaceStatus>
{
public:
  FaceStatus();

  explicit
  FaceStatus(const Block& block);

  /** \brief Prepend FaceStatus to the encoder.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** \brief Encode FaceStatus.
   */
  const Block&
  wireEncode() const;

  /** \brief Decode FaceStatus.
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  bool
  hasExpirationPeriod() const
  {
    return !!m_expirationPeriod;
  }

  time::milliseconds
  getExpirationPeriod() const
  {
    BOOST_ASSERT(hasExpirationPeriod());
    return *m_expirationPeriod;
  }

  FaceStatus&
  setExpirationPeriod(time::milliseconds expirationPeriod);

  FaceStatus&
  unsetExpirationPeriod();

  bool
  hasBaseCongestionMarkingInterval() const
  {
    return !!m_baseCongestionMarkingInterval;
  }

  time::nanoseconds
  getBaseCongestionMarkingInterval() const
  {
    BOOST_ASSERT(hasBaseCongestionMarkingInterval());
    return *m_baseCongestionMarkingInterval;
  }

  FaceStatus&
  setBaseCongestionMarkingInterval(time::nanoseconds interval);

  FaceStatus&
  unsetBaseCongestionMarkingInterval();

  bool
  hasDefaultCongestionThreshold() const
  {
    return !!m_defaultCongestionThreshold;
  }

  /** \brief Get default congestion threshold (measured in bytes).
   */
  uint64_t
  getDefaultCongestionThreshold() const
  {
    BOOST_ASSERT(hasDefaultCongestionThreshold());
    return *m_defaultCongestionThreshold;
  }

  /** \brief Set default congestion threshold (measured in bytes).
   */
  FaceStatus&
  setDefaultCongestionThreshold(uint64_t threshold);

  FaceStatus&
  unsetDefaultCongestionThreshold();

  bool
  hasMtu() const
  {
    return !!m_mtu;
  }

  /** \brief Get MTU (measured in bytes).
   *
   *  This value is capped at MAX_NDN_PACKET_SIZE, even if the MTU of the face is unlimited.
   */
  uint64_t
  getMtu() const
  {
    BOOST_ASSERT(hasMtu());
    return *m_mtu;
  }

  /** \brief Set MTU (measured in bytes).
   *
   *  This value is capped at MAX_NDN_PACKET_SIZE, even if the MTU of the face is unlimited.
   */
  FaceStatus&
  setMtu(uint64_t mtu);

  FaceStatus&
  unsetMtu();

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  FaceStatus&
  setNInInterests(uint64_t nInInterests);

  uint64_t
  getNInData() const
  {
    return m_nInData;
  }

  FaceStatus&
  setNInData(uint64_t nInData);

  uint64_t
  getNInNacks() const
  {
    return m_nInNacks;
  }

  FaceStatus&
  setNInNacks(uint64_t nInNacks);

  uint64_t
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  FaceStatus&
  setNOutInterests(uint64_t nOutInterests);

  uint64_t
  getNOutData() const
  {
    return m_nOutData;
  }

  FaceStatus&
  setNOutData(uint64_t nOutData);

  uint64_t
  getNOutNacks() const
  {
    return m_nOutNacks;
  }

  FaceStatus&
  setNOutNacks(uint64_t nOutNacks);

  uint64_t
  getNInBytes() const
  {
    return m_nInBytes;
  }

  FaceStatus&
  setNInBytes(uint64_t nInBytes);

  uint64_t
  getNOutBytes() const
  {
    return m_nOutBytes;
  }

  FaceStatus&
  setNOutBytes(uint64_t nOutBytes);

private:
  std::optional<time::milliseconds> m_expirationPeriod;
  std::optional<time::nanoseconds> m_baseCongestionMarkingInterval;
  std::optional<uint64_t> m_defaultCongestionThreshold;
  std::optional<uint64_t> m_mtu;
  uint64_t m_nInInterests = 0;
  uint64_t m_nInData = 0;
  uint64_t m_nInNacks = 0;
  uint64_t m_nOutInterests = 0;
  uint64_t m_nOutData = 0;
  uint64_t m_nOutNacks = 0;
  uint64_t m_nInBytes = 0;
  uint64_t m_nOutBytes = 0;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(FaceStatus);

bool
operator==(const FaceStatus& a, const FaceStatus& b);

inline bool
operator!=(const FaceStatus& a, const FaceStatus& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const FaceStatus& status);

} // namespace ndn::nfd

#endif // NDN_CXX_MGMT_NFD_FACE_STATUS_HPP
