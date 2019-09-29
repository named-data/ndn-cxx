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

#include "ndn-cxx/security/transform/signer-filter.hpp"

#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/key-params.hpp"
#include "ndn-cxx/security/transform/base64-decode.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/private-key.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"

#include "tests/boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestSignerFilter)

const uint8_t DATA[] = {0x01, 0x02, 0x03, 0x04};

BOOST_AUTO_TEST_CASE(Rsa)
{
  const std::string publicKeyPkcs8 =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAw0WM1/WhAxyLtEqsiAJg\n"
    "WDZWuzkYpeYVdeeZcqRZzzfRgBQTsNozS5t4HnwTZhwwXbH7k3QN0kRTV826Xobw\n"
    "s3iigohnM9yTK+KKiayPhIAm/+5HGT6SgFJhYhqo1/upWdueojil6RP4/AgavHho\n"
    "pxlAVbk6G9VdVnlQcQ5Zv0OcGi73c+EnYD/YgURYGSngUi/Ynsh779p2U69/te9g\n"
    "ZwIL5PuE9BiO6I39cL9z7EK1SfZhOWvDe/qH7YhD/BHwcWit8FjRww1glwRVTJsA\n"
    "9rH58ynaAix0tcR/nBMRLUX+e3rURHg6UbSjJbdb9qmKM1fTGHKUzL/5pMG6uBU0\n"
    "ywIDAQAB\n";
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

  OBufferStream os1;
  bufferSource(publicKeyPkcs8) >> base64Decode() >> streamSink(os1);
  auto pubKey = os1.buf();

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.data()), privateKeyPkcs1.size());

  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::NONE, sKey), Error);

  OBufferStream os2;
  bufferSource(DATA, sizeof(DATA)) >> signerFilter(DigestAlgorithm::SHA256, sKey) >> streamSink(os2);
  auto sig = os2.buf();

  BOOST_CHECK(verifySignature(DATA, sizeof(DATA), sig->data(), sig->size(), pubKey->data(), pubKey->size()));
}

BOOST_AUTO_TEST_CASE(Ecdsa)
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
  const std::string publicKeyPkcs8 =
    "MIIBSzCCAQMGByqGSM49AgEwgfcCAQEwLAYHKoZIzj0BAQIhAP////8AAAABAAAA\n"
    "AAAAAAAAAAAA////////////////MFsEIP////8AAAABAAAAAAAAAAAAAAAA////\n"
    "///////////8BCBaxjXYqjqT57PrvVV2mIa8ZR0GsMxTsPY7zjw+J9JgSwMVAMSd\n"
    "NgiG5wSTamZ44ROdJreBn36QBEEEaxfR8uEsQkf4vOblY6RA8ncDfYEt6zOg9KE5\n"
    "RdiYwpZP40Li/hp/m47n60p8D54WK84zV2sxXs7LtkBoN79R9QIhAP////8AAAAA\n"
    "//////////+85vqtpxeehPO5ysL8YyVRAgEBA0IABGhuFibgwLdEJBDOLdvSg1Hc\n"
    "5EJTDxq6ls5FoYLfThp8HOjuwGSz0qw8ocMqyku1y0V5peQ4rEPd0bwcpZd9svA=\n";

  OBufferStream os1;
  bufferSource(publicKeyPkcs8) >> base64Decode() >> streamSink(os1);
  auto pubKey = os1.buf();

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.data()), privateKeyPkcs1.size());

  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::NONE, sKey), Error);

  OBufferStream os2;
  bufferSource(DATA, sizeof(DATA)) >> signerFilter(DigestAlgorithm::SHA256, sKey) >> streamSink(os2);
  auto sig = os2.buf();

  BOOST_CHECK(verifySignature(DATA, sizeof(DATA), sig->data(), sig->size(), pubKey->data(), pubKey->size()));
}

BOOST_AUTO_TEST_SUITE(Hmac)

// Test vectors from RFC 4231
// https://tools.ietf.org/html/rfc4231#section-4
BOOST_AUTO_TEST_CASE(Rfc4231Test1)
{
  // Test case 1
  const uint8_t rawKey[] = {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b};
  const std::string data("Hi There");
  const uint8_t hmacSha224[] = {0x89, 0x6f, 0xb1, 0x12, 0x8a, 0xbb, 0xdf, 0x19, 0x68, 0x32,
                                0x10, 0x7c, 0xd4, 0x9d, 0xf3, 0x3f, 0x47, 0xb4, 0xb1, 0x16,
                                0x99, 0x12, 0xba, 0x4f, 0x53, 0x68, 0x4b, 0x22};
  const uint8_t hmacSha256[] = {0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8,
                                0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00,
                                0xc9, 0x83, 0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32,
                                0xcf, 0xf7};
  const uint8_t hmacSha384[] = {0xaf, 0xd0, 0x39, 0x44, 0xd8, 0x48, 0x95, 0x62, 0x6b, 0x08,
                                0x25, 0xf4, 0xab, 0x46, 0x90, 0x7f, 0x15, 0xf9, 0xda, 0xdb,
                                0xe4, 0x10, 0x1e, 0xc6, 0x82, 0xaa, 0x03, 0x4c, 0x7c, 0xeb,
                                0xc5, 0x9c, 0xfa, 0xea, 0x9e, 0xa9, 0x07, 0x6e, 0xde, 0x7f,
                                0x4a, 0xf1, 0x52, 0xe8, 0xb2, 0xfa, 0x9c, 0xb6};
  const uint8_t hmacSha512[] = {0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d, 0x4f, 0xf0,
                                0xb4, 0x24, 0x1a, 0x1d, 0x6c, 0xb0, 0x23, 0x79, 0xf4, 0xe2,
                                0xce, 0x4e, 0xc2, 0x78, 0x7a, 0xd0, 0xb3, 0x05, 0x45, 0xe1,
                                0x7c, 0xde, 0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7, 0x02,
                                0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4, 0xbe, 0x9d,
                                0x91, 0x4e, 0xeb, 0x61, 0xf1, 0x70, 0x2e, 0x69, 0x6c, 0x20,
                                0x3a, 0x12, 0x68, 0x54};

  PrivateKey key;
  key.loadRaw(KeyType::HMAC, rawKey, sizeof(rawKey));

  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::NONE, key), Error);

  OBufferStream os224;
  bufferSource(data) >> signerFilter(DigestAlgorithm::SHA224, key) >> streamSink(os224);
  auto sig = os224.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(sig->begin(), sig->end(), hmacSha224, hmacSha224 + sizeof(hmacSha224));

  OBufferStream os256;
  bufferSource(data) >> signerFilter(DigestAlgorithm::SHA256, key) >> streamSink(os256);
  sig = os256.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(sig->begin(), sig->end(), hmacSha256, hmacSha256 + sizeof(hmacSha256));

  OBufferStream os384;
  bufferSource(data) >> signerFilter(DigestAlgorithm::SHA384, key) >> streamSink(os384);
  sig = os384.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(sig->begin(), sig->end(), hmacSha384, hmacSha384 + sizeof(hmacSha384));

  OBufferStream os512;
  bufferSource(data) >> signerFilter(DigestAlgorithm::SHA512, key) >> streamSink(os512);
  sig = os512.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(sig->begin(), sig->end(), hmacSha512, hmacSha512 + sizeof(hmacSha512));
}

BOOST_AUTO_TEST_CASE(Rfc4231Test2)
{
  // Test case 2 (HMAC-SHA-256 only)
  const char rawKey[] = "Jefe";
  const std::string data("what do ya want for nothing?");
  const uint8_t hmacSha256[] = {0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e, 0x6a, 0x04,
                                0x24, 0x26, 0x08, 0x95, 0x75, 0xc7, 0x5a, 0x00, 0x3f, 0x08,
                                0x9d, 0x27, 0x39, 0x83, 0x9d, 0xec, 0x58, 0xb9, 0x64, 0xec,
                                0x38, 0x43};

  PrivateKey key;
  key.loadRaw(KeyType::HMAC, reinterpret_cast<const uint8_t*>(rawKey), std::strlen(rawKey));

  OBufferStream os256;
  bufferSource(data) >> signerFilter(DigestAlgorithm::SHA256, key) >> streamSink(os256);
  auto sig = os256.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(sig->begin(), sig->end(), hmacSha256, hmacSha256 + sizeof(hmacSha256));
}

BOOST_AUTO_TEST_CASE(Rfc4231Test3)
{
  // Test case 3 (HMAC-SHA-256 only)
  const uint8_t rawKey[] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                            0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
  const uint8_t data[] = {0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                          0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                          0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                          0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
                          0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd};
  const uint8_t hmacSha256[] = {0x77, 0x3e, 0xa9, 0x1e, 0x36, 0x80, 0x0e, 0x46, 0x85, 0x4d,
                                0xb8, 0xeb, 0xd0, 0x91, 0x81, 0xa7, 0x29, 0x59, 0x09, 0x8b,
                                0x3e, 0xf8, 0xc1, 0x22, 0xd9, 0x63, 0x55, 0x14, 0xce, 0xd5,
                                0x65, 0xfe};

  PrivateKey key;
  key.loadRaw(KeyType::HMAC, rawKey, sizeof(rawKey));

  OBufferStream os256;
  bufferSource(data, sizeof(data)) >> signerFilter(DigestAlgorithm::SHA256, key) >> streamSink(os256);
  auto sig = os256.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(sig->begin(), sig->end(), hmacSha256, hmacSha256 + sizeof(hmacSha256));
}

BOOST_AUTO_TEST_SUITE_END() // Hmac

BOOST_AUTO_TEST_CASE(InvalidKey)
{
  PrivateKey key;
  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::SHA256, key), Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestSignerFilter
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
