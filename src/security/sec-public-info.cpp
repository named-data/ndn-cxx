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

#include "sec-public-info.hpp"

namespace ndn {

SecPublicInfo::SecPublicInfo(const std::string& location)
  : m_location(location)
{
}

SecPublicInfo::~SecPublicInfo()
{
}

std::string
SecPublicInfo::getPibLocator()
{
  return this->getScheme() + ":" + m_location;
}

void
SecPublicInfo::addPublicKey(const Name& keyName, KeyType keyType, const PublicKey& publicKey)
{
  addKey(keyName, publicKey);
}

void
SecPublicInfo::setDefaultIdentity(const Name& identityName)
{
  setDefaultIdentityInternal(identityName);
  refreshDefaultCertificate();
}

void
SecPublicInfo::setDefaultKeyNameForIdentity(const Name& keyName)
{
  setDefaultKeyNameForIdentityInternal(keyName);
  refreshDefaultCertificate();
}

void
SecPublicInfo::setDefaultCertificateNameForKey(const Name& certificateName)
{
  setDefaultCertificateNameForKeyInternal(certificateName);
  refreshDefaultCertificate();
}

Name
SecPublicInfo::getDefaultCertificateNameForIdentity(const Name& identityName)
{
  return getDefaultCertificateNameForKey(getDefaultKeyNameForIdentity(identityName));
}

Name
SecPublicInfo::getDefaultCertificateName()
{
  if (m_defaultCertificate == nullptr)
    refreshDefaultCertificate();

  if (m_defaultCertificate == nullptr)
    BOOST_THROW_EXCEPTION(Error("No default certificate is set"));

  return m_defaultCertificate->getName();
}

Name
SecPublicInfo::getNewKeyName(const Name& identityName, bool useKsk)
{
  std::ostringstream oss;

  if (useKsk)
    oss << "ksk-";
  else
    oss << "dsk-";

  oss << time::toUnixTimestamp(time::system_clock::now()).count();

  Name keyName = Name(identityName).append(oss.str());

  if (doesPublicKeyExist(keyName))
    BOOST_THROW_EXCEPTION(Error("Key name already exists: " + keyName.toUri()));

  return keyName;
}

void
SecPublicInfo::addCertificateAsKeyDefault(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  setDefaultCertificateNameForKeyInternal(certificate.getName());
  refreshDefaultCertificate();
}

void
SecPublicInfo::addCertificateAsIdentityDefault(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  Name certName = certificate.getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certName);
  setDefaultKeyNameForIdentityInternal(keyName);
  setDefaultCertificateNameForKeyInternal(certName);
  refreshDefaultCertificate();
}

void
SecPublicInfo::addCertificateAsSystemDefault(const IdentityCertificate& certificate)
{
  addCertificate(certificate);
  Name certName = certificate.getName();
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certName);
  setDefaultIdentityInternal(keyName.getPrefix(-1));
  setDefaultKeyNameForIdentityInternal(keyName);
  setDefaultCertificateNameForKeyInternal(certName);
  refreshDefaultCertificate();
}

shared_ptr<IdentityCertificate>
SecPublicInfo::defaultCertificate()
{
  return getDefaultCertificate();
}

shared_ptr<IdentityCertificate>
SecPublicInfo::getDefaultCertificate()
{
  return m_defaultCertificate;
}

void
SecPublicInfo::refreshDefaultCertificate()
{
  try {
    Name certName = getDefaultCertificateNameForIdentity(getDefaultIdentity());
    m_defaultCertificate = getCertificate(certName);
  }
  catch (SecPublicInfo::Error&) {
    m_defaultCertificate.reset();
  }
}

} // namespace ndn
