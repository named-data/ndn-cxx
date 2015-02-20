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

#include "security/key-params.hpp"

#include "boost-test.hpp"


namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityKeyParams)

BOOST_AUTO_TEST_CASE(RsaParameter)
{
  RsaKeyParams params;
  BOOST_CHECK_EQUAL(params.getKeyType(), KEY_TYPE_RSA);
  BOOST_CHECK_EQUAL(params.getKeySize(), 2048);

  RsaKeyParams params2(1024);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KEY_TYPE_RSA);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 1024);

  RsaKeyParams params3(3);
  BOOST_CHECK_EQUAL(params3.getKeyType(), KEY_TYPE_RSA);
  BOOST_CHECK_EQUAL(params3.getKeySize(), 2048);
}

BOOST_AUTO_TEST_CASE(EcdsaParameter)
{
  EcdsaKeyParams params;
  BOOST_CHECK_EQUAL(params.getKeyType(), KEY_TYPE_ECDSA);
  BOOST_CHECK_EQUAL(params.getKeySize(), 256);

  EcdsaKeyParams params2(384);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KEY_TYPE_ECDSA);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 384);

  EcdsaKeyParams params3(3);
  BOOST_CHECK_EQUAL(params3.getKeyType(), KEY_TYPE_ECDSA);
  BOOST_CHECK_EQUAL(params3.getKeySize(), 256);
}

BOOST_AUTO_TEST_CASE(AesParameter)
{
  AesKeyParams params;
  BOOST_CHECK_EQUAL(params.getKeyType(), KEY_TYPE_AES);
  BOOST_CHECK_EQUAL(params.getKeySize(), 64);

  AesKeyParams params2(128);
  BOOST_CHECK_EQUAL(params2.getKeyType(), KEY_TYPE_AES);
  BOOST_CHECK_EQUAL(params2.getKeySize(), 128);

  AesKeyParams params3(256);
  BOOST_CHECK_EQUAL(params3.getKeyType(), KEY_TYPE_AES);
  BOOST_CHECK_EQUAL(params3.getKeySize(), 256);

  AesKeyParams params4(4);
  BOOST_CHECK_EQUAL(params4.getKeyType(), KEY_TYPE_AES);
  BOOST_CHECK_EQUAL(params4.getKeySize(), 64);
}

BOOST_AUTO_TEST_CASE(Error)
{
  EcdsaKeyParams params;
  BOOST_REQUIRE_THROW((RsaKeyParams(params)), KeyParams::Error);

  AesKeyParams params2;
  BOOST_REQUIRE_THROW((RsaKeyParams(params2)), KeyParams::Error);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
