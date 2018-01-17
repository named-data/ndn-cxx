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

#include "certificate-cache.hpp"
#include "util/logger.hpp"

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.CertificateCache);

time::nanoseconds
CertificateCache::getDefaultLifetime()
{
  return 1_h;
}

CertificateCache::CertificateCache(const time::nanoseconds& maxLifetime)
  : m_certsByTime(m_certs.get<0>())
  , m_certsByName(m_certs.get<1>())
  , m_maxLifetime(maxLifetime)
{
}

void
CertificateCache::insert(const Certificate& cert)
{
  time::system_clock::TimePoint notAfterTime = cert.getValidityPeriod().getPeriod().second;
  time::system_clock::TimePoint now = time::system_clock::now();
  if (notAfterTime < now) {
    NDN_LOG_DEBUG("Not adding " << cert.getName() << ": already expired at " << time::toIsoString(notAfterTime));
    return;
  }

  time::system_clock::TimePoint removalTime = std::min(notAfterTime, now + m_maxLifetime);
  NDN_LOG_DEBUG("Adding " << cert.getName() << ", will remove in "
                << time::duration_cast<time::seconds>(removalTime - now));
  m_certs.insert(Entry(cert, removalTime));
}

void
CertificateCache::clear()
{
  m_certs.clear();
}

const Certificate*
CertificateCache::find(const Name& certPrefix) const
{
  const_cast<CertificateCache*>(this)->refresh();
  if (certPrefix.size() > 0 && certPrefix[-1].isImplicitSha256Digest()) {
    NDN_LOG_INFO("Certificate search using name with the implicit digest is not yet supported");
  }
  auto itr = m_certsByName.lower_bound(certPrefix);
  if (itr == m_certsByName.end() || !certPrefix.isPrefixOf(itr->getCertName()))
    return nullptr;
  return &itr->cert;
}

const Certificate*
CertificateCache::find(const Interest& interest) const
{
  if (interest.getChildSelector() >= 0) {
    NDN_LOG_DEBUG("Certificate search using ChildSelector is not supported, searching as if selector not specified");
  }
  if (interest.getName().size() > 0 && interest.getName()[-1].isImplicitSha256Digest()) {
    NDN_LOG_INFO("Certificate search using name with implicit digest is not yet supported");
  }
  const_cast<CertificateCache*>(this)->refresh();

  for (auto i = m_certsByName.lower_bound(interest.getName());
       i != m_certsByName.end() && interest.getName().isPrefixOf(i->getCertName());
       ++i) {
    const auto& cert = i->cert;
    if (interest.matchesData(cert)) {
      return &cert;
    }
  }
  return nullptr;
}

void
CertificateCache::refresh()
{
  time::system_clock::TimePoint now = time::system_clock::now();

  auto cIt = m_certsByTime.begin();
  while (cIt != m_certsByTime.end() && cIt->removalTime < now) {
    m_certsByTime.erase(cIt);
    cIt = m_certsByTime.begin();
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
