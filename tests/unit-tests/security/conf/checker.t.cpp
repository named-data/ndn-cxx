/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "security/conf/checker.hpp"
#include "security/key-chain.hpp"
#include "identity-management-fixture.hpp"
#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace conf {
namespace tests {

using namespace ndn::tests;

BOOST_FIXTURE_TEST_SUITE(SecurityConfChecker, IdentityManagementFixture)

BOOST_AUTO_TEST_CASE(CustomizedCheckerTest1)
{
  using security::conf::CustomizedChecker;
  using security::conf::KeyLocatorChecker;
  using security::conf::RelationKeyLocatorNameChecker;

  Name identity("/SecurityTestConfChecker/CustomizedCheckerTest1");
  BOOST_REQUIRE(addIdentity(identity, RsaKeyParams()));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name identity2("/SecurityTestConfChecker/CustomizedCheckerTest1Wrong");
  BOOST_REQUIRE(addIdentity(identity2, RsaKeyParams()));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);

  Name packetName("/SecurityTestConfChecker/CustomizedCheckerTest1/Data");
  shared_ptr<Data> data1 = make_shared<Data>(packetName);
  m_keyChain.sign(*data1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data2 = make_shared<Data>(packetName);
  m_keyChain.sign(*data2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  shared_ptr<Interest> interest1 = make_shared<Interest>(packetName);
  m_keyChain.sign(*interest1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Interest> interest2 = make_shared<Interest>(packetName);
  m_keyChain.sign(*interest2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  int8_t result = 0;


  shared_ptr<RelationKeyLocatorNameChecker> keyLocatorCheckerEqual1 =
    make_shared<RelationKeyLocatorNameChecker>(certName.getPrefix(-1),
                                               KeyLocatorChecker::RELATION_EQUAL);
  CustomizedChecker checker1(tlv::SignatureSha256WithRsa, keyLocatorCheckerEqual1);

  result = checker1.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker1.check(*interest1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*interest2);
  BOOST_CHECK_EQUAL(result, -1);


  shared_ptr<RelationKeyLocatorNameChecker> keyLocatorCheckerEqual2 =
    make_shared<RelationKeyLocatorNameChecker>(identity,
                                               KeyLocatorChecker::RELATION_EQUAL);
  CustomizedChecker checker2(tlv::SignatureSha256WithRsa, keyLocatorCheckerEqual2);

  result = checker2.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker2.check(*interest1);
  BOOST_CHECK_EQUAL(result, -1);


  shared_ptr<RelationKeyLocatorNameChecker> keyLocatorCheckerPrefix1 =
    make_shared<RelationKeyLocatorNameChecker>(certName.getPrefix(-1),
                                               KeyLocatorChecker::RELATION_IS_PREFIX_OF);
  CustomizedChecker checker3(tlv::SignatureSha256WithRsa, keyLocatorCheckerPrefix1);

  result = checker3.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker3.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);


  shared_ptr<RelationKeyLocatorNameChecker> keyLocatorCheckerPrefix2 =
    make_shared<RelationKeyLocatorNameChecker>(identity,
                                               KeyLocatorChecker::RELATION_IS_PREFIX_OF);
  CustomizedChecker checker4(tlv::SignatureSha256WithRsa, keyLocatorCheckerPrefix2);

  result = checker4.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker4.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);


  shared_ptr<RelationKeyLocatorNameChecker> keyLocatorCheckerStrict1 =
    make_shared<RelationKeyLocatorNameChecker>(certName.getPrefix(-1),
                                               KeyLocatorChecker::RELATION_IS_STRICT_PREFIX_OF);
  CustomizedChecker checker5(tlv::SignatureSha256WithRsa, keyLocatorCheckerStrict1);

  result = checker5.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker5.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);

  shared_ptr<RelationKeyLocatorNameChecker> keyLocatorCheckerStrict2 =
    make_shared<RelationKeyLocatorNameChecker>(identity,
                                               KeyLocatorChecker::RELATION_IS_STRICT_PREFIX_OF);
  CustomizedChecker checker6(tlv::SignatureSha256WithRsa, keyLocatorCheckerStrict2);

  result = checker6.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker6.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);
}

BOOST_AUTO_TEST_CASE(CustomizedCheckerTest2)
{
  using security::conf::CustomizedChecker;
  using security::conf::KeyLocatorChecker;
  using security::conf::RegexKeyLocatorNameChecker;

  Name identity("/SecurityTestConfChecker/CustomizedCheckerTest2");
  BOOST_REQUIRE(addIdentity(identity, RsaKeyParams()));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name identity2("/SecurityTestConfChecker/CustomizedCheckerTest2Wrong");
  BOOST_REQUIRE(addIdentity(identity2, RsaKeyParams()));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);

  Name packetName("/SecurityTestConfChecker/CustomizedCheckerTest2/Data");
  shared_ptr<Data> data1 = make_shared<Data>(packetName);
  m_keyChain.sign(*data1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data2 = make_shared<Data>(packetName);
  m_keyChain.sign(*data2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  shared_ptr<Interest> interest1 = make_shared<Interest>(packetName);
  m_keyChain.sign(*interest1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Interest> interest2 = make_shared<Interest>(packetName);
  m_keyChain.sign(*interest2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  int8_t result = 0;


  shared_ptr<RegexKeyLocatorNameChecker> keyLocatorCheckerRegex1 =
    make_shared<RegexKeyLocatorNameChecker>(
      Regex("^<SecurityTestConfChecker><CustomizedCheckerTest2>"));
  CustomizedChecker checker1(tlv::SignatureSha256WithRsa, keyLocatorCheckerRegex1);

  result = checker1.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker1.check(*interest1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*interest2);
  BOOST_CHECK_EQUAL(result, -1);
}

BOOST_AUTO_TEST_CASE(CustomizedCheckerTest3)
{
  using security::conf::CustomizedChecker;
  using security::conf::KeyLocatorChecker;
  using security::conf::RegexKeyLocatorNameChecker;

  Name identity("/SecurityTestConfChecker/CustomizedCheckerTest3");
  BOOST_REQUIRE(addIdentity(identity, EcdsaKeyParams()));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name identity2("/SecurityTestConfChecker/CustomizedCheckerTest3Wrong");
  BOOST_REQUIRE(addIdentity(identity2, EcdsaKeyParams()));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);

  Name packetName("/SecurityTestConfChecker/CustomizedCheckerTest3/Data");
  shared_ptr<Data> data1 = make_shared<Data>(packetName);
  m_keyChain.sign(*data1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data2 = make_shared<Data>(packetName);
  m_keyChain.sign(*data2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  shared_ptr<Interest> interest1 = make_shared<Interest>(packetName);
  m_keyChain.sign(*interest1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Interest> interest2 = make_shared<Interest>(packetName);
  m_keyChain.sign(*interest2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  int8_t result = 0;


  shared_ptr<RegexKeyLocatorNameChecker> keyLocatorCheckerRegex1 =
    make_shared<RegexKeyLocatorNameChecker>(
      Regex("^<SecurityTestConfChecker><CustomizedCheckerTest3>"));
  CustomizedChecker checker1(tlv::SignatureSha256WithEcdsa, keyLocatorCheckerRegex1);

  result = checker1.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker1.check(*interest1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*interest2);
  BOOST_CHECK_EQUAL(result, -1);


  CustomizedChecker checker2(tlv::SignatureSha256WithRsa, keyLocatorCheckerRegex1);

  result = checker2.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);
}

BOOST_AUTO_TEST_CASE(HierarchicalCheckerTest1)
{
  using security::conf::HierarchicalChecker;

  Name identity("/SecurityTestConfChecker/HierarchicalCheckerTest1");
  BOOST_REQUIRE(addIdentity(identity, EcdsaKeyParams()));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);

  Name identity2("/SecurityTestConfChecker/HierarchicalCheckerTest1/Data");
  BOOST_REQUIRE(addIdentity(identity2, RsaKeyParams()));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);

  Name packetName("/SecurityTestConfChecker/HierarchicalCheckerTest1/Data");
  Name packetName2("/SecurityTestConfChecker");
  Name packetName3("/SecurityTestConfChecker/HierarchicalCheckerTest1");

  shared_ptr<Data> data1 = make_shared<Data>(packetName);
  m_keyChain.sign(*data1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data2 = make_shared<Data>(packetName2);
  m_keyChain.sign(*data2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data3 = make_shared<Data>(packetName3);
  m_keyChain.sign(*data3,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data4 = make_shared<Data>(packetName);
  m_keyChain.sign(*data4,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  shared_ptr<Data> data5 = make_shared<Data>(packetName2);
  m_keyChain.sign(*data5,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  shared_ptr<Data> data6 = make_shared<Data>(packetName3);
  m_keyChain.sign(*data6,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  int8_t result = 0;

  HierarchicalChecker checker1(tlv::SignatureSha256WithEcdsa);

  result = checker1.check(*data1);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker1.check(*data3);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker1.check(*data4);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker1.check(*data5);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker1.check(*data6);
  BOOST_CHECK_EQUAL(result, -1);


  HierarchicalChecker checker2(tlv::SignatureSha256WithRsa);

  result = checker2.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker2.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker2.check(*data3);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker2.check(*data4);
  BOOST_CHECK_EQUAL(result, 0);

  result = checker2.check(*data5);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker2.check(*data6);
  BOOST_CHECK_EQUAL(result, -1);
}

BOOST_AUTO_TEST_CASE(FixedSignerCheckerTest1)
{
  using security::conf::FixedSignerChecker;

  Name identity("/SecurityTestConfChecker/FixedSignerCheckerTest1");
  BOOST_REQUIRE(addIdentity(identity, EcdsaKeyParams()));
  Name certName = m_keyChain.getDefaultCertificateNameForIdentity(identity);
  shared_ptr<v1::IdentityCertificate> cert1 = m_keyChain.getCertificate(certName);

  Name identity2("/SecurityTestConfChecker/FixedSignerCheckerTest1Wrong");
  BOOST_REQUIRE(addIdentity(identity2, RsaKeyParams()));
  Name certName2 = m_keyChain.getDefaultCertificateNameForIdentity(identity2);
  shared_ptr<v1::IdentityCertificate> cert2 = m_keyChain.getCertificate(certName2);

  Name packetName("/Test/Data");

  shared_ptr<Data> data1 = make_shared<Data>(packetName);
  m_keyChain.sign(*data1,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity));

  shared_ptr<Data> data2 = make_shared<Data>(packetName);
  m_keyChain.sign(*data2,
                  security::SigningInfo(security::SigningInfo::SIGNER_TYPE_ID,
                                        identity2));

  std::vector<shared_ptr<v1::IdentityCertificate> > certSet1;
  certSet1.push_back(cert1);

  std::vector<shared_ptr<v1::IdentityCertificate> > certSet2;
  certSet2.push_back(cert2);


  int8_t result = 0;

  FixedSignerChecker checker1(tlv::SignatureSha256WithEcdsa, certSet1);

  result = checker1.check(*data1);
  BOOST_CHECK_EQUAL(result, 1);

  result = checker1.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);


  FixedSignerChecker checker2(tlv::SignatureSha256WithRsa, certSet1);

  result = checker2.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker2.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);


  FixedSignerChecker checker3(tlv::SignatureSha256WithEcdsa, certSet2);

  result = checker3.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker3.check(*data2);
  BOOST_CHECK_EQUAL(result, -1);


  FixedSignerChecker checker4(tlv::SignatureSha256WithRsa, certSet2);

  result = checker4.check(*data1);
  BOOST_CHECK_EQUAL(result, -1);

  result = checker4.check(*data2);
  BOOST_CHECK_EQUAL(result, 1);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace conf
} // namespace security
} // namespace ndn
