/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_CERTIFICATE_CACHE_HPP
#define NDN_SECURITY_CERTIFICATE_CACHE_HPP

#include "../name.hpp"
#include "identity-certificate.hpp"

namespace ndn {

class CertificateCache
{
public:
  virtual
  ~CertificateCache() {}
    
  virtual void
  insertCertificate(ptr_lib::shared_ptr<const IdentityCertificate> certificate) = 0;

  virtual ptr_lib::shared_ptr<const IdentityCertificate> 
  getCertificate(const Name& certificateNameWithoutVersion) = 0;
};

} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_CACHE_HPP
