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

#include "ndn-cxx/prefix-announcement.hpp"
#include "ndn-cxx/encoding/tlv-nfd.hpp"

namespace ndn {

static const name::Component KEYWORD_PA_COMP = "20025041"_block;

PrefixAnnouncement::PrefixAnnouncement() = default;

PrefixAnnouncement::PrefixAnnouncement(Data data)
  : m_data(std::move(data))
{
  if (m_data->getContentType() != tlv::ContentType_PrefixAnn) {
    NDN_THROW(Error("Data is not a prefix announcement: ContentType is " +
                    to_string(m_data->getContentType())));
  }

  const Name& dataName = m_data->getName();
  if (dataName.size() < 3 || dataName[-3] != KEYWORD_PA_COMP ||
      !dataName[-2].isVersion() || !dataName[-1].isSegment()) {
    NDN_THROW(Error("Data is not a prefix announcement: wrong name structure"));
  }
  m_announcedName = dataName.getPrefix(-3);

  const Block& payload = m_data->getContent();
  payload.parse();

  m_expiration = time::milliseconds(readNonNegativeInteger(payload.get(tlv::nfd::ExpirationPeriod)));

  auto validityElement = payload.find(tlv::ValidityPeriod);
  if (validityElement != payload.elements_end()) {
    m_validity.emplace(*validityElement);
  }

  for (const Block& element : payload.elements()) {
    if (element.type() != tlv::nfd::ExpirationPeriod && element.type() != tlv::ValidityPeriod &&
        tlv::isCriticalType(element.type())) {
      NDN_THROW(Error("unrecognized element of critical type " + to_string(element.type())));
    }
  }
}

const Data&
PrefixAnnouncement::toData(KeyChain& keyChain, const ndn::security::SigningInfo& si,
                           optional<uint64_t> version) const
{
  if (!m_data) {
    Name dataName = m_announcedName;
    dataName.append(KEYWORD_PA_COMP);
    dataName.appendVersion(version.value_or(time::toUnixTimestamp(time::system_clock::now()).count()));
    dataName.appendSegment(0);
    m_data.emplace(dataName);
    m_data->setContentType(tlv::ContentType_PrefixAnn);

    Block content(tlv::Content);
    content.push_back(makeNonNegativeIntegerBlock(tlv::nfd::ExpirationPeriod,
                                                  m_expiration.count()));
    if (m_validity) {
      content.push_back(m_validity->wireEncode());
    }
    content.encode();
    m_data->setContent(content);

    keyChain.sign(*m_data, si);
  }
  return *m_data;
}

PrefixAnnouncement&
PrefixAnnouncement::setAnnouncedName(Name name)
{
  m_data.reset();
  m_announcedName = std::move(name);
  return *this;
}

PrefixAnnouncement&
PrefixAnnouncement::setExpiration(time::milliseconds expiration)
{
  if (expiration < 0_ms) {
    NDN_THROW(std::invalid_argument("expiration period is negative"));
  }
  m_data.reset();
  m_expiration = expiration;
  return *this;
}

PrefixAnnouncement&
PrefixAnnouncement::setValidityPeriod(optional<security::ValidityPeriod> validity)
{
  m_data.reset();
  m_validity = std::move(validity);
  return *this;
}

bool
operator==(const PrefixAnnouncement& lhs, const PrefixAnnouncement& rhs)
{
  return lhs.getAnnouncedName() == rhs.getAnnouncedName() &&
         lhs.getExpiration() == rhs.getExpiration() &&
         lhs.getValidityPeriod() == rhs.getValidityPeriod();
}

std::ostream&
operator<<(std::ostream& os, const PrefixAnnouncement& pa)
{
  os << pa.getAnnouncedName() << " expires=" << pa.getExpiration();
  if (pa.getValidityPeriod()) {
    os << " validity=" << *pa.getValidityPeriod();
  }
  return os;
}

} // namespace ndn
