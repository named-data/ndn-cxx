/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "security/pib/certificate-container.hpp"
#include "security/pib/pib.hpp"
#include "security/pib/pib-memory.hpp"

#include "boost-test.hpp"
#include "pib-data-fixture.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace tests {

using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_FIXTURE_TEST_SUITE(TestCertificateContainer, PibDataFixture)

using pib::Pib;

BOOST_AUTO_TEST_CASE(Basic)
{
  auto pibImpl = make_shared<PibMemory>();

  // start with an empty container
  CertificateContainer container(id1Key1Name, pibImpl);
  BOOST_CHECK_EQUAL(container.size(), 0);
  BOOST_CHECK_EQUAL(container.getCache().size(), 0);

  // add one cert
  container.add(id1Key1Cert1);
  BOOST_CHECK_EQUAL(container.size(), 1);
  BOOST_CHECK_EQUAL(container.getCache().size(), 1);
  BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());

  // add the same cert again
  container.add(id1Key1Cert1);
  BOOST_CHECK_EQUAL(container.size(), 1);
  BOOST_CHECK_EQUAL(container.getCache().size(), 1);
  BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());

  // add another cert
  container.add(id1Key1Cert2);
  BOOST_CHECK_EQUAL(container.size(), 2);
  BOOST_CHECK_EQUAL(container.getCache().size(), 2);
  BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());
  BOOST_CHECK(container.find(id1Key1Cert2.getName()) != container.end());

  // get certs
  BOOST_REQUIRE_NO_THROW(container.get(id1Key1Cert1.getName()));
  BOOST_REQUIRE_NO_THROW(container.get(id1Key1Cert2.getName()));
  Name id1Key1Cert3Name = id1Key1Name;
  id1Key1Cert3Name.append("issuer").appendVersion(3);
  BOOST_CHECK_THROW(container.get(id1Key1Cert3Name), Pib::Error);

  // check cert
  v2::Certificate cert1 = container.get(id1Key1Cert1.getName());
  v2::Certificate cert2 = container.get(id1Key1Cert2.getName());
  BOOST_CHECK_EQUAL(cert1, id1Key1Cert1);
  BOOST_CHECK_EQUAL(cert2, id1Key1Cert2);

  // create another container from the same PibImpl
  // cache should be empty
  CertificateContainer container2(id1Key1Name, pibImpl);
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getCache().size(), 0);

  // get certificate, cache should be filled
  BOOST_REQUIRE_NO_THROW(container2.get(id1Key1Cert1.getName()));
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getCache().size(), 1);

  BOOST_REQUIRE_NO_THROW(container2.get(id1Key1Cert2.getName()));
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getCache().size(), 2);

  // remove a certificate
  container2.remove(id1Key1Cert1.getName());
  BOOST_CHECK_EQUAL(container2.size(), 1);
  BOOST_CHECK_EQUAL(container2.getCache().size(), 1);
  BOOST_CHECK(container2.find(id1Key1Cert1.getName()) == container2.end());
  BOOST_CHECK(container2.find(id1Key1Cert2.getName()) != container2.end());

  // remove another certificate
  container2.remove(id1Key1Cert2.getName());
  BOOST_CHECK_EQUAL(container2.size(), 0);
  BOOST_CHECK_EQUAL(container2.getCache().size(), 0);
  BOOST_CHECK(container2.find(id1Key1Cert2.getName()) == container2.end());
}

BOOST_AUTO_TEST_CASE(Errors)
{
  auto pibImpl = make_shared<PibMemory>();

  CertificateContainer container(id1Key1Name, pibImpl);

  BOOST_CHECK_THROW(container.add(id1Key2Cert1), std::invalid_argument);
  BOOST_CHECK_THROW(container.remove(id1Key2Cert1.getName()), std::invalid_argument);
  BOOST_CHECK_THROW(container.get(id1Key2Cert1.getName()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(Iterator)
{
  auto pibImpl = make_shared<PibMemory>();

  // start with an empty container
  CertificateContainer container(id1Key1Name, pibImpl);
  container.add(id1Key1Cert1);
  container.add(id1Key1Cert2);

  std::set<Name> certNames;
  certNames.insert(id1Key1Cert1.getName());
  certNames.insert(id1Key1Cert2.getName());

  CertificateContainer::const_iterator it = container.begin();
  std::set<Name>::const_iterator testIt = certNames.begin();
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  it++;
  testIt++;
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  ++it;
  testIt++;
  BOOST_CHECK(it == container.end());

  size_t count = 0;
  testIt = certNames.begin();
  for (const auto& cert : container) {
    BOOST_CHECK_EQUAL(cert.getName(), *testIt);
    testIt++;
    count++;
  }
  BOOST_CHECK_EQUAL(count, 2);

  BOOST_CHECK(CertificateContainer::const_iterator() == CertificateContainer::const_iterator());
  BOOST_CHECK(CertificateContainer::const_iterator() == container.end());
  BOOST_CHECK(container.end() == CertificateContainer::const_iterator());
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateContainer
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
