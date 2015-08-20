/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
 * @author Zhiyi Zhang <dreamerbarrychang@gmail.com>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "certificate.hpp"
#include "../../encoding/block-helpers.hpp"

namespace ndn {
namespace security {
namespace v2 {

BOOST_CONCEPT_ASSERT((WireEncodable<Certificate>));
BOOST_CONCEPT_ASSERT((WireDecodable<Certificate>));

// /<NameSpace>/KEY/[KeyId]/[IssuerId]/[Version]

const ssize_t Certificate::VERSION_OFFSET = -1;
const ssize_t Certificate::ISSUER_ID_OFFSET = -2;
const ssize_t Certificate::KEY_ID_OFFSET = -3;
const ssize_t Certificate::KEY_COMPONENT_OFFSET = -4;
const size_t Certificate::MIN_CERT_NAME_LENGTH = 4;
const size_t Certificate::MIN_KEY_NAME_LENGTH = 2;
const name::Component Certificate::KEY_COMPONENT("KEY");

Certificate::Certificate()
{
  setContentType(tlv::ContentTypeValue::ContentType_Key);
}

Certificate::Certificate(Data&& data)
  : Data(data)
{
  if (!isValidName(getName())) {
    BOOST_THROW_EXCEPTION(Data::Error("Name does not follow the naming convention for certificate"));
  }
  if (getContentType() != tlv::ContentTypeValue::ContentType_Key) {
    BOOST_THROW_EXCEPTION(Data::Error("ContentType is not KEY"));
  }
  if (getFreshnessPeriod() < time::seconds::zero()) {
    BOOST_THROW_EXCEPTION(Data::Error("FreshnessPeriod is not set"));
  }
  if (getContent().value_size() == 0) {
    BOOST_THROW_EXCEPTION(Data::Error("Content is empty"));
  }
}

Certificate::Certificate(const Data& data)
  : Certificate(Data(data))
{
}

Certificate::Certificate(const Block& block)
  : Certificate(Data(block))
{
}

Name
Certificate::getKeyName() const
{
  return getName().getPrefix(KEY_ID_OFFSET + 1);
}

Name
Certificate::getIdentity() const
{
  return getName().getPrefix(KEY_COMPONENT_OFFSET);
}

name::Component
Certificate::getKeyId() const
{
  return getName().at(KEY_ID_OFFSET);
}

name::Component
Certificate::getIssuerId() const
{
  return getName().at(ISSUER_ID_OFFSET);
}

const Buffer
Certificate::getPublicKey() const
{
  if (getContent().value_size() == 0)
    BOOST_THROW_EXCEPTION(Data::Error("Content is empty"));
  return Buffer(getContent().value(), getContent().value_size());
}

ValidityPeriod
Certificate::getValidityPeriod() const
{
  return getSignature().getSignatureInfo().getValidityPeriod();
}

bool
Certificate::isValid(const time::system_clock::TimePoint& ts) const
{
  return getSignature().getSignatureInfo().getValidityPeriod().isValid(ts);
}

const Block&
Certificate::getExtension(uint32_t type) const
{
  return getSignature().getSignatureInfo().getTypeSpecificTlv(type);
}

bool
Certificate::isValidName(const Name& certName)
{
  // /<NameSpace>/KEY/[KeyId]/[IssuerId]/[Version]
  return (certName.size() >= Certificate::MIN_CERT_NAME_LENGTH &&
          certName.get(Certificate::KEY_COMPONENT_OFFSET) == Certificate::KEY_COMPONENT);
}

} // namespace v2
} // namespace security
} // namespace ndn
