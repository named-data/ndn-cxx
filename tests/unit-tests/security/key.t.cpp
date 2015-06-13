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

#include "security/key.hpp"
#include "security/pib.hpp"
#include "security/pib-memory.hpp"
#include "pib-data-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityKey)

BOOST_FIXTURE_TEST_CASE(ValidityChecking, PibDataFixture)
{
  // key
  Key key;

  BOOST_CHECK_EQUAL(static_cast<bool>(key), false);
  BOOST_CHECK_EQUAL(!key, true);

  if (key)
    BOOST_CHECK(false);
  else
    BOOST_CHECK(true);

  auto pibImpl = make_shared<PibMemory>();
  key = Key(id1, id1Key1Name.get(-1), id1Key1, pibImpl);

  BOOST_CHECK_EQUAL(static_cast<bool>(key), true);
  BOOST_CHECK_EQUAL(!key, false);

  if (key)
    BOOST_CHECK(true);
  else
    BOOST_CHECK(false);
}

BOOST_FIXTURE_TEST_CASE(TestCertificateOperation, PibDataFixture)
{
  auto pibImpl = make_shared<PibMemory>();

  Key key11(id1, id1Key1Name.get(-1), id1Key1, pibImpl);

  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  key11.addCertificate(id1Key1Cert1);
  BOOST_CHECK_NO_THROW(key11.getCertificate(id1Key1Cert1.getName()));
  key11.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);

  BOOST_CHECK_THROW(key11.getDefaultCertificate(), Pib::Error);
  BOOST_REQUIRE_THROW(key11.setDefaultCertificate(id1Key1Cert1.getName()), Pib::Error);
  BOOST_REQUIRE_NO_THROW(key11.setDefaultCertificate(id1Key1Cert1));
  BOOST_REQUIRE_NO_THROW(key11.getDefaultCertificate());

  const IdentityCertificate& defaultCert = key11.getDefaultCertificate();
  BOOST_CHECK_EQUAL_COLLECTIONS(defaultCert.wireEncode().wire(),
                                defaultCert.wireEncode().wire() + defaultCert.wireEncode().size(),
                                id1Key1Cert1.wireEncode().wire(),
                                id1Key1Cert1.wireEncode().wire() + id1Key1Cert1.wireEncode().size());

  key11.removeCertificate(id1Key1Cert1.getName());
  BOOST_CHECK_THROW(key11.getCertificate(id1Key1Cert1.getName()), Pib::Error);
  BOOST_CHECK_THROW(key11.getDefaultCertificate(), Pib::Error);
}


BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
