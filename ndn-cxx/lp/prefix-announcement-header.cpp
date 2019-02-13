/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/lp/prefix-announcement-header.hpp"
#include "ndn-cxx/lp/tlv.hpp"

namespace ndn {
namespace lp {

PrefixAnnouncementHeader::PrefixAnnouncementHeader() = default;

PrefixAnnouncementHeader::PrefixAnnouncementHeader(const Block& block)
{
  wireDecode(block);
}

PrefixAnnouncementHeader::PrefixAnnouncementHeader(PrefixAnnouncement prefixAnn)
  : m_prefixAnn(std::move(prefixAnn))
{
  if (m_prefixAnn->getData() == nullopt) {
    NDN_THROW(Error("PrefixAnnouncement does not contain Data"));
  }
}

template<encoding::Tag TAG>
size_t
PrefixAnnouncementHeader::wireEncode(EncodingImpl<TAG>& encoder) const
{
  if (m_prefixAnn == nullopt) {
    NDN_THROW(Error("PrefixAnnouncementHeader does not contain a PrefixAnnouncement"));
  }

  size_t length = 0;
  length += m_prefixAnn->getData()->wireEncode(encoder);
  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(tlv::PrefixAnnouncement);
  return length;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(PrefixAnnouncementHeader);

void
PrefixAnnouncementHeader::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::PrefixAnnouncement) {
    NDN_THROW(Error("PrefixAnnouncement", wire.type()));
  }

  wire.parse();
  m_prefixAnn.emplace(Data(wire.get(ndn::tlv::Data)));
}
} // namespace lp
} // namespace ndn
