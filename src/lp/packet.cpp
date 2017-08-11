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

#include "packet.hpp"
#include "fields.hpp"

#include <boost/bind.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/range/adaptor/reversed.hpp>

namespace ndn {
namespace lp {

namespace {

template<typename TAG>
int
getLocationSortOrder();

template<>
int
getLocationSortOrder<field_location_tags::Header>()
{
  return 1;
}

template<>
int
getLocationSortOrder<field_location_tags::Fragment>()
{
  return 2;
}

class FieldInfo
{
public:
  FieldInfo();

  explicit
  FieldInfo(uint64_t tlv);

public:
  uint64_t tlvType;       ///< TLV-TYPE of the field; 0 if field does not exist
  bool isRecognized;      ///< is this field known
  bool canIgnore;         ///< can this unknown field be ignored
  bool isRepeatable;      ///< is the field repeatable
  int locationSortOrder;  ///< sort order of field_location_tag
};

class ExtractFieldInfo
{
public:
  using result_type = void;

  template<typename T>
  void
  operator()(FieldInfo* info, T) const
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
    canIgnore = tlv::HEADER3_MIN <= tlvType &&
                tlvType <= tlv::HEADER3_MAX &&
                (tlvType & 0x03) == 0x00;
  }
}

bool
compareFieldSortOrder(const FieldInfo& first, const FieldInfo& second)
{
  return (first.locationSortOrder < second.locationSortOrder) ||
         (first.locationSortOrder == second.locationSortOrder && first.tlvType < second.tlvType);
}

} // namespace

Packet::Packet()
  : m_wire(Block(tlv::LpPacket))
{
}

Packet::Packet(const Block& wire)
{
  wireDecode(wire);
}

Block
Packet::wireEncode() const
{
  // If no header or trailer, return bare network packet
  Block::element_container elements = m_wire.elements();
  if (elements.size() == 1 && elements.front().type() == FragmentField::TlvType::value) {
    elements.front().parse();
    return elements.front().elements().front();
  }

  m_wire.encode();
  return m_wire;
}

void
Packet::wireDecode(const Block& wire)
{
  if (wire.type() == ndn::tlv::Interest || wire.type() == ndn::tlv::Data) {
    m_wire = Block(tlv::LpPacket);
    add<FragmentField>(make_pair(wire.begin(), wire.end()));
    return;
  }

  if (wire.type() != tlv::LpPacket) {
    BOOST_THROW_EXCEPTION(Error("unrecognized TLV-TYPE " + to_string(wire.type())));
  }

  wire.parse();

  bool isFirst = true;
  FieldInfo prev;
  for (const Block& element : wire.elements()) {
    FieldInfo info(element.type());

    if (!info.isRecognized && !info.canIgnore) {
      BOOST_THROW_EXCEPTION(Error("unrecognized field " + to_string(element.type()) + " cannot be ignored"));
    }

    if (!isFirst) {
      if (info.tlvType == prev.tlvType && !info.isRepeatable) {
        BOOST_THROW_EXCEPTION(Error("non-repeatable field " + to_string(element.type()) + " cannot be repeated"));
      }

      else if (info.tlvType != prev.tlvType && !compareFieldSortOrder(prev, info)) {
        BOOST_THROW_EXCEPTION(Error("fields are not in correct sort order"));
      }
    }

    isFirst = false;
    prev = info;
  }

  m_wire = wire;
}

bool
Packet::comparePos(uint64_t first, const Block& second)
{
  FieldInfo firstInfo(first);
  FieldInfo secondInfo(second.type());
  return compareFieldSortOrder(firstInfo, secondInfo);
}

} // namespace lp
} // namespace ndn
