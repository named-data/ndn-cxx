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

#include "security/key-container.hpp"
#include "security/pib.hpp"
#include "security/pib-memory.hpp"
#include "pib-data-fixture.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityKeyContainer)

BOOST_FIXTURE_TEST_CASE(TestKeyContainer, PibDataFixture)
{
  auto pibImpl = make_shared<PibMemory>();
  Pib pib("pib-memory", "", pibImpl);

  Identity identity1 = pib.addIdentity(id1);

  Key key11 = identity1.addKey(id1Key1, id1Key1Name.get(-1));
  Key key12 = identity1.addKey(id1Key2, id1Key2Name.get(-1));

  KeyContainer container = identity1.getKeys();
  BOOST_CHECK_EQUAL(container.size(), 2);
  BOOST_CHECK(container.find(id1Key1Name.get(-1)) != container.end());
  BOOST_CHECK(container.find(id1Key2Name.get(-1)) != container.end());

  std::set<name::Component> keyNames;
  keyNames.insert(id1Key1Name.get(-1));
  keyNames.insert(id1Key2Name.get(-1));

  KeyContainer::const_iterator it = container.begin();
  std::set<name::Component>::const_iterator testIt = keyNames.begin();
  BOOST_CHECK_EQUAL((*it).getKeyId(), *testIt);
  it++;
  testIt++;
  BOOST_CHECK_EQUAL((*it).getKeyId(), *testIt);
  ++it;
  testIt++;
  BOOST_CHECK(it == container.end());

  size_t count = 0;
  testIt = keyNames.begin();
  for (const auto& key : container) {
    BOOST_CHECK_EQUAL(key.getIdentity(), id1);
    BOOST_CHECK_EQUAL(key.getKeyId(), *testIt);
    testIt++;
    count++;
  }
  BOOST_CHECK_EQUAL(count, 2);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
