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

#ifndef NDN_SECURITY_CERTIFICATE_CACHE_HPP
#define NDN_SECURITY_CERTIFICATE_CACHE_HPP

#include "../name.hpp"
#include "identity-certificate.hpp"

namespace ndn {

class CertificateCache
{
public:
  virtual
  ~CertificateCache()
  {
  }

  virtual void
  insertCertificate(shared_ptr<const IdentityCertificate> certificate) = 0;

  virtual shared_ptr<const IdentityCertificate>
  getCertificate(const Name& certificateNameWithoutVersion) = 0;
};

} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_CACHE_HPP
