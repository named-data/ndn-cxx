/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 */

#ifndef NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP
#define NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP

#include "../common.hpp"
#include "certificate-cache.hpp"
#include "../util/scheduler.hpp"

namespace ndn {
namespace security {

/**
 * @brief Cache of validated certificates with freshness-based eviction policy
 *
 * Validated certificates will stay in cache for the duration of their freshness period.
 * The lifetime of the certificate in cache can be extended by "re-inserting" it in the cache.
 */
class CertificateCacheTtl : public CertificateCache
{
public:
  explicit
  CertificateCacheTtl(boost::asio::io_service& io,
                      const time::seconds& defaultTtl = time::seconds(3600));

  virtual
  ~CertificateCacheTtl();

  virtual void
  insertCertificate(shared_ptr<const v1::IdentityCertificate> certificate);

  virtual shared_ptr<const v1::IdentityCertificate>
  getCertificate(const Name& certificateNameWithoutVersion);

  virtual void
  reset();

  virtual size_t
  getSize();

private:
  void
  insert(shared_ptr<const v1::IdentityCertificate> certificate);

  void
  remove(const Name& certificateName);

  void
  removeAll();

protected:
  typedef std::map<Name, std::pair<shared_ptr<const v1::IdentityCertificate>, EventId> > Cache;

  time::seconds m_defaultTtl;
  Cache m_cache;
  boost::asio::io_service& m_io;
  Scheduler m_scheduler;
};

} // namespace security

using security::CertificateCacheTtl;

} // namespace ndn

#endif // NDN_SECURITY_CERTIFICATE_CACHE_TTL_HPP
