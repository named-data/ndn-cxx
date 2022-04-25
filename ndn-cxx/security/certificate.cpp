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
 * @author Zhiyi Zhang <dreamerbarrychang@gmail.com>
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "ndn-cxx/security/certificate.hpp"
#include "ndn-cxx/security/additional-description.hpp"
#include "ndn-cxx/security/transform.hpp"
#include "ndn-cxx/util/indented-stream.hpp"

namespace ndn {
namespace security {
inline namespace v2 {

BOOST_CONCEPT_ASSERT((WireEncodable<Certificate>));
BOOST_CONCEPT_ASSERT((WireDecodable<Certificate>));

// /<IdentityName>/KEY/<KeyId>/<IssuerId>/<Version>
const ssize_t Certificate::VERSION_OFFSET = -1;
const ssize_t Certificate::ISSUER_ID_OFFSET = -2;
const ssize_t Certificate::KEY_ID_OFFSET = -3;
const ssize_t Certificate::KEY_COMPONENT_OFFSET = -4;
const size_t Certificate::MIN_CERT_NAME_LENGTH = 4;
const size_t Certificate::MIN_KEY_NAME_LENGTH = 2;
const name::Component Certificate::KEY_COMPONENT("KEY");
const name::Component Certificate::DEFAULT_ISSUER_ID("NA");

Certificate::Certificate()
{
  setContentType(tlv::ContentType_Key);
}

Certificate::Certificate(Data&& data)
  : Data(std::move(data))
{
  if (!isValidName(getName())) {
    NDN_THROW(Data::Error("Name does not follow the naming convention for certificate"));
  }
  if (getContentType() != tlv::ContentType_Key) {
    NDN_THROW(Data::Error("Expecting ContentType Key, got " + to_string(getContentType())));
  }
  if (getFreshnessPeriod() < time::seconds::zero()) {
    NDN_THROW(Data::Error("FreshnessPeriod is not set"));
  }
  if (getContent().value_size() == 0) {
    NDN_THROW(Data::Error("Content is empty"));
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

Buffer
Certificate::getPublicKey() const
{
  if (getContent().value_size() == 0)
    NDN_THROW(Data::Error("Certificate Content is empty"));

  return {getContent().value_begin(), getContent().value_end()};
}

ValidityPeriod
Certificate::getValidityPeriod() const
{
  return getSignatureInfo().getValidityPeriod();
}

bool
Certificate::isValid(const time::system_clock::TimePoint& ts) const
{
  return getSignatureInfo().getValidityPeriod().isValid(ts);
}

Block
Certificate::getExtension(uint32_t type) const
{
  auto block = getSignatureInfo().getCustomTlv(type);
  if (!block) {
    NDN_THROW(Error("TLV-TYPE " + to_string(type) + " sub-element does not exist in SignatureInfo"));
  }
  return *block;
}

bool
Certificate::isValidName(const Name& certName)
{
  // /<IdentityName>/KEY/<KeyId>/<IssuerId>/<Version>
  return certName.size() >= Certificate::MIN_CERT_NAME_LENGTH &&
         certName[Certificate::KEY_COMPONENT_OFFSET] == Certificate::KEY_COMPONENT;
}

std::ostream&
operator<<(std::ostream& os, const Certificate& cert)
{
  os << "Certificate Name:\n"
     << "  " << cert.getName() << "\n";

  auto optAddlDesc = cert.getSignatureInfo().getCustomTlv(tlv::AdditionalDescription);
  if (optAddlDesc) {
    os << "Additional Description:\n";
    try {
      AdditionalDescription additionalDesc(*optAddlDesc);
      for (const auto& item : additionalDesc) {
        os << "  " << item.first << ": " << item.second << "\n";
      }
    }
    catch (const tlv::Error&) {
      using namespace transform;
      util::IndentedStream os2(os, "  ");
      bufferSource(optAddlDesc->value_bytes()) >> base64Encode() >> streamSink(os2);
    }
  }

  os << "Public Key:\n";
  {
    using namespace transform;

    os << "  Key Type: ";
    try {
      PublicKey key;
      key.loadPkcs8(cert.getPublicKey());
      os << key.getKeySize() << "-bit " << key.getKeyType();
    }
    catch (const std::runtime_error&) {
      os << "Unknown (" << cert.getContent().value_size() << " bytes)";
    }
    os << "\n";

    if (cert.getContent().value_size() > 0) {
      util::IndentedStream os2(os, "  ");
      bufferSource(cert.getPublicKey()) >> base64Encode() >> streamSink(os2);
    }
  }

  try {
    const auto& validityPeriod = cert.getValidityPeriod().getPeriod();
    os << "Validity:\n"
       << "  Not Before: " << time::toIsoExtendedString(validityPeriod.first) << "\n"
       << "  Not After: "  << time::toIsoExtendedString(validityPeriod.second)  << "\n";
  }
  catch (const tlv::Error&) {
    // ignore
  }

  os << "Signature Information:\n"
     << "  Signature Type: " << static_cast<tlv::SignatureTypeValue>(cert.getSignatureType()) << "\n";

  auto keyLoc = cert.getKeyLocator();
  if (keyLoc) {
    os << "  Key Locator: " << *keyLoc << "\n";
    if (keyLoc->getType() == tlv::Name && keyLoc->getName() == cert.getKeyName()) {
      os << "  Self-Signed: yes\n";
    }
  }

  return os;
}

Name
extractIdentityFromCertName(const Name& certName)
{
  if (!Certificate::isValidName(certName)) {
    NDN_THROW(std::invalid_argument("Certificate name `" + certName.toUri() + "` "
                                    "does not respect the naming conventions"));
  }

  return certName.getPrefix(Certificate::KEY_COMPONENT_OFFSET); // trim everything after and including "KEY"
}

Name
extractKeyNameFromCertName(const Name& certName)
{
  if (!Certificate::isValidName(certName)) {
    NDN_THROW(std::invalid_argument("Certificate name `" + certName.toUri() + "` "
                                    "does not respect the naming conventions"));
  }

  return certName.getPrefix(Certificate::KEY_ID_OFFSET + 1); // trim everything after key id
}

} // inline namespace v2
} // namespace security
} // namespace ndn
