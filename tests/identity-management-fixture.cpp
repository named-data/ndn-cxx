/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "identity-management-fixture.hpp"
#include "util/io.hpp"
#include "security/v2/additional-description.hpp"

#include <boost/filesystem.hpp>

namespace ndn {
namespace tests {

namespace v1 = security::v1;
namespace v2 = security::v2;

IdentityManagementBaseFixture::~IdentityManagementBaseFixture()
{
  boost::system::error_code ec;
  for (const auto& certFile : m_certFiles) {
    boost::filesystem::remove(certFile, ec); // ignore error
  }
}

bool
IdentityManagementBaseFixture::saveCertToFile(const Data& obj, const std::string& filename)
{
  m_certFiles.insert(filename);
  try {
    io::save(obj, filename);
    return true;
  }
  catch (const io::Error&) {
    return false;
  }
}

IdentityManagementV1Fixture::~IdentityManagementV1Fixture()
{
  for (const auto& identity : m_identities) {
    m_keyChain.deleteIdentity(identity);
  }
}

Name
IdentityManagementV1Fixture::addIdentity(const Name& identity, const KeyParams& params)
{
  Name certName = m_keyChain.createIdentity(identity, params);
  m_identities.insert(identity);
  return certName;
}

bool
IdentityManagementV1Fixture::saveIdentityCertificate(const Name& certName, const std::string& filename)
{
  try {
    auto cert = m_keyChain.getCertificate(certName);
    return saveCertToFile(*cert, filename);
  }
  catch (const v1::SecPublicInfo::Error&) {
    return false;
  }
}

bool
IdentityManagementV1Fixture::addSubCertificate(const Name& subIdentity, const Name& issuer, const KeyParams& params)
{
  if (!m_keyChain.doesIdentityExist(issuer))
    return false;
  if (!m_keyChain.doesIdentityExist(subIdentity)) {
    addIdentity(subIdentity, params);
  }
  Name identityKeyName;
  try {
    identityKeyName = m_keyChain.getDefaultKeyNameForIdentity(subIdentity);
  }
  catch (const v1::SecPublicInfo::Error&) {
    identityKeyName = m_keyChain.generateRsaKeyPairAsDefault(subIdentity, true);
  }
  std::vector<v1::CertificateSubjectDescription> subjectDescription;
  shared_ptr<v1::IdentityCertificate> identityCert =
    m_keyChain.prepareUnsignedIdentityCertificate(identityKeyName,
                                                  issuer,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*identityCert, signingByIdentity(issuer));
  m_keyChain.addCertificateAsIdentityDefault(*identityCert);
  return true;
}

IdentityManagementV2Fixture::IdentityManagementV2Fixture()
  : m_keyChain("pib-memory:", "tpm-memory:")
{
}

security::Identity
IdentityManagementV2Fixture::addIdentity(const Name& identityName, const KeyParams& params)
{
  auto identity = m_keyChain.createIdentity(identityName, params);
  m_identities.insert(identityName);
  return identity;
}

bool
IdentityManagementV2Fixture::saveIdentityCertificate(const security::Identity& identity,
                                                     const std::string& filename)
{
  try {
    auto cert = identity.getDefaultKey().getDefaultCertificate();
    return saveCertToFile(cert, filename);
  }
  catch (const security::Pib::Error&) {
    return false;
  }
}

security::Identity
IdentityManagementV2Fixture::addSubCertificate(const Name& subIdentityName,
                                               const security::Identity& issuer, const KeyParams& params)
{
  auto subIdentity = addIdentity(subIdentityName, params);

  v2::Certificate request = subIdentity.getDefaultKey().getDefaultCertificate();

  request.setName(request.getKeyName().append("parent").appendVersion());

  SignatureInfo info;
  info.setValidityPeriod(security::ValidityPeriod(time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300)));

  v2::AdditionalDescription description;
  description.set("type", "sub-certificate");
  info.appendTypeSpecificTlv(description.wireEncode());

  m_keyChain.sign(request, signingByIdentity(issuer).setSignatureInfo(info));
  m_keyChain.setDefaultCertificate(subIdentity.getDefaultKey(), request);

  return subIdentity;
}

v2::Certificate
IdentityManagementV2Fixture::addCertificate(const security::Key& key, const std::string& issuer)
{
  Name certificateName = key.getName();
  certificateName
    .append(issuer)
    .appendVersion();
  v2::Certificate certificate;
  certificate.setName(certificateName);

  // set metainfo
  certificate.setContentType(tlv::ContentType_Key);
  certificate.setFreshnessPeriod(time::hours(1));

  // set content
  certificate.setContent(key.getPublicKey().buf(), key.getPublicKey().size());

  // set signature-info
  SignatureInfo info;
  info.setValidityPeriod(security::ValidityPeriod(time::system_clock::now(),
                                                  time::system_clock::now() + time::days(10)));

  m_keyChain.sign(certificate, signingByKey(key).setSignatureInfo(info));
  return certificate;
}


} // namespace tests
} // namespace ndn
