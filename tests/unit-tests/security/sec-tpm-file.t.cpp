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

#include "security/sec-tpm-file.hpp"
#include "security/key-chain.hpp"
#include "security/cryptopp.hpp"

#include "util/time.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecuritySecTpmFile)

BOOST_AUTO_TEST_CASE(Delete)
{
  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/Delete/ksk-" +
               boost::lexical_cast<std::string>(time::toUnixTimestamp(time::system_clock::now())));
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
  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/SignVerify/ksk-" +
               boost::lexical_cast<std::string>(time::toUnixTimestamp(time::system_clock::now())));
  RsaKeyParams params(2048);
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  Data data("/tmp/test/1");
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

BOOST_AUTO_TEST_CASE(RandomGenerator)
{
  SecTpmFile tpm;

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

BOOST_AUTO_TEST_CASE(ImportExportKey)
{
  using namespace CryptoPP;

  std::string imported =
"MIIFDjBABgkqhkiG9w0BBQ0wMzAbBgkqhkiG9w0BBQwwDgQIIJzwsbTIVqgCAggAMBQGCCqG"
"SIb3DQMHBAiEsSKdgGkCEQSCBMjaYsEeHM/EPzGNDAPaSxE1ASKxjyNkXEVPQbtGx/ju1PlQ"
"QakTCzPpia8ziVGihrVfuacHrxxbLh+ufNZtbVVsv9kGQL2g15BN2Nic8VqyTxplKrduFkEb"
"eOipqu627gb6eTsrSj7kubXfs/w9OAdhRZFaEc7I/S6rcPUaDai4i3O03VuvuTOrzZL/unhD"
"CDzOa3pr2aUQlO/5Pck6GbSYL1/4aW/fB7djZrZAjsGPTqIYpPa16oWlrzsILQ4650Zlvdn8"
"feh6elTrA4qy6RljZfhIHxzGAeuGbVVLT7zavs01y+QE88sxPpeZ0RGtlVxCYp+xoB5hOO/x"
"Xm3n0r+HVIaBV9rHLuUSUiHVAtzwuOj/XKh7YBNXeF58lcv8Npsx10etrPuV5hRQelYirKo1"
"QbT+Aq7sLdWI09eGClDHjvRgw54lCFEUAVUFLxhBnzfVUfw/UiR2SheixH+c7cY3gOOvSMQh"
"1oL9Omii4S6yhzfbD3hRN/wBKC0NgiIOLRR+Ti0mEEbTxYQrVcHC9rpRcnr1fFAiQvOXrOw0"
"HyxsbnOeYpoURgLJmUyItPi09xUMCHwUPZ/sI6aG5eRqVUGgcYaQib7MBbGG/D2PlWaKZx1w"
"iEYtYUI7rrc7Qc2ltp4i1u46ZLtSOxUi8kM7qK2Yp1AOtcWFlGn0XBXpGsHanZRzs8KzfDjQ"
"OPmRIuFXFmuJweqUGmg6c8P3wHYueenB4oowYELYaDwmmV96obXMG5hbsKne5kcem875d6K+"
"hL2QzjAl+na4tUZyXYXDdUO2lgTT8IItujBn6c+b8vDcZ24NcYyWPHHb16J4uvKi/6Zb245g"
"/N0NC6hB43KE+2F0BH0eyXRzCnen6AjF8nvx/wgYrXsFlsU47P8gaL1e7V6QECkY4vIk6//b"
"LINmpcgZoAv23fgjvYYTMfS/IyPsooS5DJarbHzbsgAWOuqP2ewYulrKqLi0vaUyoErRedQC"
"8J5p9c4KF56++ameiwxbyKnDynHIIFCOM8eamLnDH4VvU9E2mtfZxmi8kWDWx/NhKEK9zbQv"
"WuaGDpOysgPK4myTp2QGQNQDvBB92gMcrVTcY//3OGHSXeJrkUfDKLOUDyz7ROghEt1vpRTL"
"j9Xdbm/01O/0MLBqyXDY119b1qAVVY7Y2CxRIRFdw2V76INWNIwPi46t/VUGwMBGmEux8S8+"
"79Mv7UyGhKvSUr88pWCS1KdSz1HhN1VWjDvyXPyg96+SecpZMwXCdkUCLdLdD3/8rJLt7wS5"
"3K5/1V4sacds8GHx7NckhQ/KV0VDlUEFuaOCS4Sd3XXetXs4IFSqtKH+Rn0jX7d8Igcw7X2b"
"eaV1gxOVquDEwLDT5+wXURw96ahq0caL/4YfoBUQIOQ6HGtPTWJz1Yax0pGu9F30VEY/ObrK"
"L/B6peJKfqhQGJ+MObqOiP6yHBZH75EImLAkksSVmREdNVjwWkz9D9vzPoAnmP8sQ3/NZarB"
"Ak+ynQjc19t+CCecPrS2Tlh0cJZkHRiGFF7J5a3ci7yBvg3HfD86AXMRBQuSG1UG8TrzC6BK"
"+qIQ8DWecQwBMZ3nv/Flo2Fejd/G4/wWe6ObMytC5SB8pJNOq9ri0Ff6Zh3rPrwaSv1PPgFJ"
"GOw=";

  std::string decoded;
  BOOST_CHECK_NO_THROW(StringSource source(imported,
                                           true,
                                           new Base64Decoder(new StringSink(decoded))));

  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/ImportKey/ksk-" +
               boost::lexical_cast<std::string>(time::toUnixTimestamp(time::system_clock::now())));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);

  BOOST_REQUIRE_NO_THROW(
    tpm.importPrivateKeyPkcs5IntoTpm(keyName,
                                     reinterpret_cast<const uint8_t*>(decoded.c_str()),
                                     decoded.size(),
                                     "1234"));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);

  shared_ptr<PublicKey> publicKey;
  BOOST_CHECK_NO_THROW(publicKey = tpm.getPublicKeyFromTpm(keyName));

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

  ConstBufferPtr exported;
  BOOST_CHECK_NO_THROW(exported = tpm.exportPrivateKeyPkcs5FromTpm(keyName, "5678"));

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);

  BOOST_REQUIRE(tpm.importPrivateKeyPkcs5IntoTpm(keyName, exported->buf(), exported->size(),
                                                 "5678"));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);

  const uint8_t content2[] = {0x05, 0x06, 0x07, 0x08};
  Block sigBlock2;
  BOOST_CHECK_NO_THROW(sigBlock2 = tpm.signInTpm(content2, sizeof(content2),
                                                 keyName, DIGEST_ALGORITHM_SHA256));

  try
    {
      using namespace CryptoPP;

      RSA::PublicKey rsaPublicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(publicKey->get().buf()), publicKey->get().size());
      rsaPublicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier(rsaPublicKey);
      bool isVerified = verifier.VerifyMessage(content2, sizeof(content2),
                                               sigBlock2.value(), sigBlock2.value_size());

      BOOST_CHECK_EQUAL(isVerified, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
}

BOOST_AUTO_TEST_CASE(EcdsaSigning)
{
  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/EcdsaSigning/ksk-" +
               boost::lexical_cast<std::string>(time::toUnixTimestamp(time::system_clock::now())));
  EcdsaKeyParams params;
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, params));

  Data data("/TestSecTpmFile/EcdsaSigning/Data/1");
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


BOOST_AUTO_TEST_CASE(ImportExportEcdsaKey)
{
  using namespace CryptoPP;

  std::string imported =
    "MIGMMEAGCSqGSIb3DQEFDTAzMBsGCSqGSIb3DQEFDDAOBAhqkJiLfzFWtQICCAAw"
    "FAYIKoZIhvcNAwcECJ1HLtP8OZC6BEgNv9OH2mZdbkxvqTVlRBkUqPbbP3580OG6"
    "f0htqWSRppcb4IEKYfuPt2qPCYKL2GcAN2pU3eJqhiM7LFTSFaxgBRFozzIwusk=";

  std::string decoded;
  BOOST_CHECK_NO_THROW(StringSource source(imported,
                                           true,
                                           new Base64Decoder(new StringSink(decoded))));

  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/ImportExportEcdsaKey/ksk-" +
               boost::lexical_cast<std::string>(time::toUnixTimestamp(time::system_clock::now())));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);

  BOOST_REQUIRE_NO_THROW(
    tpm.importPrivateKeyPkcs5IntoTpm(keyName,
                                     reinterpret_cast<const uint8_t*>(decoded.c_str()),
                                     decoded.size(),
                                     "5678"));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);

  shared_ptr<PublicKey> publicKey;
  BOOST_CHECK_NO_THROW(publicKey = tpm.getPublicKeyFromTpm(keyName));

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
                                                  sigBlock.value(), sigBlock.value_size(), DSA_DER);

      ECDSA<ECP, SHA256>::Verifier verifier(ecdsaPublicKey);
      bool isVerified = verifier.VerifyMessage(content, sizeof(content),
                                               buffer, usedSize);

      BOOST_CHECK_EQUAL(isVerified, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  ConstBufferPtr exported;
  BOOST_CHECK_NO_THROW(exported = tpm.exportPrivateKeyPkcs5FromTpm(keyName, "1234"));

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);

  BOOST_REQUIRE(tpm.importPrivateKeyPkcs5IntoTpm(keyName, exported->buf(), exported->size(),
                                                 "1234"));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);

  const uint8_t content2[] = {0x05, 0x06, 0x07, 0x08};
  Block sigBlock2;
  BOOST_CHECK_NO_THROW(sigBlock2 = tpm.signInTpm(content2, sizeof(content2),
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
                                                  sigBlock2.value(), sigBlock2.value_size(),
                                                  DSA_DER);

      ECDSA<ECP, SHA256>::Verifier verifier(ecdsaPublicKey);
      bool isVerified = verifier.VerifyMessage(content2, sizeof(content2),
                                               buffer, usedSize);

      BOOST_CHECK_EQUAL(isVerified, true);

    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
