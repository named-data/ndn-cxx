/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "security/key-chain.hpp"
#include "security/validator.hpp"

#include "security/cryptopp.hpp"

#include "boost-test.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestSignatureSha256)

string SHA256_RESULT("a883dafc480d466ee04e0d6da986bd78eb1fdd2178d04693723da3a8f95d42f4");

BOOST_AUTO_TEST_CASE (Sha256)
{
  using namespace CryptoPP;

  char content[6] = "1234\n";
  ConstBufferPtr buf = crypto::sha256(reinterpret_cast<uint8_t*>(content), 5);
  string result;
  StringSource(buf->buf(), buf->size(), true, new HexEncoder(new StringSink(result), false));

  BOOST_REQUIRE_EQUAL(SHA256_RESULT, result);
}

BOOST_AUTO_TEST_CASE (Signature)
{
  using namespace CryptoPP;

  Name name("/TestSignatureSha/Basic");
  Data testData(name);
  char content[5] = "1234";
  testData.setContent(reinterpret_cast<uint8_t*>(content), 5);

  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  keyChain.signWithSha256(testData);

  testData.wireEncode();

  SignatureSha256 sig(testData.getSignature());

  BOOST_REQUIRE(Validator::verifySignature(testData, sig));
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
