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
