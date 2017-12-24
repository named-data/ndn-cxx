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

#include "security/tpm/back-end.hpp"
#include "security/tpm/back-end-mem.hpp"
#include "security/tpm/key-handle.hpp"
#include "security/tpm/tpm.hpp"
#include "security/transform.hpp"
#include "security/transform/public-key.hpp"
#include "security/transform/private-key.hpp"
#include "encoding/buffer-stream.hpp"
#include "security/pib/key.hpp"

#include "back-end-wrapper-file.hpp"
#include "back-end-wrapper-mem.hpp"
#ifdef NDN_CXX_HAVE_OSX_FRAMEWORKS
#include "back-end-wrapper-osx.hpp"
#endif // NDN_CXX_HAVE_OSX_FRAMEWORKS

#include "boost-test.hpp"

#include <boost/mpl/vector.hpp>
#include <set>

namespace ndn {
namespace security {
namespace tpm {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Tpm)
BOOST_AUTO_TEST_SUITE(TestBackEnd)

using tpm::Tpm;

using TestBackEnds = boost::mpl::vector<
#ifdef NDN_CXX_HAVE_OSX_FRAMEWORKS
  BackEndWrapperOsx,
#endif // NDN_CXX_HAVE_OSX_FRAMEWORKS
  BackEndWrapperMem,
  BackEndWrapperFile>;

BOOST_AUTO_TEST_CASE_TEMPLATE(KeyManagement, T, TestBackEnds)
{
  T wrapper;
  BackEnd& tpm = wrapper.getTpm();

  Name identity("/Test/KeyName");
  name::Component keyId("1");
  Name keyName = v2::constructKeyName(identity, keyId);

  // key should not exist
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), false);
  BOOST_CHECK(tpm.getKeyHandle(keyName) == nullptr);

  // create key, should exist
  BOOST_CHECK(tpm.createKey(identity, RsaKeyParams(keyId)) != nullptr);
  BOOST_CHECK(tpm.hasKey(keyName));
  BOOST_CHECK(tpm.getKeyHandle(keyName) != nullptr);

  // create a key with the same name, should throw error
  BOOST_CHECK_THROW(tpm.createKey(identity, RsaKeyParams(keyId)), Tpm::Error);

  // delete key, should not exist
  tpm.deleteKey(keyName);
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), false);
  BOOST_CHECK(tpm.getKeyHandle(keyName) == nullptr);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RsaSigning, T, TestBackEnds)
{
  T wrapper;
  BackEnd& tpm = wrapper.getTpm();

  // create an rsa key
  Name identity("/Test/KeyName");

  unique_ptr<KeyHandle> key = tpm.createKey(identity, RsaKeyParams());
  Name keyName = key->getKeyName();

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};
  Block sigBlock(tlv::SignatureValue, key->sign(DigestAlgorithm::SHA256, content, sizeof(content)));

  transform::PublicKey pubKey;
  ConstBufferPtr pubKeyBits = key->derivePublicKey();
  pubKey.loadPkcs8(pubKeyBits->data(), pubKeyBits->size());

  bool result;
  {
    using namespace transform;
    bufferSource(content, sizeof(content)) >>
      verifierFilter(DigestAlgorithm::SHA256, pubKey, sigBlock.value(), sigBlock.value_size()) >>
      boolSink(result);
  }
  BOOST_CHECK_EQUAL(result, true);

  tpm.deleteKey(keyName);
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), false);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RsaDecryption, T, TestBackEnds)
{
  T wrapper;
  BackEnd& tpm = wrapper.getTpm();

  // create an rsa key
  Name identity("/Test/KeyName");

  unique_ptr<KeyHandle> key = tpm.createKey(identity, RsaKeyParams());
  Name keyName = key->getKeyName();

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};

  transform::PublicKey pubKey;
  ConstBufferPtr pubKeyBits = key->derivePublicKey();
  pubKey.loadPkcs8(pubKeyBits->data(), pubKeyBits->size());

  ConstBufferPtr cipherText = pubKey.encrypt(content, sizeof(content));

  ConstBufferPtr plainText = key->decrypt(cipherText->data(), cipherText->size());

  BOOST_CHECK_EQUAL_COLLECTIONS(content, content + sizeof(content),
                                plainText->begin(), plainText->end());

  tpm.deleteKey(keyName);
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), false);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(EcdsaSigning, T, TestBackEnds)
{
  T wrapper;
  BackEnd& tpm = wrapper.getTpm();

  // create an ec key
  Name identity("/Test/Ec/KeyName");

  unique_ptr<KeyHandle> key = tpm.createKey(identity, EcKeyParams());
  Name ecKeyName = key->getKeyName();

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};
  Block sigBlock(tlv::SignatureValue, key->sign(DigestAlgorithm::SHA256, content, sizeof(content)));

  transform::PublicKey pubKey;
  ConstBufferPtr pubKeyBits = key->derivePublicKey();
  pubKey.loadPkcs8(pubKeyBits->data(), pubKeyBits->size());

  bool result;
  {
    using namespace transform;
    bufferSource(content, sizeof(content)) >> verifierFilter(DigestAlgorithm::SHA256, pubKey,
                                                             sigBlock.value(), sigBlock.value_size())
                                           >> boolSink(result);
  }
  BOOST_CHECK_EQUAL(result, true);

  tpm.deleteKey(ecKeyName);
  BOOST_CHECK_EQUAL(tpm.hasKey(ecKeyName), false);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ImportExport, T, TestBackEnds)
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

  T wrapper;
  BackEnd& tpm = wrapper.getTpm();

  Name keyName("/Test/KeyName/KEY/1");
  tpm.deleteKey(keyName);
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), false);

  transform::PrivateKey sKey;
  sKey.loadPkcs1Base64(reinterpret_cast<const uint8_t*>(privateKeyPkcs1.c_str()), privateKeyPkcs1.size());

  std::string password("password");
  OBufferStream os;
  sKey.savePkcs8(os, password.c_str(), password.size());
  ConstBufferPtr privateKeyBuffer = os.buf();

  tpm.importKey(keyName, privateKeyBuffer->data(), privateKeyBuffer->size(), password.c_str(), password.size());
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), true);
  BOOST_CHECK_THROW(tpm.importKey(keyName, privateKeyBuffer->data(), privateKeyBuffer->size(), password.c_str(), password.size()),
                    BackEnd::Error);

  ConstBufferPtr exportedKey = tpm.exportKey(keyName, password.c_str(), password.size());
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), true);

  transform::PrivateKey sKey2;
  sKey2.loadPkcs8(exportedKey->data(), exportedKey->size(), password.c_str(), password.size());
  OBufferStream os2;
  sKey.savePkcs1Base64(os2);
  ConstBufferPtr pkcs1Buffer = os2.buf();

  BOOST_CHECK_EQUAL_COLLECTIONS(privateKeyPkcs1.begin(), privateKeyPkcs1.end(),
                                pkcs1Buffer->begin(), pkcs1Buffer->end());

  tpm.deleteKey(keyName);
  BOOST_CHECK_EQUAL(tpm.hasKey(keyName), false);
  BOOST_CHECK_THROW(tpm.exportKey(keyName, password.c_str(), password.size()), BackEnd::Error);
}

BOOST_AUTO_TEST_CASE(RandomKeyId)
{
  BackEndWrapperMem wrapper;
  BackEnd& tpm = wrapper.getTpm();
  Name identity("/Test/KeyName");

  std::set<Name> keyNames;
  for (int i = 0; i < 100; i++) {
    auto key = tpm.createKey(identity, RsaKeyParams());
    Name keyName = key->getKeyName();
    tpm.deleteKey(keyName);
    BOOST_CHECK(keyNames.insert(keyName).second);
  }
}

BOOST_AUTO_TEST_SUITE_END() // TestBackEnd
BOOST_AUTO_TEST_SUITE_END() // Tpm
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace tpm
} // namespace security
} // namespace ndn
