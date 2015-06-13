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

#include "security/pib.hpp"
#include "security/pib-memory.hpp"
#include "pib-data-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityPib)

BOOST_FIXTURE_TEST_CASE(ValidityChecking, PibDataFixture)
{
  auto pibImpl = make_shared<PibMemory>();
  Pib pib("pib-memory", "", pibImpl);

  Identity id = pib.addIdentity(id1);

  BOOST_CHECK_EQUAL(bool(id), true);
  BOOST_CHECK_EQUAL(!id, false);

  if (id)
    BOOST_CHECK(true);
  else
    BOOST_CHECK(false);

  // key
  Key key = id.addKey(id1Key1, id1Key1Name.get(-1));

  BOOST_CHECK_EQUAL(bool(key), true);
  BOOST_CHECK_EQUAL(!key, false);

  if (key)
    BOOST_CHECK(true);
  else
    BOOST_CHECK(false);
}

BOOST_FIXTURE_TEST_CASE(TestIdentityOperation, PibDataFixture)
{
  auto pibImpl = make_shared<PibMemory>();
  Pib pib("pib-memory", "", pibImpl);

  BOOST_CHECK_THROW(pib.getIdentity(id1), Pib::Error);
  Identity identity1 = pib.addIdentity(id1);
  BOOST_CHECK_NO_THROW(pib.getIdentity(id1));
  pib.removeIdentity(id1);
  BOOST_CHECK_THROW(pib.getIdentity(id1), Pib::Error);

  BOOST_CHECK_THROW(pib.getDefaultIdentity(), Pib::Error);
  BOOST_REQUIRE_NO_THROW(pib.setDefaultIdentity(id1));
  BOOST_REQUIRE_NO_THROW(pib.getDefaultIdentity());
  BOOST_CHECK_EQUAL(pib.getDefaultIdentity().getName(), id1);
  pib.removeIdentity(id1);
  BOOST_CHECK_THROW(pib.getIdentity(id1), Pib::Error);
  BOOST_CHECK_THROW(pib.getDefaultIdentity(), Pib::Error);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
