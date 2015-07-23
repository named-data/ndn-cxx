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

#include "field-info.hpp"

#include <boost/mpl/for_each.hpp>
#include <boost/bind.hpp>

namespace ndn {
namespace lp {
namespace detail {

struct ExtractFieldInfo
{
  typedef void result_type;

  template<typename T>
  void
  operator()(FieldInfo* info, T)
  {
    if (T::TlvType::value != info->tlvType) {
      return;
    }
    info->isRecognized = true;
    info->canIgnore = false;
    info->isRepeatable = T::IsRepeatable::value;
    info->locationSortOrder = getLocationSortOrder<typename T::FieldLocation>();
  }
};

FieldInfo::FieldInfo()
  : tlvType(0)
  , isRecognized(false)
  , canIgnore(false)
  , isRepeatable(false)
  , locationSortOrder(getLocationSortOrder<field_location_tags::Header>())
{
}

FieldInfo::FieldInfo(uint64_t tlv)
  : tlvType(tlv)
  , isRecognized(false)
  , canIgnore(false)
  , isRepeatable(false)
  , locationSortOrder(getLocationSortOrder<field_location_tags::Header>())
{
  boost::mpl::for_each<FieldSet>(boost::bind(ExtractFieldInfo(), this, _1));
  if (!isRecognized) {
    canIgnore = tlv::HEADER3_MIN <= tlvType && tlvType <= tlv::HEADER3_MAX &&
                (tlvType & 0x01) == 0x01;
  }
}

} // namespace detail
} // namespace lp
} // namespace ndn