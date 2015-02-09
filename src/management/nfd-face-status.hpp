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

#ifndef NDN_MANAGEMENT_NFD_FACE_STATUS_HPP
#define NDN_MANAGEMENT_NFD_FACE_STATUS_HPP

#include "nfd-face-traits.hpp" // include this first, to ensure it compiles on its own.
#include "../encoding/block.hpp"
#include "../util/time.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents Face status
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceStatus : public FaceTraits<FaceStatus>
{
public:
  FaceStatus();

  explicit
  FaceStatus(const Block& block);

  /** \brief prepend FaceStatus to the encoder
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** \brief encode FaceStatus
   */
  const Block&
  wireEncode() const;

  /** \brief decode FaceStatus
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  bool
  hasExpirationPeriod() const
  {
    return m_hasExpirationPeriod;
  }

  const time::milliseconds&
  getExpirationPeriod() const
  {
    BOOST_ASSERT(m_hasExpirationPeriod);
    return m_expirationPeriod;
  }

  FaceStatus&
  setExpirationPeriod(const time::milliseconds& expirationPeriod);

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  FaceStatus&
  setNInInterests(uint64_t nInInterests);

  uint64_t
  getNInDatas() const
  {
    return m_nInDatas;
  }

  FaceStatus&
  setNInDatas(uint64_t nInDatas);

  uint64_t
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  FaceStatus&
  setNOutInterests(uint64_t nOutInterests);

  uint64_t
  getNOutDatas() const
  {
    return m_nOutDatas;
  }

  FaceStatus&
  setNOutDatas(uint64_t nOutDatas);

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

protected:
  void
  wireReset() const;

private:
  time::milliseconds m_expirationPeriod;
  bool m_hasExpirationPeriod;
  uint64_t m_nInInterests;
  uint64_t m_nInDatas;
  uint64_t m_nOutInterests;
  uint64_t m_nOutDatas;
  uint64_t m_nInBytes;
  uint64_t m_nOutBytes;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const FaceStatus& status);

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_STATUS_HPP
