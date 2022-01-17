/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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
 * @author Chavoosh Ghasemi <chghasemi@cs.arizona.edu>
 */

#include "ndn-cxx/metadata-object.hpp"

namespace ndn {

static_assert(std::is_base_of<tlv::Error, MetadataObject::Error>::value,
              "MetadataObject::Error must inherit from tlv::Error");

MetadataObject::MetadataObject() = default;

MetadataObject::MetadataObject(const Data& data)
{
  if (!isValidName(data.getName())) {
    NDN_THROW(Error("Name " + data.getName().toUri() + " is not a valid MetadataObject name"));
  }

  if (data.getContentType() != tlv::ContentType_Blob) {
    NDN_THROW(Error("MetadataObject has invalid ContentType " + to_string(data.getContentType())));
  }

  if (data.getContent().value_size() == 0) {
    NDN_THROW(Error("MetadataObject is empty"));
  }

  data.getContent().parse();
  // ignore non-Name elements before the first one
  m_versionedName.wireDecode(data.getContent().get(tlv::Name));
}

Data
MetadataObject::makeData(Name discoveryInterestName,
                         KeyChain& keyChain,
                         const ndn::security::SigningInfo& si,
                         optional<uint64_t> version,
                         time::milliseconds freshnessPeriod) const
{
  if (discoveryInterestName.empty() || discoveryInterestName[-1] != getKeywordComponent()) {
    NDN_THROW(Error("Name " + discoveryInterestName.toUri() +
                    " is not a valid discovery Interest name"));
  }
  discoveryInterestName.appendVersion(version);
  discoveryInterestName.appendSegment(0);

  Data data(discoveryInterestName);
  data.setContent(m_versionedName.wireEncode());
  data.setFreshnessPeriod(freshnessPeriod);
  keyChain.sign(data, si);

  return data;
}

MetadataObject&
MetadataObject::setVersionedName(const Name& name)
{
  m_versionedName = name;
  return *this;
}

const name::Component&
MetadataObject::getKeywordComponent()
{
  static const name::Component nc(tlv::KeywordNameComponent, {'m', 'e', 't', 'a', 'd', 'a', 't', 'a'});
  return nc;
}

bool
MetadataObject::isValidName(const Name& name)
{
  return name.size() >= 3 && name[-3] == getKeywordComponent() &&
         name[-2].isVersion() && name[-1].isSegment();
}

Interest
MetadataObject::makeDiscoveryInterest(Name name)
{
  return Interest(name.append(getKeywordComponent()))
         .setCanBePrefix(true)
         .setMustBeFresh(true);
}

} // namespace ndn
