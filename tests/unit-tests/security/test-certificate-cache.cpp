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
#include "util/time-unit-test-clock.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityTestCertificateCache)

class UnitTestTimeFixture
{
public:
  UnitTestTimeFixture()
    : steadyClock(make_shared<time::UnitTestSteadyClock>())
    , scheduler(io)
    , cache(make_shared<CertificateCacheTtl>(ref(io), time::seconds(1)))
  {
    time::setCustomClocks(steadyClock);

    cert1 = make_shared<IdentityCertificate>();
    Name certName1("/tmp/KEY/ksk-1/ID-CERT/1");
    cert1->setName(certName1);
    cert1->setFreshnessPeriod(time::milliseconds(500));

    cert2 = make_shared<IdentityCertificate>();
    Name certName2("/tmp/KEY/ksk-2/ID-CERT/2");
    cert2->setName(certName2);
    cert2->setFreshnessPeriod(time::milliseconds(1000));

    name1 = certName1.getPrefix(-1);
    name2 = certName2.getPrefix(-1);
  }

  ~UnitTestTimeFixture()
  {
    time::setCustomClocks(nullptr, nullptr);
  }

public:
  shared_ptr<time::UnitTestSteadyClock> steadyClock;

  boost::asio::io_service io;
  Scheduler scheduler;

  shared_ptr<CertificateCacheTtl> cache;

  shared_ptr<IdentityCertificate> cert1;
  shared_ptr<IdentityCertificate> cert2;

  Name name1;
  Name name2;
};


BOOST_FIXTURE_TEST_CASE(Expiration, UnitTestTimeFixture)
{
  cache->insertCertificate(cert1);
  cache->insertCertificate(cert2);

  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 2);

  scheduler.scheduleEvent(time::milliseconds(200), [&] {
      BOOST_CHECK_EQUAL(cache->getSize(), 2);
      BOOST_CHECK_EQUAL(static_cast<bool>(cache->getCertificate(name1)), true);
      BOOST_CHECK_EQUAL(static_cast<bool>(cache->getCertificate(name2)), true);
    });

  steadyClock->advance(time::milliseconds(200));
  io.poll();

  // cert1 should removed from the cache
  scheduler.scheduleEvent(time::milliseconds(700), [&] {
      BOOST_CHECK_EQUAL(static_cast<bool>(cache->getCertificate(name1)), false);
      BOOST_CHECK_EQUAL(static_cast<bool>(cache->getCertificate(name2)), true);
    });

  steadyClock->advance(time::milliseconds(700));
  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 1);

  steadyClock->advance(time::milliseconds(700));
  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 0);
}

BOOST_FIXTURE_TEST_CASE(TtlRefresh, UnitTestTimeFixture)
{
  cache->insertCertificate(cert1); // 500ms

  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 1);

  steadyClock->advance(time::milliseconds(400));
  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 1);

    // Refresh certificate in cache
  cache->insertCertificate(cert1); // +500ms

  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 1);

  steadyClock->advance(time::milliseconds(400));
  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 1);

  steadyClock->advance(time::milliseconds(200));
  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 0);
}

BOOST_FIXTURE_TEST_CASE(Reset, UnitTestTimeFixture)
{
  cache->insertCertificate(cert1);
  cache->insertCertificate(cert2);

  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 2);

  cache->reset();

  io.poll();
  BOOST_CHECK_EQUAL(cache->getSize(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
