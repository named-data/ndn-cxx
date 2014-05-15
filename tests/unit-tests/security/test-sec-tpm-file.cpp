/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include <boost/lexical_cast.hpp>

#include "boost-test.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(SecurityTestSecTpmFile)

BOOST_AUTO_TEST_CASE (Delete)
{
  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/Delete/ksk-" +
               boost::lexical_cast<string>(time::toUnixTimestamp(time::system_clock::now())));
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048));

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
}

BOOST_AUTO_TEST_CASE (SignVerify)
{
  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/SignVerify/ksk-" +
               boost::lexical_cast<string>(time::toUnixTimestamp(time::system_clock::now())));
  BOOST_CHECK_NO_THROW(tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048));

  Data data("/tmp/test/1");
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
  SecTpmFile tpm;

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

BOOST_AUTO_TEST_CASE (ImportExportKey)
{
  using namespace CryptoPP;

  string imported(
"3082050E304006092A864886F70D01050D3033301B06092A864886F70D01050C300E0408209CF0B1B4C856A802020800301406082A864886F70D0307040884B1229D80690211048204C8DA62C11E1CCFC43F318D0C03DA4B11350122B18F23645C454F41BB46C7F8EED4F95041A9130B33E989AF338951A286B55FB9A707AF1C5B2E1FAE7CD66D6D556CBFD90640BDA0D7904DD8D89CF15AB24F1A652AB76E16411B78E8A9AAEEB6EE06FA793B2B4A3EE4B9B5DFB3FC3D38076145915A11CEC8FD2EAB70F51A0DA8B88B73B4DD5BAFB933ABCD92FFBA7843083CCE6B7A6BD9A51094EFF93DC93A19B4982F5FF8696FDF07B76366B6408EC18F4EA218A4F6B5EA85A5AF3B082D0E3AE74665BDD9FC7DE87A7A54EB038AB2E9196365F8481F1CC601EB866D554B4FBCDABECD35CBE404F3CB313E9799D111AD955C42629FB1A01E6138EFF15E6DE7D2BF8754868157DAC72EE5125221D502DCF0B8E8FF5CA87B601357785E7C95CBFC369B31D747ADACFB95E614507A5622ACAA3541B4FE02AEEC2DD588D3D7860A50C78EF460C39E250851140155052F18419F37D551FC3F5224764A17A2C47F9CEDC63780E3AF48C421D682FD3A68A2E12EB28737DB0F785137FC01282D0D82220E2D147E4E2D261046D3C5842B55C1C2F6BA51727AF57C502242F397ACEC341F2C6C6E739E629A144602C9994C88B4F8B4F7150C087C143D9FEC23A686E5E46A5541A071869089BECC05B186FC3D8F95668A671D7088462D61423BAEB73B41CDA5B69E22D6EE3A64BB523B1522F2433BA8AD98A7500EB5C5859469F45C15E91AC1DA9D9473B3C2B37C38D038F99122E157166B89C1EA941A683A73C3F7C0762E79E9C1E28A306042D8683C26995F7AA1B5CC1B985BB0A9DEE6471E9BCEF977A2BE84BD90CE3025FA76B8B546725D85C37543B69604D3F0822DBA3067E9CF9BF2F0DC676E0D718C963C71DBD7A278BAF2A2FFA65BDB8E60FCDD0D0BA841E37284FB6174047D1EC974730A77A7E808C5F27BF1FF0818AD7B0596C538ECFF2068BD5EED5E90102918E2F224EBFFDB2C8366A5C819A00BF6DDF823BD861331F4BF2323ECA284B90C96AB6C7CDBB200163AEA8FD9EC18BA5ACAA8B8B4BDA532A04AD179D402F09E69F5CE0A179EBEF9A99E8B0C5BC8A9C3CA71C820508E33C79A98B9C31F856F53D1369AD7D9C668BC9160D6C7F3612842BDCDB42F5AE6860E93B2B203CAE26C93A7640640D403BC107DDA031CAD54DC63FFF73861D25DE26B9147C328B3940F2CFB44E82112DD6FA514CB8FD5DD6E6FF4D4EFF430B06AC970D8D75F5BD6A015558ED8D82C5121115DC3657BE88356348C0F8B8EADFD5506C0C046984BB1F12F3EEFD32FED4C8684ABD252BF3CA56092D4A752CF51E13755568C3BF25CFCA0F7AF9279CA593305C27645022DD2DD0F7FFCAC92EDEF04B9DCAE7FD55E2C69C76CF061F1ECD724850FCA574543954105B9A3824B849DDD75DEB57B382054AAB4A1FE467D235FB77C220730ED7D9B79A575831395AAE0C4C0B0D3E7EC17511C3DE9A86AD1C68BFF861FA0151020E43A1C6B4F4D6273D586B1D291AEF45DF454463F39BACA2FF07AA5E24A7EA850189F8C39BA8E88FEB21C1647EF910898B02492C49599111D3558F05A4CFD0FDBF33E802798FF2C437FCD65AAC1024FB29D08DCD7DB7E08279C3EB4B64E58747096641D1886145EC9E5ADDC8BBC81BE0DC77C3F3A017311050B921B5506F13AF30BA04AFAA210F0359E710C01319DE7BFF165A3615E8DDFC6E3FC167BA39B332B42E5207CA4934EABDAE2D057FA661DEB3EBC1A4AFD4F3E014918EC");

  string decoded;
  BOOST_CHECK_NO_THROW(StringSource source(imported,
                                           true,
                                           new HexDecoder(new StringSink(decoded)))); //StringSource

  SecTpmFile tpm;

  Name keyName("/TestSecTpmFile/ImportKey/ksk-" +
               boost::lexical_cast<string>(time::toUnixTimestamp(time::system_clock::now())));

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);

  BOOST_REQUIRE_NO_THROW(
    tpm.importPrivateKeyPkcs5IntoTpm(keyName,
                                     reinterpret_cast<const uint8_t*>(decoded.c_str()),
                                     decoded.size(),
                                     "1234"));

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == true);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == true);

  shared_ptr<PublicKey> pubkeyPtr;
  BOOST_CHECK_NO_THROW(pubkeyPtr = tpm.getPublicKeyFromTpm(keyName));

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

  ConstBufferPtr exported;
  BOOST_CHECK_NO_THROW(exported = tpm.exportPrivateKeyPkcs5FromTpm(keyName, "5678"));

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);

  BOOST_REQUIRE(tpm.importPrivateKeyPkcs5IntoTpm(keyName, exported->buf(), exported->size(),
                                                 "5678"));

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == true);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == true);

  const uint8_t content2[] = {0x05, 0x06, 0x07, 0x08};
  Block sigBlock2;
  BOOST_CHECK_NO_THROW(sigBlock2 = tpm.signInTpm(content2, sizeof(content2),
                                                 keyName, DIGEST_ALGORITHM_SHA256));

  try
    {
      using namespace CryptoPP;

      RSA::PublicKey publicKey;
      ByteQueue queue;
      queue.Put(reinterpret_cast<const byte*>(pubkeyPtr->get().buf()), pubkeyPtr->get().size());
      publicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier (publicKey);
      bool result = verifier.VerifyMessage(content2, sizeof(content2),
                                           sigBlock2.value(), sigBlock2.value_size());

      BOOST_CHECK_EQUAL(result, true);
    }
  catch (CryptoPP::Exception& e)
    {
      BOOST_CHECK(false);
    }

  tpm.deleteKeyPairInTpm(keyName);

  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE) == false);
  BOOST_REQUIRE(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC) == false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
