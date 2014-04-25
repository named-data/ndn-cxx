/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#include "common.hpp"
#include "certificate-cache-ttl.hpp"

using namespace std;

namespace ndn {

CertificateCacheTtl::CertificateCacheTtl(shared_ptr<boost::asio::io_service> io,
                                         const time::seconds& defaultTtl)
  : m_defaultTtl(defaultTtl)
  , m_scheduler(*io)
{
}

CertificateCacheTtl::~CertificateCacheTtl()
{
}

void
CertificateCacheTtl::insertCertificate(shared_ptr<const IdentityCertificate> certificate)
{
  time::milliseconds expire = (certificate->getFreshnessPeriod() >= time::seconds::zero() ?
                               certificate->getFreshnessPeriod() : m_defaultTtl);

  Name trackerIndex = certificate->getName().getPrefix(-1);
  EventTracker::iterator it = m_tracker.find(trackerIndex);
  if (it != m_tracker.end())
      m_scheduler.cancelEvent(m_tracker[trackerIndex]);

  m_scheduler.scheduleEvent(time::seconds(0),
                            bind(&CertificateCacheTtl::insert, this, certificate));
  m_tracker[trackerIndex] = m_scheduler.scheduleEvent(expire,
                                                      bind(&CertificateCacheTtl::remove,
                                                           this, certificate->getName()));
}

void
CertificateCacheTtl::insert(shared_ptr<const IdentityCertificate> certificate)
{
  Name name = certificate->getName().getPrefix(-1);
  m_cache[name] = certificate;
}

void
CertificateCacheTtl::remove(const Name& certificateName)
{
  Name name = certificateName.getPrefix(-1);
  Cache::iterator it = m_cache.find(name);
  if (it != m_cache.end())
    m_cache.erase(it);
}

shared_ptr<const IdentityCertificate>
CertificateCacheTtl::getCertificate(const Name& certificateName)
{
  Cache::iterator it = m_cache.find(certificateName);
  if (it != m_cache.end())
    return it->second;
  else
    return shared_ptr<IdentityCertificate>();
}

} // namespace ndn
