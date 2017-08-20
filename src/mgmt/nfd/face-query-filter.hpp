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

#ifndef NDN_MGMT_NFD_FACE_QUERY_FILTER_HPP
#define NDN_MGMT_NFD_FACE_QUERY_FILTER_HPP

#include "../../encoding/block.hpp"
#include "../../encoding/nfd-constants.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents Face Query Filter
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Query-Operation
 */
class FaceQueryFilter
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

  FaceQueryFilter();

  explicit
  FaceQueryFilter(const Block& block);

  /** \brief prepend FaceQueryFilter to the encoder
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** \brief encode FaceQueryFilter
   */
  const Block&
  wireEncode() const;

  /** \brief decode FaceQueryFilter
   */
  void
  wireDecode(const Block& wire);

  /** \return whether the filter is empty
   */
  bool
  empty() const;

public: // getters & setters
  bool
  hasFaceId() const
  {
    return !!m_faceId;
  }

  uint64_t
  getFaceId() const
  {
    BOOST_ASSERT(this->hasFaceId());
    return *m_faceId;
  }

  FaceQueryFilter&
  setFaceId(uint64_t faceId);

  FaceQueryFilter&
  unsetFaceId();

  bool
  hasUriScheme() const
  {
    return !m_uriScheme.empty();
  }

  const std::string&
  getUriScheme() const
  {
    BOOST_ASSERT(this->hasUriScheme());
    return m_uriScheme;
  }

  FaceQueryFilter&
  setUriScheme(const std::string& uriScheme);

  FaceQueryFilter&
  unsetUriScheme();

  bool
  hasRemoteUri() const
  {
    return !m_remoteUri.empty();
  }

  const std::string&
  getRemoteUri() const
  {
    BOOST_ASSERT(this->hasRemoteUri());
    return m_remoteUri;
  }

  FaceQueryFilter&
  setRemoteUri(const std::string& remoteUri);

  FaceQueryFilter&
  unsetRemoteUri();

  bool
  hasLocalUri() const
  {
    return !m_localUri.empty();
  }

  const std::string&
  getLocalUri() const
  {
    BOOST_ASSERT(this->hasLocalUri());
    return m_localUri;
  }

  FaceQueryFilter&
  setLocalUri(const std::string& localUri);

  FaceQueryFilter&
  unsetLocalUri();

  bool
  hasFaceScope() const
  {
    return !!m_faceScope;
  }

  FaceScope
  getFaceScope() const
  {
    BOOST_ASSERT(this->hasFaceScope());
    return *m_faceScope;
  }

  FaceQueryFilter&
  setFaceScope(FaceScope faceScope);

  FaceQueryFilter&
  unsetFaceScope();

  bool
  hasFacePersistency() const
  {
    return !!m_facePersistency;
  }

  FacePersistency
  getFacePersistency() const
  {
    BOOST_ASSERT(this->hasFacePersistency());
    return *m_facePersistency;
  }

  FaceQueryFilter&
  setFacePersistency(FacePersistency facePersistency);

  FaceQueryFilter&
  unsetFacePersistency();

  bool
  hasLinkType() const
  {
    return !!m_linkType;
  }

  LinkType
  getLinkType() const
  {
    BOOST_ASSERT(this->hasLinkType());
    return *m_linkType;
  }

  FaceQueryFilter&
  setLinkType(LinkType linkType);

  FaceQueryFilter&
  unsetLinkType();

private:
  optional<uint64_t> m_faceId;
  std::string m_uriScheme;
  std::string m_remoteUri;
  std::string m_localUri;
  optional<FaceScope> m_faceScope;
  optional<FacePersistency> m_facePersistency;
  optional<LinkType> m_linkType;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(FaceQueryFilter);

bool
operator==(const FaceQueryFilter& a, const FaceQueryFilter& b);

inline bool
operator!=(const FaceQueryFilter& a, const FaceQueryFilter& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const FaceQueryFilter& filter);

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_FACE_QUERY_FILTER_HPP
