/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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
 *
 * @author Teng Liang <philoliang@email.arizona.edu>
 */

#include "prefix-announcement.hpp"
#include "tlv.hpp"

namespace ndn {
namespace lp {

static const name::Component SELF_LEARNING_PREFIX("self-learning");

PrefixAnnouncement::PrefixAnnouncement() = default;

PrefixAnnouncement::PrefixAnnouncement(const Block& block)
{
  wireDecode(block);
}

PrefixAnnouncement::PrefixAnnouncement(shared_ptr<const Data> data)
{
  setData(std::move(data));
}

template<encoding::Tag TAG>
size_t
PrefixAnnouncement::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t length = 0;
  length += m_data->wireEncode(encoder);
  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(tlv::PrefixAnnouncement);
  return length;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(PrefixAnnouncement);

void
PrefixAnnouncement::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::PrefixAnnouncement) {
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV-TYPE " + to_string(wire.type())));
  }

  wire.parse();
  setData(make_shared<Data>(wire.get(ndn::tlv::Data)));
}

Name
PrefixAnnouncement::getAnnouncedName() const
{
  if (m_data == nullptr) {
    BOOST_THROW_EXCEPTION(Error("Data is unset in PrefixAnnouncement"));
  }

  const Name& dataName = m_data->getName();
  BOOST_ASSERT(dataName.at(0) == SELF_LEARNING_PREFIX);
  BOOST_ASSERT(dataName.at(-1).isVersion());

  return dataName.getSubName(1, dataName.size() - 2);
}

PrefixAnnouncement&
PrefixAnnouncement::setData(shared_ptr<const Data> data)
{
  if (data == nullptr) {
    BOOST_THROW_EXCEPTION(Error("Unexpected nullptr"));
  }

  if (data->getName().at(0) != SELF_LEARNING_PREFIX) {
    BOOST_THROW_EXCEPTION(Error("Unexpected prefix in name " + data->getName().toUri()));
  }

  if (!data->getName().get(-1).isVersion()) {
    BOOST_THROW_EXCEPTION(Error("Last name component of " + data->getName().toUri() +
                                " is not a version"));
  }

  if (data->getContent().value_size() != 0 ||
      data->getMetaInfo().wireEncode().value_size() != 0) {
    BOOST_THROW_EXCEPTION(Error("Both Data MetaInfo and Content must be empty"));
  }

  m_data = std::move(data);
  return *this;
}

} // namespace lp
} // namespace ndn
