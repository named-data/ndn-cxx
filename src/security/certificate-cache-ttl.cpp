/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "certificate-cache-ttl.hpp"

#include <iostream>

#include "../util/logging.hpp"


INIT_LOGGER("CertificateCacheTtl")

using namespace std;

namespace ndn
{    
CertificateCacheTtl::CertificateCacheTtl(shared_ptr<boost::asio::io_service> io, int defaultTtl)
  : m_defaultTtl(defaultTtl)
  , m_scheduler(*io)
{}

CertificateCacheTtl::~CertificateCacheTtl()
{}
  
void
CertificateCacheTtl::insertCertificate(ptr_lib::shared_ptr<const IdentityCertificate> certificate)
{
  time::Duration expire = (certificate->getFreshnessPeriod() >= 0 ? time::milliseconds(certificate->getFreshnessPeriod()) : time::seconds(m_defaultTtl));

  Name trackerIndex = certificate->getName().getPrefix(-1);
  EventTracker::iterator it = m_tracker.find(trackerIndex);
  if(it != m_tracker.end())
      m_scheduler.cancelEvent(m_tracker[trackerIndex]);

  m_scheduler.scheduleEvent(time::seconds(0), bind(&CertificateCacheTtl::insert, this, certificate));  
  m_tracker[trackerIndex] = m_scheduler.scheduleEvent(expire, bind(&CertificateCacheTtl::remove, this, certificate->getName()));

}

void
CertificateCacheTtl::insert(ptr_lib::shared_ptr<const IdentityCertificate> certificate)
{ 
  Name name = certificate->getName().getPrefix(-1);
  m_cache[name] = certificate;
}

void
CertificateCacheTtl::remove(const Name &certificateName)
{
  Name name = certificateName.getPrefix(-1);
  Cache::iterator it = m_cache.find(name);
  if(it != m_cache.end())
    m_cache.erase(it);
}

ptr_lib::shared_ptr<const IdentityCertificate> 
CertificateCacheTtl::getCertificate(const Name & certificateName)
{
  Cache::iterator it = m_cache.find(certificateName);
  if(it != m_cache.end())
    return it->second;
  else
    return ptr_lib::shared_ptr<IdentityCertificate>();
}

}//ndn


