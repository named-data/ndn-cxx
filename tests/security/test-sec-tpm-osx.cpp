/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "security/key-chain.hpp"
#include "util/time.hpp"
#include "security/cryptopp.hpp"

#include "boost-test.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestSecTpmOsx)

BOOST_AUTO_TEST_CASE (Delete)
{
  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/Delete/ksk-" +
               boost::lexical_cast<string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
}

BOOST_AUTO_TEST_CASE (SignVerify)
{
  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/SignVerify/ksk-" +
               boost::lexical_cast<string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048));

  Data data("/TestSecTpmOsx/SignVaerify/Data/1");
  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};

  Block sigBlock;
  BOOST_CHECK_NO_THROW(sigBlock = tpm.signInTpm(content, sizeof(content),
                                                keyName, DIGEST_ALGORITHM_SHA256));

  shared_ptr<PublicKey> pubkeyPtr;
  BOOST_CHECK_NO_THROW(pubkeyPtr = tpm.getPublicKeyFromTpm(keyName));
  try
    {
      using namespace CryptoPP;

      RSA::PublicKey publicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(pubkeyPtr->get().buf()), pubkeyPtr->get().size());
      publicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier (publicKey);
      bool result = verifier.VerifyMessage(content, sizeof(content),
                                           sigBlock.value(), sigBlock.value_size());

      BOOST_CHECK_EQUAL(result, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);
}

BOOST_AUTO_TEST_CASE (RandomGenerator)
{
  SecTpmOsx tpm;

  size_t scale = 1000;
  size_t size = 256 * scale;
  uint8_t* block = new uint8_t[size];
  tpm.generateRandomBlock(block, size);

  map<uint8_t, int> counter;
  for(size_t i = 0; i < size; i++)
    counter[block[i]] += 1;

  float dev = 0.0;
  for(size_t i = 0; i != 255; i++)
    dev += ((counter[i] - scale) * (counter[i] - scale)) * 1.0 / (scale * scale);

  BOOST_CHECK_CLOSE(dev / 256, 0.001, 100);

}

BOOST_AUTO_TEST_CASE (ExportImportKey)
{
  using namespace CryptoPP;

  SecTpmOsx tpm;

  Name keyName("/TestSecTpmOsx/ExportImportKey/ksk-" +
               boost::lexical_cast<string>(
                 time::toUnixTimestamp(time::system_clock::now()).count()));

  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048));

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == true);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == true);

  ConstBufferPtr exported;
  BOOST_CHECK_NO_THROW(exported = tpm.exportPrivateKeyPkcs5FromTpm(keyName, "1234"));
  shared_ptr<PublicKey> pubkeyPtr;
  BOOST_REQUIRE_NO_THROW(pubkeyPtr = tpm.getPublicKeyFromTpm(keyName));

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);

  BOOST_REQUIRE(tpm.importPrivateKeyPkcs5IntoTpm(keyName,
                                                 exported->buf(), exported->size(),
                                                 "1234"));

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == true);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == true);

  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};
  Block sigBlock;
  BOOST_CHECK_NO_THROW(sigBlock = tpm.signInTpm(content, sizeof(content),
                                                keyName, DIGEST_ALGORITHM_SHA256));

  try
    {
      using namespace CryptoPP;

      RSA::PublicKey publicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(pubkeyPtr->get().buf()), pubkeyPtr->get().size());
      publicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier (publicKey);
      bool result = verifier.VerifyMessage(content, sizeof(content),
                                           sigBlock.value(), sigBlock.value_size());

      BOOST_CHECK_EQUAL(result, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);
  // This is some problem related to Mac OS Key chain, and we will fix it later.
  // BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  // BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
