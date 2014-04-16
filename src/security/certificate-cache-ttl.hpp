/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
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
  CertificateCacheTtl(shared_ptr<boost::asio::io_service> io,
                      const time::seconds& defaultTtl = time::seconds(3600));

  virtual
  ~CertificateCacheTtl();

  virtual void
  insertCertificate(shared_ptr<const IdentityCertificate> certificate);

  virtual shared_ptr<const IdentityCertificate>
  getCertificate(const Name& certificateNameWithoutVersion);

private:
  void
  insert(shared_ptr<const IdentityCertificate> certificate);

  void
  remove(const Name& certificateName);

protected:
  typedef std::map<Name, shared_ptr<const IdentityCertificate> > Cache;
  typedef std::map<Name, EventId> EventTracker;

  time::seconds m_defaultTtl;
  Cache m_cache;
  EventTracker m_tracker;
  Scheduler m_scheduler;
};

} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP
