/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/certificate-cache.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"
#include "tests/unit/clock-fixture.hpp"

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

using namespace ndn::tests;

class CertificateCacheFixture : public ClockFixture, public KeyChainFixture
{
public:
  CertificateCacheFixture()
    : certCache(10_s)
  {
    identity = m_keyChain.createIdentity("/TestCertificateCache");
    cert = identity.getDefaultKey().getDefaultCertificate();
  }

  void
  checkFindByInterest(const Name& name, bool canBePrefix, optional<Certificate> expected) const
  {
    Interest interest(name);
    interest.setCanBePrefix(canBePrefix);
    BOOST_TEST_CONTEXT(interest) {
      auto found = certCache.find(interest);
      if (expected) {
        BOOST_REQUIRE(found != nullptr);
        BOOST_CHECK_EQUAL(found->getName(), expected->getName());
      }
      else {
        BOOST_CHECK(found == nullptr);
      }
    }
  }

public:
  CertificateCache certCache;
  Identity identity;
  Certificate cert;
};

BOOST_AUTO_TEST_SUITE(Security)
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

  checkFindByInterest(cert.getIdentity(), true, cert);
  checkFindByInterest(cert.getKeyName(), true, cert);
  checkFindByInterest(cert.getName(), false, cert);
  checkFindByInterest(Name(cert.getName()).appendVersion(), true, nullopt);

  advanceClocks(12_s);
  checkFindByInterest(cert.getIdentity(), true, nullopt);
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateCache
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn
