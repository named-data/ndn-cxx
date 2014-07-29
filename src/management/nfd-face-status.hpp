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

#ifndef NDN_MANAGEMENT_NFD_FACE_STATUS_HPP
#define NDN_MANAGEMENT_NFD_FACE_STATUS_HPP

// This include must be kept as the first one, to ensure nfd-face-flags.hpp compiles on its own.
#include "nfd-face-flags.hpp"

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"
#include "../util/time.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents Face status
 * \sa http://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Dataset
 */
class FaceStatus : public FaceFlagsTraits<FaceStatus>
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

  FaceStatus();

  explicit
  FaceStatus(const Block& block)
  {
    this->wireDecode(block);
  }

  /** \brief prepend FaceStatus to the encoder
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  /** \brief encode FaceStatus
   */
  const Block&
  wireEncode() const;

  /** \brief decode FaceStatus
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  FaceStatus&
  setFaceId(uint64_t faceId)
  {
    m_wire.reset();
    m_faceId = faceId;
    return *this;
  }

  const std::string&
  getRemoteUri() const
  {
    return m_remoteUri;
  }

  FaceStatus&
  setRemoteUri(const std::string& remoteUri)
  {
    m_wire.reset();
    m_remoteUri = remoteUri;
    return *this;
  }

  const std::string&
  getLocalUri() const
  {
    return m_localUri;
  }

  FaceStatus&
  setLocalUri(const std::string& localUri)
  {
    m_wire.reset();
    m_localUri = localUri;
    return *this;
  }

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
  setExpirationPeriod(const time::milliseconds& expirationPeriod)
  {
    m_expirationPeriod = expirationPeriod;
    m_hasExpirationPeriod = true;
    return *this;
  }

  uint64_t
  getFlags() const
  {
    return m_flags;
  }

  FaceStatus&
  setFlags(uint64_t flags)
  {
    m_wire.reset();
    m_flags = flags;
    return *this;
  }

  uint64_t
  getNInInterests() const
  {
    return m_nInInterests;
  }

  FaceStatus&
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

  FaceStatus&
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

  FaceStatus&
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

  FaceStatus&
  setNOutDatas(uint64_t nOutDatas)
  {
    m_wire.reset();
    m_nOutDatas = nOutDatas;
    return *this;
  }

  uint64_t
  getNInBytes() const
  {
    return m_nInBytes;
  }

  FaceStatus&
  setNInBytes(uint64_t nInBytes)
  {
    m_wire.reset();
    m_nInBytes = nInBytes;
    return *this;
  }

  uint64_t
  getNOutBytes() const
  {
    return m_nOutBytes;
  }

  FaceStatus&
  setNOutBytes(uint64_t nOutBytes)
  {
    m_wire.reset();
    m_nOutBytes = nOutBytes;
    return *this;
  }

private:
  uint64_t m_faceId;
  std::string m_remoteUri;
  std::string m_localUri;
  time::milliseconds m_expirationPeriod;
  bool m_hasExpirationPeriod;
  uint64_t m_flags;
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
