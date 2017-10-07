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

#include "security/transform/public-key.hpp"

#include "encoding/buffer-stream.hpp"
#include "security/transform.hpp"

#include "boost-test.hpp"
#include <boost/mpl/vector.hpp>

#include <sstream>

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestPublicKey)

struct RsaKeyTestData
{
  const std::string publicKeyPkcs8 =
      "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAw0WM1/WhAxyLtEqsiAJg\n"
      "WDZWuzkYpeYVdeeZcqRZzzfRgBQTsNozS5t4HnwTZhwwXbH7k3QN0kRTV826Xobw\n"
      "s3iigohnM9yTK+KKiayPhIAm/+5HGT6SgFJhYhqo1/upWdueojil6RP4/AgavHho\n"
      "pxlAVbk6G9VdVnlQcQ5Zv0OcGi73c+EnYD/YgURYGSngUi/Ynsh779p2U69/te9g\n"
      "ZwIL5PuE9BiO6I39cL9z7EK1SfZhOWvDe/qH7YhD/BHwcWit8FjRww1glwRVTJsA\n"
      "9rH58ynaAix0tcR/nBMRLUX+e3rURHg6UbSjJbdb9qmKM1fTGHKUzL/5pMG6uBU0\n"
      "ywIDAQAB\n";
};

struct EcKeyTestData
{
  const std::string publicKeyPkcs8 =
      "MIIBSzCCAQMGByqGSM49AgEwgfcCAQEwLAYHKoZIzj0BAQIhAP////8AAAABAAAA\n"
      "AAAAAAAAAAAA////////////////MFsEIP////8AAAABAAAAAAAAAAAAAAAA////\n"
      "///////////8BCBaxjXYqjqT57PrvVV2mIa8ZR0GsMxTsPY7zjw+J9JgSwMVAMSd\n"
      "NgiG5wSTamZ44ROdJreBn36QBEEEaxfR8uEsQkf4vOblY6RA8ncDfYEt6zOg9KE5\n"
      "RdiYwpZP40Li/hp/m47n60p8D54WK84zV2sxXs7LtkBoN79R9QIhAP////8AAAAA\n"
      "//////////+85vqtpxeehPO5ysL8YyVRAgEBA0IABGhuFibgwLdEJBDOLdvSg1Hc\n"
      "5EJTDxq6ls5FoYLfThp8HOjuwGSz0qw8ocMqyku1y0V5peQ4rEPd0bwcpZd9svA=\n";
};

using KeyTestDataSets = boost::mpl::vector<RsaKeyTestData, EcKeyTestData>;

BOOST_AUTO_TEST_CASE_TEMPLATE(SaveLoad, T, KeyTestDataSets)
{
  T dataSet;

  const uint8_t* pKeyPkcs8Base64 = reinterpret_cast<const uint8_t*>(dataSet.publicKeyPkcs8.c_str());
  size_t pKeyPkcs8Base64Len = dataSet.publicKeyPkcs8.size();
  OBufferStream os;
  bufferSource(pKeyPkcs8Base64, pKeyPkcs8Base64Len) >> base64Decode() >> streamSink(os);
  ConstBufferPtr pKeyPkcs8Buf = os.buf();
  const uint8_t* pKeyPkcs8 = pKeyPkcs8Buf->data();
  size_t pKeyPkcs8Len = pKeyPkcs8Buf->size();

  PublicKey pKey1;
  BOOST_CHECK_NO_THROW(pKey1.loadPkcs8Base64(pKeyPkcs8Base64, pKeyPkcs8Base64Len));

  std::stringstream ss2(dataSet.publicKeyPkcs8);
  PublicKey pKey2;
  BOOST_CHECK_NO_THROW(pKey2.loadPkcs8Base64(ss2));

  PublicKey pKey3;
  BOOST_CHECK_NO_THROW(pKey3.loadPkcs8(pKeyPkcs8, pKeyPkcs8Len));

  std::stringstream ss4;
  ss4.write(reinterpret_cast<const char*>(pKeyPkcs8), pKeyPkcs8Len);
  PublicKey pKey4;
  BOOST_CHECK_NO_THROW(pKey4.loadPkcs8(ss4));

  OBufferStream os5;
  BOOST_REQUIRE_NO_THROW(pKey1.savePkcs8Base64(os5));
  BOOST_CHECK_EQUAL_COLLECTIONS(pKeyPkcs8Base64, pKeyPkcs8Base64 + pKeyPkcs8Base64Len,
                                os5.buf()->begin(), os5.buf()->end());

  OBufferStream os6;
  BOOST_REQUIRE_NO_THROW(pKey1.savePkcs8(os6));
  BOOST_CHECK_EQUAL_COLLECTIONS(pKeyPkcs8, pKeyPkcs8 + pKeyPkcs8Len,
                                os6.buf()->begin(), os6.buf()->end());
}

// NOTE: We cannot test RSA encryption by comparing the computed ciphertext to
//       a known-good one, because OAEP padding is randomized and would produce
//       different results every time. An encrypt/decrypt round-trip test is
//       performed in private-key.t.cpp

BOOST_AUTO_TEST_CASE(UnsupportedEcEncryption)
{
  EcKeyTestData dataSet;

  PublicKey pKey;
  pKey.loadPkcs8Base64(reinterpret_cast<const uint8_t*>(dataSet.publicKeyPkcs8.c_str()),
                       dataSet.publicKeyPkcs8.size());
  BOOST_CHECK_EQUAL(pKey.getKeyType(), KeyType::EC);

  OBufferStream os;
  bufferSource("Y2lhbyFob2xhIWhlbGxvIQ==") >> base64Decode() >> streamSink(os);

  BOOST_CHECK_THROW(pKey.encrypt(os.buf()->data(), os.buf()->size()), PublicKey::Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestPublicKey
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
