/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "security/certificate-cache-ttl.hpp"
#include "face.hpp"

#include "boost-test.hpp"

using namespace std;

namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestCertificateCache)

void
getCertificateTtl(shared_ptr<CertificateCacheTtl> cache, const Name &name, bool cached)
{
  BOOST_CHECK_EQUAL(static_cast<bool>(cache->getCertificate(name)), cached);
}

void
checkSize(shared_ptr<CertificateCacheTtl> cache, size_t size)
{
  BOOST_CHECK_EQUAL(cache->getSize(), size);
}


BOOST_AUTO_TEST_CASE (Ttl)
{
  boost::asio::io_service io;
  shared_ptr<CertificateCacheTtl> cache =
    make_shared<CertificateCacheTtl>(ref(io), time::seconds(1));
  Scheduler scheduler(io);

  shared_ptr<IdentityCertificate> cert1 = make_shared<IdentityCertificate>();
  Name certName1("/tmp/KEY/ksk-1/ID-CERT/1");
  cert1->setName(certName1);
  cert1->setFreshnessPeriod(time::milliseconds(500));
  shared_ptr<IdentityCertificate> cert2 = make_shared<IdentityCertificate>();
  Name certName2("/tmp/KEY/ksk-2/ID-CERT/2");
  cert2->setName(certName2);
  cert2->setFreshnessPeriod(time::milliseconds(1000));

  Name name1 = certName1.getPrefix(-1);
  Name name2 = certName2.getPrefix(-1);

  cache->insertCertificate(cert1);
  cache->insertCertificate(cert2);

  scheduler.scheduleEvent(time::milliseconds(200), bind(&checkSize, cache, 2));
  scheduler.scheduleEvent(time::milliseconds(200), bind(&getCertificateTtl, cache, name1, true));
  scheduler.scheduleEvent(time::milliseconds(200), bind(&getCertificateTtl, cache, name2, true));

  // cert1 should removed from the cache
  scheduler.scheduleEvent(time::milliseconds(900), bind(&checkSize, cache, 1));
  scheduler.scheduleEvent(time::milliseconds(900), bind(&getCertificateTtl, cache, name1, false));
  scheduler.scheduleEvent(time::milliseconds(900), bind(&getCertificateTtl, cache, name2, true));

  // Refresh certificate in cache
  scheduler.scheduleEvent(time::milliseconds(900), bind(&CertificateCache::insertCertificate,
                                                        cache, cert2));
  scheduler.scheduleEvent(time::milliseconds(1500), bind(&getCertificateTtl, cache, name2, true));
  scheduler.scheduleEvent(time::milliseconds(2500), bind(&getCertificateTtl, cache, name2, false));

  // Purge
  scheduler.scheduleEvent(time::milliseconds(3000), bind(&CertificateCache::insertCertificate,
                                                         cache, cert1));
  scheduler.scheduleEvent(time::milliseconds(3000), bind(&CertificateCache::insertCertificate,
                                                         cache, cert2));
  scheduler.scheduleEvent(time::milliseconds(3100), bind(&checkSize, cache, 2));
  scheduler.scheduleEvent(time::milliseconds(3200), bind(&CertificateCache::reset, cache));
  scheduler.scheduleEvent(time::milliseconds(3300), bind(&getCertificateTtl, cache, name1, false));
  scheduler.scheduleEvent(time::milliseconds(3300), bind(&getCertificateTtl, cache, name2, false));
  scheduler.scheduleEvent(time::milliseconds(3400), bind(&checkSize, cache, 0));

  io.run();
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
