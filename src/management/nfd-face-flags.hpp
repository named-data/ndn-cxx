/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_FACE_FLAGS_HPP
#define NDN_MANAGEMENT_NFD_FACE_FLAGS_HPP

namespace ndn {
namespace nfd {

/** \class FaceFlags
 *  \brief provides additional information about a face
 */
enum {
  /** \brief face is local (for scope control purpose)
   */
  FACE_IS_LOCAL = 1,
  /** \brief face is created on demand (accepted incoming connection,
   *         not initiated outgoing connection)
   */
  FACE_IS_ON_DEMAND = 2
  // FACE_? = 4
  // FACE_? = 8
};

/** \brief implements getters to each face flag
 *
 *  \tparam T class containing a FaceFlags field and implements
 *            `FaceFlags getFlags() const` method
 */
template<typename T>
class FaceFlagsTraits
{
public:
  bool
  isLocal() const
  {
    return static_cast<const T*>(this)->getFlags() & FACE_IS_LOCAL;
  }

  bool
  isOnDemand() const
  {
    return static_cast<const T*>(this)->getFlags() & FACE_IS_ON_DEMAND;
  }
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_FACE_FLAGS_HPP
