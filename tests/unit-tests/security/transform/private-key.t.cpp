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

#include "security/transform/private-key.hpp"

#include "encoding/buffer-stream.hpp"
#include "security/key-params.hpp"
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
BOOST_AUTO_TEST_SUITE(TestPrivateKey)

struct RsaKeyTestData
{
  const std::string privateKeyPkcs1 =
      "MIIEpAIBAAKCAQEAw0WM1/WhAxyLtEqsiAJgWDZWuzkYpeYVdeeZcqRZzzfRgBQT\n"
      "sNozS5t4HnwTZhwwXbH7k3QN0kRTV826Xobws3iigohnM9yTK+KKiayPhIAm/+5H\n"
      "GT6SgFJhYhqo1/upWdueojil6RP4/AgavHhopxlAVbk6G9VdVnlQcQ5Zv0OcGi73\n"
      "c+EnYD/YgURYGSngUi/Ynsh779p2U69/te9gZwIL5PuE9BiO6I39cL9z7EK1SfZh\n"
      "OWvDe/qH7YhD/BHwcWit8FjRww1glwRVTJsA9rH58ynaAix0tcR/nBMRLUX+e3rU\n"
      "RHg6UbSjJbdb9qmKM1fTGHKUzL/5pMG6uBU0ywIDAQABAoIBADQkckOIl4IZMUTn\n"
      "W8LFv6xOdkJwMKC8G6bsPRFbyY+HvC2TLt7epSvfS+f4AcYWaOPcDu2E49vt2sNr\n"
      "cASly8hgwiRRAB3dHH9vcsboiTo8bi2RFvMqvjv9w3tK2yMxVDtmZamzrrnaV3YV\n"
      "Q+5nyKo2F/PMDjQ4eUAKDOzjhBuKHsZBTFnA1MFNI+UKj5X4Yp64DFmKlxTX/U2b\n"
      "wzVywo5hzx2Uhw51jmoLls4YUvMJXD0wW5ZtYRuPogXvXb/of9ef/20/wU11WFKg\n"
      "Xb4gfR8zUXaXS1sXcnVm3+24vIs9dApUwykuoyjOqxWqcHRec2QT2FxVGkFEraze\n"
      "CPa4rMECgYEA5Y8CywomIcTgerFGFCeMHJr8nQGqY2V/owFb3k9maczPnC9p4a9R\n"
      "c5szLxA9FMYFxurQZMBWSEG2JS1HR2mnjigx8UKjYML/A+rvvjZOMe4M6Sy2ggh4\n"
      "SkLZKpWTzjTe07ByM/j5v/SjNZhWAG7sw4/LmPGRQkwJv+KZhGojuOkCgYEA2cOF\n"
      "T6cJRv6kvzTz9S0COZOVm+euJh/BXp7oAsAmbNfOpckPMzqHXy8/wpdKl6AAcB57\n"
      "OuztlNfV1D7qvbz7JuRlYwQ0cEfBgbZPcz1p18HHDXhwn57ZPb8G33Yh9Omg0HNA\n"
      "Imb4LsVuSqxA6NwSj7cpRekgTedrhLFPJ+Ydb5MCgYEAsM3Q7OjILcIg0t6uht9e\n"
      "vrlwTsz1mtCV2co2I6crzdj9HeI2vqf1KAElDt6G7PUHhglcr/yjd8uEqmWRPKNX\n"
      "ddnnfVZB10jYeP/93pac6z/Zmc3iU4yKeUe7U10ZFf0KkiiYDQd59CpLef/2XScS\n"
      "HB0oRofnxRQjfjLc4muNT+ECgYEAlcDk06MOOTly+F8lCc1bA1dgAmgwFd2usDBd\n"
      "Y07a3e0HGnGLN3Kfl7C5i0tZq64HvxLnMd2vgLVxQlXGPpdQrC1TH+XLXg+qnlZO\n"
      "ivSH7i0/gx75bHvj75eH1XK65V8pDVDEoSPottllAIs21CxLw3N1ObOZWJm2EfmR\n"
      "cuHICmsCgYAtFJ1idqMoHxES3mlRpf2JxyQudP3SCm2WpGmqVzhRYInqeatY5sUd\n"
      "lPLHm/p77RT7EyxQHTlwn8FJPuM/4ZH1rQd/vB+Y8qAtYJCexDMsbvLW+Js+VOvk\n"
      "jweEC0nrcL31j9mF0vz5E6tfRu4hhJ6L4yfWs0gSejskeVB/w8QY4g==\n";
  const std::string privateKeyPkcs8 =
      "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\n";
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
  const std::string privateKeyPkcs1 =
      "MIIBaAIBAQQgRxwcbzK9RV6AHYFsDcykI86o3M/a1KlJn0z8PcLMBZOggfowgfcC\n"
      "AQEwLAYHKoZIzj0BAQIhAP////8AAAABAAAAAAAAAAAAAAAA////////////////\n"
      "MFsEIP////8AAAABAAAAAAAAAAAAAAAA///////////////8BCBaxjXYqjqT57Pr\n"
      "vVV2mIa8ZR0GsMxTsPY7zjw+J9JgSwMVAMSdNgiG5wSTamZ44ROdJreBn36QBEEE\n"
      "axfR8uEsQkf4vOblY6RA8ncDfYEt6zOg9KE5RdiYwpZP40Li/hp/m47n60p8D54W\n"
      "K84zV2sxXs7LtkBoN79R9QIhAP////8AAAAA//////////+85vqtpxeehPO5ysL8\n"
      "YyVRAgEBoUQDQgAEaG4WJuDAt0QkEM4t29KDUdzkQlMPGrqWzkWhgt9OGnwc6O7A\n"
      "ZLPSrDyhwyrKS7XLRXml5DisQ93RvByll32y8A==\n";
  const std::string privateKeyPkcs8 =
      "MIIBwzA9BgkqhkiG9w0BBQ0wMDAbBgkqhkiG9w0BBQwwDgQIVHkBzLGtDvICAggA\n"
      "MBEGBSsOAwIHBAhk6g9eI3toNwSCAYDd+LWPDBTrKV7vUyxTvDbpUd0eXfh73DKA\n"
      "MHkdHuVmhpmpBbsF9XvaFuL8J/1xi1Yl2XGw8j3WyrprD2YEhl/+zKjNbdTDJmNO\n"
      "SlomuwWb5AVCJ9reT94zIXKCnexUcyBFS7ep+P4dwuef0VjzprjfmnAZHrP+u594\n"
      "ELHpKwi0ZpQLtcJjjud13bn43vbXb+aU7jmPV5lU2XP8TxaQJiYIibNEh1Y3TZGr\n"
      "akJormYvhaYbiZkKLHQ9AvQMEjhoIW5WCB3q+tKZUKTzcQpjNnf9FOTeKN3jk3Kd\n"
      "2OmibPZcbMJdgCD/nRVn1cBo7Hjn3IMjgtszQHtEUphOQiAkOJUnKmy9MTYqtcNN\n"
      "6cuFItbu4QvbVwailgdUjOYwIJCmIxExlPV0ohS24pFGsO03Yn7W8rBB9VWENYmG\n"
      "HkZIbGsHv7O9Wy7fv+FJgZkjeti0807IsNXSJl8LUK0ZIhAR7OU8uONWMsbHdQnk\n"
      "q1HB1ZKa52ugACl7g/DF9b7CoSAjFeE=\n";
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

static void
checkPkcs8Encoding(ConstBufferPtr encoding, const std::string& password, ConstBufferPtr pkcs1)
{
  PrivateKey sKey;
  sKey.loadPkcs8(encoding->data(), encoding->size(), password.c_str(), password.size());
  OBufferStream os;
  sKey.savePkcs1(os);
  BOOST_CHECK_EQUAL_COLLECTIONS(pkcs1->begin(), pkcs1->end(),
                                os.buf()->begin(), os.buf()->end());
}

static void
checkPkcs8Base64Encoding(ConstBufferPtr encoding, const std::string& password, ConstBufferPtr pkcs1)
{
  OBufferStream os;
  bufferSource(*encoding) >> base64Decode() >> streamSink(os);
  checkPkcs8Encoding(os.buf(), password, pkcs1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SaveLoad, T, KeyTestDataSets)
{
  T dataSet;

  const uint8_t* sKeyPkcs1Base64 = reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.c_str());
  size_t sKeyPkcs1Base64Len = dataSet.privateKeyPkcs1.size();
  OBufferStream os;
  bufferSource(sKeyPkcs1Base64, sKeyPkcs1Base64Len) >> base64Decode() >> streamSink(os);
  ConstBufferPtr sKeyPkcs1Buf = os.buf();
  const uint8_t* sKeyPkcs1 = sKeyPkcs1Buf->data();
  size_t sKeyPkcs1Len = sKeyPkcs1Buf->size();

  // load key in base64-encoded pkcs1 format
  PrivateKey sKey;
  BOOST_CHECK_NO_THROW(sKey.loadPkcs1Base64(sKeyPkcs1Base64, sKeyPkcs1Base64Len));

  std::stringstream ss2(dataSet.privateKeyPkcs1);
  PrivateKey sKey2;
  BOOST_CHECK_NO_THROW(sKey2.loadPkcs1Base64(ss2));

  // load key in pkcs1 format
  PrivateKey sKey3;
  BOOST_CHECK_NO_THROW(sKey3.loadPkcs1(sKeyPkcs1, sKeyPkcs1Len));

  std::stringstream ss4;
  ss4.write(reinterpret_cast<const char*>(sKeyPkcs1), sKeyPkcs1Len);
  PrivateKey sKey4;
  BOOST_CHECK_NO_THROW(sKey4.loadPkcs1(ss4));

  // save key in base64-encoded pkcs1 format
  OBufferStream os2;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs1Base64(os2));
  BOOST_CHECK_EQUAL_COLLECTIONS(sKeyPkcs1Base64, sKeyPkcs1Base64 + sKeyPkcs1Base64Len,
                                os2.buf()->begin(), os2.buf()->end());

  // save key in pkcs1 format
  OBufferStream os3;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs1(os3));
  BOOST_CHECK_EQUAL_COLLECTIONS(sKeyPkcs1, sKeyPkcs1 + sKeyPkcs1Len,
                                os3.buf()->begin(), os3.buf()->end());

  const uint8_t* sKeyPkcs8Base64 = reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs8.c_str());
  size_t sKeyPkcs8Base64Len = dataSet.privateKeyPkcs8.size();
  OBufferStream os4;
  bufferSource(sKeyPkcs8Base64, sKeyPkcs8Base64Len) >> base64Decode() >> streamSink(os4);
  const uint8_t* sKeyPkcs8 = os4.buf()->data();
  size_t sKeyPkcs8Len = os4.buf()->size();

  std::string password("password");
  std::string wrongpw("wrongpw");
  auto pwCallback = [&password] (char* buf, size_t size, int) -> int {
    BOOST_REQUIRE_LE(password.size(), size);
    std::copy(password.begin(), password.end(), buf);
    return static_cast<int>(password.size());
  };

  // load key in base64-encoded pkcs8 format
  PrivateKey sKey5;
  BOOST_CHECK_NO_THROW(sKey5.loadPkcs8Base64(sKeyPkcs8Base64, sKeyPkcs8Base64Len,
                                             password.c_str(), password.size()));

  PrivateKey sKey6;
  BOOST_CHECK_NO_THROW(sKey6.loadPkcs8Base64(sKeyPkcs8Base64, sKeyPkcs8Base64Len, pwCallback));

  std::stringstream ss7(dataSet.privateKeyPkcs8);
  PrivateKey sKey7;
  BOOST_CHECK_NO_THROW(sKey7.loadPkcs8Base64(ss7, password.c_str(), password.size()));

  std::stringstream ss8(dataSet.privateKeyPkcs8);
  PrivateKey sKey8;
  BOOST_CHECK_NO_THROW(sKey8.loadPkcs8Base64(ss8, pwCallback));

  // load key in pkcs8 format
  PrivateKey sKey9;
  BOOST_CHECK_NO_THROW(sKey9.loadPkcs8(sKeyPkcs8, sKeyPkcs8Len, password.c_str(), password.size()));

  PrivateKey sKey10;
  BOOST_CHECK_NO_THROW(sKey10.loadPkcs8(sKeyPkcs8, sKeyPkcs8Len, pwCallback));

  std::stringstream ss11;
  ss11.write(reinterpret_cast<const char*>(sKeyPkcs8), sKeyPkcs8Len);
  PrivateKey sKey11;
  BOOST_CHECK_NO_THROW(sKey11.loadPkcs8(ss11, password.c_str(), password.size()));

  std::stringstream ss12;
  ss12.write(reinterpret_cast<const char*>(sKeyPkcs8), sKeyPkcs8Len);
  PrivateKey sKey12;
  BOOST_CHECK_NO_THROW(sKey12.loadPkcs8(ss12, pwCallback));

  // load key using wrong password, Error is expected
  PrivateKey sKey13;
  BOOST_CHECK_THROW(sKey13.loadPkcs8Base64(sKeyPkcs8Base64, sKeyPkcs8Base64Len, wrongpw.c_str(), wrongpw.size()),
                    PrivateKey::Error);

  // save key in base64-encoded pkcs8 format
  OBufferStream os14;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8Base64(os14, password.c_str(), password.size()));
  checkPkcs8Base64Encoding(os14.buf(), password, sKeyPkcs1Buf);

  OBufferStream os15;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8Base64(os15, pwCallback));
  checkPkcs8Base64Encoding(os15.buf(), password, sKeyPkcs1Buf);

  // save key in pkcs8 format
  OBufferStream os16;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8(os16, password.c_str(), password.size()));
  checkPkcs8Encoding(os16.buf(), password, sKeyPkcs1Buf);

  OBufferStream os17;
  BOOST_REQUIRE_NO_THROW(sKey.savePkcs8(os17, pwCallback));
  checkPkcs8Encoding(os17.buf(), password, sKeyPkcs1Buf);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DerivePublicKey, T, KeyTestDataSets)
{
  T dataSet;

  const uint8_t* sKeyPkcs1Base64 = reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.c_str());
  size_t sKeyPkcs1Base64Len = dataSet.privateKeyPkcs1.size();
  PrivateKey sKey;
  sKey.loadPkcs1Base64(sKeyPkcs1Base64, sKeyPkcs1Base64Len);

  // derive public key and compare
  ConstBufferPtr pKeyBits = sKey.derivePublicKey();
  OBufferStream os;
  bufferSource(dataSet.publicKeyPkcs8) >> base64Decode() >> streamSink(os);
  BOOST_CHECK_EQUAL_COLLECTIONS(pKeyBits->begin(), pKeyBits->end(),
                                os.buf()->begin(), os.buf()->end());
}

BOOST_AUTO_TEST_CASE(RsaDecryption)
{
  RsaKeyTestData dataSet;

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.c_str()),
                       dataSet.privateKeyPkcs1.size());
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::RSA);

  const uint8_t plainText[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

  const std::string cipherTextBase64 =
    "i2XNpZ2JbLa4JmBTdDrGmsd4/0C+p+BSCpW3MuPBNe5uChQ0eRO1dvjTnEqwSECY\n"
    "38en9JZwcyb0It/TSFNXHlq+Z1ZpffnjIJxQR9HcgwvwQJh6WRH0vu38tvGkGuNv\n"
    "60Rdn85hqSy1CikmXCeWXL9yCqeqcP21R94G/T3FuA+c1FtFko8KOzCwvrTXMO6n\n"
    "5PNsqlLXabSGr+jz4EwOsSCgPkiDf9U6tXoSPRA2/YvqFQdaiUXIVlomESvaqqZ8\n"
    "FxPs2BON0lobM8gT+xdzbRKofp+rNjNK+5uWyeOnXJwzCszh17cdJl2BH1dZwaVD\n"
    "PmTiSdeDQXZ94U5boDQ4Aw==\n";
  OBufferStream os;
  bufferSource(cipherTextBase64) >> base64Decode() >> streamSink(os);

  auto decrypted = sKey.decrypt(os.buf()->data(), os.buf()->size());
  BOOST_CHECK_EQUAL_COLLECTIONS(plainText, plainText + sizeof(plainText),
                                decrypted->begin(), decrypted->end());
}

BOOST_AUTO_TEST_CASE(RsaEncryptDecrypt)
{
  RsaKeyTestData dataSet;

  PublicKey pKey;
  pKey.loadPkcs8Base64(reinterpret_cast<const uint8_t*>(dataSet.publicKeyPkcs8.c_str()),
                       dataSet.publicKeyPkcs8.size());
  BOOST_CHECK_EQUAL(pKey.getKeyType(), KeyType::RSA);

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.c_str()),
                       dataSet.privateKeyPkcs1.size());
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::RSA);

  const uint8_t plainText[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

  auto cipherText = pKey.encrypt(plainText, sizeof(plainText));
  auto decrypted = sKey.decrypt(cipherText->data(), cipherText->size());
  BOOST_CHECK_EQUAL_COLLECTIONS(plainText, plainText + sizeof(plainText),
                                decrypted->begin(), decrypted->end());
}

BOOST_AUTO_TEST_CASE(UnsupportedEcDecryption)
{
  EcKeyTestData dataSet;

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(dataSet.privateKeyPkcs1.c_str()),
                       dataSet.privateKeyPkcs1.size());
  BOOST_CHECK_EQUAL(sKey.getKeyType(), KeyType::EC);

  OBufferStream os;
  bufferSource("Y2lhbyFob2xhIWhlbGxvIQ==") >> base64Decode() >> streamSink(os);

  BOOST_CHECK_THROW(sKey.decrypt(os.buf()->data(), os.buf()->size()), PrivateKey::Error);
}

using KeyParams = boost::mpl::vector<RsaKeyParams, EcKeyParams>;

BOOST_AUTO_TEST_CASE_TEMPLATE(GenerateKey, T, KeyParams)
{
  unique_ptr<PrivateKey> sKey = generatePrivateKey(T());
  PublicKey pKey;
  ConstBufferPtr pKeyBits = sKey->derivePublicKey();
  pKey.loadPkcs8(pKeyBits->data(), pKeyBits->size());

  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  OBufferStream os;
  BOOST_REQUIRE_NO_THROW(bufferSource(data, sizeof(data)) >>
                         signerFilter(DigestAlgorithm::SHA256, *sKey) >>
                         streamSink(os));

  ConstBufferPtr sig = os.buf();
  bool result = false;
  BOOST_REQUIRE_NO_THROW(bufferSource(data, sizeof(data)) >>
                         verifierFilter(DigestAlgorithm::SHA256, pKey, sig->data(), sig->size()) >>
                         boolSink(result));
  BOOST_CHECK(result);

  unique_ptr<PrivateKey> sKey2 = generatePrivateKey(T());

  OBufferStream os1;
  sKey->savePkcs1(os1);
  ConstBufferPtr key1Pkcs1 = os1.buf();

  OBufferStream os2;
  sKey2->savePkcs1(os2);
  ConstBufferPtr key2Pkcs1 = os2.buf();

  BOOST_CHECK(*key1Pkcs1 != *key2Pkcs1);
}

BOOST_AUTO_TEST_CASE(UnsupportedKeyType)
{
  BOOST_CHECK_THROW(generatePrivateKey(AesKeyParams()), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END() // TestPrivateKey
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
