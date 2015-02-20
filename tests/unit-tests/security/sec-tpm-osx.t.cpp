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

#include "security/sec-tpm-osx.hpp"
#include "security/cryptopp.hpp"

#include "util/time.hpp"

#include <boost/lexical_cast.hpp>
#include <Availability.h>

#include "boost-test.hpp"

namespace ndn {
namespace tests {

class OsxKeyChainTestFixture
{
public:
  OsxKeyChainTestFixture()
  {
    std::string oldHOME;
    if (std::getenv("OLD_HOME"))
      oldHOME = std::getenv("OLD_HOME");

    if (std::getenv("HOME"))
      m_HOME = std::getenv("HOME");

    if (!oldHOME.empty())
      setenv("HOME", oldHOME.c_str(), 1);
    else
      unsetenv("HOME");
  }

  ~OsxKeyChainTestFixture()
  {
    if (!m_HOME.empty())
      setenv("HOME", m_HOME.c_str(), 1);
    else
      unsetenv("HOME");
  }

protected:
  std::string m_HOME;
};

BOOST_FIXTURE_TEST_SUITE(SecuritySecTpmOsx, OsxKeyChainTestFixture)

BOOST_AUTO_TEST_CASE(Delete)
{
  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/Delete/ksk-" +
               boost::lexical_cast<std::string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));
  RsaKeyParams params(2048);
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
}

BOOST_AUTO_TEST_CASE(SignVerify)
{
  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/SignVerify/ksk-" +
               boost::lexical_cast<std::string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));
  RsaKeyParams params(2048);
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  Data data("/TestSecTpmOsx/SignVaerify/Data/1");
  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};

  Block sigBlock;
  BOOST_CHECK_NO_THROW(sigBlock = tpm.signInTpm(content, sizeof(content),
                                                keyName, DIGEST_ALGORITHM_SHA256));

  shared_ptr<PublicKey> publicKey;
  BOOST_CHECK_NO_THROW(publicKey = tpm.getPublicKeyFromTpm(keyName));
  try
    {
      using namespace CryptoPP;

      RSA::PublicKey rsaPublicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(publicKey->get().buf()), publicKey->get().size());
      rsaPublicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier(rsaPublicKey);
      bool isVerified = verifier.VerifyMessage(content, sizeof(content),
                                               sigBlock.value(), sigBlock.value_size());

      BOOST_CHECK_EQUAL(isVerified, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);
}

BOOST_AUTO_TEST_CASE(RandomGenerator)
{
  SecTpmOsx tpm;

  size_t scale = 1000;
  size_t size = 256 * scale;
  uint8_t* block = new uint8_t[size];
  tpm.generateRandomBlock(block, size);

  std::map<uint8_t, int> counter;
  for (size_t i = 0; i < size; i++)
    {
      counter[block[i]] += 1;
    }

  float dev = 0.0;
  for (size_t i = 0; i != 255; i++)
    {
      dev += ((counter[i] - scale) * (counter[i] - scale)) * 1.0 / (scale * scale);
    }

  BOOST_CHECK_CLOSE(dev / 256, 0.001, 100);

}

BOOST_AUTO_TEST_CASE(ExportImportKey)
{
  using namespace CryptoPP;

  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/ExportImportKey/ksk-" +
               boost::lexical_cast<std::string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));

  RsaKeyParams params(2048);
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);

  ConstBufferPtr exported;
  BOOST_CHECK_NO_THROW(exported = tpm.exportPrivateKeyPkcs5FromTpm(keyName, "1234"));
  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = tpm.getPublicKeyFromTpm(keyName));

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);

  BOOST_REQUIRE(tpm.importPrivateKeyPkcs5IntoTpm(keyName,
                                                 exported->buf(), exported->size(),
                                                 "1234"));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};
  Block sigBlock;
  BOOST_CHECK_NO_THROW(sigBlock = tpm.signInTpm(content, sizeof(content),
                                                keyName, DIGEST_ALGORITHM_SHA256));

  try
    {
      using namespace CryptoPP;

      RSA::PublicKey rsaPublicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(publicKey->get().buf()), publicKey->get().size());
      rsaPublicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier(rsaPublicKey);
      bool isVerified = verifier.VerifyMessage(content, sizeof(content),
                                               sigBlock.value(), sigBlock.value_size());

      BOOST_CHECK_EQUAL(isVerified, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);
  // This is some problem related to Mac OS Key chain.
  // On OSX 10.8, we cannot delete imported keys, but there is no such problem on OSX 10.9.
#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_9
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
#endif
#endif
}

BOOST_AUTO_TEST_CASE(NonExistingKey)
{
  using namespace CryptoPP;

  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/NonExistingKey");

  BOOST_REQUIRE_THROW(tpm.getPublicKeyFromTpm(keyName), SecTpmOsx::Error);

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};
  BOOST_REQUIRE_THROW(tpm.signInTpm(content, sizeof(content), keyName, DIGEST_ALGORITHM_SHA256),
                      SecTpmOsx::Error);

  BOOST_REQUIRE_THROW(tpm.signInTpm(0, 1, keyName, DIGEST_ALGORITHM_SHA256),
                      SecTpmOsx::Error);
}

BOOST_AUTO_TEST_CASE(EcdsaSigning)
{
  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/EcdsaSigning/ksk-" +
               boost::lexical_cast<std::string>(time::toUnixTimestamp(time::system_clock::now())));
  EcdsaKeyParams params;
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  Data data("/TestSecTpmOsx/EcdsaSigning/Data/1");
  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};

  Block sigBlock;
  BOOST_CHECK_NO_THROW(sigBlock = tpm.signInTpm(content, sizeof(content),
                                                keyName, DIGEST_ALGORITHM_SHA256));

  shared_ptr<PublicKey> pubkeyPtr;
  BOOST_CHECK_NO_THROW(pubkeyPtr = tpm.getPublicKeyFromTpm(keyName));

  try
    {
      using namespace CryptoPP;

      ECDSA<ECP, SHA256>::PublicKey publicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(pubkeyPtr->get().buf()), pubkeyPtr->get().size());
      publicKey.Load(queue);

      uint8_t buffer[64];
      size_t usedSize = DSAConvertSignatureFormat(buffer, 64, DSA_P1363,
                                                  sigBlock.value(), sigBlock.value_size(), DSA_DER);

      ECDSA<ECP, SHA256>::Verifier verifier(publicKey);
      bool result = verifier.VerifyMessage(content, sizeof(content),
                                           buffer, usedSize);

      BOOST_CHECK_EQUAL(result, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);
}


BOOST_AUTO_TEST_CASE(ExportImportEcdsaKey)
{
  using namespace CryptoPP;

  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/ExportImportEcdsaKey/ksk-" +
               boost::lexical_cast<std::string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));

  EcdsaKeyParams params;
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);

  ConstBufferPtr exported;
  BOOST_CHECK_NO_THROW(exported = tpm.exportPrivateKeyPkcs5FromTpm(keyName, "1234"));

  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = tpm.getPublicKeyFromTpm(keyName));

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);

  BOOST_REQUIRE(tpm.importPrivateKeyPkcs5IntoTpm(keyName,
                                                 exported->buf(), exported->size(),
                                                 "1234"));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};
  Block sigBlock;
  BOOST_CHECK_NO_THROW(sigBlock = tpm.signInTpm(content, sizeof(content),
                                                keyName, DIGEST_ALGORITHM_SHA256));

  try
    {
      using namespace CryptoPP;

      ECDSA<ECP, SHA256>::PublicKey ecdsaPublicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(publicKey->get().buf()), publicKey->get().size());
      ecdsaPublicKey.Load(queue);

      uint8_t buffer[64];
      size_t usedSize = DSAConvertSignatureFormat(buffer, 64, DSA_P1363,
                                                  sigBlock.value(), sigBlock.value_size(),
                                                  DSA_DER);

      ECDSA<ECP, SHA256>::Verifier verifier(ecdsaPublicKey);
      bool isVerified = verifier.VerifyMessage(content, sizeof(content),
                                               buffer, usedSize);

      BOOST_CHECK_EQUAL(isVerified, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);
  // This is some problem related to Mac OS Key chain.
  // On OSX 10.8, we cannot delete imported keys, but there is no such problem on OSX 10.9.
#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_9
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
#endif
#endif
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
