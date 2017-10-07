/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "security/pib/identity.hpp"
#include "security/pib/pib.hpp"
#include "security/pib/pib-memory.hpp"
#include "security/pib/detail/identity-impl.hpp"

#include "boost-test.hpp"
#include "pib-data-fixture.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace tests {

using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_FIXTURE_TEST_SUITE(TestIdentity, PibDataFixture)

using pib::Pib;

BOOST_AUTO_TEST_CASE(ValidityChecking)
{
  using security::pib::detail::IdentityImpl;

  Identity id;

  BOOST_CHECK_EQUAL(static_cast<bool>(id), false);
  BOOST_CHECK_EQUAL(!id, true);

  if (id)
    BOOST_CHECK(false);
  else
    BOOST_CHECK(true);

  auto identityImpl = make_shared<IdentityImpl>(id1, make_shared<PibMemory>(), true);
  id = Identity(identityImpl);

  BOOST_CHECK_EQUAL(static_cast<bool>(id), true);
  BOOST_CHECK_EQUAL(!id, false);

  if (id)
    BOOST_CHECK(true);
  else
    BOOST_CHECK(false);
}

/**
 * pib::Identity is a wrapper of pib::detail::IdentityImpl.  Since the functionalities of
 * IdentityImpl have already been tested in detail/identity-impl.t.cpp, we only test the shared
 * property of pib::Identity in this test case.
 */
BOOST_AUTO_TEST_CASE(Share)
{
  using security::pib::detail::IdentityImpl;

  auto identityImpl = make_shared<IdentityImpl>(id1, make_shared<pib::PibMemory>(), true);
  Identity identity1(identityImpl);
  Identity identity2(identityImpl);
  BOOST_CHECK_EQUAL(identity1, identity2);
  BOOST_CHECK_NE(identity1, Identity());
  BOOST_CHECK_EQUAL(Identity(), Identity());

  identity1.addKey(id1Key1.data(), id1Key1.size(), id1Key1Name);
  BOOST_CHECK_NO_THROW(identity2.getKey(id1Key1Name));
  identity2.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), Pib::Error);

  identity1.setDefaultKey(id1Key1.data(), id1Key1.size(), id1Key1Name);
  BOOST_CHECK_NO_THROW(identity2.getDefaultKey());
}

BOOST_AUTO_TEST_SUITE_END() // TestIdentity
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
