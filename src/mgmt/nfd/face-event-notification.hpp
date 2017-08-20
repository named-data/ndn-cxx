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

#ifndef NDN_MGMT_NFD_FACE_EVENT_NOTIFICATION_HPP
#define NDN_MGMT_NFD_FACE_EVENT_NOTIFICATION_HPP

#include "face-traits.hpp"

namespace ndn {
namespace nfd {

/**
 * \ingroup management
 * \brief represents a Face status change notification
 * \sa https://redmine.named-data.net/projects/nfd/wiki/FaceMgmt#Face-Status-Change-Notification
 */
class FaceEventNotification : public FaceTraits<FaceEventNotification>
{
public:
  FaceEventNotification();

  explicit
  FaceEventNotification(const Block& block);

  /** \brief prepend FaceEventNotification to the encoder
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /** \brief encode FaceEventNotification
   */
  const Block&
  wireEncode() const;

  /** \brief decode FaceEventNotification
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters
  FaceEventKind
  getKind() const
  {
    return m_kind;
  }

  FaceEventNotification&
  setKind(FaceEventKind kind);

private:
  FaceEventKind m_kind;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(FaceEventNotification);

bool
operator==(const FaceEventNotification& a, const FaceEventNotification& b);

inline bool
operator!=(const FaceEventNotification& a, const FaceEventNotification& b)
{
  return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const FaceEventNotification& notification);

} // namespace nfd
} // namespace ndn

#endif // NDN_MGMT_NFD_FACE_EVENT_NOTIFICATION_HPP
