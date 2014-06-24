/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "security/validator-null.hpp"
#include "security/key-chain.hpp"
#include "util/time.hpp"

#include "boost-test.hpp"

namespace ndn {

using std::string;

BOOST_AUTO_TEST_SUITE(SecurityTestValidator)

void
onValidated(const shared_ptr<const Data>& data)
{
  BOOST_CHECK(true);
}

void
onValidationFailed(const shared_ptr<const Data>& data, const string& failureInfo)
{
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE(Null)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestValidator/Null");
  identity.appendVersion();

  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));

  Name dataName = identity;
  dataName.append("1");
  shared_ptr<Data> data = make_shared<Data>(dataName);

  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(*data, identity));

  ValidatorNull validator;

  // data must be a shared pointer
  validator.validate(*data,
                     bind(&onValidated, _1),
                     bind(&onValidationFailed, _1, _2));

  keyChain.deleteIdentity(identity);
}

const uint8_t ecdsaSigInfo[] = {
0x16, 0x1b, // SignatureInfo
  0x1b, 0x01, // SignatureType
    0x03,
  0x1c, 0x16, // KeyLocator
    0x07, 0x14, // Name
      0x08, 0x04,
        0x74, 0x65, 0x73, 0x74,
      0x08, 0x03,
        0x6b, 0x65, 0x79,
      0x08, 0x07,
        0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
};

const uint8_t ecdsaSigValue[] = {
0x17, 0x40, // SignatureValue
  0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
  0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
  0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
  0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
  0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b
};

BOOST_AUTO_TEST_CASE(RsaSignatureVerification)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestValidator/RsaSignatureVerification");
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity));
  Name keyName = keyChain.getDefaultKeyNameForIdentity(identity);
  shared_ptr<PublicKey> publicKey = keyChain.getPublicKey(keyName);

  Name identity2("/TestValidator/RsaSignatureVerification/id2");
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity2));
  Name keyName2 = keyChain.getDefaultKeyNameForIdentity(identity2);
  shared_ptr<PublicKey> publicKey2 = keyChain.getPublicKey(keyName2);


  Data data("/TestData/1");
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(data, identity));
  BOOST_CHECK_EQUAL(Validator::verifySignature(data, *publicKey), true);
  BOOST_CHECK_EQUAL(Validator::verifySignature(data, *publicKey2), false);

  Interest interest("/TestInterest/1");
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(interest, identity));
  BOOST_CHECK_EQUAL(Validator::verifySignature(interest, *publicKey), true);
  BOOST_CHECK_EQUAL(Validator::verifySignature(interest, *publicKey2), false);

  Data wrongData("/TestData/2");
  Block ecdsaSigInfoBlock(ecdsaSigInfo, sizeof(ecdsaSigInfo));
  Block ecdsaSigValueBlock(ecdsaSigValue, sizeof(ecdsaSigValue));
  Signature ecdsaSig(ecdsaSigInfoBlock, ecdsaSigValueBlock);
  wrongData.setSignature(ecdsaSig);
  BOOST_CHECK_EQUAL(Validator::verifySignature(wrongData, *publicKey), false);

  keyChain.deleteIdentity(identity);
  keyChain.deleteIdentity(identity2);
}

const uint8_t rsaSigInfo[] = {
0x16, 0x1b, // SignatureInfo
  0x1b, 0x01, // SignatureType
    0x01,
  0x1c, 0x16, // KeyLocator
    0x07, 0x14, // Name
      0x08, 0x04,
        0x74, 0x65, 0x73, 0x74,
      0x08, 0x03,
        0x6b, 0x65, 0x79,
      0x08, 0x07,
        0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
};

const uint8_t rsaSigValue[] = {
0x17, 0x80, // SignatureValue
  0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
  0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
  0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
  0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
  0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
  0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
  0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
  0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
  0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
  0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1
};


BOOST_AUTO_TEST_CASE(EcdsaSignatureVerification)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestValidator/EcdsaSignatureVerification");
  EcdsaKeyParams params;
  // BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity, params));
  try
    {
      keyChain.createIdentity(identity, params);
    }
  catch (std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
    }
  Name keyName = keyChain.getDefaultKeyNameForIdentity(identity);
  shared_ptr<PublicKey> publicKey = keyChain.getPublicKey(keyName);

  Name identity2("/TestValidator/EcdsaSignatureVerification/id2");
  BOOST_REQUIRE_NO_THROW(keyChain.createIdentity(identity2, params));
  Name keyName2 = keyChain.getDefaultKeyNameForIdentity(identity2);
  shared_ptr<PublicKey> publicKey2 = keyChain.getPublicKey(keyName2);


  Data data("/TestData/1");
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(data, identity));
  BOOST_CHECK_EQUAL(Validator::verifySignature(data, *publicKey), true);
  BOOST_CHECK_EQUAL(Validator::verifySignature(data, *publicKey2), false);

  Interest interest("/TestInterest/1");
  BOOST_CHECK_NO_THROW(keyChain.signByIdentity(interest, identity));
  BOOST_CHECK_EQUAL(Validator::verifySignature(interest, *publicKey), true);
  BOOST_CHECK_EQUAL(Validator::verifySignature(interest, *publicKey2), false);

  Data wrongData("/TestData/2");
  Block rsaSigInfoBlock(rsaSigInfo, sizeof(rsaSigInfo));
  Block rsaSigValueBlock(rsaSigValue, sizeof(rsaSigValue));
  Signature rsaSig(rsaSigInfoBlock, rsaSigValueBlock);
  wrongData.setSignature(rsaSig);
  BOOST_CHECK_EQUAL(Validator::verifySignature(wrongData, *publicKey), false);

  keyChain.deleteIdentity(identity);
  keyChain.deleteIdentity(identity2);
}

BOOST_AUTO_TEST_CASE(EcdsaSignatureVerification2)
{
  KeyChain keyChain("sqlite3", "file");

  EcdsaKeyParams params;

  Name ecdsaIdentity("/SecurityTestValidator/EcdsaSignatureVerification2/ecdsa");
  Name ecdsaCertName = keyChain.createIdentity(ecdsaIdentity, params);
  shared_ptr<IdentityCertificate> ecdsaCert = keyChain.getCertificate(ecdsaCertName);

  Name rsaIdentity("/SecurityTestValidator/EcdsaSignatureVerification2/rsa");
  Name rsaCertName = keyChain.createIdentity(rsaIdentity);
  shared_ptr<IdentityCertificate> rsaCert = keyChain.getCertificate(rsaCertName);

  Name packetName("/Test/Packet/Name");

  shared_ptr<Data> testDataRsa = make_shared<Data>(packetName);
  keyChain.signByIdentity(*testDataRsa, rsaIdentity);
  shared_ptr<Data> testDataEcdsa = make_shared<Data>(packetName);
  keyChain.signByIdentity(*testDataEcdsa, ecdsaIdentity);
  shared_ptr<Interest> testInterestRsa = make_shared<Interest>(packetName);
  keyChain.signByIdentity(*testInterestRsa, rsaIdentity);
  shared_ptr<Interest> testInterestEcdsa = make_shared<Interest>(packetName);
  keyChain.signByIdentity(*testInterestEcdsa, ecdsaIdentity);

  BOOST_CHECK(Validator::verifySignature(*ecdsaCert, ecdsaCert->getPublicKeyInfo()));
  BOOST_CHECK_EQUAL(Validator::verifySignature(*ecdsaCert, rsaCert->getPublicKeyInfo()), false);
  BOOST_CHECK_EQUAL(Validator::verifySignature(*rsaCert, ecdsaCert->getPublicKeyInfo()), false);
  BOOST_CHECK(Validator::verifySignature(*rsaCert, rsaCert->getPublicKeyInfo()));

  BOOST_CHECK(Validator::verifySignature(*testDataEcdsa, ecdsaCert->getPublicKeyInfo()));
  BOOST_CHECK_EQUAL(Validator::verifySignature(*testDataEcdsa, rsaCert->getPublicKeyInfo()), false);
  BOOST_CHECK_EQUAL(Validator::verifySignature(*testDataRsa, ecdsaCert->getPublicKeyInfo()), false);
  BOOST_CHECK(Validator::verifySignature(*testDataRsa, rsaCert->getPublicKeyInfo()));

  BOOST_CHECK(Validator::verifySignature(*testInterestEcdsa, ecdsaCert->getPublicKeyInfo()));
  BOOST_CHECK_EQUAL(Validator::verifySignature(*testInterestEcdsa, rsaCert->getPublicKeyInfo()),
                    false);
  BOOST_CHECK_EQUAL(Validator::verifySignature(*testInterestRsa, ecdsaCert->getPublicKeyInfo()),
                    false);
  BOOST_CHECK(Validator::verifySignature(*testInterestRsa, rsaCert->getPublicKeyInfo()));

  keyChain.deleteIdentity(ecdsaIdentity);
  keyChain.deleteIdentity(rsaIdentity);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
