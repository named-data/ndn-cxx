/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/security/pib/certificate-container.hpp"
#include "ndn-cxx/security/pib/impl/pib-memory.hpp"
#include "ndn-cxx/util/concepts.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn::tests {

using namespace ndn::security::pib;

NDN_CXX_ASSERT_FORWARD_ITERATOR(CertificateContainer::const_iterator);

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestCertificateContainer, PibDataFixture)

BOOST_AUTO_TEST_CASE(AddGetRemove)
{
  auto pibImpl = make_shared<PibMemory>();

  {
    // start with an empty container
    CertificateContainer container(id1Key1Name, pibImpl);
    BOOST_CHECK_EQUAL(container.size(), 0);
    BOOST_CHECK_EQUAL(container.m_certs.size(), 0);

    // add one cert
    container.add(id1Key1Cert1);
    BOOST_CHECK_EQUAL(container.size(), 1);
    BOOST_CHECK_EQUAL(container.m_certs.size(), 1);
    BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());

    // add the same cert again
    container.add(id1Key1Cert1);
    BOOST_CHECK_EQUAL(container.size(), 1);
    BOOST_CHECK_EQUAL(container.m_certs.size(), 1);
    BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());

    // add another cert
    container.add(id1Key1Cert2);
    BOOST_CHECK_EQUAL(container.size(), 2);
    BOOST_CHECK_EQUAL(container.m_certs.size(), 2);
    BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());
    BOOST_CHECK(container.find(id1Key1Cert2.getName()) != container.end());

    // check certs
    Certificate cert1 = container.get(id1Key1Cert1.getName());
    Certificate cert2 = container.get(id1Key1Cert2.getName());
    BOOST_CHECK_EQUAL(cert1, id1Key1Cert1);
    BOOST_CHECK_EQUAL(cert2, id1Key1Cert2);
    Name id1Key1Cert3Name = Name(id1Key1Name).append("issuer").appendVersion(3);
    BOOST_CHECK_THROW(container.get(id1Key1Cert3Name), Pib::Error);
  }

  {
    // create a container from an existing (non-empty) PibImpl
    // names are loaded immediately but the certificate cache should initially be empty
    CertificateContainer container2(id1Key1Name, pibImpl);
    BOOST_CHECK_EQUAL(container2.size(), 2);
    BOOST_CHECK_EQUAL(container2.m_certs.size(), 0);

    // fetching the certificates should populate the cache
    BOOST_CHECK_EQUAL(container2.get(id1Key1Cert1.getName()), id1Key1Cert1);
    BOOST_CHECK_EQUAL(container2.size(), 2);
    BOOST_CHECK_EQUAL(container2.m_certs.size(), 1);

    BOOST_CHECK_EQUAL(container2.get(id1Key1Cert2.getName()), id1Key1Cert2);
    BOOST_CHECK_EQUAL(container2.size(), 2);
    BOOST_CHECK_EQUAL(container2.m_certs.size(), 2);

    // remove a certificate
    container2.remove(id1Key1Cert1.getName());
    BOOST_CHECK_EQUAL(container2.size(), 1);
    BOOST_CHECK_EQUAL(container2.m_certs.size(), 1);
    BOOST_CHECK(container2.find(id1Key1Cert1.getName()) == container2.end());
    BOOST_CHECK(container2.find(id1Key1Cert2.getName()) != container2.end());

    // removing the same certificate again is a no-op
    container2.remove(id1Key1Cert1.getName());
    BOOST_CHECK_EQUAL(container2.size(), 1);
    BOOST_CHECK_EQUAL(container2.m_certs.size(), 1);
    BOOST_CHECK(container2.find(id1Key1Cert1.getName()) == container2.end());
    BOOST_CHECK(container2.find(id1Key1Cert2.getName()) != container2.end());

    // remove another certificate
    container2.remove(id1Key1Cert2.getName());
    BOOST_CHECK_EQUAL(container2.size(), 0);
    BOOST_CHECK_EQUAL(container2.m_certs.size(), 0);
    BOOST_CHECK(container2.find(id1Key1Cert2.getName()) == container2.end());
  }
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
  CertificateContainer container(id1Key1Name, pibImpl);
  container.add(id1Key1Cert1);
  container.add(id1Key1Cert2);

  const std::set<Name> certNames{id1Key1Cert1.getName(), id1Key1Cert2.getName()};

  CertificateContainer::const_iterator it = container.begin();
  auto testIt = certNames.begin();
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  it++;
  testIt++;
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  ++it;
  testIt++;
  BOOST_CHECK(it == container.end());

  // test range-based for
  int count = 0;
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
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace ndn::tests
