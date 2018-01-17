/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "security/v2/certificate-cache.hpp"

#include "../../identity-management-time-fixture.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)

class CertificateCacheFixture : public ndn::tests::IdentityManagementTimeFixture
{
public:
  CertificateCacheFixture()
    : certCache(10_s)
  {
    identity = addIdentity("/TestCertificateCache/");
    cert = identity.getDefaultKey().getDefaultCertificate();
  }

public:
  CertificateCache certCache;
  Identity identity;
  Certificate cert;
};

BOOST_FIXTURE_TEST_SUITE(TestCertificateCache, CertificateCacheFixture)

BOOST_AUTO_TEST_CASE(RemovalTime)
{
  // Cache lifetime is capped to 10 seconds during cache construction

  BOOST_CHECK_NO_THROW(certCache.insert(cert));
  BOOST_CHECK(certCache.find(cert.getName()) != nullptr);

  advanceClocks(11_s, 1);
  BOOST_CHECK(certCache.find(cert.getName()) == nullptr);

  BOOST_CHECK_NO_THROW(certCache.insert(cert));
  BOOST_CHECK(certCache.find(cert.getName()) != nullptr);

  advanceClocks(5_s);
  BOOST_CHECK(certCache.find(cert.getName()) != nullptr);

  advanceClocks(15_s);
  BOOST_CHECK(certCache.find(cert.getName()) == nullptr);
}

BOOST_AUTO_TEST_CASE(FindByInterest)
{
  BOOST_CHECK_NO_THROW(certCache.insert(cert));

  // Find by interest
  BOOST_CHECK(certCache.find(Interest(cert.getIdentity())) != nullptr);
  BOOST_CHECK(certCache.find(Interest(cert.getKeyName())) != nullptr);
  BOOST_CHECK(certCache.find(Interest(Name(cert.getName()).appendVersion())) == nullptr);

  advanceClocks(12_s);
  BOOST_CHECK(certCache.find(Interest(cert.getIdentity())) == nullptr);

  Certificate cert3 = addCertificate(identity.getDefaultKey(), "3");
  Certificate cert4 = addCertificate(identity.getDefaultKey(), "4");
  Certificate cert5 = addCertificate(identity.getDefaultKey(), "5");

  certCache.insert(cert3);
  certCache.insert(cert4);
  certCache.insert(cert5);

  Interest interest4(cert3.getKeyName());
  interest4.setExclude(Exclude().excludeOne(cert3.getName().at(Certificate::ISSUER_ID_OFFSET)));
  BOOST_CHECK(certCache.find(interest4) != nullptr);
  BOOST_CHECK_NE(certCache.find(interest4)->getName(), cert3.getName());

  // TODO cover more cases with different interests
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateCache
BOOST_AUTO_TEST_SUITE_END() // V2
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
