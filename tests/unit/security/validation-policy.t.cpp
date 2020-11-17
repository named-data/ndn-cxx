/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#include "ndn-cxx/security/validation-policy.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestValidationPolicy, ndn::tests::KeyChainFixture)

BOOST_AUTO_TEST_CASE(ExtractIdentityNameFromKeyLocator)
{
  auto id = m_keyChain.createIdentity("/random/identity");

  auto keyName = id.getDefaultKey().getName();
  auto certName = id.getDefaultKey().getDefaultCertificate().getName();
  auto partialCertName = id.getDefaultKey().getDefaultCertificate().getName().getPrefix(-1);

  BOOST_CHECK_EQUAL(extractIdentityNameFromKeyLocator(keyName), "/random/identity");
  BOOST_CHECK_EQUAL(extractIdentityNameFromKeyLocator(certName), "/random/identity");
  BOOST_CHECK_EQUAL(extractIdentityNameFromKeyLocator(partialCertName), "/random/identity");

  BOOST_CHECK_THROW(extractIdentityNameFromKeyLocator(Name("/name/without/key/component")), std::runtime_error);
  BOOST_CHECK_THROW(extractIdentityNameFromKeyLocator(Name("/name/with/KEY/but/in/a/wrong/place")), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END() // TestValidationPolicy
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn
