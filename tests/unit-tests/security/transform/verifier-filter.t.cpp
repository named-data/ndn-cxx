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

#include "security/transform/verifier-filter.hpp"

#include "encoding/buffer-stream.hpp"
#include "security/transform/base64-decode.hpp"
#include "security/transform/bool-sink.hpp"
#include "security/transform/buffer-source.hpp"
#include "security/transform/private-key.hpp"
#include "security/transform/public-key.hpp"
#include "security/transform/signer-filter.hpp"
#include "security/transform/stream-sink.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestVerifierFilter)

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
    "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDDRYzX9aEDHIu0\n"
    "SqyIAmBYNla7ORil5hV155lypFnPN9GAFBOw2jNLm3gefBNmHDBdsfuTdA3SRFNX\n"
    "zbpehvCzeKKCiGcz3JMr4oqJrI+EgCb/7kcZPpKAUmFiGqjX+6lZ256iOKXpE/j8\n"
    "CBq8eGinGUBVuTob1V1WeVBxDlm/Q5waLvdz4SdgP9iBRFgZKeBSL9ieyHvv2nZT\n"
    "r3+172BnAgvk+4T0GI7ojf1wv3PsQrVJ9mE5a8N7+oftiEP8EfBxaK3wWNHDDWCX\n"
    "BFVMmwD2sfnzKdoCLHS1xH+cExEtRf57etREeDpRtKMlt1v2qYozV9MYcpTMv/mk\n"
    "wbq4FTTLAgMBAAECggEANCRyQ4iXghkxROdbwsW/rE52QnAwoLwbpuw9EVvJj4e8\n"
    "LZMu3t6lK99L5/gBxhZo49wO7YTj2+3aw2twBKXLyGDCJFEAHd0cf29yxuiJOjxu\n"
    "LZEW8yq+O/3De0rbIzFUO2ZlqbOuudpXdhVD7mfIqjYX88wONDh5QAoM7OOEG4oe\n"
    "xkFMWcDUwU0j5QqPlfhinrgMWYqXFNf9TZvDNXLCjmHPHZSHDnWOaguWzhhS8wlc\n"
    "PTBblm1hG4+iBe9dv+h/15//bT/BTXVYUqBdviB9HzNRdpdLWxdydWbf7bi8iz10\n"
    "ClTDKS6jKM6rFapwdF5zZBPYXFUaQUStrN4I9riswQKBgQDljwLLCiYhxOB6sUYU\n"
    "J4wcmvydAapjZX+jAVveT2ZpzM+cL2nhr1FzmzMvED0UxgXG6tBkwFZIQbYlLUdH\n"
    "aaeOKDHxQqNgwv8D6u++Nk4x7gzpLLaCCHhKQtkqlZPONN7TsHIz+Pm/9KM1mFYA\n"
    "buzDj8uY8ZFCTAm/4pmEaiO46QKBgQDZw4VPpwlG/qS/NPP1LQI5k5Wb564mH8Fe\n"
    "nugCwCZs186lyQ8zOodfLz/Cl0qXoABwHns67O2U19XUPuq9vPsm5GVjBDRwR8GB\n"
    "tk9zPWnXwccNeHCfntk9vwbfdiH06aDQc0AiZvguxW5KrEDo3BKPtylF6SBN52uE\n"
    "sU8n5h1vkwKBgQCwzdDs6MgtwiDS3q6G316+uXBOzPWa0JXZyjYjpyvN2P0d4ja+\n"
    "p/UoASUO3obs9QeGCVyv/KN3y4SqZZE8o1d12ed9VkHXSNh4//3elpzrP9mZzeJT\n"
    "jIp5R7tTXRkV/QqSKJgNB3n0Kkt5//ZdJxIcHShGh+fFFCN+Mtzia41P4QKBgQCV\n"
    "wOTTow45OXL4XyUJzVsDV2ACaDAV3a6wMF1jTtrd7QcacYs3cp+XsLmLS1mrrge/\n"
    "Eucx3a+AtXFCVcY+l1CsLVMf5cteD6qeVk6K9IfuLT+DHvlse+Pvl4fVcrrlXykN\n"
    "UMShI+i22WUAizbULEvDc3U5s5lYmbYR+ZFy4cgKawKBgC0UnWJ2oygfERLeaVGl\n"
    "/YnHJC50/dIKbZakaapXOFFgiep5q1jmxR2U8seb+nvtFPsTLFAdOXCfwUk+4z/h\n"
    "kfWtB3+8H5jyoC1gkJ7EMyxu8tb4mz5U6+SPB4QLSetwvfWP2YXS/PkTq19G7iGE\n"
    "novjJ9azSBJ6OyR5UH/DxBji\n";
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};

  OBufferStream os1;
  bufferSource(publicKeyPkcs8) >> base64Decode() >> streamSink(os1);
  auto pubKey = os1.buf();

  PublicKey pKey;
  pKey.loadPkcs8(pubKey->data(), pubKey->size());

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.data()), privateKeyPkcs1.size());

  OBufferStream os2;
  bufferSource(data, sizeof(data)) >> signerFilter(DigestAlgorithm::SHA256, sKey) >> streamSink(os2);
  auto sig = os2.buf();

  BOOST_CHECK_THROW(VerifierFilter(DigestAlgorithm::NONE, pKey, sig->data(), sig->size()), Error);

  bool result = false;
  bufferSource(data, sizeof(data)) >>
    verifierFilter(DigestAlgorithm::SHA256, pKey, sig->data(), sig->size()) >>
    boolSink(result);

  BOOST_CHECK_EQUAL(result, true);
}

BOOST_AUTO_TEST_CASE(Ecdsa)
{
  const std::string privateKeyPkcs1 =
    "MIIBeQIBADCCAQMGByqGSM49AgEwgfcCAQEwLAYHKoZIzj0BAQIhAP////8AAAAB\n"
    "AAAAAAAAAAAAAAAA////////////////MFsEIP////8AAAABAAAAAAAAAAAAAAAA\n"
    "///////////////8BCBaxjXYqjqT57PrvVV2mIa8ZR0GsMxTsPY7zjw+J9JgSwMV\n"
    "AMSdNgiG5wSTamZ44ROdJreBn36QBEEEaxfR8uEsQkf4vOblY6RA8ncDfYEt6zOg\n"
    "9KE5RdiYwpZP40Li/hp/m47n60p8D54WK84zV2sxXs7LtkBoN79R9QIhAP////8A\n"
    "AAAA//////////+85vqtpxeehPO5ysL8YyVRAgEBBG0wawIBAQQgRxwcbzK9RV6A\n"
    "HYFsDcykI86o3M/a1KlJn0z8PcLMBZOhRANCAARobhYm4MC3RCQQzi3b0oNR3ORC\n"
    "Uw8aupbORaGC304afBzo7sBks9KsPKHDKspLtctFeaXkOKxD3dG8HKWXfbLw\n";
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

  PublicKey pKey;
  pKey.loadPkcs8(pubKey->data(), pubKey->size());

  PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.data()), privateKeyPkcs1.size());

  OBufferStream os2;
  bufferSource(data, sizeof(data)) >> signerFilter(DigestAlgorithm::SHA256, sKey) >> streamSink(os2);
  auto sig = os2.buf();

  BOOST_CHECK_THROW(VerifierFilter(DigestAlgorithm::NONE, pKey, sig->data(), sig->size()), Error);

  bool result = false;
  bufferSource(data, sizeof(data)) >>
    verifierFilter(DigestAlgorithm::SHA256, pKey, sig->data(), sig->size()) >>
    boolSink(result);

  BOOST_CHECK_EQUAL(result, true);
}

BOOST_AUTO_TEST_CASE(InvalidKey)
{
  PublicKey pKey;
  BOOST_CHECK_THROW(VerifierFilter(DigestAlgorithm::SHA256, pKey, nullptr, 0), Error);
}

BOOST_AUTO_TEST_SUITE_END() // TestVerifierFilter
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
