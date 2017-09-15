/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "security/key-params.hpp"

#include "boost-test.hpp"

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
  BOOST_CHECK(params.getKeyIdType() == KeyIdType::RANDOM);

  RsaKeyParams params2(1024, KeyIdType::SHA256);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KeyType::RSA);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 1024);
  BOOST_CHECK(params2.getKeyIdType() == KeyIdType::SHA256);

  BOOST_CHECK_THROW(RsaKeyParams(3), KeyParams::Error);

  name::Component keyId("keyId");
  RsaKeyParams params4(keyId);
  BOOST_CHECK(params4.getKeyType() == KeyType::RSA);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 2048);
  BOOST_CHECK(params4.getKeyIdType() == KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params4.getKeyId(), keyId);
}

BOOST_AUTO_TEST_CASE(Ec)
{
  EcKeyParams params;
  BOOST_CHECK_EQUAL(params.getKeyType(), KeyType::EC);
  BOOST_CHECK_EQUAL(params.getKeySize(), 256);
  BOOST_CHECK(params.getKeyIdType() == KeyIdType::RANDOM);

  EcKeyParams params2(384, KeyIdType::SHA256);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KeyType::EC);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 384);
  BOOST_CHECK(params2.getKeyIdType() == KeyIdType::SHA256);

  BOOST_CHECK_THROW(EcKeyParams(3), KeyParams::Error);

  name::Component keyId("keyId");
  EcKeyParams params4(keyId);
  BOOST_CHECK(params4.getKeyType() == KeyType::EC);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 256);
  BOOST_CHECK(params4.getKeyIdType() == KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params4.getKeyId(), keyId);
}

BOOST_AUTO_TEST_CASE(Aes)
{
  name::Component keyId("keyId");
  AesKeyParams params(keyId);
  BOOST_CHECK_EQUAL(params.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params.getKeySize(), 128);
  BOOST_CHECK_EQUAL(params.getKeyIdType(), KeyIdType::USER_SPECIFIED);

  AesKeyParams params2(keyId, 192);
  BOOST_CHECK(params2.getKeyType() == KeyType::AES);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 192);
  BOOST_CHECK(params.getKeyIdType() == KeyIdType::USER_SPECIFIED);

  AesKeyParams params3(keyId, 256);
  BOOST_CHECK_EQUAL(params3.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params3.getKeySize(), 256);
  BOOST_CHECK(params.getKeyIdType() == KeyIdType::USER_SPECIFIED);

  BOOST_CHECK_THROW(AesKeyParams(keyId, 4), KeyParams::Error);

  AesKeyParams params5(keyId);
  BOOST_CHECK_EQUAL(params5.getKeyType(), KeyType::AES);
  BOOST_CHECK_EQUAL(params5.getKeySize(), 128);
  BOOST_CHECK_EQUAL(params5.getKeyIdType(), KeyIdType::USER_SPECIFIED);
  BOOST_CHECK_EQUAL(params5.getKeyId(), keyId);

  AesKeyParams params6(192);
  BOOST_CHECK(params6.getKeyType() == KeyType::AES);
  BOOST_CHECK_EQUAL(params6.getKeySize(), 192);
  BOOST_CHECK(params6.getKeyIdType() == KeyIdType::RANDOM);
}

BOOST_AUTO_TEST_CASE(KeyIdTypeInfo)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyIdType::USER_SPECIFIED), "USER_SPECIFIED");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyIdType::SHA256), "SHA256");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(KeyIdType::RANDOM), "RANDOM");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(static_cast<KeyIdType>(12345)), "12345");
}

BOOST_AUTO_TEST_SUITE_END() // TestKeyParams
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace ndn
