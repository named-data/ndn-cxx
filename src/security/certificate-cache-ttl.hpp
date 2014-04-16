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

#ifndef NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP
#define NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP

#include "../common.hpp"
#include "certificate-cache.hpp"
#include "../util/scheduler.hpp"

namespace ndn {

class CertificateCacheTtl : public CertificateCache
{
public:
  CertificateCacheTtl(boost::asio::io_service& io,
                      const time::seconds& defaultTtl = time::seconds(3600))
    : m_defaultTtl(defaultTtl)
    , m_scheduler(io)
  {
  }

  virtual
  ~CertificateCacheTtl()
  {
  }

  virtual inline void
  insertCertificate(shared_ptr<const IdentityCertificate> certificate);

  virtual inline shared_ptr<const IdentityCertificate>
  getCertificate(const Name& certificateNameWithoutVersion);

  virtual inline void
  reset();

  virtual inline size_t
  getSize();

private:
  inline void
  insert(shared_ptr<const IdentityCertificate> certificate);

  inline void
  remove(const Name& certificateName);

  inline void
  removeAll();

protected:
  typedef std::map<Name, std::pair<shared_ptr<const IdentityCertificate>, EventId> > Cache;

  time::seconds m_defaultTtl;
  Cache m_cache;
  Scheduler m_scheduler;
};

inline void
CertificateCacheTtl::insertCertificate(shared_ptr<const IdentityCertificate> certificate)
{
  m_scheduler.scheduleEvent(time::seconds(0),
                            bind(&CertificateCacheTtl::insert, this, certificate));
}

inline shared_ptr<const IdentityCertificate>
CertificateCacheTtl::getCertificate(const Name& certificateName)
{
  Cache::iterator it = m_cache.find(certificateName);
  if (it != m_cache.end())
    return it->second.first;
  else
    return shared_ptr<IdentityCertificate>();
}

inline void
CertificateCacheTtl::reset()
{
  m_scheduler.scheduleEvent(time::seconds(0),
                            bind(&CertificateCacheTtl::removeAll, this));
}

inline size_t
CertificateCacheTtl::getSize()
{
  return m_cache.size();
}

inline void
CertificateCacheTtl::insert(shared_ptr<const IdentityCertificate> certificate)
{
  time::milliseconds expire = (certificate->getFreshnessPeriod() >= time::seconds::zero() ?
                               certificate->getFreshnessPeriod() : m_defaultTtl);

  Name index = certificate->getName().getPrefix(-1);

  Cache::iterator it = m_cache.find(index);
  if (it != m_cache.end())
    m_scheduler.cancelEvent(it->second.second);

  EventId eventId = m_scheduler.scheduleEvent(expire,
                                              bind(&CertificateCacheTtl::remove,
                                                   this, certificate->getName()));

  m_cache[index] = std::make_pair(certificate, eventId);
}

inline void
CertificateCacheTtl::remove(const Name& certificateName)
{
  Name name = certificateName.getPrefix(-1);
  Cache::iterator it = m_cache.find(name);
  if (it != m_cache.end())
    m_cache.erase(it);
}

inline void
CertificateCacheTtl::removeAll()
{
  for(Cache::iterator it = m_cache.begin(); it != m_cache.end(); it++)
    m_scheduler.cancelEvent(it->second.second);

  m_cache.clear();
}


} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP
