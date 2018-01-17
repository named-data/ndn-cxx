/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "security/v2/trust-anchor-container.hpp"
#include "util/io.hpp"

#include "../../identity-management-time-fixture.hpp"
#include "boost-test.hpp"

#include <boost/filesystem.hpp>

namespace ndn {
namespace security {
namespace v2 {
namespace tests {

using namespace ndn::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(V2)

/**
 * This fixture creates a directory and prepares two certificates.
 * cert1 is written to a file under the directory, while cert2 is not.
 */
class AnchorContainerTestFixture : public IdentityManagementTimeFixture
{
public:
  AnchorContainerTestFixture()
  {
    boost::filesystem::create_directory(boost::filesystem::path(UNIT_TEST_CONFIG_PATH));

    certDirPath = boost::filesystem::path(UNIT_TEST_CONFIG_PATH) / std::string("test-cert-dir");
    boost::filesystem::create_directory(certDirPath);

    certPath1 = boost::filesystem::path(UNIT_TEST_CONFIG_PATH) /
      std::string("test-cert-dir") / std::string("trust-anchor-1.cert");

    certPath2 = boost::filesystem::path(UNIT_TEST_CONFIG_PATH) /
      std::string("test-cert-dir") / std::string("trust-anchor-2.cert");

    identity1 = addIdentity("/TestAnchorContainer/First");
    cert1 = identity1.getDefaultKey().getDefaultCertificate();
    saveCertToFile(cert1, certPath1.string());

    identity2 = addIdentity("/TestAnchorContainer/Second");
    cert2 = identity2.getDefaultKey().getDefaultCertificate();
    saveCertToFile(cert2, certPath2.string());
  }

  ~AnchorContainerTestFixture()
  {
    boost::filesystem::remove_all(UNIT_TEST_CONFIG_PATH);
  }

public:
  TrustAnchorContainer anchorContainer;

  boost::filesystem::path certDirPath;
  boost::filesystem::path certPath1;
  boost::filesystem::path certPath2;

  Identity identity1;
  Identity identity2;

  Certificate cert1;
  Certificate cert2;
};

BOOST_FIXTURE_TEST_SUITE(TestTrustAnchorContainer, AnchorContainerTestFixture)

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

  saveCertToFile(cert2, certPath2.string());

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

BOOST_FIXTURE_TEST_CASE(FindByInterest, AnchorContainerTestFixture)
{
  anchorContainer.insert("group1", certPath1.string(), 1_s);
  Interest interest(identity1.getName());
  BOOST_CHECK(anchorContainer.find(interest) != nullptr);
  Interest interest1(identity1.getName().getPrefix(-1));
  BOOST_CHECK(anchorContainer.find(interest1) != nullptr);
  Interest interest2(Name(identity1.getName()).appendVersion());
  BOOST_CHECK(anchorContainer.find(interest2) == nullptr);

  Certificate cert3 = addCertificate(identity1.getDefaultKey(), "3");
  Certificate cert4 = addCertificate(identity1.getDefaultKey(), "4");
  Certificate cert5 = addCertificate(identity1.getDefaultKey(), "5");

  Certificate cert3Copy = cert3;
  anchorContainer.insert("group2", std::move(cert3Copy));
  anchorContainer.insert("group3", std::move(cert4));
  anchorContainer.insert("group4", std::move(cert5));

  Interest interest3(cert3.getKeyName());
  const Certificate* foundCert = anchorContainer.find(interest3);
  BOOST_REQUIRE(foundCert != nullptr);
  BOOST_CHECK(interest3.getName().isPrefixOf(foundCert->getName()));
  BOOST_CHECK_EQUAL(foundCert->getName(), cert3.getName());

  interest3.setExclude(Exclude().excludeOne(cert3.getName().at(Certificate::ISSUER_ID_OFFSET)));
  foundCert = anchorContainer.find(interest3);
  BOOST_REQUIRE(foundCert != nullptr);
  BOOST_CHECK(interest3.getName().isPrefixOf(foundCert->getName()));
  BOOST_CHECK_NE(foundCert->getName(), cert3.getName());
}

BOOST_AUTO_TEST_SUITE_END() // TestTrustAnchorContainer
BOOST_AUTO_TEST_SUITE_END() // Detail
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace v2
} // namespace security
} // namespace ndn
