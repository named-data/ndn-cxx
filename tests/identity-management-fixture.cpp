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

#include <boost/filesystem.hpp>

namespace ndn {
namespace tests {

IdentityManagementFixture::IdentityManagementFixture()
{
}

IdentityManagementFixture::~IdentityManagementFixture()
{
  for (const auto& identity : m_identities) {
    m_keyChain.deleteIdentity(identity);
  }

  boost::system::error_code ec;
  for (const auto& certFile : m_certFiles) {
    boost::filesystem::remove(certFile, ec); // ignore error
  }
}

bool
IdentityManagementFixture::addIdentity(const Name& identity, const KeyParams& params)
{
  try {
    m_keyChain.createIdentity(identity, params);
    m_identities.push_back(identity);
    return true;
  }
  catch (std::runtime_error&) {
    return false;
  }
}

bool
IdentityManagementFixture::saveIdentityCertificate(const Name& identity,
                                                   const std::string& filename, bool wantAdd)
{
  shared_ptr<security::v1::IdentityCertificate> cert;
  try {
    cert = m_keyChain.getCertificate(m_keyChain.getDefaultCertificateNameForIdentity(identity));
  }
  catch (const security::v1::SecPublicInfo::Error&) {
    if (wantAdd && this->addIdentity(identity)) {
      return this->saveIdentityCertificate(identity, filename, false);
    }
    return false;
  }

  m_certFiles.push_back(filename);
  try {
    io::save(*cert, filename);
    return true;
  }
  catch (const io::Error&) {
    return false;
  }
}

bool
IdentityManagementFixture::addSubCertificate(const Name& identity, const Name& issuer,
                                             const KeyParams& params)
{
  if (!m_keyChain.doesIdentityExist(issuer))
    return false;
  if (!m_keyChain.doesIdentityExist(identity)) {
    addIdentity(identity, params);
  }
  Name identityKeyName;
  try {
    identityKeyName = m_keyChain.getDefaultKeyNameForIdentity(identity);
  }
  catch (const security::v1::SecPublicInfo::Error&) {
    identityKeyName = m_keyChain.generateRsaKeyPairAsDefault(identity, true);
  }
  std::vector<security::v1::CertificateSubjectDescription> subjectDescription;
  shared_ptr<security::v1::IdentityCertificate> identityCert =
    m_keyChain.prepareUnsignedIdentityCertificate(identityKeyName,
                                                  issuer,
                                                  time::system_clock::now(),
                                                  time::system_clock::now() + time::days(7300),
                                                  subjectDescription);
  m_keyChain.sign(*identityCert, security::signingByIdentity(issuer));
  m_keyChain.addCertificateAsIdentityDefault(*identityCert);
  return true;
}

} // namespace tests
} // namespace ndn
