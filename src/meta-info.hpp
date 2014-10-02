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

#ifndef NDN_META_INFO_HPP
#define NDN_META_INFO_HPP

#include "common.hpp"
#include "encoding/block.hpp"
#include "encoding/encoding-buffer.hpp"
#include "util/time.hpp"
#include "name-component.hpp"

namespace ndn {

/**
 * An MetaInfo holds the meta info which is signed inside the data packet.
 */
class MetaInfo
{
public:
  enum {
    TYPE_DEFAULT = 0,
    TYPE_LINK = 1,
    TYPE_KEY = 2
  };

  MetaInfo();

  /**
   * @brief Create from wire encoding
   */
  explicit
  MetaInfo(const Block& block);

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

  ///////////////////////////////////////////////////////////////////////////////
  // Getters/setters

  uint32_t
  getType() const;

  MetaInfo&
  setType(uint32_t type);

  const time::milliseconds&
  getFreshnessPeriod() const;

  MetaInfo&
  setFreshnessPeriod(const time::milliseconds& freshnessPeriod);

  const name::Component&
  getFinalBlockId() const;

  MetaInfo&
  setFinalBlockId(const name::Component& finalBlockId);

public: // EqualityComparable concept
  bool
  operator==(const MetaInfo& other) const;

  bool
  operator!=(const MetaInfo& other) const;

private:
  uint32_t m_type;
  time::milliseconds m_freshnessPeriod;
  name::Component m_finalBlockId;

  mutable Block m_wire;
};

std::ostream&
operator<<(std::ostream& os, const MetaInfo& info);

/////////////////////////////////////////////////////////////////////////

inline uint32_t
MetaInfo::getType() const
{
  return m_type;
}

inline const time::milliseconds&
MetaInfo::getFreshnessPeriod() const
{
  return m_freshnessPeriod;
}

inline const name::Component&
MetaInfo::getFinalBlockId() const
{
  return m_finalBlockId;
}

inline bool
MetaInfo::operator==(const MetaInfo& other) const
{
  return wireEncode() == other.wireEncode();
}

inline bool
MetaInfo::operator!=(const MetaInfo& other) const
{
  return !(*this == other);
}

} // namespace ndn

#endif // NDN_META_INFO_HPP
