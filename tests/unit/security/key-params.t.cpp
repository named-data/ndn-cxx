/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/security/key-params.hpp"

#include "tests/boost-test.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(TestKeyParams)

BOOST_AUTO_TEST_CASE(Rsa)
{
  RsaKeyParams params;
  BOOST_CHECK_EQUAL(params.getKeyType(), KeyType::RSA);
  BOOST_CHECK_EQUAL(params.getKeySize(), 2048);
  BOOST_CHECK_EQUAL(params.getKeyIdType(), KeyIdType::RANDOM);

  RsaKeyParams params2(4096, KeyIdType::SHA256);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KeyType::RSA);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 4096);
  BOOST_CHECK_EQUAL(params2.getKeyIdType(), KeyIdType::SHA256);

  BOOST_CHECK_THROW(RsaKeyParams(1024), KeyParams::Error);

  name::Component keyId("keyId");
  RsaKeyParams params4(keyId);
  BOOST_CHECK_EQUAL(params4.getKeyType(), KeyType::RSA);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 2048);
  BOOST_CHECK_EQUAL(params4.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params4.getKeyId(), keyId);
}

BOOST_AUTO_TEST_CASE(Ec)
{
  EcKeyParams params;
  BOOST_CHECK_EQUAL(params.getKeyType(), KeyType::EC);
  BOOST_CHECK_EQUAL(params.getKeySize(), 256);
  BOOST_CHECK_EQUAL(params.getKeyIdType(), KeyIdType::RANDOM);

  EcKeyParams params2(384, KeyIdType::SHA256);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KeyType::EC);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 384);
  BOOST_CHECK_EQUAL(params2.getKeyIdType(), KeyIdType::SHA256);

  BOOST_CHECK_THROW(EcKeyParams(64), KeyParams::Error);

  name::Component keyId("keyId");
  EcKeyParams params4(keyId);
  BOOST_CHECK_EQUAL(params4.getKeyType(), KeyType::EC);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 256);
  BOOST_CHECK_EQUAL(params4.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params4.getKeyId(), keyId);
}

BOOST_AUTO_TEST_CASE(Aes)
{
  name::Component keyId("keyId");
  AesKeyParams params(keyId);
  BOOST_CHECK_EQUAL(params.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params.getKeySize(), 128);
  BOOST_CHECK_EQUAL(params.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params.getKeyId(), keyId);

  AesKeyParams params2(keyId, 192);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 192);
  BOOST_CHECK_EQUAL(params2.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params2.getKeyId(), keyId);

  AesKeyParams params3(keyId, 256);
  BOOST_CHECK_EQUAL(params3.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params3.getKeySize(), 256);
  BOOST_CHECK_EQUAL(params3.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params3.getKeyId(), keyId);

  BOOST_CHECK_THROW(AesKeyParams(keyId, 64), KeyParams::Error);

  AesKeyParams params4;
  BOOST_CHECK_EQUAL(params4.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 128);
  BOOST_CHECK_EQUAL(params4.getKeyIdType(), KeyIdType::RANDOM);

  AesKeyParams params5(192);
  BOOST_CHECK_EQUAL(params5.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params5.getKeySize(), 192);
  BOOST_CHECK_EQUAL(params5.getKeyIdType(), KeyIdType::RANDOM);
}

BOOST_AUTO_TEST_CASE(Hmac)
{
  name::Component keyId("keyId");
  HmacKeyParams params(keyId);
  BOOST_CHECK_EQUAL(params.getKeyType(), KeyType::HMAC);
  BOOST_CHECK_EQUAL(params.getKeySize(), 256);
  BOOST_CHECK_EQUAL(params.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params.getKeyId(), keyId);

  HmacKeyParams params2(keyId, 384);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KeyType::HMAC);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 384);
  BOOST_CHECK_EQUAL(params2.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params2.getKeyId(), keyId);

  BOOST_CHECK_THROW(HmacKeyParams(keyId, 0), KeyParams::Error);
  BOOST_CHECK_THROW(HmacKeyParams(keyId, 300), KeyParams::Error); // not a multiple of 8

  HmacKeyParams params3;
  BOOST_CHECK_EQUAL(params3.getKeyType(), KeyType::HMAC);
  BOOST_CHECK_EQUAL(params3.getKeySize(), 256);
  BOOST_CHECK_EQUAL(params3.getKeyIdType(), KeyIdType::RANDOM);

  HmacKeyParams params4(1024);
  BOOST_CHECK_EQUAL(params4.getKeyType(), KeyType::HMAC);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 1024);
  BOOST_CHECK_EQUAL(params4.getKeyIdType(), KeyIdType::RANDOM);
}

BOOST_AUTO_TEST_CASE(KeyTypeToString)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyType::NONE), "NONE");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyType::RSA), "RSA");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyType::EC), "EC");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyType::AES), "AES");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyType::HMAC), "HMAC");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<KeyType>(12345)), "12345");
}

BOOST_AUTO_TEST_CASE(KeyIdTypeToString)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyIdType::USER_SPECIFIED), "USER-SPECIFIED");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyIdType::SHA256), "SHA256");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyIdType::RANDOM), "RANDOM");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<KeyIdType>(12345)), "12345");
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyParams
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace ndn
