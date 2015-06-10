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

#include "security/signing-info.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecuritySigningInfo)

BOOST_AUTO_TEST_CASE(Basic)
{
  Name id("/id");
  Name key("/key");
  Name cert("/cert");

  SigningInfo info;

  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_NULL);
  BOOST_CHECK_EQUAL(info.getSignerName(), SigningInfo::EMPTY_NAME);
  BOOST_CHECK_EQUAL(info.getDigestAlgorithm(), DIGEST_ALGORITHM_SHA256);

  const SignatureInfo& sigInfo = info.getSignatureInfo();
  BOOST_CHECK_EQUAL(sigInfo.getSignatureType(), -1);
  BOOST_CHECK_EQUAL(sigInfo.hasKeyLocator(), false);

  info.setSigningIdentity(id);
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_ID);
  BOOST_CHECK_EQUAL(info.getSignerName(), id);

  SigningInfo infoId(SigningInfo::SIGNER_TYPE_ID, id);
  BOOST_CHECK_EQUAL(infoId.getSignerType(), SigningInfo::SIGNER_TYPE_ID);
  BOOST_CHECK_EQUAL(infoId.getSignerName(), id);

  info.setSigningKeyName(key);
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_KEY);
  BOOST_CHECK_EQUAL(info.getSignerName(), key);

  SigningInfo infoKey(SigningInfo::SIGNER_TYPE_KEY, key);
  BOOST_CHECK_EQUAL(infoKey.getSignerType(), SigningInfo::SIGNER_TYPE_KEY);
  BOOST_CHECK_EQUAL(infoKey.getSignerName(), key);

  info.setSigningCertName(cert);
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_CERT);
  BOOST_CHECK_EQUAL(info.getSignerName(), cert);

  SigningInfo infoCert(SigningInfo::SIGNER_TYPE_CERT, cert);
  BOOST_CHECK_EQUAL(infoCert.getSignerType(), SigningInfo::SIGNER_TYPE_CERT);
  BOOST_CHECK_EQUAL(infoCert.getSignerName(), cert);

  info.setSha256Signing();
  BOOST_CHECK_EQUAL(info.getSignerType(), SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(info.getSignerName(), SigningInfo::EMPTY_NAME);

  SigningInfo infoSha(SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(infoSha.getSignerType(), SigningInfo::SIGNER_TYPE_SHA256);
  BOOST_CHECK_EQUAL(infoSha.getSignerName(), SigningInfo::EMPTY_NAME);
}

BOOST_AUTO_TEST_CASE(CustomSignatureInfo)
{
  SigningInfo info1;
  BOOST_CHECK(info1.getSignatureInfo() == SignatureInfo());

  SignatureInfo si;
  si.setKeyLocator(Name("ndn:/test/key/locator"));
  info1.setSignatureInfo(si);

  BOOST_CHECK(info1.getSignatureInfo() == si);

  SigningInfo info2(SigningInfo::SIGNER_TYPE_NULL, SigningInfo::EMPTY_NAME, si);
  BOOST_CHECK(info2.getSignatureInfo() == si);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace security
} // namespace ndn
