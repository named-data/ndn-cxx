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
 */

#include "certificate-storage.hpp"

namespace ndn {
namespace security {
namespace v2 {

CertificateStorage::CertificateStorage()
  : m_verifiedCertCache(1_h)
  , m_unverifiedCertCache(5_min)
{
}

const Certificate*
CertificateStorage::findTrustedCert(const Interest& interestForCert) const
{
  auto cert = m_trustAnchors.find(interestForCert);
  if (cert != nullptr) {
    return cert;
  }

  cert = m_verifiedCertCache.find(interestForCert);
  return cert;
}

bool
CertificateStorage::isCertKnown(const Name& certName) const
{
  return (m_trustAnchors.find(certName) != nullptr ||
          m_verifiedCertCache.find(certName) != nullptr ||
          m_unverifiedCertCache.find(certName) != nullptr);
}

void
CertificateStorage::loadAnchor(const std::string& groupId, Certificate&& cert)
{
  m_trustAnchors.insert(groupId, std::move(cert));
}

void
CertificateStorage::loadAnchor(const std::string& groupId, const std::string& certfilePath,
                               time::nanoseconds refreshPeriod, bool isDir)
{
  m_trustAnchors.insert(groupId, certfilePath, refreshPeriod, isDir);
}

void
CertificateStorage::resetAnchors()
{
  m_trustAnchors.clear();
}

void
CertificateStorage::cacheVerifiedCert(Certificate&& cert)
{
  m_verifiedCertCache.insert(std::move(cert));
}

void
CertificateStorage::resetVerifiedCerts()
{
  m_verifiedCertCache.clear();
}

void
CertificateStorage::cacheUnverifiedCert(Certificate&& cert)
{
  m_unverifiedCertCache.insert(std::move(cert));
}

const TrustAnchorContainer&
CertificateStorage::getTrustAnchors() const
{
  return m_trustAnchors;
}

const CertificateCache&
CertificateStorage::getVerifiedCertCache() const
{
  return m_verifiedCertCache;
}

const CertificateCache&
CertificateStorage::getUnverifiedCertCache() const
{
  return m_unverifiedCertCache;
}

} // namespace v2
} // namespace security
} // namespace ndn
