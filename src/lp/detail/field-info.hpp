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

#ifndef NDN_CXX_LP_DETAIL_FIELD_INFO_HPP
#define NDN_CXX_LP_DETAIL_FIELD_INFO_HPP

#include "../../common.hpp"

#include "../fields.hpp"

namespace ndn {
namespace lp {
namespace detail {

class FieldInfo
{
public:
  FieldInfo();

  explicit
  FieldInfo(uint64_t tlv);

public:
  /**
   * \brief TLV-TYPE of the field; 0 if field does not exist
   */
  uint64_t tlvType;

  /**
   * \brief is this field known
   */
  bool isRecognized;

  /**
   * \brief can this unknown field be ignored
   */
  bool canIgnore;

  /**
   * \brief is the field repeatable
   */
  bool isRepeatable;

  /**
   * \brief sort order of field_location_tag
   */
  int locationSortOrder;
};

template<typename TAG>
inline int
getLocationSortOrder();

template<>
inline int
getLocationSortOrder<field_location_tags::Header>()
{
  return 1;
}

template<>
inline int
getLocationSortOrder<field_location_tags::Fragment>()
{
  return 2;
}

inline bool
compareFieldSortOrder(const FieldInfo& first, const FieldInfo& second)
{
  return (first.locationSortOrder < second.locationSortOrder) ||
         (first.locationSortOrder == second.locationSortOrder && first.tlvType < second.tlvType);
}

} // namespace detail
} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_DETAIL_FIELD_INFO_HPP