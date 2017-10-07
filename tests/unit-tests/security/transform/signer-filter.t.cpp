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

#include "security/transform/signer-filter.hpp"

#include "encoding/buffer-stream.hpp"
#include "security/transform/base64-decode.hpp"
#include "security/transform/buffer-source.hpp"
#include "security/transform/private-key.hpp"
#include "security/transform/stream-sink.hpp"
#include "security/verification-helpers.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestSignerFilter)

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
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};

  OBufferStream os1;
  bufferSource(publicKeyPkcs8) >> base64Decode() >> streamSink(os1);
  auto pubKey = os1.buf();

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.data()), privateKeyPkcs1.size());

  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::NONE, sKey), Error);

  OBufferStream os2;
  bufferSource(data, sizeof(data)) >> signerFilter(DigestAlgorithm::SHA256, sKey) >> streamSink(os2);
  auto sig = os2.buf();

  BOOST_CHECK(verifySignature(data, sizeof(data), sig->data(), sig->size(), pubKey->data(), pubKey->size()));
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
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};

  OBufferStream os1;
  bufferSource(publicKeyPkcs8) >> base64Decode() >> streamSink(os1);
  auto pubKey = os1.buf();

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.data()), privateKeyPkcs1.size());

  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::NONE, sKey), Error);

  OBufferStream os2;
  bufferSource(data, sizeof(data)) >> signerFilter(DigestAlgorithm::SHA256, sKey) >> streamSink(os2);
  auto sig = os2.buf();

  BOOST_CHECK(verifySignature(data, sizeof(data), sig->data(), sig->size(), pubKey->data(), pubKey->size()));
}

BOOST_AUTO_TEST_CASE(InvalidKey)
{
  PrivateKey sKey;
  BOOST_CHECK_THROW(SignerFilter(DigestAlgorithm::SHA256, sKey), Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestSignerFilter
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
