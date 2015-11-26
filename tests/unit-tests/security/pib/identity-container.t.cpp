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

#include "security/pib/identity-container.hpp"
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
BOOST_FIXTURE_TEST_SUITE(TestIdentityContainer, PibDataFixture)

using pib::Pib;

BOOST_AUTO_TEST_CASE(Basic)
{
  auto pibImpl = make_shared<PibMemory>();

  // start with an empty container
  IdentityContainer container(pibImpl);
  BOOST_CHECK_EQUAL(container.size(), 0);
  BOOST_CHECK_EQUAL(container.getLoadedIdentities().size(), 0);

  // add the first identity
  Identity identity11 = container.add(id1);
  BOOST_CHECK_EQUAL(identity11.getName(), id1);
  BOOST_CHECK_EQUAL(container.size(), 1);
  BOOST_CHECK_EQUAL(container.getLoadedIdentities().size(), 1);
  BOOST_CHECK(container.find(id1) != container.end());

  // add the same identity again
  Identity identity12 = container.add(id1);
  BOOST_CHECK_EQUAL(identity12.getName(), id1);
  BOOST_CHECK_EQUAL(container.size(), 1);
  BOOST_CHECK_EQUAL(container.getLoadedIdentities().size(), 1);
  BOOST_CHECK(container.find(id1) != container.end());

  // add the second identity
  Identity identity21 = container.add(id2);
  BOOST_CHECK_EQUAL(identity21.getName(), id2);
  BOOST_CHECK_EQUAL(container.size(), 2);
  BOOST_CHECK_EQUAL(container.getLoadedIdentities().size(), 2);
  BOOST_CHECK(container.find(id1) != container.end());
  BOOST_CHECK(container.find(id2) != container.end());

  // get identities
  BOOST_REQUIRE_NO_THROW(container.get(id1));
  BOOST_REQUIRE_NO_THROW(container.get(id2));
  BOOST_CHECK_THROW(container.get(Name("/non-existing")), Pib::Error);

  // check identity
  Identity identity1 = container.get(id1);
  Identity identity2 = container.get(id2);
  BOOST_CHECK_EQUAL(identity1.getName(), id1);
  BOOST_CHECK_EQUAL(identity2.getName(), id2);

  // create another container from the same PibImpl
  // cache should be empty
  IdentityContainer container2(pibImpl);
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getLoadedIdentities().size(), 0);

  // get key, cache should be filled
  BOOST_REQUIRE_NO_THROW(container2.get(id1));
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getLoadedIdentities().size(), 1);

  BOOST_REQUIRE_NO_THROW(container2.get(id2));
  BOOST_CHECK_EQUAL(container2.size(), 2);
  BOOST_CHECK_EQUAL(container2.getLoadedIdentities().size(), 2);

  // remove a key
  container2.remove(id1);
  BOOST_CHECK_EQUAL(container2.size(), 1);
  BOOST_CHECK_EQUAL(container2.getLoadedIdentities().size(), 1);
  BOOST_CHECK(container2.find(id1) == container2.end());
  BOOST_CHECK(container2.find(id2) != container2.end());

  // remove another key
  container2.remove(id2);
  BOOST_CHECK_EQUAL(container2.size(), 0);
  BOOST_CHECK_EQUAL(container2.getLoadedIdentities().size(), 0);
  BOOST_CHECK(container2.find(id2) == container2.end());

}

BOOST_AUTO_TEST_CASE(Iterator)
{
  auto pibImpl = make_shared<PibMemory>();
  IdentityContainer container(pibImpl);
  container.add(id1);
  container.add(id2);

  std::set<Name> idNames;
  idNames.insert(id1);
  idNames.insert(id2);

  IdentityContainer::const_iterator it = container.begin();
  std::set<Name>::const_iterator testIt = idNames.begin();
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  it++;
  testIt++;
  BOOST_CHECK_EQUAL((*it).getName(), *testIt);
  ++it;
  testIt++;
  BOOST_CHECK(it == container.end());

  size_t count = 0;
  testIt = idNames.begin();
  for (const auto& identity : container) {
    BOOST_CHECK_EQUAL(identity.getName(), *testIt);
    testIt++;
    count++;
  }
  BOOST_CHECK_EQUAL(count, 2);

  BOOST_CHECK(IdentityContainer::const_iterator() == IdentityContainer::const_iterator());
  BOOST_CHECK(IdentityContainer::const_iterator() == container.end());
  BOOST_CHECK(container.end() == IdentityContainer::const_iterator());
}

BOOST_AUTO_TEST_SUITE_END() // TestIdentityContainer
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
