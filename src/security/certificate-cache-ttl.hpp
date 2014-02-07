/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CERTIFICATE_CACHE_TTL_HPP
#define NDN_CERTIFICATE_CACHE_TTL_HPP

#include "../common.hpp"
#include "certificate-cache.hpp"
#include "../util/scheduler.hpp"
#include "../util/time.hpp"

namespace ndn {
 
class CertificateCacheTtl : public CertificateCache
{
public:
  CertificateCacheTtl(shared_ptr<boost::asio::io_service> io, int defaultTtl = 3600);
  
  virtual
  ~CertificateCacheTtl();
    
  virtual void
  insertCertificate(ptr_lib::shared_ptr<const IdentityCertificate> certificate);
  
  virtual ptr_lib::shared_ptr<const IdentityCertificate> 
  getCertificate(const Name & certificateNameWithoutVersion);

private:  
  void
  insert(ptr_lib::shared_ptr<const IdentityCertificate> certificate);
  
  void
  remove(const Name &certificateName);

protected:
  typedef std::map<Name, ptr_lib::shared_ptr<const IdentityCertificate> > Cache;
  typedef std::map<Name, EventId> EventTracker;

  int m_defaultTtl;
  Cache m_cache;
  EventTracker m_tracker;
  Scheduler m_scheduler;
};

}//ndn

#endif
