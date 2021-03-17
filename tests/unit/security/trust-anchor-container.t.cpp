/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/security/trust-anchor-container.hpp"
#include "ndn-cxx/util/io.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"
#include "tests/unit/clock-fixture.hpp"

#include <boost/filesystem.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

using namespace ndn::tests;

/**
 * This fixture creates a directory and prepares two certificates.
 * cert1 is written to a file under the directory, while cert2 is not.
 */
class TrustAnchorContainerFixture : public ClockFixture, public KeyChainFixture
{
public:
  TrustAnchorContainerFixture()
  {
    boost::filesystem::create_directories(certDirPath);

    identity1 = m_keyChain.createIdentity("/TestAnchorContainer/First");
    cert1 = identity1.getDefaultKey().getDefaultCertificate();
    saveCert(cert1, certPath1.string());

    identity2 = m_keyChain.createIdentity("/TestAnchorContainer/Second");
    cert2 = identity2.getDefaultKey().getDefaultCertificate();
    saveCert(cert2, certPath2.string());
  }

  ~TrustAnchorContainerFixture() override
  {
    boost::filesystem::remove_all(certDirPath);
  }

public:
  const boost::filesystem::path certDirPath{boost::filesystem::path(UNIT_TESTS_TMPDIR) / "test-cert-dir"};
  const boost::filesystem::path certPath1{certDirPath / "trust-anchor-1.cert"};
  const boost::filesystem::path certPath2{certDirPath / "trust-anchor-2.cert"};

  TrustAnchorContainer anchorContainer;

  Identity identity1;
  Identity identity2;

  Certificate cert1;
  Certificate cert2;
};

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestTrustAnchorContainer, TrustAnchorContainerFixture)

// one static group and one dynamic group created from file
BOOST_AUTO_TEST_CASE(Insert)
{
  // Static
  anchorContainer.insert("group1", Certificate(cert1));
  BOOST_CHECK(anchorContainer.find(cert1.getName()) != nullptr);
  BOOST_CHECK(anchorContainer.find(identity1.getName()) != nullptr);
  const Certificate* cert = anchorContainer.find(cert1.getName());
  BOOST_CHECK_NO_THROW(anchorContainer.insert("group1", Certificate(cert1)));
  BOOST_CHECK_EQUAL(cert, anchorContainer.find(cert1.getName())); // still the same instance of the certificate
  // cannot add dynamic group when static already exists
  BOOST_CHECK_THROW(anchorContainer.insert("group1", certPath1.string(), 1_s), TrustAnchorContainer::Error);
  BOOST_CHECK_EQUAL(anchorContainer.getGroup("group1").size(), 1);
  BOOST_CHECK_EQUAL(anchorContainer.size(), 1);

  // From file
  anchorContainer.insert("group2", certPath2.string(), 1_s);
  BOOST_CHECK(anchorContainer.find(cert2.getName()) != nullptr);
  BOOST_CHECK(anchorContainer.find(identity2.getName()) != nullptr);
  BOOST_CHECK_THROW(anchorContainer.insert("group2", Certificate(cert2)), TrustAnchorContainer::Error);
  BOOST_CHECK_THROW(anchorContainer.insert("group2", certPath2.string(), 1_s), TrustAnchorContainer::Error);
  BOOST_CHECK_EQUAL(anchorContainer.getGroup("group2").size(), 1);
  BOOST_CHECK_EQUAL(anchorContainer.size(), 2);

  boost::filesystem::remove(certPath2);
  advanceClocks(1_s, 11);

  BOOST_CHECK(anchorContainer.find(identity2.getName()) == nullptr);
  BOOST_CHECK(anchorContainer.find(cert2.getName()) == nullptr);
  BOOST_CHECK_EQUAL(anchorContainer.getGroup("group2").size(), 0);
  BOOST_CHECK_EQUAL(anchorContainer.size(), 1);

  TrustAnchorGroup& group = anchorContainer.getGroup("group1");
  auto staticGroup = dynamic_cast<StaticTrustAnchorGroup*>(&group);
  BOOST_REQUIRE(staticGroup != nullptr);
  BOOST_CHECK_EQUAL(staticGroup->size(), 1);
  staticGroup->remove(cert1.getName());
  BOOST_CHECK_EQUAL(staticGroup->size(), 0);
  BOOST_CHECK_EQUAL(anchorContainer.size(), 0);

  BOOST_CHECK_THROW(anchorContainer.getGroup("non-existing-group"), TrustAnchorContainer::Error);
}

BOOST_AUTO_TEST_CASE(DynamicAnchorFromDir)
{
  boost::filesystem::remove(certPath2);

  anchorContainer.insert("group", certDirPath.string(), 1_s, true /* isDir */);

  BOOST_CHECK(anchorContainer.find(identity1.getName()) != nullptr);
  BOOST_CHECK(anchorContainer.find(identity2.getName()) == nullptr);
  BOOST_CHECK_EQUAL(anchorContainer.getGroup("group").size(), 1);

  saveCert(cert2, certPath2.string());

  advanceClocks(100_ms, 11);

  BOOST_CHECK(anchorContainer.find(identity1.getName()) != nullptr);
  BOOST_CHECK(anchorContainer.find(identity2.getName()) != nullptr);
  BOOST_CHECK_EQUAL(anchorContainer.getGroup("group").size(), 2);

  boost::filesystem::remove_all(certDirPath);

  advanceClocks(100_ms, 11);

  BOOST_CHECK(anchorContainer.find(identity1.getName()) == nullptr);
  BOOST_CHECK(anchorContainer.find(identity2.getName()) == nullptr);
  BOOST_CHECK_EQUAL(anchorContainer.getGroup("group").size(), 0);
}

BOOST_AUTO_TEST_CASE(FindByInterest)
{
  anchorContainer.insert("group1", certPath1.string(), 1_s);
  Interest interest1;
  interest1.setCanBePrefix(true);
  interest1.setName(identity1.getName());
  BOOST_CHECK(anchorContainer.find(interest1) != nullptr);
  interest1.setName(identity1.getName().getPrefix(-1));
  BOOST_CHECK(anchorContainer.find(interest1) != nullptr);
  interest1.setName(Name(identity1.getName()).appendVersion());
  BOOST_CHECK(anchorContainer.find(interest1) == nullptr);

  auto cert3 = makeCert(identity1.getDefaultKey(), "3");
  auto cert4 = makeCert(identity1.getDefaultKey(), "4");
  auto cert5 = makeCert(identity1.getDefaultKey(), "5");

  Certificate cert3Copy = cert3;
  anchorContainer.insert("group2", std::move(cert3Copy));
  anchorContainer.insert("group3", std::move(cert4));
  anchorContainer.insert("group4", std::move(cert5));

  auto interest3 = Interest(cert3.getKeyName()).setCanBePrefix(true);
  const Certificate* foundCert = anchorContainer.find(interest3);
  BOOST_REQUIRE(foundCert != nullptr);
  BOOST_CHECK(interest3.getName().isPrefixOf(foundCert->getName()));
  BOOST_CHECK_EQUAL(foundCert->getName(), cert3.getName());
}

BOOST_AUTO_TEST_SUITE_END() // TestTrustAnchorContainer
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn
