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

#include "security/signing-info.hpp"

#include "boost-test.hpp"

#include <boost/lexical_cast.hpp>
#include <sstream>

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(TestSigningInfo)

BOOST_AUTO_TEST_CASE(Basic)
{
  Name id("/my-identity");
  Name key("/my-key");
  Name cert("/my-cert");

  SigningInfo info;

  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_NULL);
  BOOST_CHECK_EQUAL(info.getSignerName(), SigningInfo::getEmptyName());
  BOOST_CHECK_EQUAL(info.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  const SignatureInfo& sigInfo = info.getSignatureInfo();
  BOOST_CHECK_EQUAL(sigInfo.getSignatureType(), -1);
  BOOST_CHECK_EQUAL(sigInfo.hasKeyLocator(), false);

  info.setSigningIdentity(id);
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_ID);
  BOOST_CHECK_EQUAL(info.getSignerName(), id);
  BOOST_CHECK_EQUAL(info.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoId(SigningInfo::SIGNER_TYPE_ID, id);
  BOOST_CHECK_EQUAL(infoId.getSignerType(), SigningInfo::SIGNER_TYPE_ID);
  BOOST_CHECK_EQUAL(infoId.getSignerName(), id);
  BOOST_CHECK_EQUAL(infoId.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  info.setSigningKeyName(key);
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_KEY);
  BOOST_CHECK_EQUAL(info.getSignerName(), key);
  BOOST_CHECK_EQUAL(info.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoKey(SigningInfo::SIGNER_TYPE_KEY, key);
  BOOST_CHECK_EQUAL(infoKey.getSignerType(), SigningInfo::SIGNER_TYPE_KEY);
  BOOST_CHECK_EQUAL(infoKey.getSignerName(), key);
  BOOST_CHECK_EQUAL(infoKey.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  info.setSigningCertName(cert);
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_CERT);
  BOOST_CHECK_EQUAL(info.getSignerName(), cert);
  BOOST_CHECK_EQUAL(info.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoCert(SigningInfo::SIGNER_TYPE_CERT, cert);
  BOOST_CHECK_EQUAL(infoCert.getSignerType(), SigningInfo::SIGNER_TYPE_CERT);
  BOOST_CHECK_EQUAL(infoCert.getSignerName(), cert);
  BOOST_CHECK_EQUAL(infoCert.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  info.setSha256Signing();
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(info.getSignerName(), SigningInfo::getEmptyName());
  BOOST_CHECK_EQUAL(info.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoSha(SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(infoSha.getSignerType(), SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(infoSha.getSignerName(), SigningInfo::getEmptyName());
  BOOST_CHECK_EQUAL(infoSha.getDigestAlgorithm(), DigestAlgorithm::SHA256);
}

BOOST_AUTO_TEST_CASE(CustomSignatureInfo)
{
  SigningInfo info1;
  BOOST_CHECK_EQUAL(info1.getSignatureInfo(), SignatureInfo());

  SignatureInfo si;
  si.setKeyLocator(Name("ndn:/test/key/locator"));
  info1.setSignatureInfo(si);

  BOOST_CHECK_EQUAL(info1.getSignatureInfo(), si);

  SigningInfo info2(SigningInfo::SIGNER_TYPE_NULL, SigningInfo::getEmptyName(), si);
  BOOST_CHECK_EQUAL(info2.getSignatureInfo(), si);
}

BOOST_AUTO_TEST_CASE(FromString)
{
  SigningInfo infoDefault("");
  BOOST_CHECK_EQUAL(infoDefault.getSignerType(), SigningInfo::SIGNER_TYPE_NULL);
  BOOST_CHECK_EQUAL(infoDefault.getSignerName(), SigningInfo::getEmptyName());
  BOOST_CHECK_EQUAL(infoDefault.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoId("id:/my-identity");
  BOOST_CHECK_EQUAL(infoId.getSignerType(), SigningInfo::SIGNER_TYPE_ID);
  BOOST_CHECK_EQUAL(infoId.getSignerName(), "/my-identity");
  BOOST_CHECK_EQUAL(infoId.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoKey("key:/my-key");
  BOOST_CHECK_EQUAL(infoKey.getSignerType(), SigningInfo::SIGNER_TYPE_KEY);
  BOOST_CHECK_EQUAL(infoKey.getSignerName(), "/my-key");
  BOOST_CHECK_EQUAL(infoKey.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoCert("cert:/my-cert");
  BOOST_CHECK_EQUAL(infoCert.getSignerType(), SigningInfo::SIGNER_TYPE_CERT);
  BOOST_CHECK_EQUAL(infoCert.getSignerName(), "/my-cert");
  BOOST_CHECK_EQUAL(infoCert.getDigestAlgorithm(), DigestAlgorithm::SHA256);

  SigningInfo infoSha("id:/localhost/identity/digest-sha256");
  BOOST_CHECK_EQUAL(infoSha.getSignerType(), SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(infoSha.getSignerName(), SigningInfo::getEmptyName());
  BOOST_CHECK_EQUAL(infoSha.getDigestAlgorithm(), DigestAlgorithm::SHA256);
}

BOOST_AUTO_TEST_CASE(ToString)
{
  // We can't use lexical_cast due to Boost Bug 6298.
  std::stringstream ss;
  ss << SigningInfo();
  BOOST_CHECK_EQUAL(ss.str(), "");

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                    SigningInfo(SigningInfo::SIGNER_TYPE_ID, "/my-identity")), "id:/my-identity");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                    SigningInfo(SigningInfo::SIGNER_TYPE_KEY, "/my-key")), "key:/my-key");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                    SigningInfo(SigningInfo::SIGNER_TYPE_CERT, "/my-cert")), "cert:/my-cert");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(
                    SigningInfo(SigningInfo::SIGNER_TYPE_SHA256)),
                    "id:/localhost/identity/digest-sha256");
}

BOOST_AUTO_TEST_CASE(Chaining)
{
  SigningInfo info = SigningInfo()
    .setSigningIdentity("/identity")
    .setSigningKeyName("/key/name")
    .setSigningCertName("/cert/name")
    .setPibIdentity(Identity())
    .setPibKey(Key())
    .setSha256Signing()
    .setDigestAlgorithm(DigestAlgorithm::SHA256)
    .setSignatureInfo(SignatureInfo());

  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(info), "id:/localhost/identity/digest-sha256");
}

BOOST_AUTO_TEST_CASE(OperatorEquals)
{
  // Check name equality
  SigningInfo info1("id:/my-id");
  SigningInfo info2("id:/my-id");
  BOOST_CHECK_EQUAL(info1, info2);
  // Change name, check inequality
  info2 = SigningInfo("id:/not-same-id");
  BOOST_CHECK_NE(info1, info2);

  // Check name, digest algo equality
  info1 = SigningInfo("id:/my-id");
  info2 = SigningInfo("id:/my-id");
  info1.setDigestAlgorithm(DigestAlgorithm::SHA256);
  info2.setDigestAlgorithm(DigestAlgorithm::SHA256);
  BOOST_CHECK_EQUAL(info1, info2);
  // Change digest algo, check inequality
  info2.setDigestAlgorithm(DigestAlgorithm::NONE);
  BOOST_CHECK_NE(info1, info2);

  // Check name, digest algo, signature info equality
  info1 = SigningInfo("id:/my-id");
  info2 = SigningInfo("id:/my-id");
  info1.setDigestAlgorithm(DigestAlgorithm::SHA256);
  info2.setDigestAlgorithm(DigestAlgorithm::SHA256);
  SignatureInfo sigInfo1(tlv::DigestSha256);
  info1.setSignatureInfo(sigInfo1);
  info2.setSignatureInfo(sigInfo1);
  BOOST_CHECK_EQUAL(info1, info2);
  // Change signature info, check inequality
  SignatureInfo sigInfo2(tlv::SignatureSha256WithRsa);
  info2.setSignatureInfo(sigInfo2);
  BOOST_CHECK_NE(info1, info2);
}

BOOST_AUTO_TEST_CASE(OperatorEqualsDifferentTypes)
{
  SigningInfo info1("key:/my-id/KEY/1");
  SigningInfo info2("key:/my-id/KEY/1");
  // Check equality for key type
  BOOST_CHECK_EQUAL(info1, info2);
  info2 = SigningInfo("id:/my-id");
  // Change signature type, check inequality
  BOOST_CHECK_NE(info1, info2);
  info2 = SigningInfo("key:/not-same-id/KEY/1");
  // Change key name, check inequality
  BOOST_CHECK_NE(info1, info2);

  info1 = SigningInfo("cert:/my-id/KEY/1/self/%FD01");
  info2 = SigningInfo("cert:/my-id/KEY/1/self/%FD01");
  // Check equality for cert type
  BOOST_CHECK_EQUAL(info1, info2);
  info2 = SigningInfo("cert:/not-my-id/KEY/1/other/%FD01");
  // Change cert name, check inequality
  BOOST_CHECK_NE(info1, info2);
  info2 = SigningInfo("id:/my-id");
  // Change signature type, check inequality
  BOOST_CHECK_NE(info1, info2);

  info1 = SigningInfo(SigningInfo::SIGNER_TYPE_NULL);
  info2 = SigningInfo(SigningInfo::SIGNER_TYPE_NULL);
  // Check equality for null type
  BOOST_CHECK_EQUAL(info1, info2);
  info2 = SigningInfo("id:/my-id");
  // Change signature type, check inequality
  BOOST_CHECK_NE(info1, info2);

  info1 = SigningInfo(SigningInfo::SIGNER_TYPE_SHA256);
  info2 = SigningInfo(SigningInfo::SIGNER_TYPE_SHA256);
  // Check equality for SHA256 digest type
  BOOST_CHECK_EQUAL(info1, info2);
  info2 = SigningInfo("id:/my-id");
  // Change signature type, check inequality
  BOOST_CHECK_NE(info1, info2);
}

BOOST_AUTO_TEST_SUITE_END() // TestSigningInfo
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
