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

#ifndef NDN_MANAGEMENT_NFD_FACE_TRAITS_HPP
#define NDN_MANAGEMENT_NFD_FACE_TRAITS_HPP

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "../encoding/block-helpers.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief provides additional information about a faceScope
 */
enum FaceScope {
  /** \brief face is non-local
   */
  FACE_SCOPE_NON_LOCAL = 0,
  /** \brief face is local
   */
  FACE_SCOPE_LOCAL = 1
};

std::ostream&
operator<<(std::ostream& os, FaceScope faceScope);

/**
 * \ingroup management
 * \brief provides additional information about a facePersistency
 */
enum FacePersistency {
  /** \brief face is persistent
   */
  FACE_PERSISTENCY_PERSISTENT = 0,
  /** \brief face is on-demand
   */
  FACE_PERSISTENCY_ON_DEMAND = 1,
  /** \brief face is permanent
   */
  FACE_PERSISTENCY_PERMANENT = 2
};

std::ostream&
operator<<(std::ostream& os, FacePersistency facePersistency);

/**
 * \ingroup management
 * \brief provides additional information about a linkType
 */
enum LinkType {
  /** \brief link is point-to-point
   */
  LINK_TYPE_POINT_TO_POINT = 0,
  /** \brief link is multi-access
   */
  LINK_TYPE_MULTI_ACCESS = 1
};

std::ostream&
operator<<(std::ostream& os, LinkType linkType);

/** \ingroup management
 *  \brief providers getters and setters of face information fields
 *  \tparam C the concrete class; it must provide .wireReset() method
            to clear wire encoding when a field changes
 */
template<class C>
class FaceTraits
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

  FaceTraits()
    : m_faceId(0)
    , m_faceScope(FACE_SCOPE_NON_LOCAL)
    , m_facePersistency(FACE_PERSISTENCY_PERSISTENT)
    , m_linkType(LINK_TYPE_POINT_TO_POINT)
  {
  }

  uint64_t
  getFaceId() const
  {
    return m_faceId;
  }

  C&
  setFaceId(uint64_t faceId)
  {
    wireReset();
    m_faceId = faceId;
    return static_cast<C&>(*this);
  }

  const std::string&
  getRemoteUri() const
  {
    return m_remoteUri;
  }

  C&
  setRemoteUri(const std::string& remoteUri)
  {
    wireReset();
    m_remoteUri = remoteUri;
    return static_cast<C&>(*this);
  }

  const std::string&
  getLocalUri() const
  {
    return m_localUri;
  }

  C&
  setLocalUri(const std::string& localUri)
  {
    wireReset();
    m_localUri = localUri;
    return static_cast<C&>(*this);
  }

  FaceScope
  getFaceScope() const
  {
    return m_faceScope;
  }

  C&
  setFaceScope(FaceScope faceScope)
  {
    wireReset();
    m_faceScope = faceScope;
    return static_cast<C&>(*this);
  }

  FacePersistency
  getFacePersistency() const
  {
    return m_facePersistency;
  }

  C&
  setFacePersistency(FacePersistency facePersistency)
  {
    wireReset();
    m_facePersistency = facePersistency;
    return static_cast<C&>(*this);
  }

  LinkType
  getLinkType() const
  {
    return m_linkType;
  }

  C&
  setLinkType(LinkType linkType)
  {
    wireReset();
    m_linkType = linkType;
    return static_cast<C&>(*this);
  }

protected:
  virtual void
  wireReset() const = 0;

protected:
  uint64_t m_faceId;
  std::string m_remoteUri;
  std::string m_localUri;
  FaceScope m_faceScope;
  FacePersistency  m_facePersistency;
  LinkType m_linkType;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_TRAITS_HPP
