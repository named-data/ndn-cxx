/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "security/certificate-container.hpp"
#include "security/pib.hpp"
#include "security/pib-memory.hpp"
#include "pib-data-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityCertificateContainer)

BOOST_FIXTURE_TEST_CASE(TestCertificateContainer, PibDataFixture)
{
  auto pibImpl = make_shared<PibMemory>();
  Pib pib("pib-memory", "", pibImpl);

  Identity identity1 = pib.addIdentity(id1);
  Key key11 = identity1.addKey(id1Key1, id1Key1Name.get(-1));
  key11.addCertificate(id1Key1Cert1);
  key11.addCertificate(id1Key1Cert2);

  CertificateContainer container = key11.getCertificates();
  BOOST_CHECK_EQUAL(container.size(), 2);
  BOOST_CHECK(container.find(id1Key1Cert1.getName()) != container.end());
  BOOST_CHECK(container.find(id1Key1Cert2.getName()) != container.end());

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
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
