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


BOOST_AUTO_TEST_CASE (Ttl)
{
  shared_ptr<boost::asio::io_service> io = make_shared<boost::asio::io_service>();
  shared_ptr<CertificateCacheTtl> cache = make_shared<CertificateCacheTtl>(io, time::seconds(1));
  Scheduler scheduler(*io);

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

  scheduler.scheduleEvent(time::milliseconds(200),  bind(&getCertificateTtl, cache, name1, true));
  scheduler.scheduleEvent(time::milliseconds(200),  bind(&getCertificateTtl, cache, name2, true));
  scheduler.scheduleEvent(time::milliseconds(900),  bind(&getCertificateTtl, cache, name1, false));
  scheduler.scheduleEvent(time::milliseconds(900),  bind(&getCertificateTtl, cache, name2, true));
  scheduler.scheduleEvent(time::milliseconds(900),  bind(&CertificateCache::insertCertificate, cache, cert2));
  scheduler.scheduleEvent(time::milliseconds(1500), bind(&getCertificateTtl, cache, name2, true));
  scheduler.scheduleEvent(time::milliseconds(2500), bind(&getCertificateTtl, cache, name2, false));

  io->run();
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
